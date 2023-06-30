/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_SDMMC_EMMC_H
#define HPM_SDMMC_EMMC_H

#include "hpm_sdmmc_common.h"
#include "hpm_sdmmc_card.h"

typedef enum {
    emmc_switch_cmd_access_mode_command_set = 0,
    emmc_switch_cmd_access_mode_set_bits = 1,
    emmc_switch_cmd_access_mode_clear_bits = 2,
    emmc_switch_cmd_access_mode_write_byte = 3,
} emmc_switch_cmd_access_mode_t;

typedef union {
    struct {
        uint8_t cmd_set;
        uint8_t value;
        uint8_t index;
        uint8_t access;
    };
    uint32_t argument;
} emmc_switch_cmd_arg_t;


#define EMMC_EXT_CSD_INDEX_CMD_SET (191)
#define EMMC_EXT_CSD_INDEX_POWER_CLASS (187)
#define EMMC_EXT_CSD_INDEX_HS_TIMING (185)
#define EMMC_EXT_CSD_INDEX_BUS_WDITH (183)
#define EMMC_EXT_CSD_INDEX_PARTITION_CONFIG (179)
#define EMMC_EXT_CSD_INDEX_BOOT_CONFIG_PROT (178)
#define EMMC_EXT_CSD_INDEX_BOOT_BUS_CONDITIONS (177)
#define EMMC_EXT_CSD_INDEX_ERASE_GROUP_DEF (175)
#define EMMC_EXT_CSD_INDEX_BOOT_WP (173)
#define EMMC_EXT_CSD_INDEX_USER_WP (171)
#define EMMC_EXT_CSD_INDEX_FW_CONFIG (169)
#define EMMC_EXT_CSD_INDEX_WR_REL_SET (167)
#define EMMC_EXT_CSD_INDEX_SANITIZE_START (165)
#define EMMC_EXT_CSD_INDEX_BKOPS_START (164)
#define EMMC_EXT_CSD_INDEX_BKOPS_EN (163)
#define EMMC_EXT_CSD_INDEX_RSTn_FUNCTION (162)
#define EMMC_EXT_CSD_INDEX_HPI_MGMT (161)
#define EMMC_EXT_CSD_INDEX_PARTITIONS_ATTRIBUTE (156)
#define EMMC_EXT_CSD_INDEX_PARTITION_SETTING_COMPLETED (155)
#define EMMC_EXT_CSD_INDEX_SEC_BAD_BLK_MGMNT (134)
#define EMMC_EXT_CSD_INDEX_PRODUCTION_STATE_AWARENESS (133)
#define EMMC_EXT_CSD_INDEX_PERIODIC_WAKEUP (131)
#define EMMC_EXT_CSD_INDEX_USE_NATIVE_SECTOR (62)
#define EMMC_EXT_CSD_INDEX_CLASS_6_CTRL (59)

#define EMMC_EXT_CSD_INDEX_POWER_OFF_NOTIFICATION (34)
#define EMMC_EXT_CSD_INDEX_CACHE_CTRL (33)
#define EMMC_EXT_CSD_INDEX_FLUSH_CACHE (32)
#define EMMC_EXT_CSD_INDEX_BARRIER_CTRL (31)
#define EMMC_EXT_CSD_INDEX_MODE_CONFIG (30)
#define EMMC_EXT_CSD_INDEX_PRODUCT_STATE_AWARENESS_ENABLEMENT (17)
#define EMMC_EXT_CSD_INDEX_SECURE_REMOVAL_TYPE (16)
#define EMMC_EXT_CSD_INDEX_CMDQ_MODE_EN (15)

typedef union {
    struct {
        uint32_t : 7;
        uint32_t voltage_170_195: 1;
        uint32_t voltage_200_260: 7;
        uint32_t voltage_270_360: 9;
        uint32_t : 5;
        uint32_t access_mode: 2;
        uint32_t powerup_status: 1;
    };
    uint32_t ocr_word;
} emmc_ocr_t;

typedef union {
    struct {
        uint8_t reserved;
        uint8_t mdt;        /*!< Manufacturing Date */
        uint8_t psn[4];     /*!< Product serial number */
        uint8_t prv;        /*!< Product revision */
        uint8_t pnm[6];     /*!< Product name */
        uint8_t oid;        /*!< OEM/Application ID */
        struct {
            uint8_t cbx: 2; /*!< Device/BGA */
            uint8_t bin: 6; /*!< Bank Index Number */
        };
        uint8_t mid;       /*!< Manufacturer ID */
    };
    uint32_t cid_words[4];
} emmc_cid_t;

