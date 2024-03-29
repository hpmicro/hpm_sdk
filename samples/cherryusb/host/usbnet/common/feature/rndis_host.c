/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

#include <stdio.h>
#include "board.h"
#include "netif/etharp.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "usbh_core.h"
#include "usbh_rndis.h"
#include "rndis_protocol.h"

/* Macro Definition */
/*--------------- Tasks Priority -------------*/
#define RNDIS_RECV_DATA_TASK_PRIO (tskIDLE_PRIORITY + 6)

/* eth rx size must be a multiple of 512 or 64 */
#define CONFIG_USBHOST_RNDIS_ETH_MAX_RX_SIZE    (2048)
#define CONFIG_USBHOST_RNDIS_ETH_MAX_TX_SIZE    (2048)

/* Static Variable Definition*/
static struct usbh_rndis *s_rndis_class_ptr;
static struct netif netif_data;
static ip4_addr_t ipaddr;
static ip4_addr_t netmask;
static ip4_addr_t gateway;
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t tx_buffer[CONFIG_USBHOST_RNDIS_ETH_MAX_TX_SIZE];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t rx_buffer[CONFIG_USBHOST_RNDIS_ETH_MAX_RX_SIZE];
static uint8_t *rx_buf_ptr;
SemaphoreHandle_t sig_sem_handle;
TimerHandle_t timer_handle;
TaskHandle_t data_recv_task_handle;

/* Static Function Declaration */
static void rndis_dev_keepalive_timeout(void *pdata);
static err_t netif_init_cb(struct netif *netif);
static err_t linkoutput_fn(struct netif *netif, struct pbuf *p);
static int usbh_rndis_eth_tx(struct pbuf *p);
static int rndis_msg_data_send(struct usbh_rndis *rndis_class, uint8_t *buffer, int nbytes);
static void lwip_rx_handle(void *pdata);

extern int usbh_rndis_query_msg_transfer(struct usbh_rndis *rndis_class,
                                         uint32_t oid, uint32_t query_len,
                                         uint8_t *info, uint32_t *info_len);

void usbh_rndis_run(struct usbh_rndis *rndis_class)
{
    struct netif *netif = &netif_data;

    s_rndis_class_ptr = rndis_class;
    rx_buf_ptr        = rx_buffer;

    netif->hwaddr_len = 6;
    memcpy(netif->hwaddr, rndis_class->mac, 6);

    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gateway, 0, 0, 0, 0);

    netif = netif_add(netif, &ipaddr, &netmask, &gateway, NULL, netif_init_cb, tcpip_input);
    netif_set_default(netif);
    netif_set_up(netif);

    sig_sem_handle = xSemaphoreCreateCounting(1, 0);
    if (NULL == sig_sem_handle) {
        printf("Rndis sig semaphore creat faile!\r\n");
        for (;;) {
            ;
        }
    }

    timer_handle =
        xTimerCreate((const char *)NULL, (TickType_t)5000, (UBaseType_t)pdTRUE, (void *const)1, (TimerCallbackFunction_t)rndis_dev_keepalive_timeout);
    if (NULL != timer_handle) {
        xTimerStart(timer_handle, 0);
    } else {
        printf("timer creation failed!.\n");
        for (;;) {
            ;
        }
    }

    if (xTaskCreate(lwip_rx_handle, "rndis_lwip_rx", configMINIMAL_STACK_SIZE + 2048U, rndis_class, RNDIS_RECV_DATA_TASK_PRIO, &data_recv_task_handle) !=
        pdPASS) {
        printf("rndis_lwip_rx Task creation failed!.\n");
        for (;;) {
            ;
        }
    }
}

void usbh_rndis_stop(struct usbh_rndis *rndis_class)
{
    (void)rndis_class;
    vTaskDelete(data_recv_task_handle);

    xTimerStop(timer_handle, 0);
    xTimerDelete(timer_handle, 0);

    vSemaphoreDelete(sig_sem_handle);

    netif_remove(&netif_data);

    printf("rndis dev stop!\n");
}

static void rndis_dev_keepalive_timeout(void *pdata)
{
    (void)pdata;
    int ret;

    ret = usbh_rndis_keepalive(s_rndis_class_ptr);
    if (ret < 0) {
        printf("rndis dev keepalive timeout!\n");
    }
}

