/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_MATH_H
#define HPM_MCL_MATH_H
#include "hpm_mcl_cfg.h"

/**
 * @brief Enable Q format
 *
 */
#ifndef HPM_MCL_Q_EN
typedef float hpm_mcl_type_t;
#else
typedef int32_t hpm_mcl_type_t;
#endif

#endif
