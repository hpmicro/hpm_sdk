/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_gpio_drv.h"
#include "hpm_ipc_event_mgr.h"
#include "hpm_l1c_drv.h"
#include "hpm_misc.h"
#include "hpm_pmp_drv.h"
#include "hpm_sysctl_drv.h"
#include "erpc_client_setup.h"
#include "erpc_motor.h"
#include "rpmsg_lite.h"
#include "multicore_common.h"
#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include "cmsis_os.h"

/*  HPM example includes. */
#include <stdio.h>
#include "common.h"
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"

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
uint8_t mac[ENET_MAC];
struct netif gnetif;

volatile float target_speed;
volatile float current_speed;
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

    #if defined(RGMII) && RGMII
    /* Set RGMII clock delay */
    board_init_enet_rgmii_clock_delay(ENET);
    #elif defined(RMII) && RMII
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

    #if defined(RGMII) && RGMII
        #if defined(__USE_DP83867) && __USE_DP83867
        dp83867_config_t phy_config;
        #else
        rtl8211_config_t phy_config;
        #endif
    #else
        #if defined(__USE_DP83848) && __USE_DP83848
        dp83848_config_t phy_config;
        #else
        rtl8201_config_t phy_config;
        #endif
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

    /* Initialize phy */
    #if defined(RGMII) && RGMII
        #if defined(__USE_DP83867) && __USE_DP83867
        dp83867_reset(ptr);
        #if defined(__DISABLE_AUTO_NEGO) && __DISABLE_AUTO_NEGO
        dp83867_set_mdi_crossover_mode(ENET, enet_phy_mdi_crossover_manual_mdix);
        #endif
        dp83867_basic_mode_default_config(ptr, &phy_config);
        if (dp83867_basic_mode_init(ptr, &phy_config) == true) {
        #else
        rtl8211_reset(ptr);
        rtl8211_basic_mode_default_config(ptr, &phy_config);
        if (rtl8211_basic_mode_init(ptr, &phy_config) == true) {
        #endif
    #else
        #if defined(__USE_DP83848) && __USE_DP83848
        dp83848_reset(ptr);
        dp83848_basic_mode_default_config(ptr, &phy_config);
        if (dp83848_basic_mode_init(ptr, &phy_config) == true) {
        #else
        rtl8201_reset(ptr);
        rtl8201_basic_mode_default_config(ptr, &phy_config);
        if (rtl8201_basic_mode_init(ptr, &phy_config) == true) {
        #endif
    #endif
            printf("Enet phy init passed !\n");
            return status_success;
        } else {
            printf("Enet phy init failed !\n");
            return status_fail;
        }
}
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ERPC_TRANSPORT_RPMSG_LITE_LINK_ID (RL_PLATFORM_HPM6XXX_DUAL_CORE_LINK_ID)
#define MATRIX_ITEM_MAX_VALUE (50)
#define APP_ERPC_READY_EVENT_DATA (1U)

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile uint16_t eRPCReadyEventData;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void init_button(void)
{
    gpio_set_pin_input(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
}

/*!
 * @brief eRPC server side ready event handler
 */
static void eRPCReadyEventHandler(uint16_t eventData, void *context)
{
    (void)context;
    eRPCReadyEventData = eventData;
}

static void client_task(void *param)
{
    (void)param;

    /* Register the application event before starting the secondary core */
    (void)ipc_register_event(ipc_remote_start_event, eRPCReadyEventHandler, NULL);

    (void)printf("\r\nPrimary core started\r\n");
    multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);
    printf("Starting secondary core...\r\n");

    /*
     * Wait until the secondary core application signals the rpmsg remote has
     * been initialized and is ready to communicate.
     */
    while (APP_ERPC_READY_EVENT_DATA != eRPCReadyEventData) {
    };

    printf("\r\nSecondary core started...\r\n");

    /* RPMsg-Lite transport layer initialization */
    erpc_transport_t transport;

    transport = erpc_transport_rpmsg_lite_rtos_master_init(100, 101, ERPC_TRANSPORT_RPMSG_LITE_LINK_ID);

    /* MessageBufferFactory initialization */
    erpc_mbf_t message_buffer_factory;
    message_buffer_factory = erpc_mbf_rpmsg_init(transport);

    /* eRPC client side initialization */
    erpc_client_init(transport, message_buffer_factory);

    vTaskDelay(8000);
    for (;;) {
        (void)printf("\r\neRPC request is sent to the server\r\n");

        erpcSetMotorSpeed(target_speed);
        current_speed = erpcGetMotorSpeed();

        vTaskDelay(1000);
    }
}

void Main_task(void *pvParameters)
{
    (void)pvParameters;

    TimerHandle_t timer_handle;

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
    if (xTaskCreate(client_task, "APP_TASK", configMINIMAL_STACK_SIZE + 256U, NULL, tskIDLE_PRIORITY + 2U, NULL) != pdPASS) {
        assert(0);
    }

    vTaskDelete(NULL);
}
/*!
 * @brief Main function
 */
int main(void)
{
    board_init();
    board_init_gpio_pins();
    bsp_init();
    init_button();
    ipc_init();
    ipc_enable_event_interrupt(2);

    xTaskCreate(Main_task, "Main", configMINIMAL_STACK_SIZE * 2, NULL, MAIN_TASK_PRIO, NULL);

    /* Start scheduler */

    vTaskStartScheduler();
}
