/*
 * Copyright (c) 2024,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "common.h"
#include "ping_thread.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "netconf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ping.h"

static void do_backspace(uint8_t *p, uint8_t *idx)
{
    if (*idx > 0) {
        (*idx)--;
        p[*idx] = '\0';
    }
}

static void ping_fetch_user_command(uint8_t *p)
{
    uint8_t cmd_str_idx = 0;
    uint8_t ch;

    while (1) {
        ch = console_try_receive_byte();

        if (ch != 0) {
            if (ch == 0x1b) {
                /* consume ANSI escape sequence to avoid garbage in buffer */
                for (int i = 0; i < 12; i++) {
                    board_delay_us(100);
                    ch = console_try_receive_byte();
                    if (ch != 0 && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '~')) {
                        break;
                    }
                }
                continue;
            }
            console_send_byte(ch);
            if (ch == '\r') {
                console_send_byte('\n');
            }

            if (isprint(ch)) {
                p[cmd_str_idx++] = ch;
            } else if (ch == 0x7f || ch == 0x08) {
                do_backspace(p, &cmd_str_idx);
            } else if (ch == '\r') {
                if (cmd_str_idx != 0) {
                    p[cmd_str_idx] = '\0';
                    break;
                }
            } else {
                continue;
            }
        }
    }
}

static void ping_thread(void *arg)
{
    struct netif *netif = (struct netif *)arg;
    uint8_t cmd_str_buff[100];

    while (!netif_is_link_up(netif)) {
        vTaskDelay(100);
    }

    while (1) {
#if defined(LWIP_DHCP) && LWIP_DHCP
        while (!enet_get_dhcp_ready_status()) {
            vTaskDelay(100);
        }
#endif
        printf("Please enter a target name(IP or URL):\n");
        ping_fetch_user_command(cmd_str_buff);
        ping((char *)cmd_str_buff, 4, 0);
    }
}

void ping_init(void *arg)
{
    sys_thread_new("ping_thread", ping_thread, arg, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