typedef union {
    struct {
        uint32_t: 5;                             /*!< Bit[4:0] */
        uint32_t app_cmd: 1;                     /*!< Bit[5] */
        uint32_t exception_event: 1;             /*!< Bit[6] */
        uint32_t switch_error: 1;                /*!< Bit[7] */
        uint32_t ready_for_data: 1;              /*!< Bit[8] */
        uint32_t current_state: 4;               /*!< Bit[12:9] */
        uint32_t erase_reset: 1;                 /*!< Bit[13] */
        uint32_t : 1;                            /*!< Bit[14] */
        uint32_t wp_erase_skip: 1;               /*!< Bit[15] */
        uint32_t cid_or_csd_overwrite: 1;        /*!< Bit[16] */
        uint32_t : 2;                            /*!< Bit[18:17] */
        uint32_t error: 1;                       /*!< Bit[19] */
        uint32_t cc_error: 1;                    /*!< Bit[20] */
        uint32_t device_ecc_failed: 1;           /*!< Bit[21] */
        uint32_t illegal_command: 1;             /*!< Bit[22] */
        uint32_t com_crc_err: 1;                 /*!< Bit[23] */
        uint32_t lock_unlock_failed: 1;          /*!< Bit[24] */
        uint32_t device_is_locked: 1;            /*!< Bit[25] */
        uint32_t wp_violation: 1;                /*!< Bit[26] */
        uint32_t erase_param: 1;                 /*!< Bit[27] */
        uint32_t erase_seq_error: 1;             /*!< Bit[28] */
        uint32_t block_len_error: 1;             /*!< Bit[29] */
        uint32_t address_error: 1;               /*!< Bit[30] */
        uint32_t out_of_range: 1;                /*!< Bit[31] */
    };
    uint32_t card_status;
} emmc_card_status_t;

typedef enum {
    emmc_csd_vdd_current_min_0_5ma,
    emmc_csd_vdd_current_min_1ma,
    emmc_csd_vdd_current_min_5ma,
    emmc_csd_vdd_current_min_10ma,
    emmc_csd_vdd_current_min_25ma,
    emmc_csd_vdd_current_min_35ma,
    emmc_csd_vdd_current_min_60ma,
    emmc_csd_vdd_current_min_100ma,
} emmc_csd_vdd_current_min_t;

typedef enum {
    emmc_csd_vdd_current_max_1ma,
    emmc_csd_vdd_current_max_5ma,
    emmc_csd_vdd_current_max_10ma,
    emmc_csd_vdd_current_max_25ma,
    emmc_csd_vdd_current_max_35ma,
    emmc_csd_vdd_current_max_45ma,
    emmc_csd_vdd_current_max_80ma,
    emmc_csd_vdd_current_max_200ma,
} emmc_csd_vdd_current_max_t;

typedef enum vdd_current_min_def {
    vdd_current_min_0_5ma,
    vdd_current_min_1ma,
    vdd_current_min_5ma,
    vdd_current_min_10ma,
    vdd_current_min_25ma,
    vdd_current_min_35ma,
    vdd_current_min_60ma,
    vdd_current_min_100ma,
} vdd_current_min_t;

typedef enum vdd_current_max_def {
    vdd_current_max_1ma,
    vdd_current_max_5ma,
    vdd_current_max_10ma,
    vdd_current_max_25ma,
    vdd_current_max_35ma,
    vdd_current_max_45ma,
    vdd_current_max_80ma,
    vdd_current_max_200ma,
} vdd_current_max_t;

typedef enum {
    emmc_csd_file_format_hard_disk_like_file_system_with_partition_table,
    emmc_csd_file_format_dos_fat_with_boot_sector_only,
    emmc_csd_file_format_universal_file_format,
    emmc_csd_file_format_others_or_unknown
} emmc_csd_file_format_t;

typedef struct {
    uint8_t csd_structure;
    uint8_t spec_version;
    uint16_t nsac_cycles;
    uint32_t taac_ns;
    bool support_read_block_partial;
    bool support_write_block_misalignment;
    bool support_read_block_misalignment;
    bool is_dsr_implemented;
    bool support_content_protect_app;
    bool is_write_protection_group_enabled;
    bool support_write_block_partial;
    bool is_predefined_file_format;
    bool support_copy;
    bool support_permanent_write_protect;
    bool support_temporary_write_protect;
    uint8_t file_format;

    uint16_t card_command_class;
    uint8_t write_speed_factor;
    emmc_csd_vdd_current_min_t read_current_vdd_min;
    emmc_csd_vdd_current_max_t read_current_vdd_max;
    emmc_csd_vdd_current_min_t write_current_vdd_min;
    emmc_csd_vdd_current_max_t write_current_vdd_max;
    uint8_t ecc_type;


    uint32_t sector_size;
    uint32_t erase_group_size;
    uint32_t max_read_block_len;
    uint32_t max_write_block_len;
    uint32_t write_protect_group_size;
    uint32_t max_freq;
    uint64_t device_size_in_bytes;

} emmc_csd_t;

typedef union {
    uint8_t device_type;
    struct {
        uint8_t support_high_speed_sdr_at_26mhz: 1;
        uint8_t support_high_speed_sdr_at_52mhz: 1;
        uint8_t support_high_speed_ddr_at_52mhz_1v8_or_3v: 1;
        uint8_t support_high_speed_ddr_at_52mhz_1v2: 1;
        uint8_t support_hs200_at_200mhz_1v8: 1;
        uint8_t support_hs200_at_200mhz_1v2: 1;
        uint8_t support_hs400_at_200mhz_1v8: 1;
        uint8_t support_hs400_at_200mhz_1v2: 1;
    };
} emmc_device_type_t;

