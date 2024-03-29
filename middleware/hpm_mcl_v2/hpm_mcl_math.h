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
 * @brief Determine if a floating point number is 0
 *
 */
#define MCL_FLOAT_IS_ZERO(val)  ((val < 0.000001f) && (val > -0.000001f))

/**
 * @brief Enable Q format
 *
 */
#ifndef HPM_MCL_Q_EN
typedef float hpm_mcl_type_t;
#define MCL_MATH_IS_ZERO(x) MCL_FLOAT_IS_ZERO((x))
#define MCL_MATH_CONVERT_FLOAT(x) (x)
#else
typedef int32_t hpm_mcl_type_t;
#define MCL_MATH_IS_ZERO(x) (x == 0)
#define MCL_MATH_CONVERT_FLOAT(x) (x)
#endif

#endif
