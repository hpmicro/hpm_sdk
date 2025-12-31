/*
 * Copyright (c) 2023-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"
#include "hpm_common.h"
#include "hpm_otp_drv.h"
#include "ethernetif.h"
#include "netconf.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "osal.h"

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

static enet_phy_status_t last_status[BOARD_ENET_COUNT] = {{.enet_phy_link = enet_phy_link_unknown}, {.enet_phy_link = enet_phy_link_unknown}};

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
    uint8_t        tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE];
} enet_desc_init_t;

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
enet_desc_init_t desc_init[BOARD_ENET_COUNT];
enet_desc_t desc[BOARD_ENET_COUNT];
uint8_t mac[BOARD_ENET_COUNT][ENET_MAC_SIZE];

struct netif gnetif[BOARD_ENET_COUNT];

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag[BOARD_ENET_COUNT];
#endif

#if defined(NO_SYS) && !NO_SYS
uint32_t msg[BOARD_ENET_COUNT];
extern osSemaphoreId_t s_xSemaphore[];
#endif

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
static void enet_desc_init(enet_desc_t *pdesc, enet_desc_init_t *pdesc_init)
{
    pdesc->tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->dma_tx_desc_tab);
    pdesc->rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->dma_rx_desc_tab);

    pdesc->tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->tx_buff);
    pdesc->tx_buff_cfg.count  = ENET_TX_BUFF_COUNT;
    pdesc->tx_buff_cfg.size   = ENET_TX_BUFF_SIZE;

    pdesc->rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)pdesc_init->rx_buff);
    pdesc->rx_buff_cfg.count  = ENET_RX_BUFF_COUNT;
    pdesc->rx_buff_cfg.size   = ENET_RX_BUFF_SIZE;
}

hpm_stat_t enet_init(uint8_t idx)
{
    enet_mac_config_t        enet_config;
    enet_tx_control_config_t enet_tx_control_config;
    enet_int_config_t        int_config = {0};
    enet_base_t              *base;
    enet_inf_type_t          itf;

    if (idx > BOARD_ENET_COUNT) {
        return status_invalid_argument;
    }

    base = board_get_enet_base(idx);
    itf  = board_get_enet_phy_itf(idx);

    /* Initialize td, rd and the corresponding buffers */
    enet_desc_init(&desc[idx], &desc_init[idx]);

    /* Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(base, &enet_tx_control_config);

    /* Set the control config for tx descriptor */
    memcpy(&desc[idx].tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Get a default MAC address */
    enet_get_mac_address(idx, mac[idx]);

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
    static uint8_t dhcp_last_state[BOARD_ENET_COUNT] = {DHCP_STATE_OFF};
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

ATTR_WEAK uint8_t enet_get_mac_address(uint8_t i, uint8_t *mac)
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
    enet_base_t *base;
    enet_phy_status_t status = {0};
    enet_line_speed_t line_speed[] = {enet_line_speed_10mbps, enet_line_speed_100mbps, enet_line_speed_1000mbps};

    char *speed_str[] = {"10Mbps", "100Mbps", "1000Mbps"};
    char *duplex_str[] = {"Half duplex", "Full duplex"};

    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        base = board_get_enet_base(netif_get_by_index(i+1)->num);
        board_get_enet_phy_status(i, &status);
        if (status.enet_phy_link || (status.enet_phy_link != last_status[i].enet_phy_link)) {
            if (memcmp(&last_status[i], &status, sizeof(enet_phy_status_t)) != 0) {
                memcpy(&last_status[i], &status, sizeof(enet_phy_status_t));
                printf("================ Network Interface %d ================\n", netif_get_by_index(i+1)->num);
                if (status.enet_phy_link) {
                    printf("Link Status: Up\n");
                    printf("Link Speed:  %s\n", speed_str[status.enet_phy_speed]);
                    printf("Link Duplex: %s\n", duplex_str[status.enet_phy_duplex]);

                    enet_set_line_speed(base, line_speed[status.enet_phy_speed]);
                    enet_set_duplex_mode(base, status.enet_phy_duplex);

                    #if defined(NO_SYS) && !NO_SYS
                    msg[i] = enet_phy_link_up;
                    sys_mbox_trypost_fromisr(&netif_status_mbox[i], &msg[i]);
                    #else
                    netif_set_link_up(netif_get_by_index(i+1));
                    #endif
                } else {
                    printf("Link Status: Down\n");
                    #if defined(NO_SYS) && !NO_SYS
                    msg[i] = enet_phy_link_down;
                    sys_mbox_trypost_fromisr(&netif_status_mbox[i], &msg[i]);
                    #else
                    netif_set_link_down(netif_get_by_index(i+1));
                    #endif
                }
            }
        }
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
