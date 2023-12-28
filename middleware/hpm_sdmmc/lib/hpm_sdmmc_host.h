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

#define HPM_SDMMC_HOST_SUPPORT_4BIT               (1UL << 0)
#define HPM_SDMMC_HOST_SUPPORT_8BIT               (1UL << 1)
#define HPM_SDMMC_HOST_SUPPORT_3V3                (1UL << 2)
#define HPM_SDMMC_HOST_SUPPORT_1V8                (1UL << 3)

#define HPM_SDMMC_HOST_SUPPORT_DDR                (1UL << 4)
#define HPM_SDMMC_HOST_SUPPORT_SDR50              (1UL << 5)
#define HPM_SDMMC_HOST_SUPPORT_SDR104             (1UL << 6)
#define HPM_SDMMC_HOST_SUPPORT_HS200              (1UL << 7)
#define HPM_SDMMC_HOST_SUPPORT_HS400              (1Ul << 8)

#define HPM_SDMMC_HOST_SUPPORT_CARD_DETECTION     (1UL << 16)
#define HPM_SDMMC_HOST_SUPPORT_VOLTAGE_SWITCH     (1UL << 17)
#define HPM_SDMMC_HOST_SUPPORT_POWER_SWITCH       (1UL << 18)
#define HPM_SDMMC_HOST_SUPPORT_WRITE_PROTECTION   (1UL << 19)
#define HPM_SDMMC_HOST_SUPPORT_RESET_PIN          (1UL << 20)
#define HPM_SDMMC_HOST_SUPPORT_DATA_STROBE        (1UL << 21)

#define HPM_SDMMC_HOST_CD_IN_IP                   (HPM_SDMMC_HOST_SUPPORT_CARD_DETECTION << 8)
#define HPM_SDMMC_HOST_VSEL_IN_IP                 (HPM_SDMMC_HOST_SUPPORT_VOLTAGE_SWITCH << 8)
#define HPM_SDMMC_HOST_PWR_IN_IP                  (HPM_SDMMC_HOST_SUPPORT_POWER_SWITCH << 8)
#define HPM_SDMMC_HOST_WP_IN_IP                   (HPM_SDMMC_HOST_SUPPORT_WRITE_PROTECTION << 8)
#define HPM_SDMMC_HOST_RST_IN_IP                  (HPM_SDMMC_HOST_SUPPORT_RESET_PIN << 8)


/**
 * @brief SD/MMC Bus Width definitions
 */
typedef enum {
    sdmmc_bus_width_1bit = 0,   /* Bus width: 1-bit */
    sdmmc_bus_width_4bit = 1,   /* Bus width: 4-bit */
    sdmmc_bus_width_8bit = 2,   /* Bus width: 8-bit */
} sdmmc_buswidth_t;

/**
 * @brief SD/MMC Device Type definitions
 */
typedef enum {
    sdmmc_dev_type_emmc = 0,    /* Device type is eMMC */
    sdmmc_dev_type_sd = 1,      /* Device type is SD */
    sdmmc_dev_type_sdio = 2,    /* Device type is SDIO */
} sdmmc_dev_type_t;

/**
 * @brief SD/MMC Host Card Detection Modes
 */
typedef enum {
    sdmmc_host_card_detection_none = 0,     /* Card detection is not enabled */
    sdmmc_host_card_detection_via_gpio = 1, /* Card detection is via GPIO */
    sdmmc_host_card_detection_via_sdxc = 2, /* Card deteciton is via SDXC CDN pin */
} sdmmc_card_detection_mode_t;

/**
 * @brief SDMMC IO Voltage
 */
typedef enum {
    hpm_sdmmc_io_voltage_3v3 = 0,   /*!< IO voltage is 3.3v */
    hpm_sdmmc_io_voltage_1v8 = 1,   /*!< IO voltage is 1.8v */
} hpm_sdmmc_io_volt_t;

/**
 * @brief SDMMC Operattion mode
 */
typedef enum {
    hpm_sdmmc_operation_mode_inactive = 0,
    hpm_sdmmc_operation_mode_identification = 1,
    hpm_sdmmc_operation_mode_transfer = 2,
    hpm_sdmmc_operation_mode_interrupt = 3,
} hpm_sdmmc_operation_mode_t;

/**
 * @brief SD/MMC Speed definitions
 */
typedef enum {
    /* SD Card Speed modes */
    sdmmc_sd_speed_normal = 0,
    sdmmc_sd_speed_high = 1,
    sdmmc_sd_speed_sdr12 = sdmmc_sd_speed_normal,
    sdmmc_sd_speed_sdr25 = sdmmc_sd_speed_high,
    sdmmc_sd_speed_sdr50 = 2,
    sdmmc_sd_speed_sdr104 = 3,
    sdmmc_sd_speed_ddr50 = 4,

    /* eMMC Card Speed modes */
    sdmmc_emmc_speed_legacy = 0,
    sdmmc_emmc_speed_high_speed_sdr = 1,
    sdmmc_emmc_speed_hs200 = 3,
    sdmmc_emmc_speed_high_speed_ddr = 4,
    sdmmc_emmc_speed_hs400 = 7,
} sdmmc_speed_mode_t;

/**
 * @brief SDMMC Pin info structure
 */
typedef struct {
    bool use_gpio;
    uint8_t polarity;
    uint16_t gpio_pin;
} hpm_sdmmc_pin_info_t;

