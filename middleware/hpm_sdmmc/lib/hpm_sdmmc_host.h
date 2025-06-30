/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SDMMC_HOST_H
#define HPM_SDMMC_HOST_H

#include "hpm_common.h"
#include "hpm_sdmmc_osal.h"
#include "hpm_sdxc_drv.h"

/**
 *
 * @brief HPM SDMMC Host APIs
 * @defgroup hpm_sdmmc HPM SDMMC stack
 *  @ingroup hpm_sdmmc_interfaces
 * @{
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

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

#define HPM_SDMMC_HOST_ADMA3_ALIGN_SIZE           (8U)


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
    sdmmc_host_card_detection_via_sdxc = 2, /* Card detection is via SDXC CDN pin */
} sdmmc_card_detection_mode_t;

/**
 * @brief SDMMC IO Voltage
 */
typedef enum {
    hpm_sdmmc_io_voltage_3v3 = 0,   /*!< IO voltage is 3.3v */
    hpm_sdmmc_io_voltage_1v8 = 1,   /*!< IO voltage is 1.8v */
} hpm_sdmmc_io_volt_t;

/**
 * @brief SDMMC Operation mode
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

#ifndef HPM_SDMMC_HOST_ADMA_TBL_SIZE
#if defined(HPM_SDMMC_USE_ADMA2) && (HPM_SDMMC_USE_ADMA2 == 1)
#define HPM_SDMMC_HOST_ADMA_TBL_SIZE (SDXC_ADMA2_DESC_WORDS * 2UL)
#else
#define HPM_SDMMC_HOST_ADMA_TBL_SIZE (SDXC_AMDA3_DESC_MIN_WORDS * 2UL)
#endif
#endif

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
    sdmmc_host_param_t host_param;                      /* Host Parameters */
    sdmmc_dev_type_t dev_type;                          /* Device Type */
    hpm_sdmmc_operation_mode_t operation_mode;          /* Operation mode */
    sdmmc_buswidth_t bus_width;                         /* Bus width */
    hpm_sdmmc_io_volt_t io_voltage;                     /* IO voltage */
    uint32_t clock_freq;                                /* Clock Frequency */
    /* Host Transfer Fields */
    sdmmchost_xfer_t xfer;                              /* xfer context */
    sdmmchost_cmd_t cmd;                                /* Command Context */
    sdmmchost_data_t data;                              /* Data Context */
    uint32_t adma_table[HPM_SDMMC_HOST_ADMA_TBL_SIZE + 1]; /* ADMA table buffer, allocate one extra word in case that the adma_table is not 8-byte aligned  */
    uint32_t buffer[128];                               /* Host buffer */

    /* Host run-time fields */
    bool card_inserted;
    bool card_init_done;
    void (*sdio_irq_handler)(void *param);
    void *sdio_irq_param;
#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
    hpm_sdmmc_osal_event_t xfer_done_or_error_event;
#if !defined(HPM_SDMMC_XFER_CALLBACK_ENABLE) || (HPM_SDMMC_XFER_CALLBACK_ENABLE == 1)
    void (*xfer_complete_callback)(void *param);      /* Transfer complete interrupt handler */
    void (*cmd_complete_callback)(void *param);           /* Command complete interrupt handler */
    void *xfer_complete_param;                       /* Parameters passed to transfer complete interrupt handler */
    void *cmd_complete_param;                            /* Parameters passed to command complete interrupt handler */
#endif /* HPM_SDMMC_XFER_CALLBACK_ENABLE */
#endif /* HPM_SDMMC_HOST_ENABLE_IRQ */

} sdmmc_host_t;

/**
 * @brief SDMMC Host Initialization
 * @param [in] host Host context
 *
 * @return Host initialization status
 */
hpm_stat_t sdmmchost_init(sdmmc_host_t *host);

/**
 * @brief Set the card bus width
 * @param [in,out] host Host context
 * @param [in] bus_width Bus width
 */
void sdmmchost_set_card_bus_width(sdmmc_host_t *host, sdmmc_buswidth_t bus_width);

