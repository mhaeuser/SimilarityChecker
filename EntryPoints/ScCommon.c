/*@file
  Implements similarity checker APIs for given code inputs.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ScCleanseConfigs.h>
#include <ScCleanseInput.h>
#include <ScDistances.h>
#include <ScFileIo.h>
#include <ScSafeInt.h>

#include "ScCommon.h"

///
/// Statically allocate the Levenshtein matrix buffers to avoid reallocations.
///
size_t mScLevenshteinMatrixInit[SC_MAX_LINE_LENGTH];

void ScLevenshteinSwapInitialise(void)
{
  //
  // Matrix[0,0] = 0 is implicit by the loop in ScLevenshteinDistance().
  // This is equivalent to: Fill Matrix[0,1:] with 0,...,Str2Length.
  //
  #pragma omp simd
  for (
    size_t ColumnIndexMinus1 = 0;
    ColumnIndexMinus1 < SC_MAX_LINE_LENGTH;
    ++ColumnIndexMinus1
    ) {
    mScLevenshteinMatrixInit[ColumnIndexMinus1] = ColumnIndexMinus1 + 1;
  }
}

double ScLevenshteinSwap(
  const sc_cleanse_file_t *File1,
  const sc_cleanse_file_t *File2,
  size_t                  NumLinesSwap
  )
{
  assert(File1 != NULL);
  assert(File1->Buffer != NULL || File1->Length == 0);
  assert(File1->LinesInfo != NULL);
  assert(File2 != NULL);
  assert(File2->Buffer != NULL || File2->Length == 0);
  assert(File2->LinesInfo != NULL);
  //
  // Make sure File1 is the shorter file to improve the control flow below.
  //
  const sc_str_lines_info_t  *LinesInfo1;
  const sc_str_lines_info_t  *LinesInfo2;
  size_t                     ShorterLen;
  if (File1->LinesInfo->NumLines > File2->LinesInfo->NumLines) {
    LinesInfo1 = File2->LinesInfo;
    LinesInfo2 = File1->LinesInfo;
    ShorterLen = File2->Length;
  } else {
    LinesInfo1 = File1->LinesInfo;
    LinesInfo2 = File2->LinesInfo;
    ShorterLen = File1->Length;
  }
  //
  // Allocate the scratch buffer on the stack to allow parallelisation.
  //
  size_t MatrixScratch[SC_MAX_LINE_LENGTH];
  //
  // Pair all lines in file 1 with appropiate lines in file 2.
  //
  size_t TotalDiff = 0;
  size_t TotalLength = 0;
  for (
    size_t Line1Index = 0;
    Line1Index < LinesInfo1->NumLines;
    ++Line1Index
    ) {
    size_t BestMatch   = SIZE_MAX;
    size_t MatchLength = 1;
    //
    // Lines were cleansed such that there are no subsequent new lines. As such,
    // the number of lines in a buffer can be at most Size / 2. Use this
    // knowledge to ensure the addition Line1Index + SC_NUM_LINES_SWAP is safe.
    //
    _Static_assert(
      SC_NUM_LINES_SWAP <= SIZE_MAX / 2,
      "The addition may overflow."
      );
    //
    // The subtraction LinesInfo2->NumLines - 1 is safe because
    // 0 < LinesInfo1->NumLines <= LinesInfo2->NumLines
    //
    assert(LinesInfo2->NumLines >= LinesInfo1->NumLines);
    //
    // Check [min{Line1Index - NumLinesSwap, 0},
    //        min{Line1Index + NumLinesSwap, LinesInfo2->NumLines - 1}]
    //
    const size_t StartIndex = Line1Index > NumLinesSwap
                                ? Line1Index - NumLinesSwap
                                : 0;
    const size_t TopIndex = LinesInfo2->NumLines - Line1Index > NumLinesSwap
                              ? Line1Index + NumLinesSwap + 1
                              : LinesInfo2->NumLines;

    double BestScore = DBL_MAX;
    //
    // TODO: Only match lines in file 2 once?
    //
    for (size_t Line2Index = StartIndex; Line2Index < TopIndex; ++Line2Index) {
      assert(Line2Index < LinesInfo2->NumLines);

      assert(LinesInfo1->MaxLineLength <= SC_MAX_LINE_LENGTH);
      assert(
        LinesInfo1->Lines[Line1Index].Length <= LinesInfo1->MaxLineLength
        );
      assert(LinesInfo2->MaxLineLength <= SC_MAX_LINE_LENGTH);
      assert(
        LinesInfo2->Lines[Line2Index].Length <= LinesInfo2->MaxLineLength
        );

      size_t Distance = ScLevenshteinDistance(
        mScLevenshteinMatrixInit,
        MatrixScratch,
        LinesInfo1->Lines[Line1Index].Start,
        LinesInfo1->Lines[Line1Index].Length,
        LinesInfo2->Lines[Line2Index].Start,
        LinesInfo2->Lines[Line2Index].Length
        );

      size_t MatchLengthTmp = SC_MAX(
        LinesInfo1->Lines[Line1Index].Length,
        LinesInfo2->Lines[Line2Index].Length
        );
      double CurLineScore;
      if (Distance == 0) {
        CurLineScore = 0;
      } else {
        CurLineScore = (double) Distance / (double) MatchLengthTmp;
      }
      //
      // Update the highest score of the pairing process for this line.
      //
      if (BestScore > CurLineScore) {
        BestScore   = CurLineScore;
        BestMatch   = Distance;
        MatchLength = MatchLengthTmp;
      }
    }
    //
    // As MatchLength can at most be the maximum of each line's length, it may
    // overflow TotalLength.
    //
    bool Result = ScSafeAddSize(TotalLength, MatchLength, &TotalLength);
    if (Result) {
      return (double) INFINITY;
    }
    //
    // The distance cannot be longer than the longer of the two references,
    // hence this cannot overflow if MatchLength does not overflow TotalLength.
    //
    TotalDiff += BestMatch;
    assert(TotalDiff >= BestMatch);
  }

  return 1. - ((double) TotalDiff / (double) TotalLength);
}

bool ScInitialiseCleanseFile(
  sc_cleanse_file_t        *File,
  sc_cleanse_config_type_t FileType
  )
{
  assert(File != NULL);
  assert(File->Buffer != NULL);
  assert(File->Length > 0);
  assert(FileType >= ScCleanseConfigTypeMin
      && FileType <= ScCleanseConfigTypeMax);
  assert((unsigned int) FileType < SC_ARRAY_LEN(gScCleanseConfigs));
  //
  // Cleanse the read file's contents using the configuration for FileType.
  //
  ScCleanseInput(File->Buffer, &File->Length, gScCleanseConfigs[FileType]);
  //
  // There is no point in returning an empty file.
  //
  if (File->Length == 0) {
    return false;
  }
  //
  // Retrieve the file lines information for the cleansed content.
  //
  File->LinesInfo = ScStrGetLineInfo(File->Buffer, File->Length);
  if (File->LinesInfo == NULL) {
    return false;
  }
  //
  // As the file is non-empty and subsequent new lines should be cleansed away,
  // no line can be empty.
  //
  for (size_t Index = 0; Index < File->LinesInfo->NumLines; ++Index) {
    assert(File->LinesInfo->Lines[Index].Length > 0);
  }
  //
  // Ensure the file line length constraints for the cleansed content.
  //
  if (File->LinesInfo->MaxLineLength > SC_MAX_LINE_LENGTH) {
    free(File->LinesInfo);
    return false;
  }

  return true;
}

bool ScReadCleansedFile(
  sc_cleanse_file_t        *File,
  const char               *FileName,
  sc_cleanse_config_type_t FileType
  )
{
  assert(File != NULL);
  assert(FileName != NULL);
  assert(FileType >= ScCleanseConfigTypeMin
      && FileType <= ScCleanseConfigTypeMax);
  //
  // Use ScCleanseConfigTypeMax as a wildcard to automatically detect the
  // cleansing configuration.
  //
  if (FileType == ScCleanseConfigTypeMax) {
    const char *FileExt = ScGetFileExtension(FileName);

    _Static_assert(
      SC_ARRAY_LEN(gScCleanseConfigs) == ScCleanseConfigTypeMax + 1,
      "The loop must be adapted."
      );
    //
    // Iterate over all cleansing configurations to match by file extension.
    //
    for (
      FileType = ScCleanseConfigTypeMin;
      FileType < ScCleanseConfigTypeMax;
      ++FileType
      ) {
      size_t ExtIndex;
      for (
        ExtIndex = 0;
        ExtIndex < gScCleanseConfigs[FileType]->NumFileExt;
        ++ExtIndex
        ) {
        int Result = strcmp(
          FileExt,
          gScCleanseConfigs[FileType]->FileExts[ExtIndex]
          );
        if (Result == 0) {
          break;
        }
      }
      //
      // If a match was found, terminate this outer loop too.
      //
      if (ExtIndex < gScCleanseConfigs[FileType]->NumFileExt) {
        break;
      }
    }
    //
    // If the type could not be automatically detected, FileType will be
    // ScCleanseConfigTypeMax and a profile for unknown inputs will be used.
    //
    assert(FileType >= ScCleanseConfigTypeMin
        && FileType <= ScCleanseConfigTypeMax);
  }
  //
  // Read the file at FileName.
  //
  File->Buffer = ScReadFile(&File->Length, FileName, SC_MAX_FILE_SIZE);
  if (File->Buffer == NULL) {
    return false;
  }
  //
  // Cleanse the read file's contents using the configuration for FileType.
  //
  bool Result = ScInitialiseCleanseFile(File, FileType);
  if (!Result) {
    free(File->Buffer);
  }

  return Result;
}

void ScFreeCleansedFile(
  sc_cleanse_file_t *File
  )
{
  assert(File != NULL);

  free(File->LinesInfo);
  free(File->Buffer);
}
