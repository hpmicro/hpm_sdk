/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <ctype.h>
#include "hpmicro_netx_driver.h"
#include "hpm_common.h"
#include "hpm_enet_drv.h"
#include "hpm_enet_phy.h"
#include "hpm_enet_phy_common.h"
#include "hpm_l1c_drv.h"
#include "hpm_otp_drv.h"
#include "nx_api.h"
#include "nx_link.h"

#ifdef NX_ENABLE_PPPOE
#include "nx_pppoe_server.h"
#endif

/* The following content is helpful for you to choose which operation method to use.
   * USE NETX_DATA_COPY_CPU - NetX packets have allocated space for data. In order to separate the driver layer
                              and the protocol stack more completely, it is common to use the memory copy method
                              to copy data between the protocol stack and driver layer.

   * USE NETX_DATA_DIRECT   - Memory copies lose some performance, which is even more severe on MCUs.Therefore,
                              by passing pointers, you can reduce the copy of memory, thereby improving performance.
                              But this requires careful thought.The main points to think about are:

                              1. For sending, a correspondence between packets and send descriptors needs to be
                                 established. Therefore, when processing the send completion event, the packet can
                                 be released accurately.

                              2. For receiving, the number of descriptor packets must be prepared and their ownership
                                 given to the hardware. At the time of receiving, it is necessary to give the ownership
                                 of the packet to netx, then alloc for a new packet, and give the ownership to the
                                 hardware.

                              3. The receive pool area must be placed in the non-cache area, because cache-invalidate
                                 operations may manipulate areas of memory outside of the data, causing the protocol
                                 stack to fail. This is caused by the nature of the cache operation.

                              4. For applications, because of the receiving pool is shared by the driver and the protocol
                                 stack, the driver will alloc for a packet after receiving the data and give the ownership
                                 to the hardware. Therefore, in the worst cases, if the applications or netx stack hold all
                                 the packets in the pool, the driver will fail to alloc a new packet, causing configuration
                                 of the rx descriptor failure. Of course, this situation is so worst and can be solved by
                                 preparing pools where only hardware has ownership. However, this processing makes the driver
                                 layer more complex.

                             - If you have seriously considered the above points, you can choose this way.*/

/* Define the Link MTU. Note this is not the same as the IP MTU.  The Link MTU
   includes the addition of the Physical Network header (usually Ethernet). This
   should be larger than the IP instance MTU by the size of the physical header. */
#define NX_LINK_MTU 1514
#define NX_MAX_PACKET_SIZE 1536

/* Define Ethernet address format.  This is prepended to the incoming IP
   and ARP/RARP messages.  The frame beginning is 14 bytes, but for speed
   purposes, we are going to assume there are 16 bytes free in front of the
   prepend pointer and that the prepend pointer is 32-bit aligned.

   Byte Offset     Size            Meaning

   0           6           Destination Ethernet Address
   6           6           Source Ethernet Address
   12          2           Ethernet Frame Type, where:

   0x0800 -> IP Datagram
   0x0806 -> ARP Request/Reply
   0x0835 -> RARP request reply

   42          18          Padding on ARP and RARP messages only.  */
#define NX_HPMICRO_INTERFACE_NAME "eth0"
#define NX_ETHERNET_IP 0x0800
#define NX_ETHERNET_ARP 0x0806
#define NX_ETHERNET_RARP 0x8035
#define NX_ETHERNET_IPV6 0x86DD
#define NX_ETHERNET_PPPOE_DISCOVERY 0x8863
#define NX_ETHERNET_PPPOE_SESSION 0x8864
#define NX_ETHERNET_SIZE 14
#define NX_ETHERNET_MAC_SIZE 6

#define NX_INTERFACE_CAPABILITY (NX_INTERFACE_CAPABILITY_IPV4_TX_CHECKSUM |   \
                                 NX_INTERFACE_CAPABILITY_IPV4_RX_CHECKSUM |   \
                                 NX_INTERFACE_CAPABILITY_TCP_TX_CHECKSUM |    \
                                 NX_INTERFACE_CAPABILITY_TCP_RX_CHECKSUM |    \
                                 NX_INTERFACE_CAPABILITY_UDP_TX_CHECKSUM |    \
                                 NX_INTERFACE_CAPABILITY_UDP_RX_CHECKSUM |    \
                                 NX_INTERFACE_CAPABILITY_ICMPV4_TX_CHECKSUM | \
                                 NX_INTERFACE_CAPABILITY_ICMPV4_RX_CHECKSUM | \
                                 NX_INTERFACE_CAPABILITY_ICMPV6_RX_CHECKSUM | \
                                 NX_INTERFACE_CAPABILITY_ICMPV6_TX_CHECKSUM | \
                                 NX_INTERFACE_CAPABILITY_IGMP_TX_CHECKSUM |   \
                                 NX_INTERFACE_CAPABILITY_IGMP_RX_CHECKSUM)

#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)

#define RX_POOL_PACKET_NUM (2 * ENET_RX_BUFF_COUNT)

#if RX_POOL_PACKET_NUM < ENET_RX_BUFF_COUNT
#error "RX_POOL_PACKET_NUM must be larger than ENET_RX_BUFF_COUNT"
#endif

#define NX_RX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * RX_POOL_PACKET_NUM)
NX_PACKET_POOL pool_receive; /* the receive packet pool */
ATTR_PLACE_AT_NONCACHEABLE_BSS ULONG pool_receive_data[NX_RX_PACKET_POOL_SIZE >> 2];

#endif

static const CHAR *nx_hpmicro_interface_name = NX_HPMICRO_INTERFACE_NAME;

ATTR_PLACE_AT_NONCACHEABLE_BSS NX_DRIVER_INFORMATION nx_driver_information;

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT]; /* Ethernet Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT]; /* Ethernet Tx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW UCHAR rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_COPY_CPU)
ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW UCHAR tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */
#endif

UCHAR mac[ENET_MAC];

