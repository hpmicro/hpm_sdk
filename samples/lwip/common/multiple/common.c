/*
 * Copyright (c) 2023-2026 HPMicro
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
#include "lwip/sys.h"
#include "osal.h"
#include "enet_phy_adaptive_lwip.h"
#include "netinfo.h"
#include "lwipopts.h"
#if defined(LWIP_PTP) && LWIP_PTP
#include "lwip_ptp_tx_ts.h"
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

static enet_phy_status_t last_status[LWIP_NETIF_COUNT] = {{.enet_phy_link = enet_phy_link_unknown}, {.enet_phy_link = enet_phy_link_unknown}};

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

typedef struct {
    enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT];
    enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT];
    uint8_t        rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE];
    /* No dedicated TX ring buffer: zero-copy TX points buffer1 at pbuf payload */
} enet_desc_init_t;

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
enet_desc_init_t desc_init[LWIP_NETIF_COUNT];
enet_desc_t desc[LWIP_NETIF_COUNT];
enet_netif_state_t enet_netif_state[LWIP_NETIF_COUNT];
uint8_t mac[LWIP_NETIF_COUNT][ENET_MAC_SIZE];

struct netif gnetif[LWIP_NETIF_COUNT];

void enet_netif_state_bind(uint8_t idx, ENET_Type *base)
{
    if (idx >= LWIP_NETIF_COUNT) {
        return;
    }
    enet_netif_state[idx].desc = &desc[idx];
    enet_netif_state[idx].base = base;
}


#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag[LWIP_NETIF_COUNT];
#endif

#if defined(NO_SYS) && !NO_SYS
uint32_t msg[LWIP_NETIF_COUNT];
extern osSemaphoreId_t s_xSemaphore[];
#endif

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
static void enet_desc_init(enet_desc_t *pdesc, enet_desc_init_t *pdesc_init)
{
    pdesc->tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->dma_tx_desc_tab);
    pdesc->rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->dma_rx_desc_tab);

    /* buffer=0: no TX ring; size is max bytes per TX desc (see ENET_TX_BUFF_SIZE) */
    pdesc->tx_buff_cfg.buffer = 0;
    pdesc->tx_buff_cfg.count  = ENET_TX_BUFF_COUNT;
    pdesc->tx_buff_cfg.size   = ENET_TX_BUFF_SIZE;

    pdesc->rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->rx_buff);
    pdesc->rx_buff_cfg.count  = ENET_RX_BUFF_COUNT;
    pdesc->rx_buff_cfg.size   = ENET_RX_BUFF_SIZE;
}

hpm_stat_t enet_init(uint8_t idx)
{
    enet_mac_config_t        enet_config;
    enet_hw_checksum_config_t hw_checksum_cfg;
    enet_int_config_t        int_config = {0};
    enet_base_t              *base;
    enet_inf_type_t          itf;
    hpm_stat_t               stat;

    if (idx > LWIP_NETIF_COUNT) {
        return status_invalid_argument;
    }

    base = board_get_enet_base(idx);
    itf  = board_get_enet_phy_itf(idx);

    /* Initialize td, rd and the corresponding buffers */
    enet_desc_init(&desc[idx], &desc_init[idx]);

    /* Get a default MAC address */
    if (ENET_MAC_ADDR_PARA_ERROR == enet_get_mac_address(idx, mac[idx])) {
        printf("Enet%d MAC address init failed!\n", idx);
        return status_fail;
    }

    /* Set MAC0 address */
    enet_set_mac_address(&enet_config, mac[idx]);

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(base);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
    /* Enable Enet IRQ */
    board_enable_enet_irq(base);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(base, &int_config);
    #endif

    /* Initialize Enet MAC */
    if (enet_controller_init(base, itf, &desc[idx], &enet_config, &int_config) != status_success) {
        printf("Enet%d MAC init failed!\n", idx);
        return status_fail;
    }

    /* Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(base, &desc[idx].tx_control_config);

    /* Set TX HW CRC mode (append / replace / off) */
    enet_tx_control_set_hw_crc_mode(base, &desc[idx].tx_control_config, ENET_TX_HW_CRC_MODE);

#if defined(CHECKSUM_BY_HARDWARE)
    enet_get_default_hw_checksum_config(&hw_checksum_cfg, true);
#else
    enet_get_default_hw_checksum_config(&hw_checksum_cfg, false);
#endif
    stat = enet_set_hw_checksum_config(base, &desc[idx].tx_control_config, &hw_checksum_cfg);
    if (stat != status_success) {
        return stat;
    }

    /* Initialize Enet PHY */
    if (board_init_enet_phy(base) != status_success) {
        printf("Enet%d PHY init failed!\n", idx);
        return status_fail;
    }

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(base);
    #endif

    return status_success;
}

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

