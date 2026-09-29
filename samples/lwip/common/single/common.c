/*
 * Copyright (c) 2022-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"
#include "hpm_common.h"
#include "hpm_interrupt.h"
#include "hpm_otp_drv.h"
#include "ethernetif.h"
#include "netconf.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/pbuf.h"
#include "lwip/err.h"
#include "osal.h"
#include "enet_phy_adaptive_lwip.h"
#include "netinfo.h"
#include "lwipopts.h"
#if defined(LWIP_PTP) && LWIP_PTP
#include "lwip_ptp_tx_ts.h"
#endif
#ifndef ENET_RETRY_CONTROLLER_INIT_CNT
#define ENET_RETRY_CONTROLLER_INIT_CNT   (3U) /**< Enet retry count for controller initialization */
#endif

#ifndef DHCP_TASK_PRIO
  #if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#define DHCP_TASK_PRIO   (tskIDLE_PRIORITY + 4)
  #elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#define DHCP_TASK_PRIO   (IDLE_TASK_PRIO - 4)
  #endif
#endif

#ifndef LOG_TASK_PRIO
  #if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
#define LOG_TASK_PRIO   (tskIDLE_PRIORITY + 3)
  #elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
#define LOG_TASK_PRIO   (IDLE_TASK_PRIO - 3)
  #endif
#endif

#define LOG_QUEUE_SIZE   (10U)
#define LOG_MESSAGE_MAX_LEN  (128U)

static enet_phy_status_t last_status = {.enet_phy_link = enet_phy_link_unknown};
static uint8_t dhcp_last_state = DHCP_STATE_OFF;

/* Log message structure */
typedef struct {
    char message[LOG_MESSAGE_MAX_LEN];
} log_message_t;

#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
static QueueHandle_t log_queue = NULL;
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
static rt_mq_t log_queue = RT_NULL;
#endif
static bool log_task_initialized = false;
#endif

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT] ; /* Ethernet Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT] ; /* Ethernet Tx DMA Descriptor */

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE)
__RW uint8_t rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

/*
 * No dedicated TX ring buffer (formerly tx_buff[][]): zero-copy TX points each
 * descriptor buffer1 at pbuf payload in enet_lwip_output. Keep tx_buff_cfg.count
 * and size (max bytes per TX segment); set buffer to 0 in enet_init.
 */

enet_desc_t desc[LWIP_NETIF_COUNT];
enet_netif_state_t enet_netif_state[LWIP_NETIF_COUNT];
uint8_t mac[LWIP_NETIF_COUNT][ENET_MAC_SIZE];

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag[LWIP_NETIF_COUNT];
#endif

struct netif gnetif;