/* Define driver prototypes.  */
static VOID _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_link_enable(NX_IP_DRIVER *driver_req_ptr);
static VOID _nx_driver_link_disable(NX_IP_DRIVER *driver_req_ptr);

static enet_phy_status_t last_status;
VOID enet_self_adaptive_port_speed(VOID)
{
    enet_phy_status_t status;

    enet_line_speed_t line_speed[] = {enet_line_speed_10mbps, enet_line_speed_100mbps, enet_line_speed_1000mbps};
    CHAR *speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    CHAR *duplex_str[] = {"Half duplex", "Full duplex"};

#if defined(RGMII) && RGMII
#if defined(__USE_DP83867) && __USE_DP83867
    dp83867_get_phy_status(ENET, &status);
#else
    rtl8211_get_phy_status(ENET, &status);
#endif
#else
#if defined(__USE_DP83848) && __USE_DP83848
    dp83848_get_phy_status(ENET, &status);
#else
    rtl8201_get_phy_status(ENET, &status);
#endif
#endif

    if (memcmp(&last_status, &status, sizeof(enet_phy_status_t)) != 0) {
        memcpy(&last_status, &status, sizeof(enet_phy_status_t));
        if (status.enet_phy_link) {
            printf("Link Status: Up\n");
            printf("Link Speed:  %s\n", speed_str[status.enet_phy_speed]);
            printf("Link Duplex: %s\n", duplex_str[status.enet_phy_duplex]);
            enet_set_line_speed(ENET, line_speed[status.enet_phy_speed]);
            enet_set_duplex_mode(ENET, status.enet_phy_duplex);
        } else {
            printf("Link Status: Down\n");
        }
    }
}
VOID sys_timer_callback(VOID)
{
    enet_self_adaptive_port_speed();
}

static VOID _nx_driver_hardware_packet_transmitted(VOID);
static VOID _nx_driver_hardware_packet_receive(VOID);
VOID isr_enet(ENET_Type *ptr)
{
    UINT status;
    UINT rxgbfrmis;
    UINT intr_status;

    status = ptr->DMA_STATUS;
    rxgbfrmis = ptr->MMC_INTR_RX;
    intr_status = ptr->INTR_STATUS;

    if (ENET_DMA_STATUS_GLPII_GET(status)) {
        /* read LPI_CSR to clear interrupt status */
        ptr->LPI_CSR;
    }

    if (ENET_INTR_STATUS_RGSMIIIS_GET(intr_status)) {
        /* read XMII_CSR to clear interrupt status */
        ptr->XMII_CSR;
    }

    if (ENET_DMA_STATUS_RI_GET(status)) {
#if NX_DRIVER_ENABLE_RX_INTERRUPT_DEFERRED_PROCESS
        /* Call NetX deferred driver processing.  */
        nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_RECEIVED;
        _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
#else
        _nx_driver_hardware_packet_receive();
#endif
        ptr->DMA_STATUS |= ENET_DMA_STATUS_RI_MASK;
    }

    if (ENET_DMA_STATUS_TI_GET(status)) {
#if NX_DRIVER_ENABLE_TX_COMPLETE_DEFERRED_PROCESS
        /* Call NetX deferred driver processing.  */
        nx_driver_information.nx_driver_information_deferred_events |= NX_DRIVER_DEFERRED_PACKET_TRANSMITTED;
        _nx_ip_driver_deferred_processing(nx_driver_information.nx_driver_information_ip_ptr);
#else
        _nx_driver_hardware_packet_transmitted();
#endif
        ptr->DMA_STATUS |= ENET_DMA_STATUS_TI_MASK;
    }

    if (ENET_MMC_INTR_RX_RXCTRLFIS_GET(rxgbfrmis)) {
        ptr->RXFRAMECOUNT_GB;
    }
}

#ifdef HPM_ENET0_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET0, isr_enet0)
VOID isr_enet0(VOID)
{
    isr_enet(ENET);
}
#endif

#ifdef HPM_ENET1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET1, isr_enet1)
VOID isr_enet1(VOID)
{
    isr_enet(ENET);
}
#endif

#ifndef NX_ENABLE_VLAN
static VOID _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr);
#endif

