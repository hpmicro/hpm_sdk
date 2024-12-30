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
#include "network.h"
#include "tcp_server.h"
#include "common_def.h"
#include "agile_modbus.h"
#include "agile_modbus_slave_util.h"

#define TIEMOUT_COUNT                 (3)
#define APP_TIMER_MS                  (2)  /* 2ms */
#define APP_MIN_RECV_LEN              (8U)

static int addr_check(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info);
static int modbus_slave_process(uint8_t *status);

extern const agile_modbus_slave_util_map_t hold_register_maps[1];

const agile_modbus_slave_util_t slave_util = {
    NULL,
    0,
    NULL,
    0,
    hold_register_maps,
    sizeof(hold_register_maps) / sizeof(hold_register_maps[0]),
    NULL,
    0,
    addr_check,
    NULL,
    NULL
};

ATTR_PLACE_AT_NONCACHEABLE uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
ATTR_PLACE_AT_NONCACHEABLE uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
ATTR_PLACE_AT_NONCACHEABLE agile_modbus_tcp_t ctx_tcp;
static volatile uint8_t seria_status;
static volatile bool time_flag;

void time_callback(void)
{
    time_flag = true;
}

int main(void)
{
    hpm_timer_t timer;
    agile_modbus_t *ctx = &ctx_tcp._ctx;
    timer.timing_ms = APP_TIMER_MS;
    timer.callback = time_callback;
    seria_status = recv_wait;
    time_flag = false;

    board_init();
    if (network_init() != status_success) {
        printf("network init fail\n");
        while (1) {
        };
    }
    if (network_tcp_server_init() != status_success) {
        printf("network tcp client init fail\n");
        while (1) {
        };
    }
    timer_init();
    if (timer_register(&timer) != status_success) {
        printf("timer register fail\n");
    }
    printf("modbus tcp slave example\n");
    agile_modbus_tcp_init(&ctx_tcp, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));
    agile_modbus_set_slave(ctx, 1);

    while (1) {
        if (time_flag == true) {
            time_flag = false;
            if (modbus_slave_process((uint8_t *)&seria_status) < 0) {
                seria_status = recv_wait;
            }
        }
        enet_common_handler(&gnetif);
    }
}

static int addr_check(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info)
{
    int slave = slave_info->sft->slave;
    if ((slave != ctx->slave) && (slave != AGILE_MODBUS_BROADCAST_ADDRESS) && (slave != 0xFF))
        return -AGILE_MODBUS_EXCEPTION_UNKNOW;

    return 0;
}

static int modbus_slave_process(uint8_t *status)
{
    static int recv_len = 0;
    volatile int sta = 0, rc = 0;
    static uint8_t count = 0;
    switch (*status) {
    case recv_wait:
        recv_len = network_tcp_s_receive(ctx_tcp._ctx.read_buf, APP_MIN_RECV_LEN);
        if (recv_len < 0) {
            sta = -1;
        } else if (recv_len > 0) {
            (*status) = recv_finsh;
        }
        break;
    case recv_finsh:
        rc = -1;
        rc = agile_modbus_slave_handle(&ctx_tcp._ctx, recv_len, 0, agile_modbus_slave_util_callback, &slave_util, NULL);
        if (rc > 0) {
            printf("parse ok, recv master msg len:%d\n", recv_len);
            network_tcp_s_send(ctx_tcp._ctx.send_buf, rc);
            (*status) = send_finsh;
        } else {
            printf("parse failed code:%d\n", rc);
            sta = -3;
        }
        break;
    case send_finsh:
        count++;
        if (count > TIEMOUT_COUNT) {
            count = 0;
            (*status) = recv_wait;
        }
        break;
    default:
        break;
    }
    return sta;
}