void enet_netif_state_bind(uint8_t idx, ENET_Type *base)
{
    if (idx >= LWIP_NETIF_COUNT) {
        return;
    }
    enet_netif_state[idx].desc = &desc[idx];
    enet_netif_state[idx].base = base;
}

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t enet_init(ENET_Type *ptr)
{
    enet_int_config_t int_config = {.int_enable = 0, .int_mask = 0};
    enet_mac_config_t enet_config;
    enet_hw_checksum_config_t hw_checksum_cfg;
    uint8_t init_retry_cnt = 0;
    hpm_stat_t stat;

    if (ptr == NULL) {
        return status_invalid_argument;
    }

    enet_netif_state_bind(0, ptr);

    /* Initialize GPIOs */
    board_init_enet_pins(ptr);

    /* Reset an enet PHY */
    board_reset_enet_phy(ptr);

    /* Set RGMII clock delay */
    #if defined(HPM_ENET_RGMII) && HPM_ENET_RGMII
    board_init_enet_rgmii_clock_delay(ptr);
    #elif defined(HPM_ENET_RMII) && HPM_ENET_RMII
    /* Set RMII reference clock */
    board_init_enet_rmii_reference_clock(ptr, BOARD_ENET_RMII_INT_REF_CLK);
    printf("Reference Clock: %s\n", BOARD_ENET_RMII_INT_REF_CLK ? "Internal Clock" : "External Clock");
    #elif defined(HPM_ENET_MII) && HPM_ENET_MII
    board_init_enet_mii_clock(ptr);
    #endif

    /* Initialize td, rd and the corresponding buffers */
    memset((uint8_t *)dma_tx_desc_tab, 0x00, sizeof(dma_tx_desc_tab));
    memset((uint8_t *)dma_rx_desc_tab, 0x00, sizeof(dma_rx_desc_tab));
    enet_lwip_dc_invalidate((uint32_t)rx_buff, sizeof(rx_buff));

    desc[0].tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_tx_desc_tab);
    desc[0].rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_rx_desc_tab);

    /* buffer=0: no TX ring; size is max bytes per TX desc (see ENET_TX_BUFF_SIZE) */
    desc[0].tx_buff_cfg.buffer = 0;
    desc[0].tx_buff_cfg.count = ENET_TX_BUFF_COUNT;
    desc[0].tx_buff_cfg.size = ENET_TX_BUFF_SIZE;

    desc[0].rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rx_buff);
    desc[0].rx_buff_cfg.count = ENET_RX_BUFF_COUNT;
    desc[0].rx_buff_cfg.size = ENET_RX_BUFF_SIZE;

    /* Get MAC address */
    if (ENET_MAC_ADDR_PARA_ERROR == enet_get_mac_address(mac[0])) {
        printf("Enet MAC address init failed!\n");
        return status_fail;
    }

    /* Set MAC0 address */
    enet_config.mac_addr_high[0] = mac[0][5] << 8 | mac[0][4];
    enet_config.mac_addr_low[0]  = mac[0][3] << 24 | mac[0][2] << 16 | mac[0][1] << 8 | mac[0][0];
    enet_config.valid_max_count  = 1;

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(ptr);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
    /* Enable Enet IRQ */
    board_enable_enet_irq(ptr);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ptr, &int_config);
    #endif

    /* Initialize enet controller */
    do {
        stat = enet_controller_init(ptr, ENET_INF_TYPE, &desc[0], &enet_config, &int_config);
    } while ((stat != status_success && init_retry_cnt++ <= ENET_RETRY_CONTROLLER_INIT_CNT));

    if (init_retry_cnt > ENET_RETRY_CONTROLLER_INIT_CNT) {
        return status_fail;
    }

    /* Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(ptr, &desc[0].tx_control_config);

    /* Set TX HW CRC mode (append / replace / off) */
    enet_tx_control_set_hw_crc_mode(ptr, &desc[0].tx_control_config, ENET_TX_HW_CRC_MODE);

#if defined(CHECKSUM_BY_HARDWARE)
    enet_get_default_hw_checksum_config(&hw_checksum_cfg, true);
#else
    enet_get_default_hw_checksum_config(&hw_checksum_cfg, false);
#endif
    stat = enet_set_hw_checksum_config(ptr, &desc[0].tx_control_config, &hw_checksum_cfg);
    if (stat != status_success) {
        return stat;
    }

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(ptr);
    #endif

    if (board_init_enet_phy(ptr) == status_success) {
        printf("Enet phy init passed !\n");
        return status_success;
    } else {
        printf("Enet phy init failed !\n");
        return status_fail;
    }
}

#if defined(NO_SYS) && !NO_SYS
uint32_t msg;
extern osSemaphoreId_t s_xSemaphore[];

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
void timer_callback(TimerHandle_t xTimer)
{
   (void)xTimer;

   enet_self_adaptive_port_speed();
}
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
void timer_callback(void *parameter)
{
   (void)parameter;

   enet_self_adaptive_port_speed();
}
#endif

/* Log task to handle unified log printing */
#if defined(LWIP_DHCP) && LWIP_DHCP
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
static void log_task(void *pvParameters) /* NOLINT */
{
    log_message_t log_msg;
    (void)pvParameters;

    for (;;) {
        if (xQueueReceive(log_queue, &log_msg, portMAX_DELAY) == pdTRUE) {
            printf("%s", log_msg.message);
        }
    }
}
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
static void log_task(void *parameter) /* NOLINT */
{
    log_message_t log_msg;
    (void)parameter;

    for (;;) {
        if (rt_mq_recv(log_queue, &log_msg, sizeof(log_message_t), RT_WAITING_FOREVER) == RT_EOK) {
            printf("%s", log_msg.message);
        }
    }
}
#endif
#endif

