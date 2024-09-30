/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_TSW_PHY_COMMON_H
#define HPM_TSW_PHY_COMMON_H

#if defined(__USE_RTL8211) && __USE_RTL8211
    #include "hpm_rtl8211.h"
    #include "hpm_rtl8211_regs.h"
#endif

#if defined(__USE_JL1111) && __USE_JL1111
    #include "hpm_jl1111.h"
    #include "hpm_jl1111_regs.h"
#endif

#endif /* HPM_TSW_PHY_H */