#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
ATTR_RAMFUNC
NX_PACKET *_nx_driver_hardware_get_frame(enet_rx_desc_t **parent_rx_desc_list_cur, UINT rx_desc_count, NX_PACKET **hpm_packets)
{
    enet_rx_frame_info_t rx_frame_info = { 0 };
    NX_PACKET *packet_ptr_rx = NULL, *packet_ptr = NULL;
    INT first_idx = 0;
    enet_frame_t frame = {0, 0, 0};
    UINT desc_scan_counter = 0;
    enet_rx_desc_t *rx_desc_list_cur = *parent_rx_desc_list_cur;

    /* scan descriptors owned by CPU */
    while ((rx_desc_list_cur->rdes0_bm.own == 0) &&
           (desc_scan_counter < rx_desc_count)) {

        /* check if first segment in frame */
        if ((rx_desc_list_cur->rdes0_bm.fs == 1) &&
            (rx_desc_list_cur->rdes0_bm.ls == 0)) {
            INT index = (nx_driver_information.current_rx_packet_index + desc_scan_counter) % rx_desc_count;

            rx_frame_info.fs_rx_desc = rx_desc_list_cur;
            rx_frame_info.seg_count = 1;
            rx_desc_list_cur = (enet_rx_desc_t *)(rx_desc_list_cur->rdes3_bm.next_desc);
            *parent_rx_desc_list_cur = rx_desc_list_cur;
            /* This BD is not the last BD of a frame. It is a intermediate descriptor.  */
            first_idx = index;

            hpm_packets[index]->nx_packet_next = hpm_packets[(index + 1) % rx_desc_count];

            hpm_packets[index]->nx_packet_append_ptr = hpm_packets[index]->nx_packet_data_end;
            nx_driver_information.current_rx_packet_index++;
            nx_driver_information.current_rx_packet_index = nx_driver_information.current_rx_packet_index % rx_desc_count;

        }

        /* check if intermediate segment */
        else if ((rx_desc_list_cur->rdes0_bm.ls == 0) &&
                 (rx_desc_list_cur->rdes0_bm.fs == 0)) {
            INT index = (nx_driver_information.current_rx_packet_index + desc_scan_counter) % rx_desc_count;

            rx_frame_info.seg_count++;
            rx_desc_list_cur = (enet_rx_desc_t *)(rx_desc_list_cur->rdes3_bm.next_desc);
            *parent_rx_desc_list_cur = rx_desc_list_cur;
            /* This BD is not the last BD of a frame. It is a intermediate descriptor.  */

            hpm_packets[index]->nx_packet_next = hpm_packets[(index + 1) % rx_desc_count];

            hpm_packets[index]->nx_packet_append_ptr = hpm_packets[index]->nx_packet_data_end;
            nx_driver_information.current_rx_packet_index++;
            nx_driver_information.current_rx_packet_index = nx_driver_information.current_rx_packet_index % rx_desc_count;

        }

        /* should be last segment */
        else {
            /* last segment */
            rx_frame_info.ls_rx_desc = rx_desc_list_cur;

            rx_frame_info.seg_count++;

            INT index = (nx_driver_information.current_rx_packet_index + desc_scan_counter) % rx_desc_count;
            /* first segment is last segment */
            if (rx_frame_info.seg_count == 1) {
                rx_frame_info.fs_rx_desc = rx_desc_list_cur;
                first_idx = index;
            }

            /* get the frame length of the received packet: substruct 4 bytes of the crc */
            frame.length = rx_desc_list_cur->rdes0_bm.fl - 4;

            /* get the address of the buffer start address */
            /* check if more than one segment in the frame */
            if (rx_frame_info.seg_count > 1) {
                frame.buffer = rx_frame_info.fs_rx_desc->rdes2_bm.buffer1;
            } else {
                frame.buffer = rx_desc_list_cur->rdes2_bm.buffer1;
            }

            frame.rx_desc = rx_frame_info.fs_rx_desc;

            rx_desc_list_cur = (enet_rx_desc_t *)(rx_desc_list_cur->rdes3_bm.next_desc);
            *parent_rx_desc_list_cur = rx_desc_list_cur;

            /* check if hardware using its own only buffers */
            if (hpm_packets[index] == (NX_PACKET *)(-1)) {
                if (nx_packet_allocate(nx_driver_information.nx_driver_information_packet_pool_ptr, &packet_ptr,
                        NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS) {
                    memcpy(packet_ptr->nx_packet_data_start, (VOID *)frame.buffer, frame.length);
                    hpm_packets[index] = packet_ptr;
                }

            }

            if (hpm_packets[index] != (NX_PACKET *)(-1)) {
                /* Yes, this BD is the last BD in the frame, set the last NX_PACKET's nx_packet_next to NULL.  */
                hpm_packets[index]->nx_packet_next = NX_NULL;

                /* Store the length of the packet in the first NX_PACKET.  */

                hpm_packets[first_idx]->nx_packet_length = (frame.length);

                /* Adjust nx_packet_append_ptr with the size of the data in this buffer.  */
                hpm_packets[index]->nx_packet_append_ptr = hpm_packets[index]->nx_packet_prepend_ptr + hpm_packets[first_idx]->nx_packet_length - index * nx_driver_information.nx_driver_information_rx_buffer_size;
                packet_ptr_rx = hpm_packets[first_idx];
            }

            /* Allocate new NX_PACKETs for BDs.  */
            for (UINT i = first_idx; i <= desc_scan_counter + first_idx; i++) {

                /* Allocate a new packet from the packet pool.  */
                if (nx_packet_allocate(nx_driver_information.nx_driver_information_packet_pool_ptr, &packet_ptr,
                        NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS) {

                    /* Adjust the new packet and assign it to the BD.  */
                    enet_dma_rx_desc_set_ch_buffer(ENET, &nx_driver_information.desc, i % rx_desc_count, packet_ptr->nx_packet_data_start, packet_ptr->nx_packet_data_end - packet_ptr->nx_packet_data_start);
                    enet_dma_rx_desc_give_to_hardware(ENET, &nx_driver_information.desc, i % rx_desc_count);
                    hpm_packets[i % rx_desc_count] = packet_ptr;
                } else {
                    /* Allocation failed, get out of the loop.  */
                    enet_dma_rx_desc_set_ch_buffer(ENET, &nx_driver_information.desc, i % rx_desc_count, (VOID *)rx_buff[i % rx_desc_count], ENET_RX_BUFF_SIZE);
                    enet_dma_rx_desc_give_to_hardware(ENET, &nx_driver_information.desc, i % rx_desc_count);
                    hpm_packets[i % rx_desc_count] = (NX_PACKET *)(-1);
                    break;
                }
            }
            nx_driver_information.current_rx_packet_index++;
            nx_driver_information.current_rx_packet_index = nx_driver_information.current_rx_packet_index % rx_desc_count;

            return packet_ptr_rx;
        }
        desc_scan_counter++;
    }

    return NULL;
}
#else
ATTR_RAMFUNC
NX_PACKET *_nx_driver_hardware_get_frame(enet_rx_desc_t **parent_rx_desc_list_cur, UINT rx_desc_count)
{
    enet_frame_t frame = {0, 0, 0};
    NX_PACKET *packet_ptr_rx = NULL;
    (void)parent_rx_desc_list_cur;
    (void)rx_desc_count;
    frame = enet_get_received_frame_interrupt(&nx_driver_information.desc.rx_desc_list_cur, &nx_driver_information.desc.rx_frame_info, ENET_RX_BUFF_COUNT);
    if (frame.length > 0) {
        if (nx_packet_allocate(nx_driver_information.nx_driver_information_packet_pool_ptr, &packet_ptr_rx,
                NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS) {
            memcpy((VOID *)packet_ptr_rx->nx_packet_data_start, (VOID *)frame.buffer, frame.length);
        }
    }
    /* Release descriptors to DMA */
    enet_rx_desc_t *dma_rx_desc = nx_driver_information.desc.rx_frame_info.fs_rx_desc;

    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    for (UINT i = 0; i < nx_driver_information.desc.rx_frame_info.seg_count; i++) {
        dma_rx_desc->rdes0_bm.own = 1;
        dma_rx_desc = (enet_rx_desc_t *)(dma_rx_desc->rdes3_bm.next_desc);
    }
    /* Clear Segment_Count */
    nx_driver_information.desc.rx_frame_info.seg_count = 0;
    return packet_ptr_rx;
}
#endif

static UINT _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr)
{
    enet_tx_control_config_t enet_tx_control_config;
#if defined(RGMII) && RGMII
#if defined(__USE_DP83867) && __USE_DP83867
    dp83867_config_t phy_config;
#else
    rtl8211_config_t phy_config;
#endif
#else
#if defined(__USE_DP83848) && __USE_DP83848
    dp83848_config_t phy_config;
#else
    rtl8201_config_t phy_config;
#endif
#endif

    /* Default to successful return.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;

    /* Setup indices.  */
    nx_driver_information.nx_driver_information_receive_current_index = 0;
    nx_driver_information.nx_driver_information_transmit_current_index = 0;
    nx_driver_information.nx_driver_information_transmit_release_index = 0;

    /* Clear the number of buffers in use counter.  */
    nx_driver_information.nx_driver_information_number_of_transmit_buffers_in_use = 0;

    /* Make sure there are receive packets... otherwise, return an error.  */
    if (nx_driver_information.nx_driver_information_packet_pool_ptr == NULL) {

        /* There must be receive packets. If not, return an error!  */
        return (NX_DRIVER_ERROR);
    }

    /* Initialize td, rd and the corresponding buffers */
    memset((UCHAR *)&nx_driver_information.desc.tx_buff_cfg, 0x00, sizeof(nx_driver_information.desc.tx_buff_cfg));
    memset((UCHAR *)&nx_driver_information.desc.rx_buff_cfg, 0x00, sizeof(nx_driver_information.desc.rx_buff_cfg));
    memset((UCHAR *)rx_buff, 0x00, sizeof(rx_buff));

#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_COPY_CPU)
    memset((UCHAR *)tx_buff, 0x00, sizeof(tx_buff));
#endif
    nx_driver_information.desc.tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (UINT)dma_tx_desc_tab);
    nx_driver_information.desc.rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (UINT)dma_rx_desc_tab);

#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
    NX_PACKET *packet_ptr;
    for (UINT i = 0; i < ENET_RX_BUFF_COUNT; i++) {
        /* Allocate a packet for the receive buffers.  */
        if (nx_packet_allocate(&pool_receive, &packet_ptr,
                NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS) {
            nx_driver_information.desc.rx_buff_cfg[i].buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, ((UINT)(packet_ptr->nx_packet_prepend_ptr + 3) & (0xfffffffc)));
            nx_driver_information.desc.rx_buff_cfg[i].size = ENET_RX_BUFF_SIZE;
            nx_driver_information.nx_driver_information_receive_packets[i] = packet_ptr;
        } else {

            /* Cannot allocate packets from the packet pool. */
            return (NX_DRIVER_ERROR);
        }
    }
    nx_driver_information.current_rx_packet_index = 0;
#else
    nx_driver_information.desc.rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (UINT)rx_buff);
    nx_driver_information.desc.rx_buff_cfg.count = ENET_RX_BUFF_COUNT;
    nx_driver_information.desc.rx_buff_cfg.size = ENET_RX_BUFF_SIZE;