/* Function to send log message to queue */
void log_send_message(const char *format, ...)
{
    log_message_t log_msg;
    va_list args;

    if (!log_task_initialized) {
        /* Fallback to direct printf if log task not initialized */
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        return;
    }

    va_start(args, format);
    vsnprintf(log_msg.message, LOG_MESSAGE_MAX_LEN, format, args);
    va_end(args);

#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
    if (log_queue != NULL) {
        xQueueSend(log_queue, &log_msg, 0);
    }
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
    if (log_queue != RT_NULL) {
        rt_mq_send(log_queue, &log_msg, sizeof(log_message_t));
    }
#endif
}
#endif

#if defined(LWIP_DHCP) && LWIP_DHCP
void enet_update_dhcp_state(struct netif *netif)
{
    struct dhcp *dhcp = netif_dhcp_data(netif);
    char state_str[32] = {0};

    if (netif_is_link_up(netif) == false) {
        return;
    }

    if (dhcp == NULL) {
        dhcp_last_state = DHCP_STATE_OFF;
    } else if (dhcp_last_state != dhcp->state) {
        dhcp_last_state = dhcp->state;

        switch (dhcp_last_state) {
        case DHCP_STATE_OFF:
            strcpy(state_str, "OFF");
            break;
        case DHCP_STATE_REQUESTING:
            strcpy(state_str, "REQUESTING");
            break;
        case DHCP_STATE_INIT:
            strcpy(state_str, "INIT");
            break;
        case DHCP_STATE_REBOOTING:
            strcpy(state_str, "REBOOTING");
            break;
        case DHCP_STATE_REBINDING:
            strcpy(state_str, "REBINDING");
            break;
        case DHCP_STATE_RENEWING:
            strcpy(state_str, "RENEWING");
            break;
        case DHCP_STATE_SELECTING:
            strcpy(state_str, "SELECTING");
            break;
        case DHCP_STATE_INFORMING:
            strcpy(state_str, "INFORMING");
            break;
        case DHCP_STATE_CHECKING:
            strcpy(state_str, "CHECKING");
            break;
        case DHCP_STATE_BOUND:
            strcpy(state_str, "BOUND");
            break;
        case DHCP_STATE_BACKING_OFF:
            strcpy(state_str, "BACKING_OFF");
            break;
        default:
            snprintf(state_str, sizeof(state_str), "%u", dhcp_last_state);
            assert(0);
            break;
        }

#if defined(NO_SYS) && !NO_SYS
        log_send_message("DHCP State: %s\r\n", state_str);
#else
        printf("DHCP State: %s\r\n", state_str);
#endif

        if (dhcp_last_state == DHCP_STATE_BOUND) {
            netif_show_ip_info(netif);
        }
    }
}
#endif

ATTR_WEAK int8_t enet_get_mac_address(uint8_t *mac)
{
    char *strtok_result = NULL;
    char tmp[32] = "";
    uint32_t macl, mach;
    uint32_t uuid[OTP_SOC_UUID_LEN / sizeof(uint32_t)];
    uint8_t idx = 0;
    char *token;

    if (mac == NULL) {
        return ENET_MAC_ADDR_PARA_ERROR;
    }

    /* load mac address from OTP MAC area */
    macl = otp_read_from_shadow(OTP_SOC_MAC0_IDX);
    mach = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 1);

    mac[0] = (macl >>  0) & 0xff;
    mac[1] = (macl >>  8) & 0xff;
    mac[2] = (macl >> 16) & 0xff;
    mac[3] = (macl >> 24) & 0xff;
    mac[4] = (mach >>  0) & 0xff;
    mac[5] = (mach >>  8) & 0xff;

    if (!IS_MAC_INVALID(mac)) {
        return ENET_MAC_ADDR_FROM_OTP_MAC;
    }

    /* load MAC address from OTP UUID area */
    for (uint32_t i = 0; i < ARRAY_SIZE(uuid); i++) {
        uuid[i] = otp_read_from_shadow(OTP_SOC_UUID_IDX + i);
    }

    if (!IS_UUID_INVALID(uuid)) {
        uuid[0] &= 0xfc;
        memcpy(mac, &uuid, ENET_MAC_SIZE);
        return ENET_MAC_ADDR_FROM_OTP_UUID;
    }

    /* load MAC address from MACRO definitions */
    strcpy(tmp, HPM_STRINGIFY(MAC0_CONFIG));
    token = strtok_r(tmp, ":", &strtok_result);
    mac[idx] = strtol(token, NULL, 16);
    while (token != NULL && ++idx < ENET_MAC_SIZE) {
        token = strtok_r(NULL, ":", &strtok_result);
        mac[idx] = strtol(token, NULL, 16);
    }

    if (idx < ENET_MAC_SIZE) {
        return ENET_MAC_ADDR_PARA_ERROR;
    }

    return ENET_MAC_ADDR_FROM_MACRO;
}

