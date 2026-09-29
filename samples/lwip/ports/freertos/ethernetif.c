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
 * Copyright (c) 2021-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "common.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/err.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include <stdio.h>
#include <string.h>

#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#include "rtthread.h"
#endif
#endif

#ifndef netifMTU
#define netifMTU                           (1500)
#endif
#define netifINTERFACE_TASK_STACK_SIZE     (1024)

/*
 * ETH RX task priority (FreeRTOS): larger value means higher priority.
 * Keep one step below configMAX_PRIORITIES so the driver stays above lwIP worker tasks.
 */
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
xSemaphoreHandle s_xSemaphore[LWIP_NETIF_COUNT];
static xSemaphoreHandle s_xTxSemaphore = NULL;
#endif

LWIP_MEMPOOL_DECLARE(enet0_rx_pool, ENET_RX_BUFF_COUNT, sizeof(my_custom_pbuf_t), "Custom RX PBUF pool");
#if LWIP_NETIF_COUNT > 1
LWIP_MEMPOOL_DECLARE(enet1_rx_pool, ENET_RX_BUFF_COUNT, sizeof(my_custom_pbuf_t), "Custom RX PBUF pool");
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
    char task_name[30] = {0};

    /* Set netif MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* Set netif MAC hardware address */
    memcpy(netif->hwaddr, mac[netif->num], ETH_HWADDR_LEN);

    /* Set netif maximum transfer unit */
    netif->mtu = netifMTU;

    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

    if (netif->num == 0) {
        LWIP_MEMPOOL_INIT(enet0_rx_pool);
#if LWIP_NETIF_COUNT > 1
    } else if (netif->num == 1) {
        LWIP_MEMPOOL_INIT(enet1_rx_pool);
#endif
    }

#if defined(NO_SYS) && !NO_SYS
    /* create binary semaphore used for informing ethernetif of frame reception */
    if (netif->num < LWIP_NETIF_COUNT) {
        if (s_xSemaphore[netif->num] == NULL) {
            vSemaphoreCreateBinary(s_xSemaphore[netif->num]);
            xSemaphoreTake(s_xSemaphore[netif->num], 0);
        }
        /* create the task that handles the ETH_MAC */
        sprintf(task_name, "Eth_if%d", netif->num);
        xTaskCreate(ethernetif_input, task_name, netifINTERFACE_TASK_STACK_SIZE, netif,
                    netifINTERFACE_TASK_PRIORITY, NULL);
    }
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
    enet_netif_state_t *fp;
#if defined(NO_SYS) && !NO_SYS
    err_t err;
#endif

    if (netif == NULL || p == NULL) {
        return ERR_VAL;
    }

    fp = enet_netif_get_state(netif);
    if ((fp == NULL) || (fp->desc == NULL) || (fp->base == NULL)) {
        return ERR_VAL;
    }

#if defined(NO_SYS) && !NO_SYS
    if (s_xTxSemaphore == NULL) {
        vSemaphoreCreateBinary(s_xTxSemaphore);
    }

    if (xSemaphoreTake(s_xTxSemaphore, netifGUARD_BLOCK_TIME) != pdTRUE) {
        return ERR_TIMEOUT;
    }
    err = enet_lwip_output(fp->base, fp->desc, p);
    xSemaphoreGive(s_xTxSemaphore);
    return err;
#else
    return enet_lwip_output(fp->base, fp->desc, p);
#endif
}

void free_rx_dma_descriptor(void *p)
{
    enet_frame_t *frame;

    frame = (enet_frame_t *)p;

    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    enet_rx_desc_t *dma_rx_desc = frame->rx_desc;

    for (uint32_t i = 0; i < frame->seg; i++) {
        enet_lwip_dc_invalidate((uint32_t)dma_rx_desc->rdes2_bm.buffer1, ENET_RX_BUFF_SIZE);
        dma_rx_desc->rdes0_bm.own = 1;
        dma_rx_desc = (enet_rx_desc_t *)(dma_rx_desc->rdes3_bm.next_desc);
    }

    /* Clear Segment_Count */
    frame->seg = 0;
    frame->used = 0;
    frame->length = 0;
    frame->ls_rx_desc = NULL;
}

void enet0_pbuf_free_custom(struct pbuf *p)
{
    SYS_ARCH_DECL_PROTECT(old_level);
    my_custom_pbuf_t *my_pbuf = (my_custom_pbuf_t *)p;

    SYS_ARCH_PROTECT(old_level);
    if (my_pbuf->dma_descriptor != NULL) {
        free_rx_dma_descriptor((void *)my_pbuf->dma_descriptor);
    }
    LWIP_MEMPOOL_FREE(enet0_rx_pool, my_pbuf);
    SYS_ARCH_UNPROTECT(old_level);
}