#if defined(NO_SYS) && !NO_SYS
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
    static uint8_t dhcp_last_state[LWIP_NETIF_COUNT] = {DHCP_STATE_OFF};
    struct dhcp *dhcp = netif_dhcp_data(netif);
    char state_str[32] = {0};

    if (netif_is_link_up(netif) == false) {
        return;
    }

    if (dhcp == NULL) {
        dhcp_last_state[netif->num] = DHCP_STATE_OFF;
    } else if (dhcp_last_state[netif->num] != dhcp->state) {
        dhcp_last_state[netif->num] = dhcp->state;

        switch (dhcp_last_state[netif->num]) {
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
            snprintf(state_str, sizeof(state_str), "%u", dhcp_last_state[netif->num]);
            assert(0);
            break;
        }

#if defined(NO_SYS) && !NO_SYS
        log_send_message("================ Network Interface %d ================\n", netif->num);
        log_send_message("DHCP State: %s\r\n", state_str);
#else
        printf("================ Network Interface %d ================\n", netif->num);
        printf("DHCP State: %s\r\n", state_str);
#endif

        if (dhcp_last_state[netif->num] == DHCP_STATE_BOUND) {
            netif_show_ip_info(netif);
        }
    }
}
#endif

ATTR_WEAK int8_t enet_get_mac_address(uint8_t i, uint8_t *mac)
{
    uint32_t macl, mach;
    uint8_t idx = 0;
    char *token;
    char *strtok_result = NULL;
    char tmp[32] = "";

    if (mac == NULL) {
        return ENET_MAC_ADDR_PARA_ERROR;
    }

    /* load mac address from OTP MAC area */
    if (i == 0) {
        macl = otp_read_from_shadow(OTP_SOC_MAC0_IDX);
        mach = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 1);

        mac[0] = (macl >>  0) & 0xff;
        mac[1] = (macl >>  8) & 0xff;
        mac[2] = (macl >> 16) & 0xff;
        mac[3] = (macl >> 24) & 0xff;
        mac[4] = (mach >>  0) & 0xff;
        mac[5] = (mach >>  8) & 0xff;
    } else {
        macl = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 1);
        mach = otp_read_from_shadow(OTP_SOC_MAC0_IDX + 2);

        mac[0] = (macl >> 16) & 0xff;
        mac[1] = (macl >> 24) & 0xff;
        mac[2] = (mach >>  0) & 0xff;
        mac[3] = (mach >>  8) & 0xff;
        mac[4] = (mach >> 16) & 0xff;
        mac[5] = (mach >> 24) & 0xff;
    }

    if (!IS_MAC_INVALID(mac)) {
        return ENET_MAC_ADDR_FROM_OTP_MAC;
    }

    /* load MAC address from MACRO definitions */
    strcpy(tmp, mac_init[i].mac_addr);
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

ATTR_WEAK void enet_set_mac_address(void *config, uint8_t *mac)
{
    enet_mac_config_t *p = (enet_mac_config_t *)config;

    p->mac_addr_high[0] = mac[5] << 8 | mac[4];
    p->mac_addr_low[0]  = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    p->valid_max_count  = 1;
}

bool enet_get_link_status(uint8_t i)
{
    return last_status[i].enet_phy_link;
}

void enet_self_adaptive_port_speed(void)
{
    for (uint8_t i = 0; i < LWIP_NETIF_COUNT; i++) {
        lwip_enet_phy_adaptive_binding_t binding = {
            .last = &last_status[i],
            .enet_base = board_get_enet_base(netif_get_by_index(i + 1)->num),
            .phy_port = i,
            .log_prefix = NULL,
            .print_port_banner = true,
            .notify_netif = true,
            .netif_idx = (uint8_t)(i + 1U),
#if defined(NO_SYS) && !NO_SYS
            .status_mbox = (void *)&netif_status_mbox[i],
            .link_msg = &msg[i],
#endif
        };

        lwip_enet_phy_adaptive_poll(&binding);
    }
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
    uint32_t head;
    uint32_t in_flight;
} enet_lwip_tx_hold_ctx_t;