#endif
#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_COPY_CPU)
    nx_driver_information.desc.tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (UINT)tx_buff);
#endif
    nx_driver_information.desc.tx_buff_cfg.count = ENET_TX_BUFF_COUNT;
    nx_driver_information.desc.tx_buff_cfg.size = ENET_TX_BUFF_SIZE;

    /*Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(ENET, &enet_tx_control_config);
    enet_tx_control_config.enable_ioc = true;
    /* Set the control config for tx descriptor */
    memcpy(&nx_driver_information.desc.tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Get MAC address */
    _hardware_get_mac_address(mac);

/* Initialize phy */
#if defined(RGMII) && RGMII
#if defined(__USE_DP83867) && __USE_DP83867
    dp83867_reset(ENET);
#if __DISABLE_AUTO_NEGO
    dp83867_set_mdi_crossover_mode(ENET, enet_phy_mdi_crossover_manual_mdix);
#endif
    dp83867_basic_mode_default_config(ENET, &phy_config);
    if (dp83867_basic_mode_init(ENET, &phy_config) == true) {
#else
    rtl8211_reset(ENET);
    rtl8211_basic_mode_default_config(ENET, &phy_config);
    if (rtl8211_basic_mode_init(ENET, &phy_config) == true) {
#endif
#else
#if defined(__USE_DP83848) && __USE_DP83848
    dp83848_reset(ENET);
    dp83848_basic_mode_default_config(ENET, &phy_config);
    if (dp83848_basic_mode_init(ENET, &phy_config) == true) {
#else
    rtl8201_reset(ENET);
    rtl8201_basic_mode_default_config(ENET, &phy_config);
    if (rtl8201_basic_mode_init(ENET, &phy_config) == true) {
#endif
#endif
        printf("Enet phy init passed !\n");
        return status_success;
    } else {
        printf("Enet phy init failed !\n");
        return status_fail;
    }
    /* Return success!  */
    return (NX_SUCCESS);
}

ATTR_RAMFUNC
static UINT _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr)
{

    TX_INTERRUPT_SAVE_AREA
    NX_PACKET *pktIdx;
#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_COPY_CPU)
    UCHAR *buffer;
#endif
    __IO enet_tx_desc_t *dma_tx_desc;
    USHORT frame_length = 0;
    UINT bytes_left_to_copy = 0;
    enet_tx_desc_t *tx_desc_list_cur = nx_driver_information.desc.tx_desc_list_cur;

    dma_tx_desc = tx_desc_list_cur;

#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_COPY_CPU)
    buffer = (UCHAR *)(dma_tx_desc->tdes2_bm.buffer1);
#endif

    for (pktIdx = packet_ptr; pktIdx != NULL; pktIdx = pktIdx->nx_packet_next) {

        bytes_left_to_copy = pktIdx->nx_packet_length;

        if (dma_tx_desc->tdes0_bm.own != 0) {
            return NX_DRIVER_ERROR;
        }

        /* Copy the remaining bytes */
#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_COPY_CPU)
        memcpy((UCHAR *)((UCHAR *)buffer), (UCHAR *)((UCHAR *)pktIdx->nx_packet_prepend_ptr), bytes_left_to_copy);
#else
        UINT aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((UINT)pktIdx->nx_packet_prepend_ptr);
        UINT aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((UINT)pktIdx->nx_packet_prepend_ptr + bytes_left_to_copy);
        UINT aligned_size = aligned_end - aligned_start;
        l1c_dc_writeback(aligned_start, aligned_size);
        UINT tx_des_index = 0;
        UINT i = 0;
        for (i = 0; i < ENET_TX_BUFF_COUNT; i++) {
            enet_tx_desc_t *desc = nx_driver_information.desc.tx_desc_list_head + i;
            if (nx_driver_information.desc.tx_desc_list_cur == desc) {
                tx_des_index = i;
                break;
            }
        }
        if (i >= ENET_TX_BUFF_COUNT) {
            printf("tx current desc pointer is error\n");
        }
        nx_driver_information.nx_driver_information_transmit_packets[tx_des_index] = pktIdx;
        nx_driver_information.desc.tx_desc_list_cur->tdes2_bm.buffer1 = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (UINT)pktIdx->nx_packet_prepend_ptr);

#endif
        frame_length = frame_length + bytes_left_to_copy;
    }
    /* Prepare transmit descriptors to give to DMA*/
    frame_length += 4;

    /* Disable interrupts.  */
    TX_DISABLE
    enet_prepare_tx_desc(ENET, &nx_driver_information.desc.tx_desc_list_cur, &nx_driver_information.desc.tx_control_config, frame_length, nx_driver_information.desc.tx_buff_cfg.size);
    TX_RESTORE
    return (NX_SUCCESS);
}

ATTR_RAMFUNC
static VOID _nx_driver_hardware_packet_receive(VOID)
{
    NX_PACKET *packet_ptr_rx;

    /* Loop to get all data received by the hardware */
    while (1) {
        enet_rx_desc_t **rx_desc_list_cur = &nx_driver_information.desc.rx_desc_list_cur;

        /* Try to get the enet data from hardware */
#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
        packet_ptr_rx = _nx_driver_hardware_get_frame(rx_desc_list_cur, ENET_RX_BUFF_COUNT, nx_driver_information.nx_driver_information_receive_packets);
#else
        packet_ptr_rx = _nx_driver_hardware_get_frame(rx_desc_list_cur, ENET_RX_BUFF_COUNT);
#endif
        if (packet_ptr_rx == NULL) {
            break;
        }

#ifdef NX_ENABLE_VLAN
    nx_link_ethernet_packet_received(nx_driver_information.nx_driver_information_ip_ptr,
                                     nx_driver_information.nx_driver_information_interface->nx_interface_index,
                                     packet_ptr_rx, NX_NULL);
#else
    _nx_driver_transfer_to_netx(nx_driver_information.nx_driver_information_ip_ptr, packet_ptr_rx);
#endif
    }
}

ATTR_RAMFUNC
static VOID _nx_driver_hardware_packet_transmitted(VOID)
{
#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
    /* Release the transmitted packets */
    for (UINT i = 0; i < ENET_TX_BUFF_COUNT; i++) {
        enet_tx_desc_t *desc = nx_driver_information.desc.tx_desc_list_head + i;
        if (desc->tdes0_bm.own == 0) {
            NX_PACKET *packet = nx_driver_information.nx_driver_information_transmit_packets[i];
            if (packet != NULL) {
                NX_DRIVER_ETHERNET_HEADER_REMOVE(packet);
                nx_packet_transmit_release(packet);
                nx_driver_information.nx_driver_information_transmit_packets[i] = NULL;
            }
        }
    }
#else
    /* Nothing need to do because we copy the packet data, and packet can be release immediately */
#endif
}

static UINT _nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr)
{
    NX_PARAMETER_NOT_USED(driver_req_ptr);

    enet_phy_status_t status;
    enet_int_config_t int_config = {.int_enable = 0, .int_mask = 0};
    enet_mac_config_t enet_config;

    /* Set MAC0 address */
    enet_config.mac_addr_high[0] = mac[5] << 8 | mac[4];
    enet_config.mac_addr_low[0] = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    enet_config.valid_max_count = 1;

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(ENET);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    /* Enable Enet IRQ */
    board_enable_enet_irq(ENET);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ENET, &int_config);

    /* Set the interrupt enable mask */
