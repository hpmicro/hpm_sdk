/*
* Copyright (c) 2023,2026 HPMicro
*
* SPDX-License-Identifier: BSD-3-Clause
*
*/

#ifndef _USER_CONFIG_H
#define _USER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Debug level: TRACE(0) INFO(1) WARN(2) ERROR(3) NONE(4)
 */
#define E2P_DEBUG_LEVEL     (1)

/**
 * Maximum number of unique block_id entries tracked in RAM
 */
#define E2P_MAX_VAR_CNT     (100)

/**
 * Static buffer size used by flush and read operations.
 * Must be >= the largest single data entry written via e2p_write().
 */
#define E2P_FLUSH_BUF_SIZE  (512)

/*
 * Optional: custom critical section (uncomment to override port default).
 * E2P_CRITICAL_ENTER() must return a uint32_t interrupt level.
 *
 * #define E2P_CRITICAL_ENTER() my_enter_critical()
 * #define E2P_CRITICAL_EXIT(level) my_exit_critical(level)
 */

#ifdef __cplusplus
}
#endif

#endif /* _USER_CONFIG_H */
