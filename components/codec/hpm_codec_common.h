/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_CODEC_COMMON_H
#define HPM_CODEC_COMMON_H

#if defined(CONFIG_CODEC_SGTL5000) && CONFIG_CODEC_SGTL5000
#include "hpm_sgtl5000.h"
#endif

#if defined(CONFIG_CODEC_WM8960) && CONFIG_CODEC_WM8960
#include "hpm_wm8960.h"
#endif

#if defined(CONFIG_CODEC_WM8978) && CONFIG_CODEC_WM8978
#include "hpm_wm8978.h"
#endif

#if defined(CONFIG_CODEC_ES8389) && CONFIG_CODEC_ES8389
#include "hpm_es8389.h"
#endif

#endif /* HPM_CODEC_COMMON_H */
