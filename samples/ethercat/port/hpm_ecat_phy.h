/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_ECAT_PHY_H
#define HPM_ECAT_PHY_H

#include "ecat_config.h"
#include "hpm_esc_drv.h"

#define JL11X1_PAGESEL_REG_OFFSET    (31)
#define JL11X1_WOLEN_REG_OFFSET      (0xF3) /* PHY LED mode configuration register address */
#define JL11X1_WOLEN_REG_VALUE       (0)
#define JL11X1_RESET_HOLD_TIME_MS    (1)
#define JL11X1_RESET_WAIT_TIME_MS    (5)
#define JL11X1_RESET_LEVEL           (0) /* low level reset */

#ifdef __cplusplus
extern "C" {
#endif


void ecat_phy_reset(void);
void ecat_phy_keep_reset(void);
hpm_stat_t ecat_phy_config(ESC_Type *ptr);

#ifdef __cplusplus
}
#endif


#endif /* HPM_ECAT_PHY_H */
