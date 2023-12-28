/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _RTT_PORT_H
#define _RTT_PORT_H
#include "hpm_common.h"
#include "hpm_soc.h"

/* mchtimer section */
#define RT_MCHTMR_FREQ_IN_HZ (24000000UL)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void rtt_base_init(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _RTT_PORT_H */
