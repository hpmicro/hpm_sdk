/*
 * Copyright (c) 2025-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "hpm_mchtmr_drv.h"
#include "CANopen.h"
#include "hpm_canopen.h"
#include "user_config.h"
#include "OD.h"

#define MASTER_ID             1
#define SDO_TIMEOUT_MS        1000
#define BITRATE               500000
#define MCHTMR_CLK_NAME       (clock_mchtmr0)

#define MASTER_NMT_CONTROL    (CO_NMT_ERR_ON_ERR_REG | \
                               CO_ERR_REG_GENERIC_ERR | CO_ERR_REG_COMMUNICATION)

typedef struct {
    enum {
        M_STATE_IDLE,
        M_STATE_SDO_REQ,
        M_STATE_SDO_POLL,
        M_STATE_SEND_START
    } state;
    uint8_t  active_id;
    uint8_t  job_idx;
    char     str_buf[33];
    bool     node_initialized[128];
} master_bus_ctx_t;

typedef struct {
    uint16_t index;
    uint8_t sub_index;
    bool is_write;
    uint32_t write_val;
    const char *label;
} sdo_job_t;

static const sdo_job_t master_jobs[] = {
    {0x1000, 0, false, 0,   "Device Type"},
    {0x1008, 0, false, 0,   "Device Name"},
    {0x1009, 0, false, 0,   "HW Version "},
    {0x100A, 0, false, 0,   "SW Version "},
    {0x1017, 0, true,  500, "Set HB (ms) "}
};

#define TOTAL_JOBS (sizeof(master_jobs) / sizeof(sdo_job_t))

CO_t *co[MAX_CANOPEN_DEVICE];
master_bus_ctx_t g_bus_ctx[MAX_CANOPEN_DEVICE];
struct canopen_context canptr[MAX_CANOPEN_DEVICE];
hpm_can_config_t hpm_canopen_config[MAX_CANOPEN_DEVICE] = {0};
hpm_can_data_t hpm_canopen_data[MAX_CANOPEN_DEVICE] = {0};
struct device hpm_canopen_dev[MAX_CANOPEN_DEVICE] = {0};

#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
ATTR_PLACE_AT(".ahb_sram") uint32_t board_app_mcan_msg_buf[MCAN_MSG_BUF_SIZE_IN_WORDS];
#endif
can_info_t m_can_info[] = {
    {
        .can_base = BOARD_APP_CAN_BASE,
        .irq_num = BOARD_APP_CAN_IRQn,
        .priority = 1,
#if defined(MCAN_SOC_MSG_BUF_IN_AHB_RAM) && (MCAN_SOC_MSG_BUF_IN_AHB_RAM == 1)
        .ram_base = (uint32_t) &board_app_mcan_msg_buf,
        .ram_size = sizeof(board_app_mcan_msg_buf),
#endif
    },
    /* init more can base here */
};

SDK_DECLARE_EXT_ISR_M(BOARD_APP_CAN_IRQn, hpm_canopen_isr);
void hpm_canopen_isr(void)
{
    for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
        canopen_irq_handler((struct device *)&hpm_canopen_dev[i]);
    }
}

