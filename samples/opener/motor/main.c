/*
 * Copyright (c) 2024-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include "cmsis_os.h"

/*  HPM example includes. */
#include <stdio.h>
#include "common.h"
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "bldc_foc.h"

/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO      (tskIDLE_PRIORITY + 2)
#define DHCP_TASK_PRIO      (tskIDLE_PRIORITY + 4)
#define NETIF_STA_TASK_PRIO (tskIDLE_PRIORITY + 4)

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_rx_desc_t dma_rx_desc_tab[ENET_RX_BUFF_COUNT]; /* Ethernet Rx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_DESC_ADDR_ALIGNMENT)
__RW enet_tx_desc_t dma_tx_desc_tab[ENET_TX_BUFF_COUNT]; /* Ethernet Tx DMA Descriptor */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t rx_buff[ENET_RX_BUFF_COUNT][ENET_RX_BUFF_SIZE]; /* Ethernet Receive Buffer */

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ENET_SOC_BUFF_ADDR_ALIGNMENT)
__RW uint8_t tx_buff[ENET_TX_BUFF_COUNT][ENET_TX_BUFF_SIZE]; /* Ethernet Transmit Buffer */

enet_desc_t desc;
uint8_t mac[ENET_MAC_SIZE];
struct netif gnetif;

void Main_task(void *pvParameters);

void bsp_init(void)
{
    /* Initialize BSP */
    board_init();

    board_init_gpio_pins();

    board_init_led_pins();

    /* Initialize GPIOs */
    board_init_enet_pins(ENET);

    /* Reset an enet PHY */
    board_reset_enet_phy(ENET);

    printf("This is an Ethernet/IP demo.\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);

    #if defined(HPM_ENET_RGMII) && HPM_ENET_RGMII
    /* Set RGMII clock delay */
    board_init_enet_rgmii_clock_delay(ENET);
    #elif defined(HPM_ENET_RMII) && HPM_ENET_RMII
    /* Set RMII reference clock */
    board_init_enet_rmii_reference_clock(ENET, BOARD_ENET_RMII_INT_REF_CLK);
    printf("Reference Clock: %s\n", BOARD_ENET_RMII_INT_REF_CLK ? "Internal Clock" : "External Clock");
    #endif
}

hpm_stat_t enet_init(ENET_Type *ptr)
{
    enet_int_config_t int_config = {.int_enable = 0, .int_mask = 0};
    enet_mac_config_t enet_config;
    enet_tx_control_config_t enet_tx_control_config;

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

    /* Enable Enet IRQ */
    board_enable_enet_irq(ENET);

    /* Get the default interrupt config */
    enet_get_default_interrupt_config(ENET, &int_config);

    /* Initialize enet controller */
    if (enet_controller_init(ptr, ENET_INF_TYPE, &desc, &enet_config, &int_config) != status_success) {
        return status_fail;
    }

    /* Disable LPI interrupt */
    enet_disable_lpi_interrupt(ENET);

    if (board_init_enet_phy(ptr) == status_success) {
        printf("Enet phy init passed !\n");
        return status_success;
    } else {
        printf("Enet phy init failed !\n");
        return status_fail;
    }

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
    setup_moter();
    /* Initialize MAC and DMA */
    enet_init(ENET);

    /* Initialize LwIP stack */
    tcpip_init(NULL, NULL);
    netif_config(&gnetif);

#if defined(LWIP_DHCP) && LWIP_DHCP
    /* Start DHCP Client */
    xTaskCreate(LwIP_DHCP_task, "DHCP", configMINIMAL_STACK_SIZE * 2, &gnetif, DHCP_TASK_PRIO, NULL);
#endif

    xTaskCreate(netif_update_link_status, "netif update status", configMINIMAL_STACK_SIZE * 2, &gnetif, NETIF_STA_TASK_PRIO, NULL);

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