/*@file
  Provides a cleansing configuration for Java.

  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#include "ScCleanseConfigs.h"
#include "ScCleanseInput.h"
#include "ScCleanseConfigsInternal.h"

///
/// File extensions for Java code files.
///
static const char *mScFileExtsJava[] = { "java" };

///
/// Characters that are to be treated as new lines for Java.
///
static const char mScNewLineCharsJava[] = { ';', '{', '}', '?', ':' };

///
/// Line drop prefixes for Java.
/// Drop single line comments.
///
static const sc_lenghted_string_t mScLineDropPrefixesJava[] = {
  SC_LENGTHED_STRING("//")
};

///
/// Generalisees for keywords of Java.
///
static const sc_lenghted_string_t mScGeneraliseesKeywordsJava[] = {
  SC_LENGTHED_STRING("static"),
  SC_LENGTHED_STRING("final"),
  SC_LENGTHED_STRING("volatile"),
  SC_LENGTHED_STRING("synchronized"),
  SC_LENGTHED_STRING("public"),
  SC_LENGTHED_STRING("protected"),
  SC_LENGTHED_STRING("private"),
};

///
/// Generalisees for primitive types of Java.
///
static const sc_lenghted_string_t mScGeneraliseesIntJava[] = {
  SC_LENGTHED_STRING("byte"),
  SC_LENGTHED_STRING("short"),
  //
  // Omit "int" as it is the generaliser.
  //
  SC_LENGTHED_STRING("long"),
  //
  // Group floating point with integer types as they are frequently not strictly
  // logically distinguished. While the generaliser "int" is slightly misleading
  // it should still be fairly obvious and saves generalising one more type.
  //
  SC_LENGTHED_STRING("float"),
  SC_LENGTHED_STRING("double")
};

///
/// Generalise operations for Java.
///
static const sc_cleanse_generalise_t mScGeneraliseJava[] = {
  {
    //
    // Remove the keywords entirely.
    //
    SC_LENGTHED_STRING(""),
    mScGeneraliseesKeywordsJava,
    SC_ARRAY_LEN(mScGeneraliseesKeywordsJava)
  },
  {
    //
    // Use "int" as generaliser as it is the most generic and shortest string.
    //
    SC_LENGTHED_STRING("int"),
    mScGeneraliseesIntJava,
    SC_ARRAY_LEN(mScGeneraliseesIntJava)
  }
};

const sc_cleanse_config_t mScCleanseConfigJava = {
  mScFileExtsJava,          SC_ARRAY_LEN(mScFileExtsJava),
  SC_LENGTHED_STRING("/*"), SC_LENGTHED_STRING("*/"),
  mScLineDropPrefixesJava,  SC_ARRAY_LEN(mScLineDropPrefixesJava),
  mScNewLineCharsJava,      SC_ARRAY_LEN(mScNewLineCharsJava),
  mScGeneraliseJava,        SC_ARRAY_LEN(mScGeneraliseJava)
};
