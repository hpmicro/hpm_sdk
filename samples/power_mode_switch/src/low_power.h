/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LOW_POWER_H
#define LOW_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

void show_cpu_status(void);
void prepare_soc_low_power(void);
void enter_wait_mode(void);
void enter_stop_mode(void);
void enter_standby_mode(void);
void enter_shutdown_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_POWER_H */