#pragma pack(1)

typedef struct {
    /*
     * Mode segment
     */
    uint8_t reserved0[15];                              /*!< [14:0] */
    uint8_t command_queue_mode_enable;                  /*!< [15] */
    uint8_t secure_removal_type;                        /*!< [16] */
    uint8_t product_state_awareness_enablement;         /*!< [17] */
    uint8_t max_pre_loading_data_size[4];               /*!< [21:18] */
    uint8_t pre_loading_data_size[4];                   /*!< [25:22] */
    uint8_t ffu_status;                                 /*!< [26] */
    uint8_t reserved1[2];                               /*!< [28:27] */
    uint8_t mode_operation_codes;                       /*!< [29] */
    uint8_t mode_config;                                /*!< [30] */
    uint8_t barrier_control;                            /*!< [31] */
    uint8_t flush_cache;                                /*!< [32] */
    uint8_t cache_control;                              /*!< [33] */
    uint8_t power_off_notification;                     /*!< [34] */
    uint8_t packed_command_failure_index;               /*!< [35] */
    uint8_t packed_command_status;                      /*!< [36] */
    uint8_t context_config[15];                         /*!< [51:37] */
    uint8_t extended_partitions_attribute[2];           /*!< [53:52] */
    uint8_t exception_events_status[2];                 /*!< [55:54] */
    uint8_t exception_events_control[2];                /*!< [57:56] */
    uint8_t dyncap_needed;                              /*!< [58] */
    uint8_t class6_commands_control;                    /*!< [59] */
    uint8_t init_timeout_after_disabling_emulation;     /*!< [60] 100ms *  init_timeout_after_disabling_emulation */
    uint8_t data_sector_size;                           /*!< [61]  0 - 512Bytes, 1 - 4KB */
    uint8_t sector_size_emulation;                      /*!< [62] */
    uint8_t native_sector_size;                         /*!< [63] 0 - 512Bytes, 1 - 4KB */
    uint8_t vendor_specific_field[64];                  /*!< [127:64] */
    uint8_t reserved02[2];                              /*!< [129:128] */
    uint8_t program_cid_or_csd_in_ddr_mode_supported;   /*!< [130] */
    uint8_t periodic_wakeup;                            /*!< [131] */
    uint8_t tcase_support;                              /*!< [132] */
    uint8_t production_state_awareness;                 /*!< [133] */
    uint8_t bad_block_management_mode;                  /*!< [134] */
    uint8_t reserved03;                                 /*!< [135] */
    uint32_t enhanced_user_data_start_address;          /*!< [139:136] */
    uint8_t enhanced_user_data_area_size[3];            /*!< [142:140] 512KB * enhanced_user_data_area_size * high_capacity_erase_unit_size */
    uint8_t general_purpose_partition_size[4][3];       /*!< [154:143] */
    uint8_t partitioning_setting;                       /*!< [155] */
    uint8_t partitioning_attribute;                     /*!< [156] */
    uint8_t max_enh_size_mult[3];                       /*!< [159:157] */
    uint8_t partitioning_support;                       /*!< [160] */
    uint8_t hw_reset_function;                          /*!< [161] */
    uint8_t hpi_management;                             /*!< [162] */
    uint8_t bkops_en;                                   /*!< [163] */
    uint8_t bkops_start;                                /*!< [164] */
    uint8_t sanitize_start;                             /*!< [165] */
    uint8_t write_reliability_parameter;                /*!< [166] */
    uint8_t write_reliability_setting;                  /*!< [167]  bit0-user area, bit[4:1] general purpose partition 4-1 */
    uint8_t rpmb_size;                                  /*!< [168] 128KB * rpmb_size */
    uint8_t fw_config;                                  /*!< [169] */
    uint8_t reserved04;                                 /*!< [170] */
    /* bit0 - US_PWR_WP_EN, bit2 - US_PERM_WP_EN, bit3 - US_PWR_WP_DIS, bit4 - US_PERM_WP_DIS
     * bit6 - CD_PERM_WP_DIS, bit7 - PERM_PSWD_DIS
     */
    uint8_t user_wp;                                    /*!< [171] */
    uint8_t reserved05;                                 /*!< [172] */
    /* bit0 - B_PWR_WP_EN, bit1 - B_PWR_WP_SEC_SEL, bit2 - B_PERM_WP_EN, bit3 - B_PERM_WP_SEC_SEL
     * bit4 - B_PERM_WP_DIS, bit6 - B_PWR_WP_DIS, bit7 - B_SEC_WP_SEL
     *
     */
    uint8_t boot_wp;                                    /*!< [173] */
    uint8_t boot_wp_status;                             /*!< [174] bit[1:0] B_AREA_1_WP, bit[3:2] B_AREA_2_WP */
    uint8_t erase_group_def;                            /*!< [175] 0 - old group size, 1 - high-capacity erase unit size */
    uint8_t reserved06;                                 /*!< [176] */
    /* bit[1:0] BOOT_BUS_WIDTH
     *  0 - x1 SDR or x4 DDR  in boot operation mode, 1 - X4 (SDR/DDR) bus width in boot operation mode
     *  2 - x8 (SDR/DDR) bus width in boot operation mode
     * bit2 RESET_BOOT_BUS_CONDITIONS
     *  0 - Reset bus width to x1
     *  1 - Retain BOOT_BUS_WIDTH and BOOT_MODE values after boot operation
     * bit[4:3] BOOT_MODE
     *  0 - SDR @ normal speed timing, 1 - SDR @ High Speed timing, 2 - DDR
     *
     */
    uint8_t boot_bus_conditions;                    /*!< [177] */
    /* bit0 - PWR_BOOT_CONFIG_PROT
     * bit4 - PERM_BOOT_CONFIG_PROT
     */
    uint8_t boot_config_prot;                       /*!< [178] */
    /* bit[2:0] PARTITION_ACCESS
     *  0 - No access to boot partition, 1 - R/W boot partition 1, 2 - R/W boot partition 2
     *  3 - R/W RPMB, 4 - Access to General Purpose partition 1, 5 - Access to General Purpose partition 2
     *  6 - Access to General Purpose partition 3, 7 - Access to General partition 4
     * bit[5:3] BOOT_PARTITION_ENABLE
     *  0 - Device not boot enabled, 1 - Boot partition 1 enabled for boot, 2 - boot partition 2 enabled for boot
     *  7 - User area enabled for boot
     * bit[6] BOOT_ACK
     */
    uint8_t partition_config;                   /*!< [179] */
    uint8_t reserved07;                         /*!< [180] */
    uint8_t erased_mem_content;                 /*!< [181] 0 - Read as all 0x00s for erased region, 1 - read as all 0xffs for erased region  */
    uint8_t reserved08;                         /*!< [182] */
    /* bit[3:0] bus mode selection: 0-1bit data bus, 1 - 4bit data bus, 2 - 8bit data bus,
     *                              5- 4bit data bus @DDR mode, 6 - 8bit data bus @DDR mode
     * bit7 Enhanced Strobe
     */
    uint8_t bus_width;                          /*!< [183] */
    uint8_t strobe_support;                     /*!< [184] 0 - no support for enhanced strobe mode, 1 - supports enhanced strobe mode */
    /* bit[3:0] timing interface: 0 - normal, 1 - high speed, 2 - HS200, 3 - HS400
     * bit[7:4] selected driver strength
     */
    uint8_t hs_timing;                          /*!< [185] */
    uint8_t reserved09;                         /*!< [186] */
    uint8_t power_class;                        /*!< [187] */
    uint8_t reserved10;                         /*!< [188] */
    uint8_t cmd_set_revision;                   /*!< [189] 0 - v4.0 */
    uint8_t reserved11;                         /*!< [190] */
    uint8_t cmd_set;                            /*!< [191] */
    /*
     * Properties Segment
     */
    uint8_t ext_csd_rev;                        /*!< [192] 0-4.0, 1-4.1, 2-4.2, 3-4.3, 5-4.41, 6-4.51, 7-5.01, 8-5.1 */
    uint8_t reserved12;                         /*!< [193] */
    uint8_t csd_structure;                      /*!< [194] 0 - CSD 1.0, 1 - CSD 1.1, 2 - CSD 1.2 */
    uint8_t reserved13;                         /*!< [195] */
    /*
     * bit0 - HS @ 26MHz, bit1 - HS @ 52MHz, bit2 - HS DDR @52MHz, 1.8V or 3V
     * bit3 - HS DDR @ 52Mhz, 12.V, bit4 - HS200 SDR @200MHz, 1.8V, bit5 - HS200 SDR @200MHz, 1.2V
     * bit6 - HS400 DDR @ 1.8V, bit7 - HS400 DDR  @ 1.2V
     */
    uint8_t device_type;                        /*!< [196] */
    uint8_t driver_strength;                    /*!< [197] bit0 - type 0, bit1 - type 1, bit2 - type 2, bit3 - type 3, bit4 - type 4 */
    uint8_t out_of_interrupt_timing;            /*!< [198] 10ms * out_of_interrupt_timing */
    uint8_t partition_switch_timing;            /*!< [199] 10ms * partition_switch_timing */
    uint8_t pwr_cl_52_195;                      /*!< [200] */
    uint8_t pwr_cl_26_195;                      /*!< [201] */
    uint8_t pwr_cl_52_360;                      /*!< [202] */
    uint8_t pwr_cl_26_360;                      /*!< [203] */
    uint8_t reserved14;                         /*!< [204] */
    uint8_t min_perf_r_4_26;                    /*!< [205] SDR mode : 150kB/S * value, DDR mode = 300kB/s * value * 2 */
    uint8_t ming_perf_w_4_26;                   /*!< [206] SDR mode : 150kB/S * value, DDR mode = 300kB/s * value * 2 */
    uint8_t min_perf_r_8_26_4_52;               /*!< [207] SDR mode : 150kB/S * value, DDR mode = 300kB/s * value * 2 */
    uint8_t min_perf_w_8_26_4_52;               /*!< [208] SDR mode : 150kB/S * value, DDR mode = 300kB/s * value * 2 */
    uint8_t min_perf_r_8_52;                    /*!< [209] SDR mode : 150kB/S * value, DDR mode = 300kB/s * value * 2 */
    uint8_t min_perf_w_8_52;                    /*!< [210] SDR mode : 150kB/S * value, DDR mode = 300kB/s * value * 2 */
    uint8_t secure_wp_info;                     /*!< [211] bit0-SECURE_WP_SUPPORT, bit1-SECURE_WP_EN_STATUS */
    uint32_t sector_count;                      /*!< [215:212] Device density = sector_count * 512Bytes */
    uint8_t sleep_notification_timeout;         /*!< [216] 10us * 2 ^ sleep_notification_timeout */
    uint8_t sleep_or_awake_timeout;             /*!< [217] 100ns * 2^sleep_or_awake_timeout */
    uint8_t production_state_awareness_timeout; /*!< [218] 100us * 2 ^ production_state_awareness_timeout */
    uint8_t sleep_current_vccq;                 /*!< [219] 1uA * 2 ^ sleep_current_vccq */
    uint8_t sleep_current_vcc;                  /*!< [220] 1uA * 2 ^ sleep_current_vcc */
    /* 512KB * high_capacity_erase_unit_size * high_capacity_write_protect_group_size */
    uint8_t high_capacity_write_protect_group_size; /*!< [221] */
    uint8_t reliable_write_sector_count;        /* !< [222] shall be set to 1 */
    uint8_t erase_timeout_mult;                 /*!< [223] 300ms * erase_timeout_mult */
    uint8_t high_capacity_erase_unit_size;      /*!< [224] 512KB * high_capacity_erase_unit_size */
    uint8_t access_size;                        /*!< [225] bit[3:0] - SUPER_PAGE_SIZE = 512 * 2 ^ (SUPER_PAGE_SIZE - 1) */
    uint8_t boot_partition_size_mult;           /*!< [226] 128KB * boot_partition_size_mult */
    uint8_t reserved15;                         /*!< [227] */
    uint8_t boot_info;                          /*!< [228] bit0-ALT_BOOT_MODE, bit1-DDR_BOOT_MODE, bit2-HS_BOOT_MODE */
    uint8_t secure_trim_mult;                   /*!< [229] 300ms * secure_trim_mult */
    uint8_t secure_erase_mult;                  /*!< [230] 300ms * secure_erase_mult */
    uint8_t secure_feature_support;             /*!< [231] bit0-SECURE_ER_EN, bitt2-SEC_BD_BLK_EN, bit4-SEC_GB_CL_EN, bit6-SEC_SANITIZE */
    uint8_t trim_mult;                          /*!< [232] 300ms * trim_mult */
    uint8_t reserved16;                         /*!< [233] */
    uint8_t min_perf_ddr_r_8_52;                /*!< [234] */
    uint8_t min_perf_ddr_w_8_52;                /*!< [235] */
    uint8_t pwr_cl_200_130;                     /*!< [236] */
    uint8_t pwr_cl_200_195;                     /*!< [237] */
    uint8_t pwr_cl_ddr_52_195;                  /*!< [238] */
    uint8_t pwr_cl_ddr_52_360;                  /*!< [239] */
    uint8_t cache_flush_policy;                 /*!< [240] 0 - none, 1 - FIFO policy */
    uint8_t init_timeout_after_partitioning;    /*!< [241] 100ms * init_timeout_after_partitioning */
    uint32_t number_of_correctly_programmed_sectors; /*!< [245:242] */
    uint8_t bkops_status;                       /*!< [246] bit[1:0] 0:no operations required, 1:non-critical, 2:lower performance, 3:critical */
    uint8_t power_off_long_timeout;             /*!< [247] 10ms * power_off_long_timeout */
    uint8_t generic_cmd6_timeout;               /*!< [248] 10ms * generic_cmd6_timeout */
    uint32_t cache_size;                        /*!< [252:249] cache_size * 1kb */
    uint8_t pwr_cl_ddr_200_360;                 /*!< [253] */
    uint8_t firmware_version[8];                /*!< [261:254] */
    uint8_t device_version[2];                  /*!< [263:262] */
    uint8_t optimal_trim_unit_size;             /*!< [264] 4KB * 2 ^(optmial_trim_unit_size - 1) */
    uint8_t optimal_write_size;                 /*!< [265] 4KB * optimal_write_size */
    uint8_t optimal_read_size;                  /*!< [266] 4KB * optimal_read_size */
    uint8_t pre_eol_info;                       /*!< [267] 1-Normal, 2-Warning, 3-Urgent */
    uint8_t device_lifetime_estimation_type_a;  /*!< [268] */
    uint8_t device_lifetime_estimation_type_b;  /*!< [269] */
    uint8_t vendor_proprietary_health_report[32];/*!< [301:270] */
    uint32_t number_of_correctly_programmed_fw_sectors; /*!< [305:302] */
    uint8_t reserved17;                         /*!< [306] */
    uint8_t cmdq_depth;                         /*!< [307] bit[4:0] N, depth = N+1 */
    uint8_t cmdq_support;                       /*!< [308] 1 - supported, others - not supported */
    uint8_t reserved18[177];                    /*!< [485:309] */
    uint8_t barrier_support;                    /*!< [486] */
    uint8_t ffu_arg[4];                         /*!< [490:487] */
    uint8_t operation_code_timeout;             /*!< [491] 100us * 2 ^ operation_code_timeout */
    uint8_t ffu_features;                       /*!< [492] 0 - device does not support mode_operation_codes */
    uint8_t supported_modes;                    /*!< [493] bit0 - FFU, 0:not supported, 1:supported, bit1 Vendor Specific Mode, 0:not supported  */
    uint8_t extended_partitions_attribute_support; /*!< [494] bit0 system code, bit1 non-persistent */
    uint8_t large_unit_size;                    /*!< [495] 1MB * large_unit_size */
    uint8_t context_capabilities;               /*!< [496] bit[3:0] max_context_id, bit[6:4] maximum large_unit_size */
    uint8_t tag_resource_size;                  /*!< [497] [n * tag_unit_size) * 2 ^tag_resource_size] / 1024 */
    uint8_t tag_unit_size;                      /*!< [498] 2^tag_unit_size * sector_size */
    uint8_t data_tag_support;                   /*!< [499] */
    uint8_t max_packed_writes;                  /*!< [500] minimum value: 3 */
    uint8_t max_packed_reads;                   /*!< [501] minimum value:5 */
    uint8_t bkops_support;                      /*!< [502] */
    uint8_t hpi_features;                       /*!< [503] */
    uint8_t supported_cmd_sets;                 /*!< [504] */
    uint8_t extended_security_error;            /*!< [505] */
    uint8_t reserved19[6];                      /*!< [511:506] */

} emmc_ext_csd_t;

