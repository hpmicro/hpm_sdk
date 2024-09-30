/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MCL_MATH_H
#define HPM_MCL_MATH_H
#include "hpm_mcl_cfg.h"

#ifndef HPM_MCL_MATH_HARDWARE_ACC
#include <math.h>
#endif
/**
 * @brief Determine if a floating point number is 0
 *
 */
#define MCL_FLOAT_IS_ZERO(val)  ((val < 0.000001f) && (val > -0.000001f))
#define MCL_FLOAT_IS_INFINITY(val) ((val == INFINITY) || (val == -INFINITY))

#define SQRT3      (1.7320508075688773f)    /**< sqrt(3) */
#define SQRT3_DIV3 (0.5773502691896258f)    /**< sqrt(3)/3 */
#define MCL_SUM_OF_SQUARE_MODE(a, b) sqrtf((float)((a * a) + (b * b)))

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
