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
#include "hpm_otp_drv.h"
#include "ethernetif.h"
#include "netconf.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "osal.h"
#include "enet_phy_adaptive_lwip.h"

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

ATTR_ALIGN(HPM_L1C_CACHELINE_SIZE)
__RW uint8_t tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */

enet_desc_t desc;
uint8_t mac[ENET_MAC_SIZE];

#if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
volatile bool rx_flag;
#endif

struct netif gnetif;

/*---------------------------------------------------------------------*
 * Initialization
 *---------------------------------------------------------------------*/
hpm_stat_t enet_init(ENET_Type *ptr)
{
    enet_int_config_t int_config = {.int_enable = 0, .int_mask = 0};
    enet_mac_config_t enet_config;
    enet_tx_control_config_t enet_tx_control_config;
    uint8_t init_retry_cnt = 0;
    hpm_stat_t stat;

    /* Initialize GPIOs */
    board_init_enet_pins(ENET);

    /* Reset an enet PHY */
    board_reset_enet_phy(ENET);

    /* Set RGMII clock delay */
    #if defined(HPM_ENET_RGMII) && HPM_ENET_RGMII
    board_init_enet_rgmii_clock_delay(ENET);
    #elif defined(HPM_ENET_RMII) && HPM_ENET_RMII
    /* Set RMII reference clock */
    board_init_enet_rmii_reference_clock(ENET, BOARD_ENET_RMII_INT_REF_CLK);
    printf("Reference Clock: %s\n", BOARD_ENET_RMII_INT_REF_CLK ? "Internal Clock" : "External Clock");
    #elif defined(HPM_ENET_MII) && HPM_ENET_MII
    board_init_enet_mii_clock(ENET);
    #endif

    /* Initialize td, rd and the corresponding buffers */
    memset((uint8_t *)dma_tx_desc_tab, 0x00, sizeof(dma_tx_desc_tab));
    memset((uint8_t *)dma_rx_desc_tab, 0x00, sizeof(dma_rx_desc_tab));
    memset((uint8_t *)rx_buff, 0x00, sizeof(rx_buff));
    memset((uint8_t *)tx_buff, 0x00, sizeof(tx_buff));

    desc.tx_desc_list_head = (enet_tx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_tx_desc_tab);
    desc.rx_desc_list_head = (enet_rx_desc_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)dma_rx_desc_tab);

    desc.tx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)tx_buff);
    desc.tx_buff_cfg.count = ENET_TX_BUFF_COUNT;
    desc.tx_buff_cfg.size = ENET_TX_BUFF_SIZE;

    desc.rx_buff_cfg.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)rx_buff);
    desc.rx_buff_cfg.count = ENET_RX_BUFF_COUNT;
    desc.rx_buff_cfg.size = ENET_RX_BUFF_SIZE;

    /*Get a default control config for tx descriptor */
    enet_get_default_tx_control_config(ENET, &enet_tx_control_config);

    /* Set the control config for tx descriptor */
    memcpy(&desc.tx_control_config, &enet_tx_control_config, sizeof(enet_tx_control_config_t));

    /* Get MAC address */
    enet_get_mac_address(mac);

    /* Set MAC0 address */
    enet_config.mac_addr_high[0] = mac[5] << 8 | mac[4];
    enet_config.mac_addr_low[0]  = mac[3] << 24 | mac[2] << 16 | mac[1] << 8 | mac[0];
    enet_config.valid_max_count  = 1;

    /* Set DMA PBL */
    enet_config.dma_pbl = board_get_enet_dma_pbl(ENET);

    /* Set SARC */
    enet_config.sarc = enet_sarc_replace_mac0;

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
    /* Enable Enet IRQ */
    board_enable_enet_irq(ENET);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ENET, &int_config);
    #endif

    /* Initialize enet controller */
    do {
        stat = enet_controller_init(ptr, ENET_INF_TYPE, &desc, &enet_config, &int_config);
    } while ((stat != status_success && init_retry_cnt++ <= ENET_RETRY_CONTROLLER_INIT_CNT));

    if (init_retry_cnt > ENET_RETRY_CONTROLLER_INIT_CNT) {
        return status_fail;
    }

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT || defined(NO_SYS) && !NO_SYS
    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(ENET);
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
extern osSemaphoreId_t s_xSemaphore;

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

ATTR_WEAK uint8_t enet_get_mac_address(uint8_t *mac)
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
        .enet_base = ENET,
        .phy_port = 0,
        .log_prefix = NULL,
        .print_port_banner = false,
        .notify_netif = true,
        .netif_idx = LWIP_NETIF_IDX,
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
        xSemaphoreGiveFromISR(s_xSemaphore, &xHigherPriorityTaskWoken);
        /* Switch tasks if necessary. */
        if (xHigherPriorityTaskWoken != pdFALSE) {
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
#elif defined(__ENABLE_RTTHREAD_NANO) && __ENABLE_RTTHREAD_NANO
        rt_sem_release(s_xSemaphore);
#endif
#else
        rx_flag = true;
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
    isr_enet(ENET);
}
#endif

#ifdef HPM_ENET1_BASE
SDK_DECLARE_EXT_ISR_M(IRQn_ENET1, isr_enet1)
void isr_enet1(void)
{
    isr_enet(ENET);
}
#endif

#endif