#pragma pack()

typedef enum _emmc_partition {
    emmc_partition_boot_area_1 = 0,
    emmc_partition_boot_area_2 = 1,
    emmc_partition_rpmb = 2,
    emmc_partition_user_data = 3,
} emmc_partition_t;

typedef enum _emmc_hs_timing {
    emmc_timing_legacy = 0,
    emmc_timing_high_speed = 1,
    emmc_timing_hs200 = 2,
    emmc_timing_hs400 = 3,
    emmc_timing_high_speed_ddr = 4,
} emmc_hs_timing_t;

typedef enum {
    emmc_power_3v6_max_rms_100ma_peak_200ma = 0,
    emmc_power_3v6_max_rms_120ma_peak_220ma = 1,
    emmc_power_3v6_max_rms_150ma_peak_250ma = 2,
    emmc_power_3v6_max_rms_180ma_peak_280ma = 3,
    emmc_power_3v6_max_rms_200ma_peak_300ma = 4,
    emmc_power_3v6_max_rms_220ma_peak_320ma = 5,
    emmc_power_3v6_max_rms_250ma_peak_350ma = 6,
    emmc_power_3v6_max_rms_300ma_peak_400ma = 7,
    emmc_power_3v6_max_rms_350ma_peak_450ma = 8,
    emmc_power_3v6_max_rms_400ma_peak_500ma = 9,
    emmc_power_3v6_max_rms_450ma_peak_550ma = 10,
    emmc_power_3v6_max_rms_500ma_peak_600ma = 11,
    emmc_power_3v6_max_rms_600ma_peak_700ma = 12,
    emmc_power_3v6_max_rms_700ma_peak_800ma = 13,
    emmc_power_3v6_max_rms_800ma_peak_900ma = 14,
    emmc_power_3v6_max_rms_gt800ma_peak_gt900ma = 15,
} emmc_power_class_3v6_t;

