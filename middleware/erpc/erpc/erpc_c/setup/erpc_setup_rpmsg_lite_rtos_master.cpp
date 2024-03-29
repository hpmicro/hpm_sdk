/*
 * Copyright (c) 2014-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * Copyright (c) 2022 HPMicro
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "erpc_manually_constructed.hpp"
#include "erpc_rpmsg_lite_rtos_transport.hpp"
#include "erpc_transport_setup.h"
#include "hpm_common.h"

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

#if !defined(SH_MEM_TOTAL_SIZE)
#define SH_MEM_TOTAL_SIZE (6144U)
#endif

ATTR_SHARE_MEM char rpmsg_lite_base[SH_MEM_TOTAL_SIZE];
ERPC_MANUALLY_CONSTRUCTED(RPMsgRTOSTransport, s_transport);

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

erpc_transport_t erpc_transport_rpmsg_lite_rtos_master_init(uint32_t src_addr, uint32_t dst_addr,
                                                            uint32_t rpmsg_link_id)
{
    erpc_transport_t transport;

    s_transport.construct();
    if (s_transport->init(src_addr, dst_addr, rpmsg_lite_base, SH_MEM_TOTAL_SIZE, rpmsg_link_id) == kErpcStatus_Success)
    {
        transport = reinterpret_cast<erpc_transport_t>(s_transport.get());
    }
    else
    {
        transport = NULL;
    }

    return transport;
}