bool enet_get_link_status(void)
{
    return last_status.enet_phy_link;
}

bool enet_get_dhcp_ready_status(void)
{
    return (dhcp_last_state == DHCP_STATE_BOUND) ? true : false;
}

void enet_self_adaptive_port_speed(void)
{
    lwip_enet_phy_adaptive_binding_t binding = {
        .last = &last_status,
        .enet_base = enet_netif_state[0].base,
        .phy_port = 0,
        .log_prefix = NULL,
        .print_port_banner = false,
        .notify_netif = true,
        .netif_idx = 1U, /* lwIP netif_get_by_index is 1-based */
#if defined(NO_SYS) && !NO_SYS
        .status_mbox = (void *)&netif_status_mbox,
        .link_msg = &msg,
#endif
    };

    lwip_enet_phy_adaptive_poll(&binding);
}

void enet_services(struct netif *netif)
{
#if defined(LWIP_DHCP) && LWIP_DHCP
    #if defined(NO_SYS) && NO_SYS
    dhcp_start(netif);
    #else
    /* Initialize log task and queue if not already initialized */
    if (!log_task_initialized) {
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
        log_queue = xQueueCreate(LOG_QUEUE_SIZE, sizeof(log_message_t));
        if (log_queue != NULL) {
            xTaskCreate(log_task, "LogTask", configMINIMAL_STACK_SIZE * 2, NULL, LOG_TASK_PRIO, NULL);
            log_task_initialized = true;
        }
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
        log_queue = rt_mq_create("log_mq", LOG_MESSAGE_MAX_LEN, LOG_QUEUE_SIZE, RT_IPC_FLAG_FIFO);
        if (log_queue != RT_NULL) {
            rt_thread_t log_thread = rt_thread_create("LogTask", log_task, NULL, 1024, LOG_TASK_PRIO, 10);
            if (log_thread != RT_NULL) {
                rt_thread_startup(log_thread);
                log_task_initialized = true;
            }
        }
#endif
    }
    /* Start DHCP Client */
    #if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
    xTaskCreate(LwIP_DHCP_task, "DHCP", configMINIMAL_STACK_SIZE * 2, netif, DHCP_TASK_PRIO, NULL);
    #elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
    rt_thread_t dhcp_thread = rt_thread_create("DHCP", LwIP_DHCP_task, netif, 1024, DHCP_TASK_PRIO, 10);
    rt_thread_startup(dhcp_thread);
    #endif
    #endif
#else
    netif_show_ip_info(netif);
#endif
}

#if defined(NO_SYS) && NO_SYS
void enet_common_handler(struct netif *netif)
{
    ethernetif_input(netif);

    /* Handle all system timeouts for all core protocols */
    #if defined(LWIP_TIMERS) && LWIP_TIMERS
    sys_check_timeouts();
    #endif

    /* update DHCP progress */
    #if defined(LWIP_DHCP) && LWIP_DHCP
    enet_update_dhcp_state(netif);
    #endif
}
#endif

#ifndef ENET_LWIP_TX_DESC_WAIT_MS
#define ENET_LWIP_TX_DESC_WAIT_MS (50U)
#endif

