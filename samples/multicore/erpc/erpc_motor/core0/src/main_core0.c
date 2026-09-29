/*
 * Copyright (c) 2022-2026 HPMicro
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
#include <stdio.h>
#include "cmsis_os.h"

/*  HPM example includes. */
#include "common.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netconf.h"
#include "opener.h"


/*--------------- Tasks Priority -------------*/
#define MAIN_TASK_PRIO      osPriorityLow
#define DHCP_TASK_PRIO      osPriorityBelowNormal
#define NETIF_STA_TASK_PRIO osPriorityBelowNormal
#define APP_TASK_PRIO       (osPriorityAboveNormal + 5U) /* higher than OpENer thread (osPriorityAboveNormal=32) */

volatile float target_speed;
volatile float current_speed;
void Main_task(void *pvParameters);

static void netif_link_callback(struct netif *netif)
{
    if (netif_is_link_up(netif)) {
        opener_init(netif);
    }
}

void bsp_init(void)
{
    board_init_gpio_pins();

    board_init_led_pins();

    printf("This is an Ethernet/IP demo.\n");
    printf("LwIP Version: %s\n", LWIP_VERSION_STRING);
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
osSemaphoreId_t xMotorSemaphore;

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

    float last_target_speed = target_speed;
    for (;;) {
        if (target_speed != last_target_speed) {
            (void)printf("\r\ntarget speed change to %f\r\n", target_speed);
        }

        erpcSetMotorSpeed(target_speed);
        current_speed = erpcGetMotorSpeed();
        last_target_speed = target_speed;

        osSemaphoreAcquire(xMotorSemaphore, osWaitForever);
    }
}

void Main_task(void *pvParameters)
{
    (void)pvParameters;

    osTimerId_t timer_handle;

    /* Initialize GPIOs, clock, MAC(DMA) and PHY */
    enet_init(ENET);

    /* Initialize LwIP stack */
    tcpip_init(NULL, NULL);
    netif_config(&gnetif);

    /* Override the link callback to directly call opener_init on link up */
    netif_set_link_callback(&gnetif, netif_link_callback);

    /* Start services */
    enet_services(&gnetif);

#if defined(LWIP_DHCP) && LWIP_DHCP
    /* Start DHCP Client */
    {
        const osThreadAttr_t dhcp_attr = {
            .name = "DHCP",
            .stack_size = configMINIMAL_STACK_SIZE * 2U * 4U,
            .priority = DHCP_TASK_PRIO,
        };
        (void)osThreadNew(LwIP_DHCP_task, &gnetif, &dhcp_attr);
    }
#endif

    {
        const osThreadAttr_t netif_attr = {
            .name = "netif update status",
            .stack_size = configMINIMAL_STACK_SIZE * 2U * 4U,
            .priority = NETIF_STA_TASK_PRIO,
        };
        (void)osThreadNew(netif_update_link_status, &gnetif, &netif_attr);
    }

    timer_handle = osTimerNew((osTimerFunc_t)timer_callback, osTimerPeriodic, (void *)1, NULL);
    if (NULL != timer_handle) {
        osTimerStart(timer_handle, 1000U);
    }
    xMotorSemaphore = osSemaphoreNew(1U, 0U, NULL);
    if (NULL == xMotorSemaphore) {
        assert(0);
    }
    {
        const osThreadAttr_t app_attr = {
            .name = "APP_TASK",
            .stack_size = (configMINIMAL_STACK_SIZE + 256U) * 4U,
            .priority = APP_TASK_PRIO,
        };
        if (osThreadNew(client_task, NULL, &app_attr) == NULL) {
            assert(0);
        }
    }

    osThreadExit();
}
/*!
 * @brief Main function
 */
int main(void)
{
    board_init();
    bsp_init();
    init_button();
    ipc_init();
    ipc_enable_event_interrupt(2);

    osKernelInitialize();
    {
        const osThreadAttr_t main_attr = {
            .name = "Main",
            .stack_size = configMINIMAL_STACK_SIZE * 2U * 4U,
            .priority = MAIN_TASK_PRIO,
        };
        (void)osThreadNew(Main_task, NULL, &main_attr);
    }

    /* Start scheduler */
    osKernelStart();
}
