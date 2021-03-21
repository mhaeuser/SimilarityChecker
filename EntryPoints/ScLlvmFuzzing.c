/*@file
  Implements a LLVM fuzzing entry point for the SimilarityChecker project.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
*/

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ScCleanseConfigs.h>
#include <ScCleanseInput.h>

#include "ScCommon.h"

static void FuzzCleanseAndLevenshteinSwap(
  uint8_t                  *Data1,
  size_t                   Data1Size,
  uint8_t                  *Data2,
  size_t                   Data2Size,
  sc_cleanse_config_type_t FileType
  )
{
  assert(Data1 != NULL || Data1Size == 0);
  assert(Data2 != NULL || Data2Size == 0);
  assert(Data2Size >= Data1Size);
  //
  // Silently discard empty or too large files.
  //
  if (Data1Size == 0 || Data2Size > SC_MAX_FILE_SIZE) {
    return;
  }
  //
  // Test cleansing on both logical files.
  //
  sc_cleanse_file_t File1 = { (char *) Data1, Data1Size, NULL, 0 };
  sc_cleanse_file_t File2 = { (char *) Data2, Data2Size, NULL, 0 };

  bool Result1 = ScInitialiseCleanseFile(&File1, FileType);
  bool Result2 = ScInitialiseCleanseFile(&File2, FileType);
  if (Result1 && Result2) {
    //
    // Test ScLevenshteinSwap() with maximum line swap radius on both logical
    // files.
    // Allow a radius larger than the largest logical file.
    //
    ScLevenshteinSwap(
      &File1,
      &File2,
      Data2Size + (Data2Size < SIZE_MAX ? 1 : 0)
      );
  }
  //
  // Free all allocated resources.
  //
  if (Result1) {
    free(File1.LinesInfo);
  }
  
  if (Result2) {
    free(File2.LinesInfo);
  }
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  //
  // Silently discard empty input.
  //
  if (Size == 0) {
    return 0;
  }

  uint8_t *DataCopy = malloc(Size);
  if (DataCopy == NULL) {
    return 0;
  }
  memcpy(DataCopy, Data, Size);
  //
  // Create two logical files from the data.
  //
  uint8_t      *Data1    = &DataCopy[0];
  const size_t Data1Size = Size / 2;
  uint8_t      *Data2    = &DataCopy[Data1Size];
  const size_t Data2Size = Size - Data1Size;

  ScLevenshteinSwapInitialise();
  //
  // Test cleansing and distances against all configurations.
  //
  for (
    sc_cleanse_config_type_t ConfigIndex = ScCleanseConfigTypeMin;
    ConfigIndex <= ScCleanseConfigTypeMax;
    ++ConfigIndex
    ) {
    //
    // Reset the data to their original state.
    //
    memcpy(DataCopy, Data, Size);
    //
    // Test cleansing and score calculation on both cleansed logical files.
    //
    FuzzCleanseAndLevenshteinSwap(
      Data1,
      Data1Size,
      Data2,
      Data2Size,
      ConfigIndex
      );
  }
  //
  // Free all allocated resources.
  //
  free(DataCopy);
  return 0;
}