#if LWIP_NETIF_COUNT > 1
void enet1_pbuf_free_custom(struct pbuf *p)
{
    SYS_ARCH_DECL_PROTECT(old_level);
    my_custom_pbuf_t *my_pbuf = (my_custom_pbuf_t *)p;

    SYS_ARCH_PROTECT(old_level);
    if (my_pbuf->dma_descriptor != NULL) {
        free_rx_dma_descriptor((void *)my_pbuf->dma_descriptor);
    }
    LWIP_MEMPOOL_FREE(enet1_rx_pool, my_pbuf);
    SYS_ARCH_UNPROTECT(old_level);
}
#endif

static void *enet0_rx_custom_alloc(void)
{
    return LWIP_MEMPOOL_ALLOC(enet0_rx_pool);
}

#if LWIP_NETIF_COUNT > 1
static void *enet1_rx_custom_alloc(void)
{
    return LWIP_MEMPOOL_ALLOC(enet1_rx_pool);
}
#endif

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
    struct pbuf *p = NULL;
    uint32_t len;
    enet_netif_state_t *fp = enet_netif_get_state(netif);
    enet_desc_t *desc;
    enet_lwip_rx_custom_alloc_fn alloc_fn;
    enet_lwip_rx_custom_free_fn free_fn;
#if defined(LWIP_PTP) && LWIP_PTP
    enet_ptp_ts_system_t rx_ts;
#endif

    if ((fp == NULL) || (fp->desc == NULL) || (fp->base == NULL)) {
        return NULL;
    }
    desc = fp->desc;

    if (fp->frame[fp->idx].used == 0) {
        #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
        fp->frame[fp->idx] = enet_get_received_frame_interrupt(&desc->rx_desc_list_cur, &desc->rx_frame_info, ENET_RX_BUFF_COUNT);
        #else
        if (enet_check_received_frame(&desc->rx_desc_list_cur, &desc->rx_frame_info) == 1) {
            fp->frame[fp->idx] = enet_get_received_frame(&desc->rx_desc_list_cur, &desc->rx_frame_info);
        }
        #endif

        /* Obtain the size of the packet and put it into the "len" variable. */
        len = fp->frame[fp->idx].length;

        if (len > 0) {
            fp->frame[fp->idx].used = 1;
            alloc_fn = enet0_rx_custom_alloc;
            free_fn = enet0_pbuf_free_custom;
#if LWIP_NETIF_COUNT > 1
            if (netif->num == 1) {
                alloc_fn = enet1_rx_custom_alloc;
                free_fn = enet1_pbuf_free_custom;
            }
#endif
            p = enet_lwip_input(&fp->frame[fp->idx], ENET_RX_BUFF_SIZE, alloc_fn, free_fn);
            if (p == NULL) {
                /* head not built: descriptors still held; mid-fail already returned them via pbuf_free */
                if (fp->frame[fp->idx].used != 0) {
                    free_rx_dma_descriptor((void *)&fp->frame[fp->idx]);
                }
            } else {
                #if defined(LWIP_PTP) && LWIP_PTP
                /* RX timestamp is valid only on last descriptor (RDES6/RDES7) */
                if (enet_get_rx_timestamp(fp->frame[fp->idx].ls_rx_desc, &rx_ts) == status_success) {
                    p->time_sec = rx_ts.sec;
                    p->time_nsec = rx_ts.nsec;
                }
                #endif
                ++fp->idx;
                fp->idx %= ENET_RX_BUFF_COUNT;
            }

            /* Clear Segment_Count */
            desc->rx_frame_info.seg_count = 0;
        }
    }

    /* Resume Rx Process */
    enet_rx_resume(fp->base);

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
    enet_netif_state_t *fp = enet_netif_get_state(netif);
    struct pbuf *p;

    for ( ;; ) {
        if ((fp != NULL) && (fp->desc != NULL)) {
            enet_lwip_tx_release(fp->desc);
        }
        if (xSemaphoreTake(s_xSemaphore[netif->num], emacBLOCK_TIME_WAITING_FOR_INPUT) == pdTRUE) {
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
    enet_netif_state_t *fp = enet_netif_get_state(netif);
#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    if (rx_flag[netif->num]) {
#endif
        if ((fp != NULL) && (fp->desc != NULL)) {
            enet_lwip_tx_release(fp->desc);
        }
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
#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
        rx_flag[netif->num] = false;
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
#if LWIP_NETIF_COUNT > 1
    netif->name[1] = netif->num + '0';
#else
    netif->name[1] = IFNAME1;
#endif

    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    etharp_init();

    return ERR_OK;
}