/**
 * @brief Set the Card clock
 * @param [in,out] host Host context
 * @param [in] freq Frequency in Hz
 * @param [in] clock_inverse Clock Inverse flag
 *
 * @return Actual clock frequency in Hz
 */
uint32_t sdmmchost_set_card_clock(sdmmc_host_t *host, uint32_t freq, bool clock_inverse);

/**
 * @brief Deinitialize the host
 * @param [in] host Host context
 */
void sdmmchost_deinit(sdmmc_host_t *host);

/**
 * @brief Reset the host
 * @param [in] host Host context
 */
void sdmmchost_reset(const sdmmc_host_t *host);

/**
 * @brief Wait until the card is active
 * @param [in] host Host context
 */
void sdmmchost_wait_card_active(const sdmmc_host_t *host);

/**
 * @brief Send command via the host
 * @param [in] host Host context
 * @param [in] cmd Command context
 *
 * @return Command execution status
 */
hpm_stat_t sdmmchost_send_command(sdmmc_host_t *host, const sdmmchost_cmd_t *cmd);

/**
 * @brief Transfer data via the host
 * @param [in] host Host context
 * @param [in] content Transfer context
 *
 * @return Transfer execution status
 */
hpm_stat_t sdmmchost_transfer(sdmmc_host_t *host, const sdmmchost_xfer_t *content);

/**
 * @brief Start transferring data via the host
 * @param [in] host Host context
 * @param [in] content Transfer context
 *
 * @return Transfer execution status
 */
hpm_stat_t sdmmchost_start_transfer(sdmmc_host_t *host, const sdmmchost_xfer_t *content);

/**
 * @brief Check whether the card is detected or not
 * @param [in] host Host context
 *
 * @return The card detection state
 */
bool sdmmchost_is_card_detected(const sdmmc_host_t *host);

/**
 * @brief Initialize the Host IO according to the operation mode
 * @param [in] host Host context
 * @param [in] operation_mode Operation mode
 */
void sdmmchost_init_io(sdmmc_host_t *host, hpm_sdmmc_operation_mode_t operation_mode);

/**
 * @brief Host delay
 * @param [in] host Host context
 * @param [in] ms Delay in milliseconds
 */
void sdmmchost_delay_ms(const sdmmc_host_t *host, uint32_t ms);

/**
 * @brief  Switch the Host to 1.8V IO voltage
 * @param [in,out] host Host context
 *
 * @return Host switch voltage status
 */
hpm_stat_t sdmmchost_switch_to_1v8(sdmmc_host_t *host);

/**
 * @brief Control the Voltage selection pin
 * @param [in] host Host context
 * @param [in] io_volt IO voltage
 */
void sdmmchost_vsel_pin_control(const sdmmc_host_t *host, hpm_sdmmc_io_volt_t io_volt);

/**
 * @brief Enable the eMMC support on the host
 * @param [in] host Host context
 * @param [in] enable Enable or disable the eMMC support
 * @note This function should be called after the host is initialized
 */
void sdmmchost_enable_emmc_support(const sdmmc_host_t *host, bool enable);

/**
 * @brief Set the speed mode via the Host
 * @param [in] host Host context
 * @param [in] speed_mode Speed mode
 *
 * @return Operation status
 */
hpm_stat_t sdmmchost_set_speed_mode(const sdmmc_host_t *host, sdmmc_speed_mode_t speed_mode);

/**
 * @brief Trigger the Error recovery via the Host
 * @param [in] host Host context
 * @param [in] cmd abort command context
 *
 * @return Operation status
 */
hpm_stat_t sdmmchost_error_recovery(sdmmc_host_t *host, sdmmchost_cmd_t *cmd);

/**
 * @brief Check whether the host support voltage switch
 * @param [in] host Host context
 *
 * @return Voltage switch support state
 */
bool sdmmchost_is_voltage_switch_supported(const sdmmc_host_t *host);

/**
 * @brief Enable the enhanced data strboe
 * @param [in] host Host context
 * @param [in] enable Enable or disable the enhanced data strobe
 */
