/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _HPMICRO_NETX_CONF_H_
#define _HPMICRO_NETX_CONF_H_

#ifndef ENET_TX_BUFF_COUNT
#define ENET_TX_BUFF_COUNT (30U)
#endif

#ifndef ENET_RX_BUFF_COUNT
#define ENET_RX_BUFF_COUNT (30U)
#endif
#define ENET_TX_BUFF_SIZE ENET_MAX_BUFF_SIZE
#define ENET_RX_BUFF_SIZE ENET_MAX_BUFF_SIZE

#define NETX_DATA_COPY_CPU 0
#define NETX_DATA_DIRECT 1

#ifndef NETX_RX_DATA_COPY_ALGORITHM
#define NETX_RX_DATA_COPY_ALGORITHM NETX_DATA_COPY_CPU
#endif

#ifndef NETX_TX_DATA_COPY_ALGORITHM
#define NETX_TX_DATA_COPY_ALGORITHM NETX_DATA_COPY_CPU
#endif

/* NX_DRIVER_ENABLE_RX_INTERRUPT_DEFERRED_PROCESS
   - 0: Net packets will be processed in the isr context
        which can affect real-time performance
   - Not zero: Net packets will be delayed for processing into
        the ip thread
*/
#ifndef NX_DRIVER_ENABLE_RX_INTERRUPT_DEFERRED_PROCESS
#define NX_DRIVER_ENABLE_RX_INTERRUPT_DEFERRED_PROCESS   1
#endif

/* NX_DRIVER_ENABLE_TX_COMPLETE_DEFERRED_PROCESS
   - 0: Net packets which already transmited will be mark
        as TX_DONE immediately
   - Not zero: Net packets will be delayed release until
        the ip thread deal with the event
*/
#ifndef NX_DRIVER_ENABLE_TX_COMPLETE_DEFERRED_PROCESS
#define NX_DRIVER_ENABLE_TX_COMPLETE_DEFERRED_PROCESS    1
#endif

#endif