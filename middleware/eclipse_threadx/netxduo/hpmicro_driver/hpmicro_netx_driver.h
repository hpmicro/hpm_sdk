/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _HPMICRO_NETX_DRIVER_H_
#define _HPMICRO_NETX_DRIVER_H_

#include "board.h"
#include "hpm_enet_drv.h"
#include "hpmicro_netx_conf.h"
#include "hpmicro_enet_adapter.h"
#include "nx_api.h"
/* Define generic constants and macros for all NetX Ethernet drivers.  */

#define NX_DRIVER_ETHERNET_IP 0x0800
#define NX_DRIVER_ETHERNET_IPV6 0x86dd
#define NX_DRIVER_ETHERNET_ARP 0x0806
#define NX_DRIVER_ETHERNET_RARP 0x8035

#define NX_DRIVER_ETHERNET_MTU 1514
#define NX_DRIVER_ETHERNET_FRAME_SIZE 14
#define NX_DRIVER_PHYSICAL_ADDRESS_SIZE 6

#define NX_DRIVER_DEFERRED_PACKET_RECEIVED 1
#define NX_DRIVER_DEFERRED_DEVICE_RESET 2
#define NX_DRIVER_DEFERRED_PACKET_TRANSMITTED 4

#define NX_DRIVER_STATE_NOT_INITIALIZED 1
#define NX_DRIVER_STATE_INITIALIZE_FAILED 2
#define NX_DRIVER_STATE_INITIALIZED 3
#define NX_DRIVER_STATE_LINK_ENABLED 4
#define NX_DRIVER_ERROR 90

/* Exported Macros------------------------------------------------------------*/
#if defined(RGMII) && RGMII
#define ENET_INF_TYPE enet_inf_rgmii
#define ENET BOARD_ENET_RGMII
#else
#define ENET_INF_TYPE enet_inf_rmii
#define ENET BOARD_ENET_RMII
#endif

/* Exported Variables ------------------------------------------------------*/
extern uint8_t mac[];

/* Exported Macros------------------------------------------------------------*/

#define NX_DRIVER_ETHERNET_HEADER_REMOVE(p)                                                  \
    {                                                                                        \
        p->nx_packet_prepend_ptr = p->nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE; \
        p->nx_packet_length = p->nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;           \
    }

typedef struct NX_DRIVER_INFORMATION_STRUCT {
    /* NetX IP instance that this driver is attached to.  */
    NX_IP *nx_driver_information_ip_ptr;

    /* Driver's current state.  */
    ULONG nx_driver_information_state;

    /* Packet pool used for receiving packets. */
    NX_PACKET_POOL *nx_driver_information_packet_pool_ptr;

    /* Define the driver interface association.  */
    NX_INTERFACE *nx_driver_information_interface;

    /* Define the deferred event field. This will contain bits representing events
       deferred from the ISR for processing in the thread context.  */
    ULONG nx_driver_information_deferred_events;

    /****** DRIVER SPECIFIC ****** Start of part/vendor specific driver information area.  Include any such constants here!  */

    /* Indices to current receive/transmit descriptors.  */
    UINT nx_driver_information_receive_current_index;
    UINT nx_driver_information_transmit_current_index;

    /* Transmit release index.  */
    UINT nx_driver_information_transmit_release_index;

    /* Define the number of transmit buffers in use.  */
    UINT nx_driver_information_number_of_transmit_buffers_in_use;

    /* Define the Ethernet RX & TX DMA Descriptors.  */

    enet_desc_t_netx desc;

    enet_rx_desc_t *nx_driver_information_dma_rx_descriptors;
    enet_tx_desc_t *nx_driver_information_dma_tx_descriptors;

#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
    /* Define the association between buffer descriptors and NetX packets.  */
    NX_PACKET *nx_driver_information_transmit_packets[ENET_TX_BUFF_COUNT];
#endif

#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
    NX_PACKET *nx_driver_information_receive_packets[ENET_RX_BUFF_COUNT];
#endif
    /* Define the size of a rx buffer size.  */
    ULONG nx_driver_information_rx_buffer_size;

    UCHAR nx_driver_information_multicast_count[64];

    UINT nx_driver_information_link_speed;
    UINT nx_driver_information_link_duplex;

    UINT current_rx_packet_index;

    /****** DRIVER SPECIFIC ****** End of part/vendor specific driver information area.  */

} NX_DRIVER_INFORMATION;

VOID sys_timer_callback(VOID);
VOID _nx_driver_hpm(NX_IP_DRIVER *driver_req_ptr);
UINT nx_ipv4addr_aton(const CHAR *cp, UINT *addr);
#endif /* _HPMICRO_NETX_DRIVER_H_ */