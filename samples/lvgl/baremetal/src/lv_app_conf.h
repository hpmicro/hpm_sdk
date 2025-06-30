/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LV_APP_CONF_H
#define LV_APP_CONF_H

/*
 * - You can change already existed configuration in lv_conf.h
 * #undef LV_XXXX
 * #define LV_XXXX NNN
 *
 * - You can add new configuration
 *
 * #define LV_XXXX NNN
 *
 * - If the macro is depended on other macro, we suggest to add it in CMakeLists.txt
 *
 *  First, check if this macro exists in lv_conf.h. If it does and is used by other macros within lv_conf.h,
 *  it is suggest to define this macro via CMakeLists.txt. otherwise, errors may occur.
 *
 * For example:
 * #ifndef LV_USE_SYSMON
 * #define LV_USE_SYSMON   0
 * #endif
 * #if LV_USE_SYSMON
 *   #define LV_SYSMON_GET_IDLE lv_timer_get_idle
 * #endif
 *
 * In CMakeLists.txt:
 *
 * sdk_compile_definitions(-DLV_USE_SYSMON=1)
 */

#undef LV_USE_HPM_MODE_DIRECT
#define LV_USE_HPM_MODE_DIRECT     0

#undef LV_USE_HPM_PDMA_FLUSH
#define LV_USE_HPM_PDMA_FLUSH      0

#undef LV_USE_HPM_PDMA_WAIT_VSYNC
#define LV_USE_HPM_PDMA_WAIT_VSYNC 0

#endif
