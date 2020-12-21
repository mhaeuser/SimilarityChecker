/*@file
  Provides functions for miscellaneous string operations.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ScSafeInt.h>
#include <ScStringMisc.h>

int ScStrnPrefix(
  const char *String,
  const char *Prefix,
  size_t     StringLength,
  size_t     PrefixLength
  ) 
{
  assert(String != NULL || StringLength == 0);
  assert(Prefix != NULL || PrefixLength == 0);

  if (PrefixLength > StringLength) {
    return 1;
  }

  return strncmp(String, Prefix, PrefixLength);
}

sc_str_lines_info_t *ScStrGetLineInfo(
  const char *String,
  size_t     StringLength
  )
{
  assert(String != NULL && StringLength != 0);
  assert(StringLength < SIZE_MAX);

  size_t NumLines = 1;
  for (size_t CharIndex = 0; CharIndex < StringLength; ++CharIndex) {
    if (String[CharIndex] == '\n') {
      //
      // This cannot wrap around with StringLength < SIZE_MAX.
      //
      ++NumLines;
    }
  }
  //
  // Calculate the size required to hold the string lines information.
  //
  size_t StrLinesInfoSize;
  bool Result = ScSafeMulSize(
    NumLines,
    sizeof(sc_str_line_info_t),
    &StrLinesInfoSize
    );
  Result |= ScSafeAddSize(
    sizeof(sc_str_lines_info_t),
    StrLinesInfoSize,
    &StrLinesInfoSize
    );
  if (Result) {
    return NULL;
  }
  //
  // Allocate and initialise the string lines information.
  //
  sc_str_lines_info_t *StrLinesInfo = malloc(StrLinesInfoSize);
  if (StrLinesInfo == NULL) {
    return NULL;
  }

  StrLinesInfo->MaxLineLength = 0;
  StrLinesInfo->NumLines      = NumLines;
  //
  // Describe the first line's start.
  // The first line starts at the start of the string.
  //
  StrLinesInfo[0].Lines[0].Start = String;

  size_t LineIndex  = 1;
  size_t LineOffset = 0;
  for (size_t CharIndex = 0; CharIndex < StringLength; ++CharIndex) {
    if (String[CharIndex] == '\n') {
      assert(LineIndex < NumLines);
      //
      // Describe the previous line's end and update the maximum.
      // Any line but the last ends before the new line character.
      //
      StrLinesInfo->Lines[LineIndex - 1].Length = CharIndex - LineOffset;
      StrLinesInfo->MaxLineLength = SC_MAX(
        StrLinesInfo->MaxLineLength,
        StrLinesInfo->Lines[LineIndex - 1].Length
        );
      //
      // Describe the current line's start.
      // Any line but the first starts after the new line character.
      //
      LineOffset = CharIndex + 1;
      StrLinesInfo->Lines[LineIndex].Start = &String[LineOffset];

      ++LineIndex;
    }
  }

  assert(LineIndex == NumLines);
  //
  // Describe the last line's end and update the maximum.
  // The last line ends before EOF.
  //
  StrLinesInfo->Lines[NumLines - 1].Length = StringLength - LineOffset;
  StrLinesInfo->MaxLineLength = SC_MAX(
    StrLinesInfo->MaxLineLength,
    StrLinesInfo->Lines[NumLines - 1].Length
    );

  return StrLinesInfo;
}
