/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*  HPM example includes. */
#include "common.h"
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "tcp_echo.h"


/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO      (tskIDLE_PRIORITY + 5)
#define DHCP_TASK_PRIO      (tskIDLE_PRIORITY + 4)
#define NETIF_STA_TASK_PRIO (tskIDLE_PRIORITY + 4)

typedef struct {
    enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT];
    enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT];
    uint8_t        rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE];
    uint8_t        tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE];
} enet_desc_init_t;

ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
enet_desc_init_t desc_init[BOARD_ENET_COUNT];
enet_desc_t desc[BOARD_ENET_COUNT];
uint8_t mac[BOARD_ENET_COUNT][ENET_MAC];

struct netif gnetif[BOARD_ENET_COUNT];

void Main_task(void *pvParameters);

void bsp_init(void)
{
    /* Initialize BSP */
    board_init();

    /* Initialize GPIOs */
    board_init_multiple_enet_pins();

    /* Reset Enet PHYs */
    board_reset_multiple_enet_phy();

    printf("This is an ethernet demo: TCP Echo With Multi Ports On FreeRTOS\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

   /* Initialize Enet Clock */
    board_init_multiple_enet_clock();
}

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

static hpm_stat_t enet_init(uint8_t idx)
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

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    /* Enable Enet IRQ */
    board_enable_enet_irq(base);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ENET, &int_config);
    #endif

    /* Initialize enet controller */
    if (enet_controller_init(base, itf, &desc[idx], &enet_config, &int_config) != status_success) {
        printf("Enet%d MAC init failed!\n", idx);
        return status_fail;
    }

    /* Initialize Enet PHY */
    if (board_init_enet_phy(base) != status_success) {
        printf("Enet%d PHY init failed!\n", idx);
        return status_fail;
    }

    #if defined(__ENABLE_ENET_RECEIVE_INTERRUPT) && __ENABLE_ENET_RECEIVE_INTERRUPT
    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(base);
    #endif

    return status_success;
}

int main(void)
{
    /* Initialize bsp */
    bsp_init();

    xTaskCreate(Main_task, "Main", configMINIMAL_STACK_SIZE * 2, NULL, MAIN_TASK_PRIO, NULL);

    /* Start scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    for ( ;; ) {

    }
}

void Main_task(void *pvParameters)
{
    (void)pvParameters;

    TimerHandle_t timer_handle;
    TaskFunction_t pxTaskCode[] = {netif0_update_link_status, netif1_update_link_status};
    char task_name[30] = {0};

    /* Initialize MAC and DMA */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        if (enet_init(i) == status_success) {
            printf("Enet%d init passed!\n", i);
        } else {
            printf("Enet%d init failed!\n", i);
        }
    }

    /* Initialize LwIP stack */
    tcpip_init(NULL, NULL);

    /* Initialize network setting, services and apps */
    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        netif_config(&gnetif[i], i);
        tcp_echo_init(&gnetif[i]);
    }

    #if defined(LWIP_DHCP) && LWIP_DHCP
    /* Start DHCP Client */
    xTaskCreate(LwIP_DHCP_task, "DHCP", configMINIMAL_STACK_SIZE * 2, gnetif, DHCP_TASK_PRIO, NULL);
    #endif

    for (uint8_t i = 0; i < BOARD_ENET_COUNT; i++) {
        sprintf(task_name, "Netif%d Link Status Update", i);
        xTaskCreate(pxTaskCode[i], task_name, configMINIMAL_STACK_SIZE * 2, &gnetif[i], NETIF_STA_TASK_PRIO, NULL);
    }

    timer_handle = xTimerCreate((const char *)NULL,
                                (TickType_t)1000,
                                (UBaseType_t)pdTRUE,
                                (void * const)1,
                                (TimerCallbackFunction_t)timer_callback);

    if (NULL != timer_handle)  {
        xTimerStart(timer_handle, 0);
    }

    vTaskDelete(NULL);
}