typedef enum {
    emmc_power_1v95_max_rms_65ma_peak_130ma = 0,
    emmc_power_1v95_max_rms_70ma_peak_140ma = 1,
    emmc_power_1v95_max_rms_80ma_peak_160ma = 2,
    emmc_power_1v95_max_rms_90ma_peak_180ma = 3,
    emmc_power_1v95_max_rms_100ma_peak_200ma = 4,
    emmc_power_1v95_max_rms_120ma_peak_220ma = 5,
    emmc_power_1v95_max_rms_140ma_peak_240ma = 6,
    emmc_power_1v95_max_rms_160ma_peak_260ma = 7,
    emmc_power_1v95_max_rms_180ma_peak_280ma = 8,
    emmc_power_1v95_max_rms_200ma_peak_300ma = 9,
    emmc_power_1v95_max_rms_250ma_peak_350ma = 10,
    emmc_power_1v95_max_rms_300ma_peak_400ma = 11,
    emmc_power_1v95_max_rms_350ma_peak_450ma = 12,
    emmc_power_1v95_max_rms_400ma_peak_500ma = 13,
    emmc_power_1v95_max_rms_500ma_peak_600ma = 14,
    emmc_power_1v95_max_rms_gt500ma_peak_gt600ma = 15,
} emmc_power_class_1v95_t;

