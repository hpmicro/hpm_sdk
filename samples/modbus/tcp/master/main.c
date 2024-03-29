/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "common_lwip.h"
#include "netconf.h"
#include "timer.h"
#include "common_def.h"
#include "network.h"
#include "tcp_client.h"
#include "agile_modbus.h"

#define TIEMOUT_COUNT                 (200U)   /*1000ms*/
#define HOLD_REGISTERS_COUNT          (10U)
#define APP_TIMER_MS                  (5U)    /* 10ms */
#define APP_BEGIN_PROCESS_MS          (100U)   /*500ms*/
#define APP_MIN_RECV_LEN              (8U)

static int modbus_master_process(uint8_t *status);

ATTR_PLACE_AT_NONCACHEABLE uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
ATTR_PLACE_AT_NONCACHEABLE uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
ATTR_PLACE_AT_NONCACHEABLE agile_modbus_tcp_t ctx_tcp;
static uint8_t tcp_status;
bool time_flag;

void time_callback(void)
{
    time_flag = true;
}


int main(void)
{
    hpm_timer_t timer;
    uint8_t count = 0;
    agile_modbus_t *ctx = &ctx_tcp._ctx;
    timer.timing_ms = APP_TIMER_MS;
    timer.callback = time_callback;

    board_init();
    if (network_init() != status_success) {
        printf("network init fail\n");
        while (1) {
        };
    }
    if (network_tcp_client_init() != status_success) {
        printf("network tcp client init fail\n");
        while (1) {
        };
    }
    timer_init();
    if (timer_register(&timer) != status_success) {
        printf("timer register fail\n");
    }
    printf("modbus tcp master example\n");
    agile_modbus_tcp_init(&ctx_tcp, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));
    agile_modbus_set_slave(ctx, 1);

    while (1) {
        enet_common_handler(&gnetif);
        if (time_flag == true) {
            time_flag = false;
            if ((tcp_client_connect_state() == true) && (count < APP_BEGIN_PROCESS_MS)) {
                count++;
                continue;
            }
            if (modbus_master_process(&tcp_status) < 0) {
                tcp_status = send_start;
            }
        }
    }
}

static int modbus_master_process(uint8_t *status)
{
    int send_len = 0;
    int sta = 0, rc = 0;
    uint16_t hold_register[HOLD_REGISTERS_COUNT];
    static uint32_t count = 0;
    static int recv_len = 0;
    switch (*status) {
    case send_start:
        send_len = agile_modbus_serialize_read_registers(&ctx_tcp._ctx, 0, HOLD_REGISTERS_COUNT);
        if (network_tcp_c_send(ctx_tcp._ctx.send_buf, send_len) >= 0) {
            (*status) = send_finsh;
        }
        break;
    case send_finsh:
        count++;
        if (count > TIEMOUT_COUNT) {
            count = 0;
            (*status) = recv_wait;
        }
        break;
    case recv_wait:
        recv_len = network_tcp_c_receive(ctx_tcp._ctx.read_buf, APP_MIN_RECV_LEN);
        if (recv_len < 0) {
            sta = -2;
            printf("recv fail\n");
        } else if (recv_len == 0) {
            count++;
            if (count > TIEMOUT_COUNT) {
                count = 0;
                sta = -3;
                printf("recv timeout\n");
            }
        } else {
            count = 0;
            (*status) = recv_finsh;
        }
        break;
    case recv_finsh:
        rc = agile_modbus_deserialize_read_registers(&ctx_tcp._ctx, recv_len, hold_register);
        if (rc < 0) {
            printf("parse failed, slave msg len:%d\n", recv_len);
            if (rc != -1) {
                printf("Error code:%d\n", -128 - rc);
            }
            sta = -4;
        } else {
            printf("recv slave msg len:%d\n", recv_len);
            printf("Hold Registers[0~%d]:", HOLD_REGISTERS_COUNT - 1);
            for (uint32_t i = 0; i < HOLD_REGISTERS_COUNT; i++) {
                printf("0x%04X ", i, hold_register[i]);
            }
            printf("\r\n\r\n");
            (*status) = send_start;
        }
        break;
    default:
        break;
    }
    return sta;
}