void sdmmchost_enable_enhanced_data_strobe(const sdmmc_host_t *host, bool enable);

/**
 * @brief  Set the Data strobe delay for Host
 * @param [in] host Host context
 */
void sdmmchost_set_data_strobe_delay(const sdmmc_host_t *host);

/**
 * @brief Select the IO voltage
 * @param [in] host Host context
 * @param [in] io_volt IO voltage
 */
void sdmmchost_select_voltage(sdmmc_host_t *host, hpm_sdmmc_io_volt_t io_volt);

/**
 * @brief Set the Card clock delay chain for the host
 * @param [in] host Host context
 */
void sdmmchost_set_cardclk_delay_chain(const sdmmc_host_t *host);

/**
 * @brief Set the Card Rx Clock delay chain for the host
 * @param [in] host Host context
 */
void sdmmchost_set_rxclk_delay_chain(sdmmc_host_t *host);

/**
 * @brief THe Host IRQ Handler
 *
 * @param [in,out] host Host context
 */
void sdmmchost_irq_handler(sdmmc_host_t *host);

/**
 * @brief Registers an SDIO interrupt callback function for the SDMMC host controller.
 *
 * This function allows the user to register a callback that will be invoked upon
 * SDIO interrupts. It is useful for handling SDIO-specific events in addition to
 * standard SDMMC transactions.
 *
 * @param [in,out] host Pointer to the SDMMC host controller structure.
 * @param [in] sdio_irq_callback Pointer to the callback function that will be called when an SDIO interrupt occurs.
 *                          The function should accept a single parameter of type `void*`.
 * @param [in] param A pointer to a user-defined data that will be passed to the callback function each time it is invoked.
 */
void sdmmchost_register_sdio_callback(sdmmc_host_t *host, void (*sdio_irq_callback)(void *param), void *param);

/**
 * @brief Registers an transfer complete interrupt callback function for the SDMMC host controller.
 *
 * This function allows the user to register a callback that will be invoked upon
 * transfer complete interrupts.
 *
 * @param [in,out] host Pointer to the SDMMC host controller structure.
 * @param [in] irq_callback Pointer to the callback function that will be called when an transfer complete interrupt occurs.
 *                          The function should accept a single parameter of type `void*`.
 * @param [in] param A pointer to a user-defined data that will be passed to the callback function each time it is invoked.
 */
void sdmmchost_register_xfer_complete_callback(sdmmc_host_t *host, void (*irq_callback)(void *param), void *param);

/**
 * @brief Registers an command complete interrupt callback function for the SDMMC host controller.
 *
 * This function allows the user to register a callback that will be invoked upon
 * command complete interrupts.
 *
 * @param [in,out] host Pointer to the SDMMC host controller structure.
 * @param [in] irq_callback Pointer to the callback function that will be called when an command complete interrupt occurs.
 *                          The function should accept a single parameter of type `void*`.
 * @param [in] param A pointer to a user-defined data that will be passed to the callback function each time it is invoked.
 */
void sdmmchost_register_cmd_complete_callback(sdmmc_host_t *host, void (*irq_callback)(void *param), void *param);

/**
 * @brief Enable or disable SDIO interrupt on the SDMMC host controller.
 *
 * This function allows controlling the SDIO interrupt signal and status based on the provided flag.
 * When enabled, it sets the interrupt signal and status masks to detect card interrupts.
 * When disabled, it clears the interrupt status mask to stop interrupt generation from card interrupts.
 *
 * @param [in] host Pointer to the SDMMC host controller structure.
 * @param [in] enable Flag to enable (true) or disable (false) the SDIO interrupt.
 */
void sdmmchost_enable_sdio_interrupt(sdmmc_host_t *host, bool enable);

/**
 * @brief Return the data pin level
 * @param [in] host Pointer to the SDMMC host controller structure.
 *
 * @return value for data pin level
 */
uint32_t sdmmchost_get_data_pin_level(sdmmc_host_t *host);


#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* HPM_SDMMC_HOST_H */