typedef union {
    emmc_power_class_3v6_t power_class_3v6;
    emmc_power_class_1v95_t power_class_1v95;
} emmc_power_class_t;

typedef enum {
    emmc_bus_mode_x1_sdr = 0,
    emmc_bus_mode_x4_sdr = 1,
    emmc_bus_mode_x8_sdr = 2,
    emmc_bus_mode_x4_ddr = 5,
    emmc_bus_mode_x8_ddr = 6,
    emmc_bus_mode_x8_ddr_ds = 0x86,
} emmc_bus_mode_t;

typedef enum {
    emmc_write_protection_mode_legacy = 0,
    emmc_write_protection_mode_secure = 1,
} emmc_write_protection_mode_t;

typedef struct {
    bool is_alt_boot_mode_supported;
    bool is_ddr_boot_mode_supported;
    bool is_hs_boot_mode_supported;
    uint8_t reserved;
} boot_info_t;

typedef struct {
    bool is_secure_erase_supported;
    bool is_secure_bad_block_management_supported;
    bool is_secure_insecure_trim_supported;
    bool is_secure_sanitize_supported;
} secure_feature_info_t;

typedef enum {
    emmc_boot_partition_mode_not_enabled = 0,
    emmc_boot_partition_mode_boot_partition1 = 1,
    emmc_boot_partition_mode_boot_partition2 = 2,
    emmc_boot_partition_mode_user_area = 7,
} emmc_boot_partition_mode_t;

