/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _PDGO_DEMO_H
#define _PDGO_DEMO_H

/* enable wakeup source */
void dgo_enable_wakeup_src(void);

/* Check wakeup source */
void dgo_check_wakeup_src(void);

/* Check dgo gpr retention */
void dgo_check_gpr_retention(void);

/* Turn off Power (VPMCCAP, VDD_SOC) */
void dgo_turn_off(uint32_t turnoff_in_us);

/* Turn off Power and Wake-up by system only once */
void dgo_turn_off_sys_oneshot_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us);

/* Turn off Power and Wake-up by system automatically */
void dgo_turn_off_sys_auto_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us);

/* Turn off Power and keep GPR register values then wake-up */
void dgo_gpr_retention_oneshot_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us);

#endif