static void lwip_rx_handle(void *pdata)
{
    err_t err;
    int rx_length = 0;
    struct pbuf *p;
    rndis_data_packet_t *pmsg;
    int pmg_offset;
    int payload_offset;
    rndis_data_packet_t rndis_buf_tmp;

    struct usbh_rndis *rndis_class = (struct usbh_rndis *)pdata;
    rx_buf_ptr = rx_buffer;

    while (1) {
        pmg_offset = 0;
        rx_length = usbh_rndis_bulk_in_transfer(rndis_class, rx_buf_ptr, CONFIG_USBHOST_RNDIS_ETH_MAX_RX_SIZE, USB_OSAL_WAITING_FOREVER);
        if (rx_length <= 0) {
            vTaskDelay(1);
            continue;
        }
        while (rx_length > 0) {
            pmsg = (rndis_data_packet_t *)(rx_buf_ptr + pmg_offset);
            if ((pmg_offset & 0x3) != 0) {
                memcpy(&rndis_buf_tmp, pmsg, sizeof(rndis_data_packet_t));
                pmsg = &rndis_buf_tmp;
            }
            if (pmsg->MessageType == REMOTE_NDIS_PACKET_MSG) {
                /* allocate buffer */
                p = pbuf_alloc(PBUF_RAW, pmsg->DataLength, PBUF_POOL);
                if (p != NULL) {
                    struct pbuf *q;
                    payload_offset = 0;
                    for (q = p; q != NULL; q = q->next) {
                        /* Copy the received frame into buffer from memory pointed by the current ETHERNET DMA Rx descriptor */
                        memcpy(q->payload, ((uint8_t *)(&pmsg->DataOffset) + pmsg->DataOffset + payload_offset), q->len);
                        payload_offset += q->len;
                    }
                    /* entry point to the LwIP stack */
                    err = netif_data.input(p, &netif_data);
                    if (err != ERR_OK) {
                        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
                        pbuf_free(p);
                    }
                    pmg_offset += pmsg->MessageLength;
                    rx_length -= pmsg->MessageLength;
                }
            } else {
                rx_length = 0;
            }
        }
    }
}

static err_t netif_init_cb(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->mtu        = 1500;
    netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    netif->state      = NULL;
    netif->name[0]    = 'E';
    netif->name[1]    = 'X';
    netif->linkoutput = linkoutput_fn;
    netif->output     = etharp_output;
    return ERR_OK;
}

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
    (void)netif;
    int ret;

    ret = usbh_rndis_eth_tx(p);

    if (-EBUSY == ret) {
        ret = ERR_BUF;
    }

    return ret;
}

static int usbh_rndis_eth_tx(struct pbuf *p)
{
    struct pbuf *q;
    uint8_t *buffer;
    rndis_data_packet_t *hdr;
    int ret;
    int recount = 5;
    uint8_t data[4];
    uint32_t info_len = 0;

    if (!s_rndis_class_ptr->link_status) {
        printf("linkdown....now connecting...\r\n");
        while (recount--) {
            ret = usbh_rndis_query_msg_transfer(s_rndis_class_ptr, OID_GEN_MEDIA_CONNECT_STATUS, sizeof(data), data, &info_len);
            if (ret < 0) {
                return -EBUSY;
            }
            if (NDIS_MEDIA_STATE_CONNECTED == data[0]) {
                s_rndis_class_ptr->link_status = true;
                printf("linkup!!!!!!\r\n");
                break;
            } else {
                s_rndis_class_ptr->link_status = false;
            }
            vTaskDelay(1000);
        }
        return 0;
    }

    assert((p->tot_len + sizeof(rndis_data_packet_t)) < sizeof(tx_buffer));
    if (p->tot_len > sizeof(tx_buffer)) {
        printf("RNDIS MTU is:%d, but the send packet size is %d\r\n", sizeof(tx_buffer), p->tot_len);
        p->tot_len = sizeof(tx_buffer);
    }

    hdr = (rndis_data_packet_t *)tx_buffer;
    memset(hdr, 0, sizeof(rndis_data_packet_t));
    hdr->MessageType   = REMOTE_NDIS_PACKET_MSG;
    hdr->MessageLength = sizeof(rndis_data_packet_t) + p->tot_len;
    hdr->DataOffset    = sizeof(rndis_data_packet_t) - sizeof(rndis_generic_msg_t);
    hdr->DataLength    = p->tot_len;

    buffer = (uint8_t *)(tx_buffer + sizeof(rndis_data_packet_t));
    for (q = p; q != NULL; q = q->next) {
        memcpy(buffer, q->payload, q->len);
        buffer += q->len;
    }
    /* send */
    if ((hdr->MessageLength & 0x1FF) == 0) {
        /* pad a dummy. */
        hdr->MessageLength += 1;
    }
    return rndis_msg_data_send(s_rndis_class_ptr, (uint8_t *)tx_buffer, hdr->MessageLength);
}

static int rndis_msg_data_send(struct usbh_rndis *rndis_class, uint8_t *buffer,
                               int nbytes)
{
    int ret = ERR_OK;
    int len = 0;
    len = usbh_rndis_bulk_out_transfer(rndis_class, buffer, nbytes, 5000);
    if (len != nbytes) {
        printf("rndis msg send fail\r\n");
        ret = -EBUSY;
    }
    return ret;
}