typedef struct {
    struct pbuf *pbuf;
    enet_tx_desc_t *last;
    uint8_t desc_cnt;
} enet_lwip_tx_hold_t;

typedef struct {
    enet_lwip_tx_hold_t entry[ENET_TX_BUFF_COUNT];
    uint32_t head;       /* first descriptor of the oldest pending frame */
    uint32_t in_flight;  /* number of frames in the in-flight window */
} enet_lwip_tx_hold_ctx_t;

static enet_lwip_tx_hold_ctx_t s_enet_tx_hold;

static void enet_lwip_dc_writeback(uint32_t addr, uint32_t len)
{
    uint32_t aligned_start;
    uint32_t aligned_end;

    if (len == 0) {
        return;
    }
    aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(addr);
    aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(addr + len);
    l1c_dc_writeback(aligned_start, aligned_end - aligned_start);
}

void enet_lwip_dc_invalidate(uint32_t addr, uint32_t len)
{
    uint32_t aligned_start;
    uint32_t aligned_end;

    if (len == 0) {
        return;
    }
    aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(addr);
    aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(addr + len);
    l1c_dc_invalidate(aligned_start, aligned_end - aligned_start);
}

static uint32_t enet_lwip_tx_desc_index(enet_desc_t *desc, enet_tx_desc_t *dma_tx_desc)
{
    return (uint32_t)(dma_tx_desc - desc->tx_desc_list_head);
}

static uint32_t enet_lwip_count_tx_descs(struct pbuf *p, uint16_t max_seg_size)
{
    struct pbuf *q;
    uint32_t count = 0;
    uint16_t left;
    uint16_t chunk;

    for (q = p; q != NULL; q = q->next) {
        left = q->len;
        while (left > 0) {
            chunk = (left > max_seg_size) ? max_seg_size : left;
            count++;
            left = (uint16_t)(left - chunk);
        }
    }
    return count;
}

static bool enet_lwip_tx_descs_ready(enet_desc_t *desc, enet_tx_desc_t *start, uint32_t need)
{
    enet_tx_desc_t *dma_tx_desc = start;
    uint32_t i;
    uint32_t hold_idx;

    hold_idx = enet_lwip_tx_desc_index(desc, start);
    if (s_enet_tx_hold.entry[hold_idx].pbuf != NULL) {
        return false;
    }

    for (i = 0; i < need; i++) {
        if (dma_tx_desc->tdes0_bm.own != 0) {
            return false;
        }
        dma_tx_desc = (enet_tx_desc_t *)(dma_tx_desc->tdes3_bm.next_desc);
    }
    return true;
}

static bool enet_lwip_wait_tx_descs(enet_desc_t *desc, uint32_t need)
{
    uint32_t start_ms;
    uint32_t waited;

    /*
     * Busy-poll with sys_now() timeout. Avoid sys_arch_msleep(1) here so
     * high-rate TX is not capped by 1 ms sleep quanta.
     */
    start_ms = sys_now();
    waited = 0;
    while (!enet_lwip_tx_descs_ready(desc, desc->tx_desc_list_cur, need)) {
        enet_lwip_tx_release(desc);
        waited = sys_now() - start_ms;
        if (waited >= ENET_LWIP_TX_DESC_WAIT_MS) {
            return false;
        }
    }
    return true;
}

void enet_lwip_tx_release(enet_desc_t *desc)
{
    enet_lwip_tx_hold_t *hold;
    struct pbuf *done;
    uint32_t head;
    uint32_t next;
    uint32_t level;

    if (desc == NULL) {
        return;
    }

    /*
     * Release completed frames from the oldest in-flight entry forward.
     * Update the hold window with IRQs masked; call pbuf_free() after restore.
     */
    for (;;) {
        level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
        if (s_enet_tx_hold.in_flight == 0) {
            restore_global_irq(level);
            break;
        }

        head = s_enet_tx_hold.head;
        hold = &s_enet_tx_hold.entry[head];
        if (hold->last->tdes0_bm.own != 0) {
            restore_global_irq(level);
            break;
        }

        next = head + hold->desc_cnt;
        done = hold->pbuf;
        hold->pbuf = NULL;
        hold->last = NULL;
        hold->desc_cnt = 0;
        s_enet_tx_hold.head = (next < ENET_TX_BUFF_COUNT) ? next : (next - ENET_TX_BUFF_COUNT);
        s_enet_tx_hold.in_flight--;
        restore_global_irq(level);

        pbuf_free(done);
    }
}

