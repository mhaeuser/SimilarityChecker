/*@file
  Implements a similarity checker entry point for given code inputs.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
*/

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ScCommon.h"

/*
  Calculates the Gauss Sum of x.
*/
#define SC_GAUSS_SUM(x)  (((x) * ((x) + 1U)) / 2U)

//
// Define SC_MAX_NUM_FILES such that no memory size overflows can occur.
//
#if SIZE_MAX == UINT16_MAX
  #define SC_MAX_NUM_FILES  INT8_MAX
#elif SIZE_MAX == UINT32_MAX
  #define SC_MAX_NUM_FILES  INT16_MAX
#elif SIZE_MAX == UINT64_MAX
  #define SC_MAX_NUM_FILES  INT32_MAX
#else
  #error The definition needs to be adapted.
#endif

/*
  Main entry point to the SimilarityChecker project. A list of similarity scores
  is output for each file pairing from argv.

  @param[in] argc  The number of elements in argv.
  @param[in] argv  The arguments given to this tool.

  @retval 0      The program has executed successfully.
  @retval other  An external error by the environment has occured.
*/
int main(int argc, char *argv[]) {
  //
  // At least the file path must be provided by the C standard library.
  //
  assert(argc >= 1);

  if (argc < 3) {
    fprintf(stderr, "%s [input file 1] ... [input file n]\n", argv[0]);
    return 0;
  }
  //
  // Allocate, read and cleanse one file per argument.
  //
  unsigned int NumFiles   = (unsigned int) argc - 1U;
  char         **FileArgs = &argv[1];
  //
  // Limit the amount of input files to prevent memory overflows.
  //
  if (argc - 1 > SC_MAX_NUM_FILES) {
    fprintf(
      stderr,
      "Truncated input files to %llu.\n",
      (unsigned long long) SC_MAX_NUM_FILES
      );
    NumFiles = SC_MAX_NUM_FILES;
  }
  //
  // SC_GAUSS_SUM((uint64_t) SC_MAX_NUM_FILES) is safe because multiplications
  // in one power of two space with at most one value being one larger than its
  // maximum value cannot overflow in any space of a higher power of two.
  //
  // This ensures
  //   1) SC_GAUSS_SUM((uint64_t) SC_MAX_NUM_FILES) cannot overflow in size_t.
  //   2) sizeof(*Files) * NumFiles cannot overflow in size_t.
  //
  _Static_assert(
    sizeof(sc_cleanse_file_t) <= UINT_MAX
    && SC_MAX_NUM_FILES <= UINT32_MAX
    && SC_GAUSS_SUM((uint64_t) SC_MAX_NUM_FILES) <= SIZE_MAX / sizeof(double),
    "The memory arithmetics below may overflow."
    );

  sc_cleanse_file_t *Files = malloc(sizeof(*Files) * NumFiles);
  //
  // Allocate the ratings result list. As NumFiles files must be
  // cross-compared, its size is precisely the Gauss Sum of NumFiles.
  //
  double *Ratings = malloc(SC_GAUSS_SUM((size_t) NumFiles) * sizeof(double));

  if (Files == NULL || Ratings == NULL) {
    fprintf(stderr, "Allocation error\n");
    free(Files);
    free(Ratings);
    return -1;
  }
  //
  // Read and cleanse all provided files.
  //
  bool FilesResult = true;
  #pragma omp parallel for
  for (unsigned int FileIndex = 0; FileIndex < NumFiles; ++FileIndex) {
    //
    // Always automatically detect the cleanse config for the moment.
    //
    bool Result = ScReadCleansedFile(
      &Files[FileIndex],
      FileArgs[FileIndex],
      ScCleanseConfigTypeMax
      );
    //
    // Use the Reserved field to store the associated file name index.
    //
    Files[FileIndex].Reserved = FileIndex;

    if (!Result) {
      #pragma omp critical
      fprintf(stderr, "Cleansed read error: %s\n", FileArgs[FileIndex]);
      //
      // FilesResult is not read within the parallel block and hence this
      // write does not need to be atomic.
      //
      FilesResult = false;
      //
      // Files[FileIndex].Buffer == NULL iff reading or cleansing failed.
      //
      Files[FileIndex].Buffer = NULL;
    } else {
      assert(Files[FileIndex].Buffer != NULL);
    }
  }
  //
  // If an error occured for any file reading or cleansing, eliminate the
  // invalid entries. This is done in a separate step from reading to not harm
  // parallelisation. As this is clearly a user error condition, performance is
  // allowed to drop in such case.
  //
  if (!FilesResult) {
    for (unsigned int FileIndex = 0; FileIndex < NumFiles; ++FileIndex) {
      if (Files[FileIndex].Buffer == NULL) {
        --NumFiles;
        memmove(
          &Files[FileIndex],
          &Files[FileIndex + 1],
          (NumFiles - FileIndex) * sizeof(*Files)
          );
        --FileIndex;
      }
    }
  }

  ScLevenshteinSwapInitialise();
  //
  // Cross-compare all files and store their ratings.
  //
  const unsigned int NumFilesMinus1      = NumFiles - 1;
  const size_t       NumFilesMinus1Gauss = SC_GAUSS_SUM(NumFilesMinus1);

  #pragma omp parallel for
  for (unsigned int File1Index = 0; File1Index < NumFiles; ++File1Index) {
    //
    // Calculate loop invariants early to benefit parallelism.
    // NumFiles - (File1Index + 1) files need to be processed starting from
    // File1Index + 1.
    //
    const unsigned int FilesLeft = NumFilesMinus1 - File1Index;
    //
    // Each step needs NumFiles - (File1Index + 1) result spots.
    // Calculate the Gauss Sum entirely to not harm parallelisation.
    //
    const size_t File1DistStart = NumFilesMinus1Gauss - SC_GAUSS_SUM(FilesLeft);

    sc_cleanse_file_t *const Files2 = &Files[File1Index + 1]; 

    #pragma omp parallel for
    for (
      unsigned int File2Index = 0;
      File2Index < FilesLeft;
      ++File2Index
      ) {
      double Score = ScLevenshteinSwap(
        &Files[File1Index],
        &Files2[File2Index],
        SC_NUM_LINES_SWAP
        );
      //
      // Check for greater-equals to silence compiler warnings as no float value
      // can be bigger anyway.
      //
      if (Score >= (double) INFINITY) {
        #pragma omp critical
        fprintf(
          stderr,
          "Failed to compare files %s and %s\n",
          FileArgs[Files[File1Index].Reserved],
          FileArgs[Files2[File2Index].Reserved]
          );
      }

      Ratings[File1DistStart + File2Index] = Score;
    }
  }
  //
  // Print the results separately from the distance loop to not harm
  // parallelisation.
  //
  size_t DistIndex = 0;
  for (unsigned int File1Index = 0; File1Index < NumFiles; ++File1Index) {
    //
    // These constants are used in the assert below. They are declared in the
    // same exact way as in the loop above to illustrate its correctness.
    //
    const unsigned int FilesLeft = NumFilesMinus1 - File1Index;
    const size_t File1DistStart = NumFilesMinus1Gauss - SC_GAUSS_SUM(FilesLeft);
    //
    // Silence the compiler for when asserts are disabled.
    //
    (void) File1DistStart;
    for (
      unsigned int File2Index = File1Index + 1;
      File2Index < NumFiles;
      ++File2Index
      ) {
      //
      // This implicitly sanitises the correctness of above's loop and ensures
      // both stay in-sync in terms of data.
      //
      assert(DistIndex == File1DistStart + (File2Index - (File1Index + 1)));
      //
      // The Reserved field is used to store the associated file name index.
      //
      fprintf(
        stdout,
        "%u %u %f\n",
        Files[File1Index].Reserved,
        Files[File2Index].Reserved,
        Ratings[DistIndex]
        );
      ++DistIndex;
    }
  }
  //
  // Free all allocated files and information structures.
  //
  #pragma omp parallel for
  for (unsigned int FileIndex = 0; FileIndex < NumFiles; ++FileIndex) {
    ScFreeCleansedFile(&Files[FileIndex]);
  }

  free(Ratings);
  free(Files);

  return 0;
}