void run_canopen_master_task(uint8_t bus_idx, CO_t *co, uint32_t diff_us)
{
    master_bus_ctx_t *ctx = &g_bus_ctx[bus_idx];
    CO_SDO_return_t sdo_ret;
    CO_SDO_abortCode_t abort_code;

    switch (ctx->state) {
    case M_STATE_IDLE:
        for (uint8_t i = 0; i < co->HBcons->numberOfMonitoredNodes; i++) {
            CO_NMT_internalState_t nmt_st;
            int8_t hb_ret = CO_HBconsumer_getNmtState(co->HBcons, i, &nmt_st);
            uint8_t node_id = co->HBcons->monitoredNodes[i].nodeId;

            if (hb_ret != 0 || nmt_st == CO_NMT_INITIALIZING) {
                if (ctx->node_initialized[node_id]) {
                    printf("[BUS-%d] Node %d Reset/Offline (st:%d), clearing init flag.\n",
                            bus_idx, node_id, nmt_st);
                    ctx->node_initialized[node_id] = false;
                }
                continue;
            }

            if (nmt_st == CO_NMT_PRE_OPERATIONAL && !ctx->node_initialized[node_id]) {
                ctx->active_id = node_id;
                ctx->job_idx = 0;
                ctx->state = M_STATE_SDO_REQ;
                printf("\n[BUS-%d] Configuring Node %d...\n", bus_idx, node_id);
                return;
            }
        }
        break;

    case M_STATE_SDO_REQ: {
        const sdo_job_t *job = &master_jobs[ctx->job_idx];

        CO_SDOclient_setup(&co->SDOclient[0],
                        0x600 + ctx->active_id, 0x580 + ctx->active_id, ctx->active_id);

        if (job->is_write) {
            sdo_ret = CO_SDOclientDownloadInitiate(&co->SDOclient[0], job->index, job->sub_index, 2, SDO_TIMEOUT_MS, false);
            if (sdo_ret >= 0) {
                CO_SDOclientDownloadBufWrite(&co->SDOclient[0], (uint8_t *)&job->write_val, 2);
            }
        } else {
            memset(ctx->str_buf, 0, sizeof(ctx->str_buf));
            sdo_ret = CO_SDOclientUploadInitiate(&co->SDOclient[0], job->index, job->sub_index, SDO_TIMEOUT_MS, false);
        }

        if (sdo_ret >= 0) {
            ctx->state = M_STATE_SDO_POLL;
        } else {
            printf("  [%s] Init Error!\n", job->label);
            ctx->state = M_STATE_IDLE;
        }
        break;
    }

    case M_STATE_SDO_POLL: {
        const sdo_job_t *job = &master_jobs[ctx->job_idx];

        sdo_ret = job->is_write ?
            CO_SDOclientDownload(&co->SDOclient[0], diff_us, false, false, &abort_code, NULL, NULL) :
            CO_SDOclientUpload(&co->SDOclient[0], diff_us, false, &abort_code, NULL, NULL, NULL);

        if (sdo_ret == CO_SDO_RT_ok_communicationEnd) {
            if (!job->is_write) {
                CO_SDOclientUploadBufRead(&co->SDOclient[0], (uint8_t *)ctx->str_buf, 32);
                if (job->index == 0x1000) {
                    printf("  [%s]: 0x%08X\n", job->label, *(uint32_t *)ctx->str_buf);
                } else {
                    printf("  [%s]: %s\n", job->label, ctx->str_buf);
                }
            } else {
                printf("  [%s]: Success\n", job->label);
            }

            if (++ctx->job_idx < TOTAL_JOBS) {
                ctx->state = M_STATE_SDO_REQ;
            } else {
                ctx->state = M_STATE_SEND_START;
            }
        } else if (sdo_ret < 0) {
            printf("  [%s] Failed! Abort: 0x%08X\n", job->label, abort_code);
            ctx->state = M_STATE_IDLE;
        }
        break;
    }

    case M_STATE_SEND_START:
        CO_NMT_sendCommand(co->NMT, CO_NMT_ENTER_OPERATIONAL, ctx->active_id);
        printf("[BUS-%d] Node %d Operational. Config Complete.\n", bus_idx, ctx->active_id);

        ctx->node_initialized[ctx->active_id] = true;
        ctx->state = M_STATE_IDLE;
        break;
    }
}

int main(void)
{
    uint32_t heap_memory_used;
    uint32_t timeout;
    uint64_t now;
    uint32_t elapsed[MAX_CANOPEN_DEVICE] = {0};
    uint64_t timestamp[MAX_CANOPEN_DEVICE] = {0};

    board_init();
    board_init_led_pins();

    uint32_t freq = clock_get_frequency(MCHTMR_CLK_NAME);

    for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
        canopen_controller_init(&canptr[i], &m_can_info[i], (uint32_t)BITRATE, i);

        co[i] = CO_new(NULL, &heap_memory_used);

        CO_CANinit(co[i], &canptr[i], (uint16_t)BITRATE);

        uint32_t err_info = 0;
        CO_CANopenInit(co[i], NULL, NULL, OD, NULL,
                    MASTER_NMT_CONTROL,
                    500, SDO_TIMEOUT_MS, SDO_TIMEOUT_MS, false,
                    MASTER_ID, &err_info);

        CO_CANsetNormalMode(co[i]->CANmodule);

        g_bus_ctx[i].state = M_STATE_IDLE;
    }

    printf("CANopen Master Started. Monitoring IDs 10-11\n");

    while (1) {
        timeout = 1U;

        for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
            timestamp[i] = mchtmr_get_count(HPM_MCHTMR);
            uint32_t time_diff_us = elapsed[i] * 1000;
            CO_process(co[i], false, time_diff_us, &timeout);

            run_canopen_master_task(i, co[i], time_diff_us);

            now = mchtmr_get_count(HPM_MCHTMR);
            elapsed[i] = (uint32_t)((now - timestamp[i]) * 1000 / freq);
        }

        if (timeout > 0) {
            board_delay_ms(timeout);
            for (uint8_t i = 0; i < MAX_CANOPEN_DEVICE; i++) {
                elapsed[i] += timeout;
            }
        }
    }
}