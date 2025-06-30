/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/err.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "ethernetif.h"

#include "board.h"
#include "lwip.h"
#include "netconf.h"
#include "hpm_enet_drv.h"

#if !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#include "rtthread.h"
#endif
#endif

#define netifMTU                           (1500)
#define netifINTERFACE_TASK_STACK_SIZE     (1024)

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#define netifINTERFACE_TASK_PRIORITY       (configMAX_PRIORITIES - 1)
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#define netifINTERFACE_TASK_PRIORITY       (RT_THREAD_PRIORITY_MAX - 1)
#endif

#define netifGUARD_BLOCK_TIME              (250)

/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT   ((portTickType)100)

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

static struct netif *s_pxNetIf = NULL;
#if !NO_SYS
xSemaphoreHandle s_xSemaphore = NULL;
#endif

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
    /* Set netif MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* Set netif MAC hardware address */
    memcpy(netif->hwaddr, mac, ETH_HWADDR_LEN);

    /* Set netif maximum transfer unit */
    netif->mtu = 1500;

    /* Set the default link status */
    netif->flags |= NETIF_FLAG_LINK_UP;

    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

    s_pxNetIf = netif;

#if !NO_SYS
    /* create binary semaphore used for informing ethernetif of frame reception */
    if (s_xSemaphore == NULL) {
        vSemaphoreCreateBinary(s_xSemaphore);
        xSemaphoreTake(s_xSemaphore, 0);
    }
    /* create the task that handles the ETH_MAC */
    xTaskCreate(ethernetif_input, "Eth_if", netifINTERFACE_TASK_STACK_SIZE, NULL,
                netifINTERFACE_TASK_PRIORITY, NULL);
#endif
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    (void)netif;

#if !NO_SYS
    static xSemaphoreHandle xTxSemaphore = NULL;
#endif
    struct pbuf *q;
    uint8_t *buffer;

    __IO enet_tx_desc_t *dma_tx_desc;
    uint16_t frame_length = 0;
    uint32_t buffer_offset = 0;
    uint32_t bytes_left_to_copy = 0;
    uint32_t payload_offset = 0;
    enet_tx_desc_t  *tx_desc_list_cur = desc.tx_desc_list_cur;

#if defined(LWIP_PTP) && LWIP_PTP
    enet_ptp_ts_system_t timestamp;
#endif

#if !NO_SYS
    if (xTxSemaphore == NULL) {
        vSemaphoreCreateBinary(xTxSemaphore);
    }

    if (xSemaphoreTake(xTxSemaphore, netifGUARD_BLOCK_TIME)) {
#endif
        dma_tx_desc = tx_desc_list_cur;
        buffer = (uint8_t *)(dma_tx_desc->tdes2_bm.buffer1);
        buffer_offset = 0;

        for (q = p; q != NULL; q = q->next) {
            /* Get bytes in current lwIP buffer  */
            bytes_left_to_copy = q->len;
            payload_offset = 0;


            if (dma_tx_desc->tdes0_bm.own != 0) {
                return ERR_INPROGRESS;
            }

            /* Check if the length of data to copy is bigger than Tx buffer size*/
            while ((bytes_left_to_copy + buffer_offset) > ENET_TX_BUFF_SIZE) {
                /* Copy data to Tx buffer*/
                memcpy((uint8_t *)((uint8_t *)buffer + buffer_offset),
                    (uint8_t *)((uint8_t *)q->payload + payload_offset),
                            ENET_TX_BUFF_SIZE - buffer_offset);

                /* Point to next descriptor */
                dma_tx_desc = (enet_tx_desc_t *)(dma_tx_desc->tdes3_bm.next_desc);

                /* Check if the buffer is available */
                if (dma_tx_desc->tdes0_bm.own != 0) {
                    return ERR_INPROGRESS;
                }

                buffer = (uint8_t *)(dma_tx_desc->tdes2_bm.buffer1);

                bytes_left_to_copy = bytes_left_to_copy - (ENET_TX_BUFF_SIZE - buffer_offset);
                payload_offset = payload_offset + (ENET_TX_BUFF_SIZE - buffer_offset);
                frame_length = frame_length + (ENET_TX_BUFF_SIZE - buffer_offset);
                buffer_offset = 0;
            }

            /* pass payload to buffer */
            desc.tx_desc_list_cur->tdes2_bm.buffer1 = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)q->payload);
            buffer_offset = buffer_offset + bytes_left_to_copy;
            frame_length = frame_length + bytes_left_to_copy;
        }
        /* Prepare transmit descriptors to give to DMA*/
        frame_length += 4;

        #if defined(LWIP_PTP) && LWIP_PTP
            enet_prepare_tx_desc_with_ts_record(ENET, &desc.tx_desc_list_cur, &desc.tx_control_config, frame_length, desc.tx_buff_cfg.size, &timestamp);
            /* Get the transmit timestamp */
            p->time_sec  = timestamp.sec;
            p->time_nsec = timestamp.nsec;
        #else
            enet_prepare_tx_desc(ENET, &desc.tx_desc_list_cur, &desc.tx_control_config, frame_length, desc.tx_buff_cfg.size);
        #endif

