/*@file
  Provides internal definitions of cleansing configurations.
  
  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#include "ScCleanseInput.h"
#include "ScCleanseConfigs.h"
#include "ScCleanseConfigsInternal.h"

const sc_cleanse_config_t *const gScCleanseConfigs[ScCleanseConfigTypeMax + 1] =
{
  [ScCleanseConfigTypeC]      = &mScCleanseConfigC,
  [ScCleanseConfigTypeJava]   = &mScCleanseConfigJava,
  [ScCleanseConfigTypeFSharp] = &mScCleanseConfigFSharp,
  [ScCleanseConfigTypeMax]    = &mScCleanseConfigUnknown
};
