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
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/icmp.h"
#include "lwip/udp.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "usbh_core.h"

/* Macro Definition */
/*--------------- Tasks Priority -------------*/
#define DHCP_TASK_PRIO            (tskIDLE_PRIORITY + 1)

#define RNDIS_ETH_BUFFER_LEN (sizeof(rndis_data_packet_t) + 1514 + 42)

/* Static Variable Definition*/
TaskHandle_t print_dhcp_state_task_handle;

/* Static Function Declaration */
static void print_dhcp_state(void *pdata);

/* Function Definition */

int main(void)
{
    board_init();
    board_init_usb_pins();

    /* set irq priority */
    intc_set_irq_priority(IRQn_USB0, 1);

    printf("Start cherryusb rndis host task...\r\n");

    usbh_initialize();

    if (xTaskCreate(print_dhcp_state, "print_dhcp_state", configMINIMAL_STACK_SIZE + 256U, NULL, DHCP_TASK_PRIO, &print_dhcp_state_task_handle) != pdPASS) {
        printf("print_dhcp_state task creation failed!.\n");
        for (;;) {
            ;
        }
    }

    vTaskStartScheduler();

    return 0;
}

/*!
 * @brief Prints DHCP status of the interface when it has changed from last status.
 *
 * @param arg pointer to network interface structure
 */
static void print_dhcp_state(void *pdata)
{
    struct dhcp *dhcp;
    struct netif *netif;
    u8_t dhcp_last_state = DHCP_STATE_OFF;

    printf("\r\n************************************************\r\n");
    printf(" DHCP example\r\n");
    printf("************************************************\r\n");

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
        if (netif_is_up(netif)) {
            dhcp = netif_dhcp_data(netif);

            if (dhcp == NULL) {
                dhcp_last_state = DHCP_STATE_OFF;
            } else if (dhcp_last_state != dhcp->state) {
                dhcp_last_state = dhcp->state;

                printf(" DHCP state       : ");
                switch (dhcp_last_state) {
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
                    printf("%u", dhcp_last_state);
                    assert(0);
                    break;
                }
                printf("\r\n");

                if (dhcp_last_state == DHCP_STATE_BOUND) {
                    printf("\r\n IPv4 Address     : %s\r\n", ipaddr_ntoa(&netif->ip_addr));
                    printf(" IPv4 Subnet mask : %s\r\n", ipaddr_ntoa(&netif->netmask));
                    printf(" IPv4 Gateway     : %s\r\n\r\n", ipaddr_ntoa(&netif->gw));
                    vTaskDelete(NULL);
                }
            }
        }
    }
}