#if NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT
    int_config.int_enable = enet_normal_int_sum_en | enet_receive_int_en | enet_transmit_int_en;
#else
    int_config.int_enable = enet_normal_int_sum_en | enet_receive_int_en;
#endif

    /* Initialize enet controller */

    enet_controller_netx_init(ENET, ENET_INF_TYPE, &nx_driver_information.desc, &enet_config, &int_config);

    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(ENET);

    while (1) {
#if defined(RGMII) && RGMII
#if defined(__USE_DP83867) && __USE_DP83867
        dp83867_get_phy_status(ENET, &status);
#else
        rtl8211_get_phy_status(ENET, &status);
#endif
#else
#if defined(__USE_DP83848) && __USE_DP83848
        dp83848_get_phy_status(ENET, &status);
#else
        rtl8201_get_phy_status(ENET, &status);
#endif
#endif
        if (status.enet_phy_link)
            break;
    }

    /* Return success!  */
    return (NX_SUCCESS);
}

static UINT _nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr)
{
    NX_PARAMETER_NOT_USED(driver_req_ptr);
    /* Disable Enet IRQ */
    board_disable_enet_irq(ENET);

    /* Return success!  */
    return (NX_SUCCESS);
}

#ifndef NX_ENABLE_VLAN
ATTR_RAMFUNC
static VOID _nx_driver_transfer_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr)
{
    USHORT packet_type;

    packet_ptr->nx_packet_ip_interface = nx_driver_information.nx_driver_information_interface;

    /* Pickup the packet header to determine where the packet needs to be
       sent.  */
    packet_type = (USHORT)(((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 12))) << 8) | ((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 13)));

    /* Route the incoming packet according to its ethernet type.  */
    if (packet_type == NX_DRIVER_ETHERNET_IP || packet_type == NX_DRIVER_ETHERNET_IPV6) {
        /* Note:  The length reported by some Ethernet hardware includes
           bytes after the packet as well as the Ethernet header.  In some
           cases, the actual packet length after the Ethernet header should
           be derived from the length in the IP header (lower 16 bits of
           the first 32-bit word).  */

        /* Clean off the Ethernet header.  */
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Adjust the packet length.  */
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Route to the ip receive function.  */
#ifdef NX_DRIVER_ENABLE_DEFERRED
        _nx_ip_packet_deferred_receive(ip_ptr, packet_ptr);
#else
        _nx_ip_packet_receive(ip_ptr, packet_ptr);
#endif
    } else if (packet_type == NX_DRIVER_ETHERNET_ARP) {
        /* Clean off the Ethernet header.  */
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Adjust the packet length.  */
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Route to the ARP receive function.  */
        _nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
    } else if (packet_type == NX_DRIVER_ETHERNET_RARP) {
        /* Clean off the Ethernet header.  */
        packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Adjust the packet length.  */
        packet_ptr->nx_packet_length = packet_ptr->nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Route to the RARP receive function.  */
        _nx_rarp_packet_deferred_receive(ip_ptr, packet_ptr);
    } else {
        /* Invalid ethernet header... release the packet.  */
        nx_packet_release(packet_ptr);
    }
}
#endif

