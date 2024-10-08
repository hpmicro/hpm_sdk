/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_JL1111_H
#define HPM_JL1111_H

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_tsw_phy.h"
#include "hpm_common.h"
#include "hpm_tsw_regs.h"
/*---------------------------------------------------------------------
 *  Macro Const Definitions
 *---------------------------------------------------------------------
 */
#ifndef JL1111_ADDR
#define JL1111_ADDR (0U)
#endif

#define JL1111_ID1  (0x937CU)
#define JL1111_ID2  (0x10U)

/*---------------------------------------------------------------------
 *  Typedef Struct Declarations
 *---------------------------------------------------------------------
 */
typedef struct {
    bool loopback;
    uint8_t speed;
    bool auto_negotiation;
    uint8_t duplex;
} jl1111_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/*---------------------------------------------------------------------
 * Exported Functions
 *---------------------------------------------------------------------
 */
void jl1111_reset(TSW_Type *ptr, uint8_t port);
void jl1111_basic_mode_default_config(TSW_Type *ptr, jl1111_config_t *config);
bool jl1111_basic_mode_init(TSW_Type *ptr, uint8_t port, jl1111_config_t *config);
void jl1111_get_phy_status(TSW_Type *ptr, uint8_t port, tsw_phy_status_t *status);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* HPM_JL1111_H */