typedef enum {
    emmc_partition_access_boot_partition_disabled = 0,
    emmc_partition_access_read_or_write_boot_partition1 = 1,
    emmc_partition_access_read_or_write_boot_partition2 = 2,
    emmc_partition_access_read_or_write_rpmb = 3,
    emmc_partition_access_access_to_gp_partition1 = 4,
    emmc_partition_access_access_to_gp_partition2 = 5,
    emmc_partition_access_access_to_gp_partition3 = 6,
    emmc_partition_access_access_to_gp_partition4 = 7,
} emmc_partition_access_t;

typedef enum {
    emmc_boot_mode_sdr_legacy = 0, emmc_boot_mode_sdr_high_speed_timing, emmc_boot_mode_ddr,
} emmc_boot_mode_t;

typedef struct {
    bool enable_boot_ack;
    bool enable_boot_config_protection_until_next_power_cycle;
    bool enable_boot_config_protection_permanently;
    bool reset_boot_bus_conditions;
    emmc_boot_mode_t boot_mode;
    emmc_bus_mode_t boot_bus_mode;
    emmc_boot_partition_mode_t emmc_boot_partition_mode;
    emmc_partition_access_t partition_access;
} emmc_boot_setting_t;

typedef struct {
    bool is_secure_write_protection_supported;
    emmc_device_type_t device_type;
    emmc_write_protection_mode_t write_protection_mode;

    boot_info_t boot_info;
    secure_feature_info_t secure_feature_info;

    uint32_t boot_partition_size;
    uint32_t rpmb_partition_size;
    uint32_t max_enhanced_area_size;

    uint32_t gp_partition_size[4];

    uint32_t super_page_size;
    uint32_t write_protect_group_size;
    uint32_t erase_group_size;
    uint64_t device_size_in_bytes;
    uint32_t sector_count;
    uint32_t sector_size;

    uint32_t large_unit_size;
    uint32_t optimal_read_size;
    uint32_t optimal_write_size;
    uint64_t optimal_trim_unit_size;

    uint32_t operation_codes_timeout_us;
    uint32_t switch_cmd_timeout_ms;
    uint32_t power_off_timeout_ms;
    uint32_t init_timeout_after_partition_ms;
    uint32_t trim_timeout_ms;
    uint32_t secure_erase_timeout_ms;
    uint32_t secure_trim_timeout_ms;

    uint32_t erase_timeout_ms;
    uint32_t partition_switch_timeout_ms;
    uint32_t out_of_interrupt_timeout_ms;
    uint32_t sleep_notification_timeout_us;
    uint32_t sleep_awake_timeout_ns;
    uint32_t production_state_awareness_timeout_us;
    uint32_t sleep_current_vcc_ua;
    uint32_t sleep_current_vccq_ua;

    bool use_high_capacity_erase_unit_size;
    bool is_all_0xffs_for_erased_region;
    bool is_enhanced_strobe_supported;
    bool is_cmd_queue_mode_enabled;
} emmc_device_attribute_t;

