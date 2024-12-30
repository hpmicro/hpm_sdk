/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>
#include "board.h"
#include "hpm_uart_drv.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "usbh_core.h"
#include "ping.h"

/* Macro Definition */
/*--------------- Tasks Priority -------------*/

#ifndef CONFIG_PING_COUNT
#define CONFIG_PING_COUNT     (5U)
#endif

/* Static Function Declaration */
static void ping_task(void *pdata);

int main(void)
{
    board_init();
    board_init_usb((USB_Type *)CONFIG_HPM_USBH_BASE);

    /* set irq priority */
    intc_set_irq_priority(CONFIG_HPM_USBH_IRQn, 1);

    printf("Start usbnet host ping task...\r\n");

    /* Initialize the LwIP stack */
    tcpip_init(NULL, NULL);

    usbh_initialize(0, CONFIG_HPM_USBH_BASE);

    if (usb_osal_thread_create("ping_task", 2048, CONFIG_USBHOST_PSC_PRIO + 2, ping_task, NULL) == NULL) {
        printf("ping_task task creation failed!.\n");
        for (;;) {
            ;
        }
    }

    vTaskStartScheduler();

    return 0;
}

static void ping_task(void *pdata)
{
    (void)pdata;

    struct netif *netif;
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

    while (1) {
        ready_ping = false;
        i = 0;
        if (netif_is_up(netif)) {
            dhcp = netif_dhcp_data(netif);
            if (dhcp == NULL) {
                dhcp_last_state = DHCP_STATE_OFF;
            } else if (dhcp_last_state != dhcp->state) {
                dhcp_last_state = dhcp->state;
                if (dhcp_last_state == DHCP_STATE_BOUND) {
                    dhcp_check = true;
                } else {
                    dhcp_check = false;
                }
            }
            if (dhcp_check == true) {
                vTaskDelay(5);
                printf("\n***********************************************************"
                        "**********************\n");
                printf("\r\ninput ping the IP or URL address and "
                        "press the enter key to end\r\n\r\n");
                printf("if want to terminate midway, please press the esc key\r\n\r\n");
                printf("***********************************************************"
                        "**********************\n");
                while (1) {
                    if (uart_check_status(BOARD_CONSOLE_UART_BASE, uart_stat_data_ready) == true) {
                        ch = uart_read_byte(BOARD_CONSOLE_UART_BASE);
                        buffer[i++] = ch;
                        /*esc key*/
                        if (ch == 0x1B) {
                            break;
                        } else {
                            uart_send_byte(BOARD_CONSOLE_UART_BASE, ch);
                        }
                        /* enter key*/
                        if (ch == 0x0d) {
                            if (i > 5) {
                                ready_ping = true;
                            }
                            break;
                        }
                    }
                    if (netif_is_up(netif) == false) {
                        dhcp_check = false;
                        printf("netif is down\n");
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
