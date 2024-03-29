/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "serial.h"
#include "timer.h"
#include "common_def.h"
#include "agile_modbus.h"

#define TIEMOUT_COUNT                 (3U)   /*1000ms*/
#define HOLD_REGISTERS_COUNT          (10U)
#define APP_TIMER_MS                  (10U)    /* 10ms */
#define APP_RESTART_MS                (100U)   /*1000ms*/
#define APP_MIN_RECV_LEN              (8U)

static int modbus_master_process(uint8_t *status);

ATTR_PLACE_AT_NONCACHEABLE uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
ATTR_PLACE_AT_NONCACHEABLE uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
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
    seria_status = send_start;
    time_flag = false;
    board_init();
    serial_uart_init();
    timer_init();
    if (timer_register(&timer) != status_success) {
        printf("timer register fail\n");
    }
    printf("modbus rtu master example\n");
    agile_modbus_rtu_init(&ctx_rtu, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));
    agile_modbus_set_slave(ctx, 1);
    while (1) {
        if (time_flag == true) {
            time_flag = false;
            if (modbus_master_process((uint8_t *)&seria_status) < 0) {
                seria_status = send_start;
                printf("restart send.....\n");
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
        send_len = agile_modbus_serialize_read_registers(&ctx_rtu._ctx, 0, HOLD_REGISTERS_COUNT);
        serial_uart_send(ctx_rtu._ctx.send_buf, send_len);
        (*status) = send_wait;
        break;
    case send_wait:
        if (serial_uart_send_finsh() == false) {
            count++;
            if (count > TIEMOUT_COUNT) {
                count = 0;
                sta = -1;
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
    case recv_wait:
        recv_len = serial_uart_receive(ctx_rtu._ctx.read_buf, APP_MIN_RECV_LEN);
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
        rc = agile_modbus_deserialize_read_registers(&ctx_rtu._ctx, recv_len, hold_register);
        if (rc < 0) {
            printf("parse failed, slave msg len:%d\n", recv_len);
            if (rc != -1) {
                printf("Error code:%d\n", -128 - rc);
            }
            sta = -4;
        } else {
            if (count == 0) {
                printf("recv slave msg len:%d\n", recv_len);
                printf("Hold Registers[0~%d]:", HOLD_REGISTERS_COUNT - 1);
                for (uint32_t i = 0; i < HOLD_REGISTERS_COUNT; i++) {
                    printf("0x%04X ", i, hold_register[i]);
                }
                printf("\r\n\r\n");
            }
            count++;
            if (count > APP_RESTART_MS) {
                (*status) = send_start;
            }
        }
        break;
    default:
        break;
    }
    return sta;
}