struct pbuf *enet_lwip_input(enet_frame_t *frame, uint16_t rx_buff_size,
                                          enet_lwip_rx_custom_alloc_fn alloc_fn,
                                          enet_lwip_rx_custom_free_fn free_fn)
{
    struct pbuf *p;
    struct pbuf *q;
    enet_lwip_rx_custom_pbuf_t *my_pbuf;
    enet_rx_desc_t *dma_rx_desc;
    uint8_t *buffer;
    uint32_t remaining;
    uint32_t chunk;
    uint32_t i;
    uint32_t seg_count;

    p = NULL;
    if ((frame == NULL) || (rx_buff_size == 0) || (alloc_fn == NULL) || (free_fn == NULL)) {
        return NULL;
    }
    if ((frame->length == 0) || (frame->seg == 0) || (frame->rx_desc == NULL)) {
        return NULL;
    }

    remaining = frame->length;
    seg_count = frame->seg;
    dma_rx_desc = frame->rx_desc;

    for (i = 0; i < seg_count; i++) {
        if (remaining == 0) {
            break;
        }
        chunk = (remaining > rx_buff_size) ? rx_buff_size : remaining;
        my_pbuf = (enet_lwip_rx_custom_pbuf_t *)alloc_fn();
        if (my_pbuf == NULL) {
            if (p != NULL) {
                pbuf_free(p);
            }
            return NULL;
        }
        my_pbuf->p.custom_free_function = free_fn;
        /* only the head custom pbuf returns the whole FS..LS chain to DMA */
        my_pbuf->dma_descriptor = (i == 0) ? (void *)frame : NULL;
        buffer = (uint8_t *)(uintptr_t)dma_rx_desc->rdes2_bm.buffer1;
        q = pbuf_alloced_custom(PBUF_RAW, (u16_t)chunk, PBUF_REF, &my_pbuf->p, buffer, rx_buff_size);
        if (q == NULL) {
            free_fn((struct pbuf *)my_pbuf);
            if (p != NULL) {
                pbuf_free(p);
            }
            return NULL;
        }
        enet_lwip_dc_invalidate((uint32_t)buffer, chunk);
        if (p == NULL) {
            p = q;
        } else {
            pbuf_cat(p, q);
        }
        remaining -= chunk;
        dma_rx_desc = (enet_rx_desc_t *)dma_rx_desc->rdes3_bm.next_desc;
    }

    return p;
}