VOID _nx_driver_set_interface_name(const CHAR *interface_name)
{
    nx_hpmicro_interface_name = interface_name;
}

UINT _nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr)
{
    NX_INTERFACE *interface_ptr;
    UINT status;

#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_receive, "NetX Rx Packet Pool", (1536 + sizeof(NX_PACKET)), (VOID *)pool_receive_data, sizeof(pool_receive_data));
#endif

    /* Setup interface pointer.  */
    interface_ptr = driver_req_ptr->nx_ip_driver_interface;

    /* Default IP pointer to NULL.  */
    nx_driver_information.nx_driver_information_ip_ptr = NX_NULL;

    /* Setup the driver state to not initialized.  */
    nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_NOT_INITIALIZED;

    /* Setup the default packet pool for the driver's received packets.  */
#if (NETX_RX_DATA_COPY_ALGORITHM == NETX_DATA_DIRECT)
    nx_driver_information.nx_driver_information_packet_pool_ptr = &pool_receive;
#else
    nx_driver_information.nx_driver_information_packet_pool_ptr = driver_req_ptr->nx_ip_driver_ptr->nx_ip_default_packet_pool;
#endif
    /* Clear the deferred events for the driver.  */
    nx_driver_information.nx_driver_information_deferred_events = 0;

    /* Call the hardware-specific ethernet controller initialization.  */
    status = _nx_driver_hardware_initialize(driver_req_ptr);

    /* Determine if the request was successful.  */
    if (status == NX_SUCCESS) {

        /* Setup driver information to point to IP pointer.  */
        nx_driver_information.nx_driver_information_ip_ptr = driver_req_ptr->nx_ip_driver_ptr;

        /* Setup the link maximum transfer unit. */
        interface_ptr->nx_interface_ip_mtu_size = NX_DRIVER_ETHERNET_MTU - NX_DRIVER_ETHERNET_FRAME_SIZE;

        /* Setup the physical address of this IP instance.  Increment the
           physical address lsw to simulate multiple nodes hanging on the
           ethernet.  */
        interface_ptr->nx_interface_physical_address_msw = (ULONG)((mac[0] << 8) | (mac[1]));
        interface_ptr->nx_interface_physical_address_lsw = (ULONG)((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | (mac[5]));

        /* Indicate to the IP software that IP to physical mapping
           is required.  */
        interface_ptr->nx_interface_address_mapping_needed = NX_TRUE;

        /* Move the driver's state to initialized.  */
        nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;

        /* Indicate successful initialize.  */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    } else {

        /* Initialization failed.  Indicate that the request failed.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
    return (NX_SUCCESS);
}

ATTR_RAMFUNC
static VOID _nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr)
{
    NX_PACKET *packet_ptr;
    ULONG *ethernet_frame_ptr;
    UINT status;
    USHORT ether_type;

    /* Check to make sure the link is up.  */
    if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED) {

        /* Indicate an unsuccessful packet send.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;

        /* Link is not up, simply free the packet.  */
        nx_packet_transmit_release(driver_req_ptr->nx_ip_driver_packet);
        return;
    }

    /* Process driver send packet.  */

    /* Place the ethernet frame at the front of the packet.  */
    packet_ptr = driver_req_ptr->nx_ip_driver_packet;

    /* Get Ethernet type.  */
    if (driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_SEND) {
        ether_type = NX_ETHERNET_ARP;
    } else if (driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND) {
        ether_type = NX_ETHERNET_ARP;
    } else if (driver_req_ptr -> nx_ip_driver_command == NX_LINK_RARP_SEND) {
        ether_type = NX_ETHERNET_RARP;
    } else if (packet_ptr -> nx_packet_ip_version == 4) {
        ether_type = NX_ETHERNET_IP;
    } else {
        ether_type = NX_ETHERNET_IPV6;
    }

#ifdef NX_ENABLE_VLAN
    /* Add Ethernet header.  */
    if (nx_link_ethernet_header_add((NX_IP *)driver_req_ptr->nx_ip_driver_ptr,
                                    driver_req_ptr -> nx_ip_driver_interface -> nx_interface_index, packet_ptr,
                                    driver_req_ptr -> nx_ip_driver_physical_address_msw,
                                    driver_req_ptr -> nx_ip_driver_physical_address_lsw,
                                    (UINT)ether_type)) {

        /* Release the packet.  */
        nx_packet_transmit_release(packet_ptr);
        return;
    }
#else
    /* Adjust the prepend pointer.  */
    packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr - NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Adjust the packet length.  */
    packet_ptr->nx_packet_length = packet_ptr->nx_packet_length + NX_DRIVER_ETHERNET_FRAME_SIZE;

    /* Setup the ethernet frame pointer to build the ethernet frame.  Backup another 2
     * bytes to get 32-bit word alignment.  */
    ethernet_frame_ptr = (ULONG *)(packet_ptr->nx_packet_prepend_ptr - 2);

    /* Set up the hardware addresses in the Ethernet header. */
    *ethernet_frame_ptr = driver_req_ptr->nx_ip_driver_physical_address_msw;

    *(ethernet_frame_ptr + 1) = driver_req_ptr->nx_ip_driver_physical_address_lsw;

    *(ethernet_frame_ptr + 2) = (driver_req_ptr->nx_ip_driver_interface->nx_interface_physical_address_msw << 16) | (driver_req_ptr->nx_ip_driver_interface->nx_interface_physical_address_lsw >> 16);

    *(ethernet_frame_ptr + 3) = (driver_req_ptr->nx_ip_driver_interface->nx_interface_physical_address_lsw << 16) | ether_type;

    /* Endian swapping if NX_LITTLE_ENDIAN is defined.  */
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 1));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 2));
    NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 3));
