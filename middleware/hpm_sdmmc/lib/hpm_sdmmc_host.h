/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_HOST_H
#define HPM_SDMMC_HOST_H

#include "hpm_common.h"
#include "hpm_sdxc_drv.h"
#include "hpm_gpio_drv.h"

typedef enum  {
    sdmmc_bus_width_1bit = 0,
    sdmmc_bus_width_4bit = 1,
    sdmmc_bus_width_8bit = 2,
} sdmmc_buswidth_t;

typedef enum {
    sdmmc_host_card_detection_none = 0,
    sdmmc_host_card_detection_via_gpio = 1,
    sdmmc_host_card_detection_via_sdxc = 2,
} sdmmc_card_detection_mode_t;

typedef enum {
    sdmmc_sd_speed_normal = 0,
    sdmmc_sd_speed_high = 1,
    sdmmc_sd_speed_sdr12 = sdmmc_sd_speed_normal,
    sdmmc_sd_speed_sdr25 = sdmmc_sd_speed_high,
    sdmmc_sd_speed_sdr50 = 2,
    sdmmc_sd_speed_sdr104 = 3,
    sdmmc_sd_speed_ddr50 = 4,

    sdmmc_emmc_speed_legacy = 0,
    sdmmc_emmc_speed_high_speed_sdr = 1,
    sdmmc_emmc_speed_hs200 = 3,
    sdmmc_emmc_speed_high_speed_ddr = 4,
    sdmmc_emmc_speed_hs400 = 7,
} sdmmc_speed_mode_t;


typedef sdxc_xfer_t sdmmchost_xfer_t;
typedef sdxc_command_t sdmmchost_cmd_t;
typedef sdxc_data_t  sdmmchost_data_t;
typedef sdxc_adma2_descriptor_t  sdmmc_adma2_desc_t;
typedef SDXC_Type SDMMCHOST_Type;
typedef sdxc_capabilities_t sdmmchost_capabilities_t;
typedef uint32_t (*sdmmchost_clock_init_func_t)(SDMMCHOST_Type *base, uint32_t clk_freq);
typedef void (*sdmmchost_power_switch_func_t)(SDMMCHOST_Type *base, bool on_off);
typedef void (*sdmmchost_io_init_func_t)(SDMMCHOST_Type *base);
typedef void (*sdmmchost_switch_1v8_io_func_t)(SDMMCHOST_Type *base);
typedef void (*sdmmchost_cmd_line_init_func_t)(SDMMCHOST_Type *base, bool push_pull);
typedef bool (*sdmmchost_card_detect_func_t)(SDMMCHOST_Type *base);


typedef struct {
    uint8_t hart_id;
    uint8_t instance_num;
    bool support_1v8;
    bool use_polling_mode;
    bool support_strobe;
    bool support_reset_pin;
    bool support_power_pin;
    SDMMCHOST_Type *base;
    uint32_t host_clk_freq;
    sdmmchost_clock_init_func_t clock_init_func;
    sdmmchost_io_init_func_t io_init_func;
    sdmmchost_switch_1v8_io_func_t switch_to_1v8;
    sdmmchost_card_detect_func_t card_detect;
    sdmmchost_power_switch_func_t power_switch;
    sdmmchost_cmd_line_init_func_t cmdline_switch;

    void (*delay_ms)(uint32_t ms);
    /* Card detection related fields */
    sdmmc_card_detection_mode_t card_detection_mode;
    uint8_t card_detection_gpio_index;
    uint8_t card_detection_pin_index;
    GPIO_Type *card_detection_gpio;
    int32_t card_detection_irq_index;
} sdmmc_host_param_t;


typedef struct {
    sdmmc_host_param_t host_param;

    /* Host Transfer Fields */
    sdmmchost_xfer_t xfer;
    sdmmchost_cmd_t cmd;
    sdmmchost_data_t data;
    sdmmc_adma2_desc_t adma2_desc;
    uint32_t buffer[128];

    /* Host run-time fields */
    bool card_inserted;
    bool card_init_done;
    bool cmd_done_or_error;
    bool transfer_complete_or_error;
    uint32_t int_stat;
    uint32_t auto_cmd_stat;
    uint32_t adma_error_stat;
} sdmmc_host_t;


typedef struct {
    SDMMCHOST_Type *base;
    sdmmchost_clock_init_func_t clock_init_func;
    sdmmchost_io_init_func_t io_init_func;
    sdmmchost_switch_1v8_io_func_t switch_to_1v8;
    sdmmchost_card_detect_func_t card_detect;
    void (*delay_ms)(uint32_t ms);
    bool support_1v8;
    bool use_polling_mode;
    sdmmc_card_detection_mode_t card_detection_mode;
    uint8_t card_detection_gpio_index;
    uint8_t card_detection_pin_index;
    uint8_t hart_id;
    int16_t card_detection_irq_index;
    GPIO_Type *card_detection_gpio;
} sdmmchost_config_t;


sdmmc_host_t *sdmmchost_get_host(void);

hpm_stat_t sdmmchost_init(sdmmc_host_t *host);

void sdmmchost_set_card_bus_width(sdmmc_host_t *host, sdmmc_buswidth_t bus_width);

uint32_t sdmmchost_set_card_clock(sdmmc_host_t *host, uint32_t freq, bool clock_inverse);

void sdmmchost_deinit(sdmmc_host_t *host);

void sdmmchost_reset(sdmmc_host_t *host);

void sdmmchost_wait_card_active(sdmmc_host_t *host);

hpm_stat_t sdmmchost_send_command(sdmmc_host_t *host, sdmmchost_cmd_t *cmd);

hpm_stat_t sdmmchost_transfer(sdmmc_host_t *host, sdmmchost_xfer_t *content);

bool sdmmchost_is_card_detected(sdmmc_host_t *host);

hpm_stat_t sdmmchost_switch_to_1v8(sdmmc_host_t *host);

void sdmmchost_enable_emmc_support(sdmmc_host_t *host, bool enable);

hpm_stat_t sdmmchost_set_speed_mode(sdmmc_host_t *host, sdmmc_speed_mode_t speed_mode);

hpm_stat_t sdmmchost_error_recovery(sdmmc_host_t *host, sdmmchost_cmd_t *abort_cmd);

hpm_stat_t sdmmchost_setup_card_detection(sdmmc_host_t *host);
hpm_stat_t  sdmmchost_handle_card_detection(sdmmc_host_t *host);





#endif /* HPM_SDMMC_HOST_H */
