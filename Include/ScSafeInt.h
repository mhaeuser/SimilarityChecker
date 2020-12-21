/*@file
  Provides APIs for safe overflow arithmetics.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/
#ifndef SC_SAFE_INT_H_
#define SC_SAFE_INT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
  Returns the smallest value out of the two operands.

  @param[in] A  The first operand.
  @param[in] B  The second operand.
*/
#define SC_MIN(A, B)  ((A) < (B) ? (A) : (B))

/*
  Returns the biggest value out of the two operands.

  @param[in] A  The first operand.
  @param[in] B  The second operand.
*/
#define SC_MAX(A, B)  ((A) > (B) ? (A) : (B))

/*
  Computes the number of elements in a compile-time array.

  @param[in] X  The array to compute the length of.
*/
#define SC_ARRAY_LEN(X)  (sizeof(X) / sizeof(*(X)))

//
// If compiler intrinsics are available, prefer them over custom implemntations.
//
#ifdef __has_builtin
  #if __has_builtin(__builtin_add_overflow)
    #define ScSafeAdd32_(A, B, Result)  \
      __builtin_add_overflow(           \
        (uint32_t) (A),                 \
        (uint32_t) (B),                 \
        (uint32_t *) (Result)           \
        )

    #define ScSafeAdd64_(A, B, Result)  \
      __builtin_add_overflow(           \
        (uint64_t) (A),                 \
        (uint64_t) (B),                 \
        (uint64_t *) (Result)           \
        )

    #define ScSafeAddSize_(A, B, Result)  \
      __builtin_add_overflow(             \
        (size_t) (A),                     \
        (size_t) (B),                     \
        (size_t *) (Result)               \
        )

    #define SC_SAFE_INT_ADD_AVAILABLE_
  #endif
    
  #if __has_builtin(__builtin_sub_overflow)
    #define ScSafeSub32_(A, B, Result)  \
      __builtin_sub_overflow(           \
        (uint32_t) (A),                 \
        (uint32_t) (B),                 \
        (uint32_t *) (Result)           \
        )

    #define ScSafeSub64_(A, B, Result)  \
      __builtin_sub_overflow(           \
        (uint64_t) (A),                 \
        (uint64_t) (B),                 \
        (uint64_t *) (Result)           \
        )

    #define SC_SAFE_INT_SUB_AVAILABLE_
  #endif

  #if __has_builtin(__builtin_mul_overflow)
    #define ScSafeMul32_(A, B, Result)  \
      __builtin_mul_overflow(           \
        (uint32_t) (A),                 \
        (uint32_t) (B),                 \
        (uint32_t *) (Result)           \
        )

    #define ScSafeMul64_(A, B, Result)  \
      __builtin_mul_overflow(           \
        (uint64_t) (A),                 \
        (uint64_t) (B),                 \
        (uint64_t *) (Result)           \
        )

    #define ScSafeMulSize_(A, B, Result)  \
      __builtin_mul_overflow((size_t) (A), (size_t) (B), (size_t *) (Result))

    #define SC_SAFE_INT_MUL_AVAILABLE_
  #endif
#endif // defined(__has_builtin)

//
// Provide custom implementations for all operations not covered by compiler
// intrinsics.
//

#ifndef SC_SAFE_INT_ADD_AVAILABLE_
  #define ScSafeAdd32_(A, B, Result)    ScCustomSafeAdd32((A), (B), (Result))
  #define ScSafeAdd64_(A, B, Result)    ScCustomSafeAdd64((A), (B), (Result))
  #define ScSafeAddSize_(A, B, Result)  ScCustomSafeAddSize((A), (B), (Result))
#endif

#ifndef SC_SAFE_INT_SUB_AVAILABLE_
  #define ScSafeSub32_(A, B, Result)  ScCustomSafeSub32((A), (B), (Result))
  #define ScSafeSub64_(A, B, Result)  ScCustomSafeSub64((A), (B), (Result))
#endif

#ifndef SC_SAFE_INT_MUL_AVAILABLE_
  #define ScSafeMul32_(A, B, Result)    ScCustomSafeMul32((A), (B), (Result))
  #define ScSafeMul64_(A, B, Result)    ScCustomSafeMul64((A), (B), (Result))
  #define ScSafeMulSize_(A, B, Result)  ScCustomSafeMulSize((A), (B), (Result))
