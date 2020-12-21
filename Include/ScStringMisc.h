/*@file
  Provides APIs for miscellaneous string operations.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/
#ifndef SC_STRING_MISC_H_
#define SC_STRING_MISC_H_

#include <stdbool.h>
#include <stddef.h>

///
/// Structure to declare string literals together with their length.
///
typedef struct {
  ///
  /// The string as a character array. It does not need to be termiminated.
  ///
  const char *String;
  ///
  /// The length, in characters, of String.
  ///
  size_t     Length;
} sc_lenghted_string_t;

typedef struct {
  ///
  /// A pointer to the line's start within the associated string. This is only
  /// valid as long as the associated string is valid and unmodified.
  ///
  const char *Start;
  ///
  /// The length of the line up until the next new line or EOF.
  ///
  size_t     Length;
} sc_str_line_info_t;

typedef struct {
  ///
  /// The length of the longest line in Lines.
  ///
  size_t             MaxLineLength;
  ///
  /// The number of elements in Lines.
  ///
  size_t             NumLines;
  ///
  /// The line information for the associated string.
  ///
  sc_str_line_info_t Lines[];
} sc_str_lines_info_t;

/*
  Compares the prefix of String to Prefix.

  @param[in] String        The string to check. It does not need to be
                           terminated.
  @param[in] Prefix        The prefix to compare to. It does not need to be
                           terminated.
  @param[in] StringLength  The length, in characters, of String.
  @param[in] PrefixLength  The length, in characters, of Prefix.

  @retval 0      String starts with Prefix.
  @retval other  String does not start with Prefix.
*/
int ScStrnPrefix(
  const char *String,
  const char *Prefix,
  size_t     StringLength,
  size_t     PrefixLength
  );

/*
  Returns line information about String.

  @param[in] String        The string to inspect. It does not need to be
                           terminated.
  @param[in] StringLength  The length, in characters, of String. It must be
                           larger than 0 and smaller than SIZE_MAX.

  @retval NULL   An error has occured.
  @retval other  A buffer with the line information of String. This buffer is
                 allocated with malloc and caller-owned.
*/
sc_str_lines_info_t *ScStrGetLineInfo(
  const char *String,
  size_t     StringLength
  );

#endif // SC_STRING_MISC_H_