#endif /* NX_ENABLE_VLAN */

    /* Determine if the packet exceeds the driver's MTU.  */
    if (packet_ptr->nx_packet_length > NX_DRIVER_ETHERNET_MTU) {

        /* This packet exceeds the size of the driver's MTU. Simply throw it away! */

        /* Remove the Ethernet header.  */
        NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);

        /* Indicate an unsuccessful packet send.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;

        /* Link is not up, simply free the packet.  */
        nx_packet_transmit_release(packet_ptr);
        return;
    }

    /* Transmit the packet through the Ethernet controller low level access routine. */
    status = _nx_driver_hardware_packet_send(packet_ptr);

    /* Determine if there was an error.  */
    if (status != NX_SUCCESS) {

        /* Driver's hardware send packet routine failed to send the packet.  */

        /* Indicate an unsuccessful packet send.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;

        /* Link is not up, simply free the packet.  */
        NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);
        nx_packet_transmit_release(packet_ptr);
    } else {

#if (NETX_TX_DATA_COPY_ALGORITHM == NETX_DATA_COPY_CPU)

        NX_DRIVER_ETHERNET_HEADER_REMOVE(packet_ptr);
        nx_packet_transmit_release(packet_ptr);

#endif
        /* Set the status of the request.  */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    }
}

