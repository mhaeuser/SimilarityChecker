/*@file
  Provides a cleansing configuration for the C Programming Language.

  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#include "ScCleanseConfigs.h"
#include "ScCleanseInput.h"
#include "ScCleanseConfigsInternal.h"

///
/// File extensions for C Programming Language code files.
///
static const char *mScFileExtsC[] = { "c", "cpp", "h", "hpp" };

///
/// Characters that are to be treated as new lines for the C Programming
/// Language.
///
static const char mScNewLineCharsC[] = { ';', '{', '}', '?', ':' };

///
/// Line drop prefixes for the C Programming Language.
/// Drop single line comments and a few kinds of preprocessor directives.
///
static const sc_lenghted_string_t mScLineDropPrefixesC[] = {
  SC_LENGTHED_STRING("//"),
  //
  // ATTENTION: This may lead to preprocessor concatenation false positives.
  // However, those are rare and probably unused in environments where this
  // tool makes sense to be used in.
  //
  SC_LENGTHED_STRING("#i"), // include, if, ifdef
  SC_LENGTHED_STRING("#e")  // elif, else, endif
};

///
/// Generalisees for keywords of the C Programming Language.
///
static const sc_lenghted_string_t mScGeneraliseesKeywordsC[] = {
  SC_LENGTHED_STRING("static"),
  SC_LENGTHED_STRING("const"),
  SC_LENGTHED_STRING("volatile"),
  SC_LENGTHED_STRING("restrict"),
  //
  // While this could lead to no type at all ("unsigned" and "signed" imply
  // int), this is barely used in practice and saved various combinations below.
  //
  SC_LENGTHED_STRING("unsigned"),
  SC_LENGTHED_STRING("signed")
};

///
/// Generalisees for primitive types of the C Programming Language.
///
static const sc_lenghted_string_t mScGeneraliseesIntC[] = {
  SC_LENGTHED_STRING("char"),
  SC_LENGTHED_STRING("short"),
  //
  // Omit "int" as it is the generaliser.
  //
  SC_LENGTHED_STRING("long"),
  //
  // "long long" is implicit by "long".
  //
  SC_LENGTHED_STRING("uint64_t"),
  SC_LENGTHED_STRING("int64_t"),
  SC_LENGTHED_STRING("uint32_t"),
  SC_LENGTHED_STRING("int32_t"),
  SC_LENGTHED_STRING("uint16_t"),
  SC_LENGTHED_STRING("int16_t"),
  SC_LENGTHED_STRING("uint8_t"),
  SC_LENGTHED_STRING("int8_t"),
  SC_LENGTHED_STRING("size_t"),
  SC_LENGTHED_STRING("uintptr_t"),
  //
  // Group floating point with integer types as they are frequently not strictly
  // logically distinguished. While the generaliser "int" is slightly misleading
  // it should still be fairly obvious and saves generalising one more type.
  //
  SC_LENGTHED_STRING("float"),
  SC_LENGTHED_STRING("double")
  //
  // "long double" is implicit by "long" and "double".
  //
};

///
/// Generalise operations for the C Programming Language.
///
static const sc_cleanse_generalise_t mScGeneraliseC[] = {
  {
    //
    // Remove the keywords entirely.
    //
    SC_LENGTHED_STRING(""),
    mScGeneraliseesKeywordsC,
    SC_ARRAY_LEN(mScGeneraliseesKeywordsC)
  },
  {
    //
    // Use "int" as generaliser as it is the most generic and shortest string.
    //
    SC_LENGTHED_STRING("int"),
    mScGeneraliseesIntC,
    SC_ARRAY_LEN(mScGeneraliseesIntC)
  }
};

const sc_cleanse_config_t mScCleanseConfigC = {
  mScFileExtsC,             SC_ARRAY_LEN(mScFileExtsC),
  SC_LENGTHED_STRING("/*"), SC_LENGTHED_STRING("*/"),
  mScLineDropPrefixesC,     SC_ARRAY_LEN(mScLineDropPrefixesC),
  mScNewLineCharsC,         SC_ARRAY_LEN(mScNewLineCharsC),
  mScGeneraliseC,           SC_ARRAY_LEN(mScGeneraliseC)
};
