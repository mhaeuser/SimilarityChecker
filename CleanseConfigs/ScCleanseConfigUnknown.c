/*@file
  Provides a cleansing configuration for when the input is of unknown format.

  Copyright (C) 2020 Marvin Häuser. All rights reserved.
  SPDX-License-Identifier: Apache-2.0
*/

#include "ScCleanseConfigs.h"
#include "ScCleanseInput.h"
#include "ScCleanseConfigsInternal.h"

const sc_cleanse_config_t mScCleanseConfigUnknown = {
  NULL,        0,
  { NULL, 0 }, { NULL, 0 },
  NULL,        0,
  NULL,        0,
  NULL,        0
};
