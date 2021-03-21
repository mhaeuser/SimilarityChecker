/*@file
  Provides APIs to calculate distances between text snippets.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
*/
#ifndef SC_DISTANCES_H_
#define SC_DISTANCES_H_

#include <stddef.h>

/*
  Calculates the Levenshtein distance from Str1 to Str2.

  @param[in]  MatrixTop      The Levenshtein matrix top row. It must be at least
                             Str2Length Bytes in size and contain valid values
                             for the first row of the matrix.
  @param[out] MatrixScratch  The Levenshtein matrix. It must be at least
                             Str2Length Bytes in size.
  @param[in]  Str1           The first string to compare.
  @param[in]  Str1Length     The length of Str1. It must be larger than 0 and
                             smaller than SIZE_MAX.
  @param[in]  Str2           The second string to compare.
  @param[in]  Str2Length     The length of Str2. It must be larger than 0 and
                             smaller than SIZE_MAX.

  @returns  The Levenshtein distance from Str1 to Str2.
*/
size_t ScLevenshteinDistance(
  const size_t *MatrixTop,
  size_t       *MatrixScratch,
  const char   *Str1,
  size_t       Str1Length,
  const char   *Str2,
  size_t       Str2Length
  );

#endif // SC_DISTANCES_H_
