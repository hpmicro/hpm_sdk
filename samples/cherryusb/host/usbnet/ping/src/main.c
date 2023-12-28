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
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "usbh_core.h"
#include "usbh_rndis.h"
#include "rndis_protocol.h"
#include "ping.h"

/* Macro Definition */
/*--------------- Tasks Priority -------------*/

#ifndef CONFIG_PING_COUNT
#define CONFIG_PING_COUNT     (5U)
#endif

#define PING_TASK_PRIO          (tskIDLE_PRIORITY + 1)

/* Static Variable Definition*/
TaskHandle_t print_dhcp_state_task_handle;
TaskHandle_t print_ping_state_task_handle;

/* Static Function Declaration */
static void printf_dhcp_state(uint8_t sta);
static void ping_task(void *pdata);

int main(void)
{
    board_init();
    board_init_usb_pins();

    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBH_IRQn, 1);

    printf("Start usbnet host ping task...\r\n");

    /* Initialize the LwIP stack */
    tcpip_init(NULL, NULL);

    usbh_initialize();

    if (xTaskCreate(ping_task, "ping_task", configMINIMAL_STACK_SIZE + 256U,
                    NULL, PING_TASK_PRIO, &print_ping_state_task_handle) != pdPASS) {
        printf("ping_task task creation failed!.\n");
        for (;;) {
            ;
        }
    }

    vTaskStartScheduler();

    return 0;
}

static void printf_dhcp_state(uint8_t sta)
{
    printf(" DHCP state       : ");
    switch (sta) {
    case DHCP_STATE_OFF:
        printf("OFF");
        break;
    case DHCP_STATE_REQUESTING:
        printf("REQUESTING");
        break;
    case DHCP_STATE_INIT:
        printf("INIT");
        break;
    case DHCP_STATE_REBOOTING:
        printf("REBOOTING");
        break;
    case DHCP_STATE_REBINDING:
        printf("REBINDING");
        break;
    case DHCP_STATE_RENEWING:
        printf("RENEWING");
        break;
    case DHCP_STATE_SELECTING:
        printf("SELECTING");
        break;
    case DHCP_STATE_INFORMING:
        printf("INFORMING");
        break;
    case DHCP_STATE_CHECKING:
        printf("CHECKING");
        break;
    case DHCP_STATE_BOUND:
        printf("BOUND");
        break;
    case DHCP_STATE_BACKING_OFF:
        printf("BACKING_OFF");
        break;
    default:
        printf("%u", sta);
        assert(0);
        break;
    }
    printf("\r\n");
}

static void ping_task(void *pdata)
{
    struct netif *netif = (struct netif *)pdata;
    bool dhcp_check = false;
    bool ready_ping = false;
    uint8_t dhcp_last_state = DHCP_STATE_OFF;
    uint16_t i = 0;
    char buffer[50];
    char ch;
    struct dhcp *dhcp;

     while (1) {
        netif = netif_find("EX");
        if (!netif) {
            vTaskDelay(2);
            continue;
        }
        break;
    }

    dhcp_start(netif);

    while (1) {
        ready_ping = false;
        i = 0;
        if (netif_is_up(netif)) {
            dhcp = netif_dhcp_data(netif);
            if (dhcp == NULL) {
                dhcp_last_state = DHCP_STATE_OFF;
            } else if (dhcp_last_state != dhcp->state) {
                dhcp_last_state = dhcp->state;
                printf_dhcp_state(dhcp_last_state);
                if (dhcp_last_state == DHCP_STATE_BOUND) {
                    printf("\r\n IPv4 Address     : %s\r\n", ipaddr_ntoa(&netif->ip_addr));
                    printf(" IPv4 Subnet mask : %s\r\n", ipaddr_ntoa(&netif->netmask));
                    printf(" IPv4 Gateway     : %s\r\n\r\n", ipaddr_ntoa(&netif->gw));
                    dhcp_check = true;
                } else {
                    dhcp_check = false;
                }
            }
            if (dhcp_check == true) {
                printf("\n***********************************************************"
                        "**********************\n");
                printf("\r\ninput ping the IP or URL address and "
                        "press the enter key to end\r\n\r\n");
                printf("if want to terminate midway, please press the esc key\r\n\r\n");
                printf("***********************************************************"
                        "**********************\n");
                while (1) {
                    ch = getchar();
                    putchar(ch);
                    buffer[i++] = ch;
                    /*esc key*/
                    if (ch == 0x1B) {
                        break;
                    }
                    /* enter key*/
                    if (ch == 0x0d) {
                        if (i > 5) {
                            ready_ping = true;
                        }
                        break;
                    }
                    vTaskDelay(2);
                }
                if (ready_ping) {
                    buffer[i - 1] = 0;
                    ping(buffer, CONFIG_PING_COUNT, 0);
                }
            }
        }
        vTaskDelay(10);
    }
}
