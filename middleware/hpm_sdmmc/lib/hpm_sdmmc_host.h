/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_HOST_H
#define HPM_SDMMC_HOST_H

#include "hpm_common.h"
#include "hpm_sdxc_drv.h"

typedef enum  {
    sdmmc_bus_width_1bit,
    sdmmc_bus_width_4bit,
    sdmmc_bus_width_8bit,
} sdmmc_buswidth_t;

typedef sdxc_xfer_t sdmmchost_xfer_t;
typedef sdxc_command_t sdmmchost_cmd_t;
typedef sdxc_data_t  sdmmchost_data_t;
typedef sdxc_adma2_descriptor_t  sdmmc_adma2_desc_t;
typedef SDXC_Type SDMMCHOST_Type;
typedef sdxc_capabilities_t sdmmchost_capabilities_t;
typedef uint32_t (*sdmmchost_clock_init_func_t)(SDMMCHOST_Type *base, uint32_t clk_freq);
typedef void (*sdmmchost_io_init_func_t)(SDMMCHOST_Type *base);
typedef void (*sdmmchost_switch_1v8_io_func_t)(SDMMCHOST_Type *base);
typedef bool (*sdmmchost_card_detect_func_t)(SDMMCHOST_Type *base);



typedef struct {
    SDXC_Type *base;
    sdmmchost_capabilities_t capabilities;
    sdmmchost_xfer_t xfer;
    sdmmchost_cmd_t cmd;
    sdmmchost_data_t data;
    sdmmc_adma2_desc_t adma2_desc;
    uint32_t buffer[128];
    sdmmchost_clock_init_func_t clock_init_func;
    sdmmchost_io_init_func_t io_init_func;
    sdmmchost_switch_1v8_io_func_t switch_to_1v8;
    sdmmchost_card_detect_func_t card_detect;
    void (*delay_ms)(uint32_t ms);
    bool support_1v8;
}sdmmc_host_t;


typedef struct {
    SDMMCHOST_Type *base;
    sdmmchost_clock_init_func_t clock_init_func;
    sdmmchost_io_init_func_t io_init_func;
    sdmmchost_switch_1v8_io_func_t switch_to_1v8;
    sdmmchost_card_detect_func_t card_detect;
    void (*delay_ms)(uint32_t ms);
    bool support_1v8;
}sdmmchost_config_t;


sdmmc_host_t *sdmmchost_get_host(void);

hpm_stat_t sdmmchost_init(sdmmc_host_t *host);

void sdmmchost_set_card_bus_width(sdmmc_host_t *host, sdmmc_buswidth_t bus_width);

uint32_t sdmmchost_set_card_clock(sdmmc_host_t *host, uint32_t freq);

void sdmmchost_deinit(sdmmc_host_t *host);

void sdmmchost_reset(sdmmc_host_t *host);

void sdmmchost_wait_card_active(sdmmc_host_t *host);

hpm_stat_t sdmmchost_send_command(sdmmc_host_t *host, sdmmchost_cmd_t *cmd);

hpm_stat_t sdmmchost_transfer(sdmmc_host_t *host, sdmmchost_xfer_t *content);

bool sdmmchost_is_card_detected(sdmmc_host_t *host);

hpm_stat_t sdmmchost_switch_to_1v8(sdmmc_host_t *host);





#endif /* HPM_SDMMC_HOST_H */
