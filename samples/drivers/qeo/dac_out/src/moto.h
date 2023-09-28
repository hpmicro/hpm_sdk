/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_MOTO_H
#define HPM_MOTO_H

#ifdef __cplusplus
extern "C" {
#endif

void mmc_open_loop_pred(void);

void init_dac(DAC_Type *ptr);

#ifdef __cplusplus
}
#endif

#endif /* HPM_MOTO_H */
