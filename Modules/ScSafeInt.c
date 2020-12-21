/*@file
  Provides custom implementations for safe overflow arithmetics.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ScSafeInt.h>

bool ScCustomSafeAdd32(
  uint32_t A,
  uint32_t B,
  uint32_t *Result
  )
{
  uint32_t ModResult = A + B;
  *Result = ModResult;
  //
  // For binary addition, the result is equal to the infinite precision result
  // if and only if it is bigger than or equal to one of the operands. This can
  // easily be observed with the binary representation of 2's complement
  // arithmetics, as -1 is equal to the maximum representable binary value.
  //
  return ModResult < A;
}

bool ScCustomSafeSub32(
  uint32_t A,
  uint32_t B,
  uint32_t *Result
  )
{
  uint32_t ModResult = A - B;
  *Result = ModResult;
  //
  // For binary subtraction, the result is equal to the infinite precision
  // result if and only if it is smaller than the first operand. This can easily
  // be observed with the binary representation of 2's complement arithmetics,
  // as -1 is equal to the maximum representable binary value.
  //
  return ModResult >= A;
}

bool ScCustomSafeMul32(
  uint32_t A,
  uint32_t B,
  uint32_t *Result
  )
{
  assert (Result != NULL);
  //
  // As 0xFFFFFFFF^2 equals to 0xFFFFFFFE00000001 <= 0xFFFFFFFFFFFFFFFF,
  // uint64_t can be used safely to multiply the operands.
  //
  uint64_t InfPrecResult = (uint64_t) A * B;
  *Result = (uint32_t) InfPrecResult;
  return InfPrecResult > SIZE_MAX;
}

bool ScCustomSafeAdd64(
  uint64_t A,
  uint64_t B,
  uint64_t *Result
  )
{
  uint64_t ModResult = A + B;
  *Result = ModResult;
  //
  // For binary addition, the result is equal to the infinite precision result
  // if and only if it is bigger than or equal to one of the operands. This can
  // easily be observed with the binary representation of 2's complement
  // arithmetics, as -1 is equal to the maximum representable binary value.
  //
  return ModResult < A;
}

bool ScCustomSafeSub64(
  uint64_t A,
  uint64_t B,
  uint64_t *Result
  )
{
  uint64_t ModResult = A - B;
  *Result = ModResult;
  //
  // For binary subtraction, the result is equal to the infinite precision
  // result if and only if it is smaller than the first operand. This can easily
  // be observed with the binary representation of 2's complement arithmetics,
  // as -1 is equal to the maximum representable binary value.
  //
  return ModResult >= A;
}

bool ScCustomSafeMul64(
  uint64_t A,
  uint64_t B,
  uint64_t *Result
  )
{
  assert (Result != NULL);

  uint64_t AHi;
  uint64_t ALo;
  uint64_t BHi;
  uint64_t BLo;
  uint64_t LoBits;
  uint64_t HiBits1;
  uint64_t HiBits2;
  bool     ScCustomSafe;

  //
  // Based on the 2nd option written by Charphacy.
  // https://stackoverflow.com/a/26320664
  //

  AHi = (A >> 32U);
  ALo = A & 0xFFFFFFFFU;
  BHi = (B >> 32U);
  BLo = B & 0xFFFFFFFFU;

  LoBits = ALo * BLo;
  if (AHi == 0U && BHi == 0U) {
    *Result = LoBits;
    return false; 
  }

  ScCustomSafe = AHi > 0U && BHi > 0U;
  HiBits1  = ALo * BHi;
  HiBits2  = AHi * BLo;

  *Result = LoBits + ((HiBits1 + HiBits2) << 32U);
  return ScCustomSafe
      || *Result < LoBits
      || (HiBits1 << 32U) != 0U
      || (HiBits2 << 32U) != 0U;
}

bool ScCustomSafeAddSize(
  size_t A,
  size_t B,
  size_t *Result
  )
{
  size_t ModResult = A + B;
  *Result = ModResult;
  //
  // For binary addition, the result is equal to the infinite precision result
  // if and only if it is bigger than or equal to one of the operands. This can
  // easily be observed with the binary representation of 2's complement
  // arithmetics, as -1 is equal to the maximum representable binary value.
  //
  return ModResult < A;
}

bool ScCustomSafeMulSize(
  size_t A,
  size_t B,
  size_t *Result
  )
{
  uint32_t Result32;
  uint64_t Result64;
  bool     Return;

  assert(Result != NULL);
  //
  // Call the correct precision function based on size_t precision.
  //
  _Static_assert(
    SIZE_MAX == UINT32_MAX || SIZE_MAX == UINT64_MAX,
    "This code needs to be extended."
    );
  if (SIZE_MAX == UINT32_MAX) {
    Return  = ScCustomSafeMul32((uint32_t) A, (uint32_t) B, &Result32);
    *Result = (size_t) Result32;
  } else if (SIZE_MAX == UINT64_MAX) {
    Return  = ScCustomSafeMul64((uint64_t) A, (uint64_t) B, &Result64);
    *Result = (size_t) Result64;
  } else {
    assert(false);
    Return = true;
  }
  
  return Return;
}
