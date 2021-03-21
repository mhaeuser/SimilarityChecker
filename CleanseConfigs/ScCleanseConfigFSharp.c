/*@file
  Provides a cleansing configuration for F#.

  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
*/

#include "ScCleanseConfigs.h"
#include "ScCleanseInput.h"
#include "ScCleanseConfigsInternal.h"

///
/// File extensions for F# code files.
///
static const char *mScFileExtsFSharp[] = {
  "fs", "fsi", "fsx", "fsscript"
};

///
/// Line drop prefixes for F#.
/// Drop single line comments.
///
static const sc_lenghted_string_t mScLineDropPrefixesFSharp[] = {
  SC_LENGTHED_STRING("//")
};

///
/// Generalisees for keywords of F#.
///
static const sc_lenghted_string_t mScGeneraliseesKeywordsFSharp[] = {
  SC_LENGTHED_STRING("override"),
  SC_LENGTHED_STRING("public"),
  SC_LENGTHED_STRING("internal"),
  SC_LENGTHED_STRING("private"),
};

///
/// Generalisees for primitive types of F#.
///
static const sc_lenghted_string_t mScGeneraliseesIntFSharp[] = {
  SC_LENGTHED_STRING("sbyte"),
  SC_LENGTHED_STRING("byte"),
  SC_LENGTHED_STRING("uint16"),
  SC_LENGTHED_STRING("int16"),
  //
  // Omit "int" as it is the generaliser.
  //
  SC_LENGTHED_STRING("uint32"),
  SC_LENGTHED_STRING("int64"),
  SC_LENGTHED_STRING("uint64"),
  SC_LENGTHED_STRING("unativeint"),
  SC_LENGTHED_STRING("nativeint"),
  //
  // Group floating point with integer types as they are frequently not strictly
  // logically distinguished. While the generaliser "int" is slightly misleading
  // it should still be fairly obvious and saves generalising one more type.
  //
  SC_LENGTHED_STRING("decimal"),
  SC_LENGTHED_STRING("float32"),
  SC_LENGTHED_STRING("float"),
  SC_LENGTHED_STRING("single"),
  SC_LENGTHED_STRING("double")
};

///
/// Generalise operations for F#.
///
static const sc_cleanse_generalise_t mScGeneraliseFSharp[] = {
  {
    //
    // Remove the keywords entirely.
    //
    SC_LENGTHED_STRING(""),
    mScGeneraliseesKeywordsFSharp,
    SC_ARRAY_LEN(mScGeneraliseesKeywordsFSharp)
  },
  {
    //
    // Use "int" as generaliser as it is the most generic and shortest string.
    //
    SC_LENGTHED_STRING("int"),
    mScGeneraliseesIntFSharp,
    SC_ARRAY_LEN(mScGeneraliseesIntFSharp)
  }
};

const sc_cleanse_config_t mScCleanseConfigFSharp = {
  mScFileExtsFSharp,         SC_ARRAY_LEN(mScFileExtsFSharp),
  SC_LENGTHED_STRING("(*"),  SC_LENGTHED_STRING("*)"),
  mScLineDropPrefixesFSharp, SC_ARRAY_LEN(mScLineDropPrefixesFSharp),
  NULL,                      0,
  mScGeneraliseFSharp,       SC_ARRAY_LEN(mScGeneraliseFSharp)
};
