/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "stdint.h"
#include "hpm_common.h"

typedef struct {
    uint32_t timing_ms;
    void (*callback)(void);
} hpm_timer_t;

void timer_init(void);
hpm_stat_t timer_register(hpm_timer_t *timer);