static enet_lwip_tx_hold_ctx_t s_enet_tx_hold[LWIP_NETIF_COUNT];

static enet_lwip_tx_hold_ctx_t *enet_lwip_hold_for_desc(enet_desc_t *d)
{
    uint8_t i;

    for (i = 0; i < LWIP_NETIF_COUNT; i++) {
        if (d == &desc[i]) {
            return &s_enet_tx_hold[i];
        }
    }
    return NULL;
}

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

static bool enet_lwip_tx_descs_ready(enet_desc_t *desc, enet_lwip_tx_hold_ctx_t *hold_ctx,
                                     enet_tx_desc_t *start, uint32_t need)
{
    enet_tx_desc_t *dma_tx_desc = start;
    uint32_t i;
    uint32_t hold_idx;

    hold_idx = enet_lwip_tx_desc_index(desc, start);
    if (hold_ctx->entry[hold_idx].pbuf != NULL) {
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

static bool enet_lwip_wait_tx_descs(enet_desc_t *desc, enet_lwip_tx_hold_ctx_t *hold_ctx, uint32_t need)
{
    uint32_t start_ms;
    uint32_t waited;

    start_ms = sys_now();
    waited = 0;
    while (!enet_lwip_tx_descs_ready(desc, hold_ctx, desc->tx_desc_list_cur, need)) {
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
    enet_lwip_tx_hold_ctx_t *hold_ctx;
    enet_lwip_tx_hold_t *hold;
    struct pbuf *done;
    uint32_t head;
    uint32_t next;
    uint32_t level;

    if (desc == NULL) {
        return;
    }
    hold_ctx = enet_lwip_hold_for_desc(desc);
    if (hold_ctx == NULL) {
        return;
    }

    for (;;) {
        level = disable_global_irq(CSR_MSTATUS_MIE_MASK);
        if (hold_ctx->in_flight == 0) {
            restore_global_irq(level);
            break;
        }

        head = hold_ctx->head;
        hold = &hold_ctx->entry[head];
        if (hold->last->tdes0_bm.own != 0) {
            restore_global_irq(level);
            break;
        }

        next = head + hold->desc_cnt;
        done = hold->pbuf;
        hold->pbuf = NULL;
        hold->last = NULL;
        hold->desc_cnt = 0;
        hold_ctx->head = (next < ENET_TX_BUFF_COUNT) ? next : (next - ENET_TX_BUFF_COUNT);
        hold_ctx->in_flight--;
        restore_global_irq(level);

        pbuf_free(done);
    }
}

err_t enet_lwip_output(ENET_Type *ptr, enet_desc_t *desc, struct pbuf *p)
{
    enet_lwip_tx_hold_ctx_t *hold_ctx;
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
    hold_ctx = enet_lwip_hold_for_desc(desc);
    if (hold_ctx == NULL) {
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

    if (!enet_lwip_wait_tx_descs(desc, hold_ctx, need)) {
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
    if (hold_ctx->entry[hold_idx].pbuf != NULL) {
        restore_global_irq(level);
        pbuf_free(p);
        return ERR_MEM;
    }
    if (hold_ctx->in_flight == 0) {
        hold_ctx->head = hold_idx;
    }
    hold_ctx->entry[hold_idx].pbuf = p;
    hold_ctx->entry[hold_idx].last = desc_list[seg - 1];
    hold_ctx->entry[hold_idx].desc_cnt = (uint8_t)seg;
    hold_ctx->in_flight++;
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

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
static void isr_enet(uint8_t idx)
{
#if defined(NO_SYS) && !NO_SYS
#if defined(__ENABLE_FREERTOS) && __ENABLE_FREERTOS
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
#endif
#endif

    uint32_t status;
    uint32_t rxgbfrmis;
    uint32_t intr_status;
    enet_base_t *ptr = board_get_enet_base(idx);

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
        xSemaphoreGiveFromISR(s_xSemaphore[idx], &xHigherPriorityTaskWoken);
        /* Switch tasks if necessary. */
        if (xHigherPriorityTaskWoken != pdFALSE) {
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
        rt_sem_release(s_xSemaphore[idx]);
#endif
    #else
        rx_flag[idx] = true;
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
    isr_enet(0);
}
#endif

#ifdef HPM_ENET1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET1, isr_enet1)
void isr_enet1(void)
{
    isr_enet(1);
}
#endif

#endif