err_t enet_lwip_output(ENET_Type *ptr, enet_desc_t *desc, struct pbuf *p)
{
    struct pbuf *q;
    enet_tx_desc_t *tx_desc_list_cur;
    enet_tx_desc_t *first_desc;
    enet_tx_desc_t *desc_list[ENET_TX_BUFF_COUNT];
    enet_tx_control_config_t tx_cfg;
    uint32_t need;
    uint32_t seg;
    uint32_t hold_idx;
    uint32_t level;
    uint16_t max_seg_size;
    uint16_t left;
    uint16_t chunk;
    uint16_t tbs1_len;
    uint16_t payload_offset;
    uint32_t payload_addr;
#if defined(LWIP_PTP) && LWIP_PTP
    enet_ptp_ts_system_t timestamp;
#endif

    if ((ptr == NULL) || (desc == NULL) || (p == NULL) || (p->tot_len == 0)) {
        return ERR_VAL;
    }

    enet_lwip_tx_release(desc);

    max_seg_size = desc->tx_buff_cfg.size;
    if (max_seg_size == 0) {
        return ERR_VAL;
    }

    need = enet_lwip_count_tx_descs(p, max_seg_size);
    if ((need == 0) || (need > ENET_TX_BUFF_COUNT)) {
        return ERR_MEM;
    }

    if (!enet_lwip_wait_tx_descs(desc, need)) {
        return ERR_MEM;
    }

    tx_cfg = desc->tx_control_config;
#if defined(LWIP_PTP) && LWIP_PTP
    tx_cfg.enable_ttse = lwip_ptp_frame_needs_tx_hw_timestamp(p);
#endif

    tx_desc_list_cur = desc->tx_desc_list_cur;
    first_desc = tx_desc_list_cur;
    seg = 0;

    for (q = p; q != NULL; q = q->next) {
        left = q->len;
        payload_offset = 0;
        while (left > 0) {
            chunk = (left > max_seg_size) ? max_seg_size : left;
            payload_addr = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)q->payload + payload_offset);
            desc_list[seg] = tx_desc_list_cur;
            /* last segment adds 4 for FCS only in CRC replace mode */
            tbs1_len = chunk;
            if ((seg == (need - 1)) && (ENET_TX_HW_CRC_MODE == enet_tx_hw_crc_replace)) {
                tbs1_len = (uint16_t)(chunk + 4U);
            }
            enet_tx_desc_fill_segment(tx_desc_list_cur, &tx_cfg, payload_addr, tbs1_len, (seg == 0), (seg == (need - 1)));
            enet_lwip_dc_writeback((uint32_t)q->payload + payload_offset, chunk);

            payload_offset = (uint16_t)(payload_offset + chunk);
            left = (uint16_t)(left - chunk);
            seg++;
            tx_desc_list_cur = (enet_tx_desc_t *)(tx_desc_list_cur->tdes3_bm.next_desc);
        }
    }

    pbuf_ref(p);
    hold_idx = enet_lwip_tx_desc_index(desc, first_desc);
    level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (s_enet_tx_hold.entry[hold_idx].pbuf != NULL) {
        /* should be cleared by ready/release; never free an in-flight hold */
        restore_global_irq(level);
        pbuf_free(p);
        return ERR_MEM;
    }
    if (s_enet_tx_hold.in_flight == 0) {
        /* empty window: re-anchor head in case it drifted from the TX ring */
        s_enet_tx_hold.head = hold_idx;
    }
    s_enet_tx_hold.entry[hold_idx].pbuf = p;
    s_enet_tx_hold.entry[hold_idx].last = desc_list[seg - 1];
    s_enet_tx_hold.entry[hold_idx].desc_cnt = (uint8_t)seg;
    s_enet_tx_hold.in_flight++;
    restore_global_irq(level);

    enet_tx_desc_handoff_segments(ptr, desc_list, seg);

#if defined(LWIP_PTP) && LWIP_PTP
    if (tx_cfg.enable_ttse) {
        if (enet_get_tx_timestamp(desc_list[seg - 1], &timestamp) != ENET_SUCCESS) {
            return ERR_TIMEOUT;
        }
        p->time_sec = timestamp.sec;
        p->time_nsec = timestamp.nsec;
    }
#endif

    desc->tx_desc_list_cur = tx_desc_list_cur;
    return ERR_OK;
}

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
void isr_enet(ENET_Type *ptr)
{
#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
#endif
#endif

    uint32_t status;
    uint32_t rxgbfrmis;
    uint32_t intr_status;

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
#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
        /* Give the semaphore to wakeup LwIP task */
        xSemaphoreGiveFromISR(s_xSemaphore[0], &xHigherPriorityTaskWoken);
        /* Switch tasks if necessary. */
        if (xHigherPriorityTaskWoken != pdFALSE) {
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
        rt_sem_release(s_xSemaphore[0]);
#endif
#else
        rx_flag[0] = true;
#endif
        ptr->DMA_STATUS |= ENET_DMA_STATUS_RI_MASK;
    }

    if (ENET_MMC_INTR_RX_RXCTRLFIS_GET(rxgbfrmis)) {
        ptr->RXFRAMECOUNT_GB;
    }
}

#ifdef HPM_ENET0_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET0, isr_enet0)
void isr_enet0(void)
{
    isr_enet(HPM_ENET0);
}
#endif

#ifdef HPM_ENET1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET1, isr_enet1)
void isr_enet1(void)
{
    isr_enet(HPM_ENET1);
}
#endif

#endif
