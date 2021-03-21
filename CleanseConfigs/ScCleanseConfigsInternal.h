/*@file
  Provides internal declarations of cleansing configurations.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef SC_CLEANSE_CONFIGS_INTERNAL_H_
#define SC_CLEANSE_CONFIGS_INTERNAL_H_

#include <ScCleanseInput.h>
#include <ScSafeInt.h>

///
/// Cleansing configuration for the C Programming Language.
///
extern const sc_cleanse_config_t mScCleanseConfigC;

///
/// Cleansing configuration for Java.
///
extern const sc_cleanse_config_t mScCleanseConfigJava;

///
/// Cleansing configuration for F#.
///
extern const sc_cleanse_config_t mScCleanseConfigFSharp;

///
/// Cleansing configuration for unknown inputs.
///
extern const sc_cleanse_config_t mScCleanseConfigUnknown;

#endif // SC_CLEANSE_CONFIGS_INTERNAL_H_