static VOID _nx_driver_link_enable(NX_IP_DRIVER *driver_req_ptr)
{
    UINT status;

    /* See if we can honor the NX_LINK_ENABLE request.  */
    if (nx_driver_information.nx_driver_information_state < NX_DRIVER_STATE_INITIALIZED) {

        /* Mark the request as not successful.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    /* Check if it is enabled by someone already */
    if (nx_driver_information.nx_driver_information_state >= NX_DRIVER_STATE_LINK_ENABLED) {

        /* Yes, the request has already been made.  */
        driver_req_ptr->nx_ip_driver_status = NX_ALREADY_ENABLED;
        return;
    }

    /* Call hardware specific enable.  */
    status = _nx_driver_hardware_enable(driver_req_ptr);

    /* Was the hardware enable successful?  */
    if (status == NX_SUCCESS) {

        /* Update the driver state to link enabled.  */
        nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_LINK_ENABLED;

        /* Mark request as successful.  */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;

        /* Mark the IP instance as link up.  */
        driver_req_ptr->nx_ip_driver_interface->nx_interface_link_up = NX_TRUE;
    } else {

        /* Enable failed.  Indicate that the request failed.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

static VOID _nx_driver_link_disable(NX_IP_DRIVER *driver_req_ptr)
{
    NX_IP *ip_ptr;
    UINT status;

    /* Setup the IP pointer from the driver request.  */
    ip_ptr = driver_req_ptr->nx_ip_driver_ptr;

    /* Check if the link is enabled.  */
    if (nx_driver_information.nx_driver_information_state != NX_DRIVER_STATE_LINK_ENABLED) {

        /* The link is not enabled, so just return an error.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
        return;
    }

    /* Call hardware specific disable.  */
    status = _nx_driver_hardware_disable(driver_req_ptr);

    /* Was the hardware disable successful?  */
    if (status == NX_SUCCESS) {

        /* Mark the IP instance as link down.  */
        ip_ptr->nx_ip_driver_link_up = NX_FALSE;

        /* Update the driver state back to initialized.  */
        nx_driver_information.nx_driver_information_state = NX_DRIVER_STATE_INITIALIZED;

        /* Mark request as successful.  */
        driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
    } else {

        /* Disable failed, return an error.  */
        driver_req_ptr->nx_ip_driver_status = NX_DRIVER_ERROR;
    }
}

ATTR_RAMFUNC
static VOID _nx_driver_deferred_processing(NX_IP_DRIVER *driver_req_ptr)
{

    TX_INTERRUPT_SAVE_AREA

    ULONG deferred_events;

    /* Disable interrupts.  */
    TX_DISABLE

    /* Pickup deferred events.  */
    deferred_events = nx_driver_information.nx_driver_information_deferred_events;
    nx_driver_information.nx_driver_information_deferred_events = 0;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Check for received packet.  */
    if (deferred_events & NX_DRIVER_DEFERRED_PACKET_RECEIVED) {
        /* Process received packet(s).  */
        _nx_driver_hardware_packet_receive();
    }

    /* Check for a transmit complete event.  */
    if (deferred_events & NX_DRIVER_DEFERRED_PACKET_TRANSMITTED) {

        /* Process transmitted packet(s).  */
        _nx_driver_hardware_packet_transmitted();
    }

    /* Mark request as successful.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;
}

/* This function deal with all the netx hardware operation requests */
ATTR_RAMFUNC
VOID _nx_driver_hpm(NX_IP_DRIVER *driver_req_ptr)
{
    NX_IP *ip_ptr;
    NX_INTERFACE *interface_ptr;
    UINT interface_index;

    /* Setup the IP pointer from the driver request.  */
    ip_ptr = driver_req_ptr->nx_ip_driver_ptr;

    /* Default to successful return.  */
    driver_req_ptr->nx_ip_driver_status = NX_SUCCESS;

#ifdef NX_ENABLE_VLAN
    /* Let link layer to preprocess the driver request and return actual interface.  */
    if (nx_link_driver_request_preprocess(driver_req_ptr, &interface_ptr) != NX_SUCCESS) {
        return;
    }
#else
    /* Setup interface pointer.  */
    interface_ptr = driver_req_ptr->nx_ip_driver_interface;
#endif /* NX_ENABLE_VLAN */

    /* Obtain the index number of the network interface. */
    interface_index = interface_ptr->nx_interface_index;

    /* Process according to the driver request type in the IP control
       block.  */
    switch (driver_req_ptr->nx_ip_driver_command) {
        case NX_LINK_INTERFACE_ATTACH: {
            nx_driver_information.nx_driver_information_interface = (NX_INTERFACE *)(driver_req_ptr->nx_ip_driver_interface);
            break;
        }

        case NX_LINK_INITIALIZE: {
            _nx_driver_initialize(driver_req_ptr);

#ifdef NX_ENABLE_INTERFACE_CAPABILITY
            nx_ip_interface_capability_set(ip_ptr, interface_index, NX_INTERFACE_CAPABILITY);
#endif /* NX_ENABLE_INTERFACE_CAPABILITY */
            break;
        }

        case NX_LINK_ENABLE: {
            _nx_driver_link_enable(driver_req_ptr);
            break;
        }

        case NX_LINK_DISABLE: {
            _nx_driver_link_disable(driver_req_ptr);
            break;
        }

        case NX_LINK_PACKET_SEND:
        case NX_LINK_PACKET_BROADCAST:
        case NX_LINK_ARP_SEND:
        case NX_LINK_ARP_RESPONSE_SEND:
        case NX_LINK_RARP_SEND:
#ifdef NX_ENABLE_PPPOE
        case NX_LINK_PPPOE_DISCOVERY_SEND:
        case NX_LINK_PPPOE_SESSION_SEND:
#endif
        {
            /* Process packet send requests.  */
            _nx_driver_packet_send(driver_req_ptr);
            break;
        }

#ifdef NX_ENABLE_VLAN
        case NX_LINK_RAW_PACKET_SEND: {

            /* Send raw packet out directly.  */
            _nx_driver_hardware_packet_send(driver_req_ptr -> nx_ip_driver_packet);
            break;
        }
#endif /* NX_ENABLE_VLAN */
        case NX_LINK_MULTICAST_JOIN: {

            /* The IP layer issues this command to join a multicast group.  Note that
            multicast operation is required for IPv6.

            On a typically Ethernet controller, the driver computes a hash value based
            on MAC address, and programs the hash table.

            It is likely the driver also needs to maintain an internal MAC address table.
            Later if a multicast address is removed, the driver needs
            to reprogram the hash table based on the remaining multicast MAC addresses. */

            break;
        }

        case NX_LINK_MULTICAST_LEAVE: {

            /* The IP layer issues this command to remove a multicast MAC address from the
            receiving list.  A device driver shall properly remove the multicast address
            from the hash table, so the hardware does not receive such traffic.  Note that
            in order to reprogram the hash table, the device driver may have to keep track of
            current active multicast MAC addresses. */

            /* The following procedure only applies to our linux network driver, which manages
            multicast MAC addresses by a simple look up table. */

            break;
        }

        case NX_LINK_GET_STATUS: {

            /* Return the link status in the supplied return pointer.  */
            *(driver_req_ptr->nx_ip_driver_return_ptr) = ip_ptr->nx_ip_interface[0].nx_interface_link_up;
            break;
        }

        case NX_LINK_DEFERRED_PROCESSING: {

            _nx_driver_deferred_processing(driver_req_ptr);

            break;
        }

        default: {

            /* Invalid driver request.  */
            /* Return the unhandled command status.  */
            driver_req_ptr->nx_ip_driver_status = NX_UNHANDLED_COMMAND;
        }
    }
}

UINT nx_ipv4addr_aton(const CHAR *cp, UINT *addr)
{
    UINT val;
    CHAR c;
    UINT parts[4];
    UINT *pp = parts;

    c = *cp;
    for (;;) {
        /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, 1-9=decimal.
     */
        if (!((UCHAR)(c) >= '0' && (UCHAR)(c) <= '9')) {
            return 0;
        }
        val = 0;
        for (;;) {
            if ((UCHAR)(c) >= '0' && (UCHAR)(c) <= '9') {
                val = (val * 10) + (UINT)(c - '0');
                c = *++cp;
            } else {
                break;
            }
        }
        if (c == '.') {
            /*
       * Internet format:
       *  a.b.c.d
       *  a.b.c   (with c treated as 16 bits)
       *  a.b (with b treated as 24 bits)
       */
            if (pp >= parts + 3) {
                return 0;
            }
            *pp++ = val;
            c = *++cp;
        } else {
            break;
        }
    }
    /*
   * Check for trailing characters.
   */
    if (c != '\0' && !isspace(c)) {
        return 0;
    }
    /*
   * Concoct the address according to
   * the number of parts specified.
   */
    if ((pp - parts + 1) == 4) {
        if (val > 0xff) {
            return 0;
        }
        if ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff)) {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
    } else {
        return 0;
    }
    *addr = val;
    return 1;
}