typedef struct _sdmmc_emmc {
    sdmmc_host_t *host;
    uint16_t relative_addr;
    emmc_cid_t cid;
    emmc_ocr_t ocr;
    emmc_csd_t csd;
    emmc_ext_csd_t ext_csd;

    emmc_device_attribute_t device_attribute;

    sdmmc_operation_voltage_t operation_voltage;
    emmc_card_status_t current_r1_status;
    emmc_partition_t current_partition;
    emmc_hs_timing_t current_hs_timing;
    emmc_bus_mode_t current_bus_mode;
    emmc_power_class_t current_power_class;
    emmc_boot_setting_t boot_setting;
    bool is_host_ready;
} emmc_card_t;

typedef enum {
    emmc_idle_option_go_idle_state = 0,
    emmc_idle_option_go_pre_idle_state = 0xF0F0F0F0U,
    emmc_idle_option_boot_initialization = 0xFFFFFFFAU,
} emmc_idle_option_t;

typedef enum {
    emmc_erase_option_erase = 0,  /*!< eMMC Erase operation */
    emmc_erase_option_trim = 1,  /*!< eMMC Trim operation */
    emmc_erase_option_discard = 2,  /*!< eMMC Discard operation */
} emmc_erase_option_t;

typedef enum {
    boot_partition_enable_option_not_enabled = 0,
    boot_partition_enable_option_boot_partition1 = 1,
    boot_partition_enable_option_boot_partition2 = 2,
    boot_partition_enable_option_user_area = 7,
} emmc_boot_partition_enable_option_t;

typedef enum {
    partition_access_option_no_access_to_boot_partition = 0,
    partition_access_option_read_write_boot_partition1 = 1,
    partition_access_option_read_write_boot_partition2 = 2,
    partition_access_option_read_write_rpmb = 3,
    partition_access_option_access_to_gp_partition1 = 4,
    partition_access_option_access_to_gp_partition2 = 5,
    partition_access_option_access_to_gp_partition3 = 6,
    partition_access_option_access_to_gp_partition4 = 7
} emmc_partition_access_option_t;

typedef struct {
    bool enable_boot_ack;
    emmc_boot_partition_enable_option_t boot_partition_enable_option;
    emmc_partition_access_t partition_access_option;
} emmc_config_partition_option_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Intialize eMMC device, include both host and the device
 * @param card
 * @return
 */
hpm_stat_t emmc_init(emmc_card_t *card);

void emmc_deinit(emmc_card_t *card);

/**
 * @brief Initialize eMMC device
 * @param card
 * @return
 */
hpm_stat_t emmc_card_init(emmc_card_t *card);

/**
 * @brief Request eMMC device to send CID to Host
 * @param card
 * @return
 */
hpm_stat_t emmc_send_cid(emmc_card_t *card);

/**
 * @brief Probe eMMC device
 * @param card
 * @return
 */
hpm_stat_t emmc_probe_device(emmc_card_t *card);

/**
 * @brief Select eMMC device
 * @param card
 * @param is_selected
 * @return
 */
hpm_stat_t emmc_select_card(emmc_card_t *card, bool is_selected);

/**
 * @brief Go to Idle mode
 * @param card
 * @param option
 * @return
 */
hpm_stat_t emmc_go_idle(emmc_card_t *card, emmc_idle_option_t option);

/**
 * @brief Switch eMMC function
 * @param card
 * @param arg
 * @param timeout_us
 * @return
 */
hpm_stat_t emmc_switch_function(emmc_card_t *card, emmc_switch_cmd_arg_t arg, uint32_t timeout_us);

/**
 * @brief Configure eMMC Partition
 * @param card
 * @param option
 * @return
 */
hpm_stat_t emmc_configure_partition(emmc_card_t *card, emmc_config_partition_option_t option);

/**
 * @brief Enable High Density Erase group
 * @param card
 * @param enable
 * @return
 */
hpm_stat_t emmc_enable_high_density_erase_group(emmc_card_t *card, bool enable);

/**
 * @brief Program CSD register
 * @param card
 * @param raw_csd
 * @return
 */
hpm_stat_t emmc_program_csd(emmc_card_t *card, const uint32_t *raw_csd);

/**
 * @brief Read eMMC blocks
 * @param card
 * @param buffer
 * @param start_block
 * @param block_count
 * @return
 */
hpm_stat_t emmc_read_blocks(emmc_card_t *card, uint8_t *buffer, uint32_t start_block, uint32_t block_count);

/**
 * @brief Write eMMC blocks
 * @param card
 * @param buffer
 * @param start_block
 * @param block_count
 * @return
 */
hpm_stat_t emmc_write_blocks(emmc_card_t *card, const uint8_t *buffer, uint32_t start_block, uint32_t block_count);

/**
 * @brief Erase eMMC Blocks
 * @param [in] card
 * @param [in] start_block
 * @param [in] block_count
 * @param [in] erase_option
 * @return
 */
hpm_stat_t emmc_erase_blocks(emmc_card_t *card, uint32_t start_block, uint32_t block_count, emmc_erase_option_t option);


#ifdef __cplusplus
}
#endif

#endif /* HPM_SDMMC_EMMC_H */
