/*@file
  Provides cleansing configuration for supported programming languages.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/
#ifndef SC_CLEANSE_CONFIGS_H_
#define SC_CLEANSE_CONFIGS_H_

#include <ScCleanseInput.h>

///
/// Cleansing configuration type enumeration.
///
typedef enum {
  ScCleanseConfigTypeMin = 0,
  ScCleanseConfigTypeC   = ScCleanseConfigTypeMin,
  ScCleanseConfigTypeJava,
  ScCleanseConfigTypeFSharp,
  ScCleanseConfigTypeMax
} sc_cleanse_config_type_t;

///
/// Cleansing configurations for the languages defined by
/// sc_cleanse_config_type_t.
///
extern const sc_cleanse_config_t *const gScCleanseConfigs[
  ScCleanseConfigTypeMax + 1
  ];

#endif // SC_CLEANSE_CONFIGS_H_