#if !NO_SYS
        /* Give semaphore and exit */
        xSemaphoreGive(xTxSemaphore);
    }
#endif

    return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
    (void)netif;

    struct pbuf *p = NULL, *q;
    u32_t len;
    uint8_t *buffer;
    enet_frame_t frame = {0, 0, 0};
    enet_rx_desc_t *dma_rx_desc;
    uint32_t buffer_offset = 0;
    uint32_t payload_offset = 0;
    uint32_t bytes_left_to_copy = 0;
    uint32_t i = 0;

    /* Check and get a received frame */
    #if (defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT) || !NO_SYS
        frame = enet_get_received_frame_interrupt(&desc.rx_desc_list_cur, &desc.rx_frame_info, ENET_RX_BUFF_COUNT);
    #else
        if (enet_check_received_frame(&desc.rx_desc_list_cur, &desc.rx_frame_info) == 1) {
            frame = enet_get_received_frame(&desc.rx_desc_list_cur, &desc.rx_frame_info);
        }
    #endif

    /* Obtain the size of the packet and put it into the "len" variable. */
    len = frame.length;
    buffer = (uint8_t *)frame.buffer;

    if (len > 0) {
    /* Allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

        if (p != NULL) {
            dma_rx_desc = frame.rx_desc;
            buffer_offset = 0;
            for (q = p; q != NULL; q = q->next) {
                bytes_left_to_copy = q->len;
                payload_offset = 0;

                /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
                while ((bytes_left_to_copy + buffer_offset) > ENET_RX_BUFF_SIZE) {
                    /* Copy data to pbuf*/
                    memcpy((uint8_t *)((uint8_t *)q->payload + payload_offset), (uint8_t *)((uint8_t *)buffer + buffer_offset), (ENET_RX_BUFF_SIZE - buffer_offset));

                    /* Point to next descriptor */
                    dma_rx_desc = (enet_rx_desc_t *)(dma_rx_desc->rdes3_bm.next_desc);
                    buffer = (uint8_t *)(dma_rx_desc->rdes2_bm.buffer1);

                    bytes_left_to_copy = bytes_left_to_copy - (ENET_RX_BUFF_SIZE - buffer_offset);
                    payload_offset = payload_offset + (ENET_RX_BUFF_SIZE - buffer_offset);
                    buffer_offset = 0;
                }

            /* pass the buffer to pbuf */
            q->payload = (void *)buffer;
            buffer_offset = buffer_offset + bytes_left_to_copy;

            #if defined(LWIP_PTP) && LWIP_PTP
            /* Get the received timestamp */
            p->time_sec  = frame.rx_desc->rdes7_bm.rtsh;
            p->time_nsec = frame.rx_desc->rdes6_bm.rtsl;
            #endif
            }
        }

        /* Release descriptors to DMA */
        dma_rx_desc = frame.rx_desc;

        /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
        for (i = 0; i < desc.rx_frame_info.seg_count; i++) {
            dma_rx_desc->rdes0_bm.own = 1;
            dma_rx_desc = (enet_rx_desc_t *)(dma_rx_desc->rdes3_bm.next_desc);
        }

        /* Clear Segment_Count */
        desc.rx_frame_info.seg_count = 0;
    }

    /* Resume Rx Process */
    enet_rx_resume(ENET);

    return p;
}


/**
 * This function is the ethernetif_input task, it is processed when a packet
 * is ready to be read from the interface. It uses the function low_level_input()
 * that should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */

/*
 invoked after receiving data packet
 */
#if !NO_SYS
void ethernetif_input(void *pvParameters)
{
    (void)pvParameters;

    struct pbuf *p;

    for ( ;; ) {
        if (xSemaphoreTake(s_xSemaphore, emacBLOCK_TIME_WAITING_FOR_INPUT) == pdTRUE) {
GET_NEXT_FRAME:
            p = low_level_input(s_pxNetIf);
            if (p != NULL) {
                if (ERR_OK != s_pxNetIf->input(p, s_pxNetIf)) {
                    pbuf_free(p);
                } else {
                    goto GET_NEXT_FRAME;
                }
            }
        }
    }
}
#else
err_t ethernetif_input(struct netif *netif)
{
    err_t err = ERR_OK;
    struct pbuf *p = NULL;
#if (defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT)
    if (rx_flag) {
#endif
        GET_NEXT_FRAME:
        /* move received packet into a new pbuf */
        p = low_level_input(netif);

        /* no packet could be read, silently ignore this */
        if (p == NULL) {
            err = ERR_MEM;
        } else {
             /* entry point to the LwIP stack */
            err = netif->input(p, netif);

            if (err != ERR_OK) {
                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
                pbuf_free(p);
            } else {
                goto GET_NEXT_FRAME;
            }
        }
#if (defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT)
        rx_flag = false;
    }
#endif
    return err;
}
#endif

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    etharp_init();

    return ERR_OK;
}