typedef enum {
    hpm_sdmmc_power_off = 0,    /*!< Power Off the SDMMC */
    hpm_sdmmc_power_up = 1,     /*!< Power up the SDMMC */
    hpm_sdmmc_power_on = 2,     /*!< Power on the SDMMC */
} hpm_sdmmc_power_option_t;

/**
 * @brief SDMMC extra Pin info
 */
typedef struct {
    hpm_sdmmc_pin_info_t cd_pin;
    hpm_sdmmc_pin_info_t vsel_pin;
    hpm_sdmmc_pin_info_t pwr_pin;
    hpm_sdmmc_pin_info_t rst_pin;
    hpm_sdmmc_pin_info_t wp_pin;
} hpm_sdmmc_extra_io_data_t;

typedef sdxc_xfer_t sdmmchost_xfer_t;
typedef sdxc_command_t sdmmchost_cmd_t;
typedef sdxc_data_t sdmmchost_data_t;
typedef sdxc_adma2_descriptor_t sdmmc_adma2_desc_t;
typedef SDXC_Type SDMMCHOST_Type;
typedef sdxc_capabilities_t sdmmchost_capabilities_t;

typedef uint32_t (*sdmmchost_clock_init_func_t)(SDMMCHOST_Type *base, uint32_t clk_freq, bool need_reverse);

typedef void (*sdmmchost_power_switch_func_t)(SDMMCHOST_Type *base, bool on_off);

typedef void (*sdmmchost_io_init_func_t)(SDMMCHOST_Type *base);

typedef void (*sdmmchost_switch_1v8_io_func_t)(SDMMCHOST_Type *base);

typedef void (*sdmmchost_cmd_line_init_func_t)(SDMMCHOST_Type *base, bool push_pull);

typedef bool (*sdmmchost_card_detect_func_t)(SDMMCHOST_Type *base);

typedef struct {

    void (*cmd_io_init)(SDMMCHOST_Type *base, bool open_drain, bool is_1v8);

    void (*clk_data_io_init)(SDMMCHOST_Type *base, uint32_t data_width, bool is_1v8);

    void (*pwr_io_init)(SDMMCHOST_Type *base, bool as_gpio);

    void (*cd_io_init)(SDMMCHOST_Type *base, bool as_gpio);

    void (*vsel_io_init)(SDMMCHOST_Type *base, bool as_gpio);

    void (*ds_io_init)(SDMMCHOST_Type *base);
} sdmmc_io_init_apis_t;


typedef struct {
    uint8_t hart_id;
    uint8_t instance_num;
    uint32_t host_flags;
    SDMMCHOST_Type *base;
    hpm_sdmmc_extra_io_data_t io_data;
    sdmmchost_clock_init_func_t clock_init_func;
    sdmmc_io_init_apis_t io_init_apis;

    void (*delay_ms)(uint32_t ms);
} sdmmc_host_param_t;


typedef struct {
    sdmmc_host_param_t host_param;

    sdmmc_dev_type_t dev_type;
    hpm_sdmmc_operation_mode_t operation_mode;
    sdmmc_buswidth_t bus_width;
    hpm_sdmmc_io_volt_t io_voltage;
    uint32_t clock_freq;
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


hpm_stat_t sdmmchost_init(sdmmc_host_t *host);

void sdmmchost_set_card_bus_width(sdmmc_host_t *host, sdmmc_buswidth_t bus_width);

uint32_t sdmmchost_set_card_clock(sdmmc_host_t *host, uint32_t freq, bool clock_inverse);

void sdmmchost_deinit(sdmmc_host_t *host);

void sdmmchost_reset(sdmmc_host_t *host);

void sdmmchost_wait_card_active(sdmmc_host_t *host);

hpm_stat_t sdmmchost_send_command(sdmmc_host_t *host, sdmmchost_cmd_t *cmd);

hpm_stat_t sdmmchost_transfer(sdmmc_host_t *host, sdmmchost_xfer_t *content);

bool sdmmchost_is_card_detected(sdmmc_host_t *host);

void sdmmchost_init_io(sdmmc_host_t *host, hpm_sdmmc_operation_mode_t operation_mode);

void sdmmchost_delay_ms(sdmmc_host_t *host, uint32_t ms);

hpm_stat_t sdmmchost_switch_to_1v8(sdmmc_host_t *host);

void sdmmchost_enable_emmc_support(sdmmc_host_t *host, bool enable);

hpm_stat_t sdmmchost_set_speed_mode(sdmmc_host_t *host, sdmmc_speed_mode_t speed_mode);

hpm_stat_t sdmmchost_error_recovery(sdmmc_host_t *host, sdmmchost_cmd_t *abort_cmd);

bool sdmmchost_is_voltage_switch_supported(sdmmc_host_t *host);

void sdmmchost_enable_enhanced_data_strobe(sdmmc_host_t *host, bool enable);

void sdmmchost_set_data_strobe_delay(sdmmc_host_t *host);

void sdmmchost_select_voltage(sdmmc_host_t *host, hpm_sdmmc_io_volt_t io_volt);

void sdmmchost_set_cardclk_delay_chain(sdmmc_host_t *host);

void sdmmchost_set_rxclk_delay_chain(sdmmc_host_t *host);


#endif /* HPM_SDMMC_HOST_H */
