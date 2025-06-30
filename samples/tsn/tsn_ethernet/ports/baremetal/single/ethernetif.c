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
 * Copyright (c) 2024-2025 HPMicro
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
#include "lwip.h"

#if defined(NO_SYS) && !NO_SYS
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

#if defined(NO_SYS) && !NO_SYS
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

    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

#if defined(NO_SYS) && !NO_SYS
    /* create binary semaphore used for informing ethernetif of frame reception */
    if (s_xSemaphore == NULL) {
        vSemaphoreCreateBinary(s_xSemaphore);
        xSemaphoreTake(s_xSemaphore, 0);
    }
    /* create the task that handles the ETH_MAC */
    xTaskCreate(ethernetif_input, "Eth_if", netifINTERFACE_TASK_STACK_SIZE, netif,
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

    hpm_stat_t stat;

#if defined(NO_SYS) && !NO_SYS
    static xSemaphoreHandle xTxSemaphore = NULL;
#endif

    struct pbuf *q;
    static uint32_t i = 0;
    uint32_t id;
    uint32_t length = TSW_SOC_SWITCH_HEADER_LEN;

#if defined(LWIP_PTP) && LWIP_PTP
    enet_ptp_ts_system_t timestamp;
#endif

#if defined(NO_SYS) && !NO_SYS
    if (xTxSemaphore == NULL) {
        vSemaphoreCreateBinary(xTxSemaphore);
    }

    if (xSemaphoreTake(xTxSemaphore, netifGUARD_BLOCK_TIME)) {
#endif

    id = i++ % TSW_SOC_DMA_MAX_DESC_COUNT;

    for (q = p; q != NULL; q = q->next) {
        memcpy(&send_buff[id][TSW_SOC_SWITCH_HEADER_LEN], q->payload, q->len);
        length += q->len;
    }

    stat = tsw_send_frame(BOARD_TSW, send_buff[id], length, id);

#if defined(NO_SYS) && !NO_SYS
        /* Give semaphore and exit */
        xSemaphoreGive(xTxSemaphore);
    }
#endif

    return (stat == status_success) ? 0 : -1;
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
    (void) netif;

    struct pbuf *p = NULL, *q;

    hpm_stat_t stat;
    tsw_frame_t frame;

    stat = tsw_recv_frame(BOARD_TSW, &frame);

    if (stat == status_success) {
        if ((frame.length > TSW_SOC_SWITCH_HEADER_LEN)) {
            /* Allocate a pbuf chain of pbufs from the Lwip buffer pool */
            p = pbuf_alloc(PBUF_RAW, frame.length - TSW_SOC_SWITCH_HEADER_LEN, PBUF_POOL);

            if (p != NULL) {
                for (q = p; q != NULL; q = q->next) {
                    /* pass the buffer to pbuf */
                    frame.buffer = recv_buff[frame.id];
                    memcpy(q->payload, &frame.buffer[TSW_SOC_SWITCH_HEADER_LEN], q->len);
                    tsw_commit_recv_desc(BOARD_TSW, recv_buff[frame.id], TSW_RECV_BUFF_LEN, frame.id);
                }
            }
        }
    } else if (stat == status_fail) {
        tsw_commit_recv_desc(BOARD_TSW, recv_buff[frame.id], TSW_RECV_BUFF_LEN, frame.id);
    } else {

    }

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
#if defined(NO_SYS) && !NO_SYS
void ethernetif_input(void *pvParameters)
{
    struct netif *netif = (struct netif *)pvParameters;

    struct pbuf *p;

    for ( ;; ) {
        if (xSemaphoreTake(s_xSemaphore, emacBLOCK_TIME_WAITING_FOR_INPUT) == pdTRUE) {
GET_NEXT_FRAME:
            p = low_level_input(netif);
            if (p != NULL) {
                if (ERR_OK != netif->input(p, netif)) {
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

#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
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
#if defined(ENABLE_TSW_RECEIVE_INTERRUPT) && ENABLE_TSW_RECEIVE_INTERRUPT
        rx_flag = false;
        intc_m_enable_irq(IRQn_TSW_0);  /* Enable TSW CPU Port IRQ */
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