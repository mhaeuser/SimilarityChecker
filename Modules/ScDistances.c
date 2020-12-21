/*@file
  Provides functions to calculate distances between text snippets.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#include <assert.h>
#include <stdlib.h>

#include <ScDistances.h>
#include <ScSafeInt.h>

size_t ScLevenshteinDistance(
  const size_t *MatrixTop,
  size_t       *MatrixScratch,
  const char   *Str1,
  size_t       Str1Length,
  const char   *Str2,
  size_t       Str2Length
  )
{
  assert(MatrixTop != NULL);
  assert(MatrixScratch != NULL);
  assert(Str1 != NULL && Str1Length != 0);
  assert(Str2 != NULL && Str2Length != 0);
  //
  // Calculate the remaining Levenshtein Matrix entries with the algorithm.
  //
  for (
    size_t RowIndexMinus1 = 0;
    RowIndexMinus1 < Str1Length;
    ++RowIndexMinus1
    ) {
    //
    // Retrieve the initial left values for this row as they are implicit by the
    // definition of the Levenshtein Matrix. They will be updated to their new
    // values inside the loop using the current set of values.
    // This means only the top values need to be stored in the scratch buffer.
    //

    //
    // This is equivalent to: Fill Matrix[0:,0] with 0,...,Str1Length.
    //
    size_t TopLeftValue = RowIndexMinus1;
    //
    // RowIndexMinus1 < Str1Length holds, hence this addition cannot overflow.
    //
    size_t LeftValue = RowIndexMinus1 + 1;
    for (
      size_t ColumnIndexMinus1 = 0;
      ColumnIndexMinus1 < Str2Length;
      ++ColumnIndexMinus1
      ) {
      size_t TopValue = MatrixTop[ColumnIndexMinus1];
      //
      // Unrolling the macro significantly improves optimisations as unnecessary
      // evaluations of the arithmetic expressions are avoided.
      //
      // This is equivalent to:
      // size_t CurrentValue = SC_MIN3(
      //   TopValue + 1,
      //   LeftValue + 1,
      //   TopLeftValue + (Str1[RowIndexMinus1] == Str2[ColumnIndexMinus1] ? 0 : 1)
      //   );
      //
      size_t CurrentValue = SC_MIN(TopValue, LeftValue);
      if (TopLeftValue <= CurrentValue) {
        CurrentValue = TopLeftValue;
        if (Str1[RowIndexMinus1] != Str2[ColumnIndexMinus1]) {
          //
          // Considering the operations performed (add, delete, replace), the
          // maximum distance between two strings can at most be the length of
          // the longest. We chose the shortest way thus far and are not done
          // yet, thus CurrentValue < MAX(Str1Length, Str2Length) must hold and
          // this cannot overflow.
          //
          assert(CurrentValue < SC_MAX(Str1Length, Str2Length));
          ++CurrentValue;
        }
      } else {
        //
        // TopLeftValue > CurrentValue holds, hence this cannot overflow.
        //
        ++CurrentValue;
      }

      MatrixScratch[ColumnIndexMinus1] = CurrentValue;
      //
      // Set the surrounding values appropiately using already known values.
      //
      LeftValue    = CurrentValue;
      TopLeftValue = TopValue;
    }
    //
    // After the first iteration, MatrixScratch contains the (new) top values.
    //
    MatrixTop = MatrixScratch;
  }
  //
  // Retrieve the final result at the end of the array.
  //
  return MatrixTop[Str2Length - 1];
}
