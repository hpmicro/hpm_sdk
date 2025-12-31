/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "stdio.h"
#include "modbus_application.h"
#include "modbus_common_def.h"
#include "modbus_tcp_server.h"
#include "agile_modbus.h"
#include "agile_modbus_tcp.h"
#include "agile_modbus_slave_util.h"

#define TIEMOUT_COUNT                 (3)
#define APP_MIN_RECV_LEN              (8U)

static int addr_check(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info);
static int modbus_slave_process(uint8_t *status);
void print_hold_register_val(void);

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


void modbus_tcp_server_init(void)
{
    agile_modbus_t *ctx = &ctx_tcp._ctx;
    seria_status = recv_wait;

    if (network_tcp_server_init() != status_success) {
        printf("ModBus TCP Server init fail\n");
        return;
    }

    agile_modbus_tcp_init(&ctx_tcp, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));
    agile_modbus_set_slave(ctx, MODBUS_SLAVE_ADDRESS);

    printf("ModBus TCP Server init success, slave address: %d\n", MODBUS_SLAVE_ADDRESS);
}

void modbus_tcp_server_process(void)
{
    if (modbus_slave_process((uint8_t *)&seria_status) < 0) {
        seria_status = recv_wait;
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
            print_hold_register_val();
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

void print_hold_register_val(void)
{
      uint16_t register_val[10];
      hold_register_maps[0].get(register_val, 10);

      printf("modbus hold registers val: ");
      for (uint16_t i = 0; i < 10; i++) {
          printf("%d ", register_val[i]);
      }
      printf("\n");
}