#endif

#undef SC_SAFE_INT_ADD_AVAILABLE_
#undef SC_SAFE_INT_SUB_AVAILABLE_
#undef SC_SAFE_INT_MUL_AVAILABLE_

/*
  Adds two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^32 is returned.

  @returns  Whether *Result is not equal to A + B.
*/
#define ScSafeAdd32(A, B, Result)  ScSafeAdd32_(A, B, Result)

/*
  Subtracts two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^32 is returned.

  @returns  Whether *Result is not equal to A - B.
*/
#define ScSafeSub32(A, B, Result)  ScSafeSub32_(A, B, Result)

/*
  Multiplies two operands and returns whether the result is equal to the
  infinite precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^64 is returned.

  @returns  Whether *Result is not equal to A * B.
*/
#define ScSafeMul32(A, B, Result)  ScSafeMul32_(A, B, Result)

/*
  Adds two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^64 is returned.

  @returns  Whether *Result is not equal to A + B.
*/
#define ScSafeAdd64(A, B, Result)  ScSafeAdd64_(A, B, Result)

/*
  Subtracts two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^64 is returned.

  @returns  Whether *Result is not equal to A - B.
*/
#define ScSafeSub64(A, B, Result)  ScSafeSub64_(A, B, Result)

/*
  Multiplies two operands and returns whether the result is equal to the
  infinite precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^64 is returned.

  @returns  Whether *Result is not equal to A * B.
*/
#define ScSafeMul64(A, B, Result)  ScSafeMul64_(A, B, Result)

/*
  Adds two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod MAX_SIZE+1 is
                      returned.

  @returns  Whether *Result is not equal to A + B.
*/
#define ScSafeAddSize(A, B, Result)  ScSafeAddSize_(A, B, Result)

/*
  Multiplies two operands and returns whether the result is equal to the
  infinite precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod MAX_SIZE+1 is
                      returned.

  @returns  Whether *Result is not equal to A * B.
*/
#define ScSafeMulSize(A, B, Result)  ScSafeMulSize_(A, B, Result)

/*
  Adds two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^32 is returned.

  @returns  Whether *Result is not equal to A + B.
*/
bool ScCustomSafeAdd32(
  uint32_t A,
  uint32_t B,
  uint32_t *Result
  );

/*
  Subtracts two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^32 is returned.

  @returns  Whether *Result is not equal to A - B.
*/
bool ScCustomSafeSub32(
  uint32_t A,
  uint32_t B,
  uint32_t *Result
  );

/*
  Multiplies two operands and returns whether the result is equal to the
  infinite precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^32 is returned.

  @returns  Whether *Result is not equal to A * B.
*/
bool ScCustomSafeMul32(
  uint32_t A,
  uint32_t B,
  uint32_t *Result
  );

/*
  Adds two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^64 is returned.

  @returns  Whether *Result is not equal to A + B.
*/
bool ScCustomSafeAdd64(
  uint64_t A,
  uint64_t B,
  uint64_t *Result
  );

/*
  Subtracts two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^64 is returned.

  @returns  Whether *Result is not equal to A - B.
*/
bool ScCustomSafeSub64(
  uint64_t A,
  uint64_t B,
  uint64_t *Result
  );

/*
  Multiplies two operands and returns whether the result is equal to the
  infinite precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod 2^64 is returned.

  @returns  Whether *Result is not equal to A * B.
*/
bool ScCustomSafeMul64(
  uint64_t A,
  uint64_t B,
  uint64_t *Result
  );

/*
  Adds two operands and returns whether the result is equal to the infinite
  precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod MAX_SIZE+1 is
                      returned.

  @returns  Whether *Result is not equal to A + B.
*/
bool ScCustomSafeAddSize(
  size_t A,
  size_t B,
  size_t *Result
  );

/*
  Multiplies two operands and returns whether the result is equal to the
  infinite precision result of the operation.

  @param[in]  A       The first operand.
  @param[in]  B       The second operand.
  @param[out] Result  A pointer into which the result mod MAX_SIZE+1 is
                      returned.

  @returns  Whether *Result is not equal to A * B.
*/
bool ScCustomSafeMulSize(
  size_t A,
  size_t B,
  size_t *Result
  );

#endif // SC_SAFE_INT_H_
