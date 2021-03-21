/*@file
  Implements an unit testing entry point for the SimilarityChecker project.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <ScDistances.h>

#include "ScCommon.h"

///
/// The Levenshtein Matrix scratch buffer required for ScLevenshteinDistance().
///
static size_t mScUnitTestMatrixScratch[SC_MAX_LINE_LENGTH];

/*
  Performs a unit test of ScLevenshteinDistance() with prepared inputs.
  The result of this test is printed to stdout.

  @param[in] String1           The first string to compare. It needs to be
                               properly terminated.
  @param[in] String2           The second string to compare. It needs to be
                               properly terminated.
  @param[in] ExpectedDistance  The correct distance between String1 and String2.
*/
static void ScUnitTestLevenshtein(
  const char *String1,
  const char *String2,
  size_t     ExpectedDistance
  )
{
  assert(String1 != NULL);
  assert(String2 != NULL);
  assert(ExpectedDistance <= strlen(String1)
      || ExpectedDistance <= strlen(String2));

  size_t Distance = ScLevenshteinDistance(
    mScLevenshteinMatrixInit,
    mScUnitTestMatrixScratch,
    String1,
    strlen(String1),
    String2,
    strlen(String2)
    );
  if (Distance != ExpectedDistance) {
    printf(
      "FAILURE[\"%s\", \"%s\"]! Expected %zu, got %zu.\n",
      String1,
      String2,
      ExpectedDistance,
      Distance
      );
    return;
  }

  printf("SUCCESS[\"%s\", \"%s\"]!\n", String1, String2);
}

/*
  Main entry point for unit testing of the SimilarityChecker project.
  A set of tests is performed and their results are printed to stdout.
*/
int main(void) {
  ScLevenshteinSwapInitialise();

  ScUnitTestLevenshtein(
    "This is a test string",
    "test",
    strlen("This is a  string")
    );

  ScUnitTestLevenshtein("Test",  "Toast",  2);
  ScUnitTestLevenshtein("House", "Mouse",  1);
  ScUnitTestLevenshtein("Claus", "clause", 2);
  ScUnitTestLevenshtein("1234",  "5678",   4);

  return 0;
}
