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
#include "queue.h"
#include "timers.h"

#include <stdio.h>
#include "board.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "usbh_core.h"
#include "usbh_rndis.h"
#include "rndis_protocol.h"
#include "iperf.h"

/* Macro Definition */
/*--------------- Tasks Priority -------------*/
#ifndef CONFIG_IPERF_TCP_SERVER_PORT
#define CONFIG_IPERF_TCP_SERVER_PORT    (5001U)
#endif
#define DHCP_TASK_PRIO                   (tskIDLE_PRIORITY + 2)
#define IPERF_TASK_PRIO                  (tskIDLE_PRIORITY + 3)

/* Static Variable Definition*/

TaskHandle_t print_dhcp_state_task_handle;
TaskHandle_t print_iperf_state_task_handle;
QueueHandle_t dhcp_state_queue;

/* Static Function Declaration */
static void print_dhcp_task(void *pdata);
static void iperf_task(void *pdata);

int main(void)
{
    board_init();
    board_init_usb_pins();

    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBH_IRQn, 1);

    printf("Start rndis host iperf task...\r\n");

    dhcp_state_queue = xQueueCreate(4, 1);

    if (dhcp_state_queue == NULL) {
        printf("crete dhcp_state_queue fail\n");
        while (1) {
        };
    }

    usbh_initialize();

    if (xTaskCreate(print_dhcp_task, "print_dhcp_task", configMINIMAL_STACK_SIZE + 256U,
                    NULL, DHCP_TASK_PRIO, &print_dhcp_state_task_handle) != pdPASS) {
        printf("print_dhcp_task task creation failed!.\n");
        for (;;) {
            ;
        }
    }

    if (xTaskCreate(iperf_task, "iperf_task", configMINIMAL_STACK_SIZE + 256U,
                    NULL, IPERF_TASK_PRIO, &print_iperf_state_task_handle) != pdPASS) {
        printf("iperf_task task creation failed!.\n");
        for (;;) {
            ;
        }
    }

    vTaskStartScheduler();

    return 0;
}

static void iperf_task(void *pdata)
{
    struct netif *netif;
    bool dhcp_check = false;
    bool ready_iperf = false;
    uint8_t dhcp_last_state = DHCP_STATE_OFF;
    uint8_t state;
    uint16_t i = 0;
    char buffer[100];
    char ch;
    while (1) {
        ready_iperf = false;
        i = 0;
        if (xQueueReceive(dhcp_state_queue, &state, 10) == pdPASS) {
            dhcp_last_state = state;
            if (dhcp_last_state == DHCP_STATE_BOUND) {
                netif = netif_find("EX");
                if (netif) {
                    printf("\r\n IPv4 Address     : %s\r\n", ipaddr_ntoa(&netif->ip_addr));
                    printf(" IPv4 Subnet mask : %s\r\n", ipaddr_ntoa(&netif->netmask));
                    printf(" IPv4 Gateway     : %s\r\n\r\n", ipaddr_ntoa(&netif->gw));
                    dhcp_check = true;
                } else {
                    dhcp_check = false;
                }
            } else {
                dhcp_check = false;
            }
        }
        if (dhcp_check == true) {
            printf("\n***********************************************************"
                   "**********************\n");
            printf("\r\ninput the iperf tcp server IP or URL address and "
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
                        ready_iperf = true;
                    }
                    break;
                }
                vTaskDelay(2);
            }
            if (ready_iperf) {
                buffer[i - 1] = 0;
                printf("clinet connecting to %s, TCP port %d\r\n\r\n", buffer, CONFIG_IPERF_TCP_SERVER_PORT);
                iperf(buffer, CONFIG_IPERF_TCP_SERVER_PORT);
            }
        }
    }
}

/*!
 * @brief Prints DHCP status of the interface when it has changed from last status.
 *
 * @param arg pointer to network interface structure
 */

static void print_dhcp_task(void *pdata)
{
    struct netif *netif;
    struct dhcp *dhcp;
    uint8_t dhcp_last_state = DHCP_STATE_OFF;

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
            vTaskDelay(1000);
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
                xQueueSend(dhcp_state_queue, &dhcp_last_state, 2);
            }
        }
    }
}


