/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "timer.h"
#include "serial.h"
#include "common_def.h"
#include "agile_modbus.h"
#include "agile_modbus_slave_util.h"

#define TIEMOUT_COUNT                 (3)
#define APP_TIMER_MS                  (20)  /* 20ms */
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
ATTR_PLACE_AT_NONCACHEABLE uint8_t serial_rx_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
ATTR_PLACE_AT_NONCACHEABLE agile_modbus_rtu_t ctx_rtu;
static volatile uint8_t seria_status;
volatile bool time_flag;

void timer_callback(void)
{
    time_flag = true;
}

int main(void)
{
    hpm_timer_t timer;
    agile_modbus_t *ctx = &ctx_rtu._ctx;
    timer.timing_ms = APP_TIMER_MS;
    timer.callback = timer_callback;
    seria_status = recv_wait;
    time_flag = false;
    board_init();
    serial_uart_init();
    timer_init();
    if (timer_register(&timer) != status_success) {
        printf("timer register fail\n");
    }
    printf("modbus rtu slave example\n");
    agile_modbus_rtu_init(&ctx_rtu, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));
    agile_modbus_set_slave(ctx, 1);
    while (1) {
        if (time_flag == true) {
            time_flag = false;
            if (modbus_slave_process((uint8_t *)&seria_status) < 0) {
                seria_status = recv_wait;
            }
        }
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
    int sta = 0, rc = 0;
    static uint8_t count = 0;
    uint8_t index = 0;
    switch (*status) {
    case recv_wait:
        recv_len = serial_uart_receive(serial_rx_buf, APP_MIN_RECV_LEN);
        if (recv_len < 0) {
            sta = -1;
        } else if (recv_len > 0) {
            (*status) = recv_finsh;
        }
        break;
    case recv_finsh:
        index = 0;
        rc = -1;
        while (recv_len >= (int)APP_MIN_RECV_LEN) {
            if (ctx_rtu._ctx.backend->check_integrity(&ctx_rtu._ctx, &serial_rx_buf[index], recv_len) > 0) {
                if (serial_rx_buf[index] != ctx_rtu._ctx.slave) {
                    return -2;
                }
                memcpy(ctx_rtu._ctx.read_buf, &serial_rx_buf[index], recv_len);
                rc = agile_modbus_slave_handle(&ctx_rtu._ctx, recv_len, 0, agile_modbus_slave_util_callback, &slave_util, NULL);
                break;
            } else {
                index++;
                recv_len--;
            }
        }
        if (rc > 0) {
            printf("parse ok, recv master msg len:%d\n", recv_len);
            serial_uart_send(ctx_rtu._ctx.send_buf, rc);
            (*status) = send_wait;
        } else {
            printf("parse failed code:%d\n", rc);
            sta = -3;
        }
        break;
    case send_wait:
        if (serial_uart_send_finsh() == false) {
            count++;
            if (count > TIEMOUT_COUNT) {
                count = 0;
                sta = -4;
                printf("send fail\n");
            }
        } else {
            count = 0;
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
    default:
        break;
    }
    return sta;
}
