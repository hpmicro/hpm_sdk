/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef LOW_POWER_INF_H
#define LOW_POWER_INF_H

#ifdef __cplusplus
extern "C" {
#endif

void soc_lowpower_init(void);
void vPreSleepProcessing(unsigned long uxExpectedIdleTime);
void vPostSleepProcessing(unsigned long uxExpectedIdleTime);
void vPreSleepUserCustomProcessing(unsigned long uxExpectedIdleTime);
void vPostSleepUserCustomProcessing(unsigned long uxExpectedIdleTime);
#ifdef __cplusplus
}
#endif

#endif /* LOW_POWER_H */
