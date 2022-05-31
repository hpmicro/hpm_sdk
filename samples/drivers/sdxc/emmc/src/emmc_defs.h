#ifndef EMMC_DEFS_H
#define EMMC_DEFS_H


#define CMD1_EMMC_SPEND_OP_COND (1)
#define CMD3_EMMC_SET_RELATIVE_ADDR (3)
#define CMD5_EMMC_SLEEP_AWAKE (5)
#define CMD21_EMMC_SEND_TUNING_BLOCK (21)
#define CMD35_EMMC_ERASE_GROUP_START (35)
#define CMD36_EMMC_ERASE_GROUP_END (36)

#define CMD0_GO_IDLE (0)

#define CMD2_ALL_SEND_CID (2)
#define CMD3_SEND_RELATIVE_ADDR (3)
#define CMD4_SET_DSR (4)
#define CMD8_EMMC_SEND_EXT_CSD (8)

#define CMD6_SWITCH_FUNC (6)
#define CMD7_SEL_DESEL_CARD (7)
#define CMD8_SEND_IF_COND (8)
#define CMD9_SEND_CSD (9)
#define CMD10_SEND_CID (10)
#define CMD11_VOLTAGTE_SWITCH    (11)
#define CMD12_STOP_TRANSMISSION (12)
#define CMD13_SEND_STATUS (13)
#define CMD15_GO_INACTIVE_STATE (15)

#define CMD16_SET_BLOCKLEN (16)
#define CMD17_READ_SINGLE_BLOCK (17)
#define CMD18_READ_MULTIPLE_BLOCK (18)
#define CMD19_SEND_TUNNING_BLOCK (19)
#define CMD20_SPEED_CLASS_CONTROL (20)
#define CMD22_ADDRESS_EXTENSION (22)
#define CMD23_SET_BLOCK_COUNT (23)

#define CMD24_WRITE_SINGLE_BLOCK (24)
#define CMD25_WRITE_MULTIPLE_BLOCK (25)
#define CMD27_PROGRAM_CSD (27)

#define CMD32_ERASE_WRITE_BLOCK_START (32)
#define CMD33_ERASE_WRITE_BLOCK_END (33)
#define CMD38_ERASE  (38)

#define CMD48_READ_EXTR_SINGLE (48)
#define CMD49_WRITE_EXTR_SINGLE (49)
#define CMD58_READ_EXTR_MULTI (58)
#define CMD59_WRITE_EXTR_MULTI (59)

#define CMD55_APP_CMD (55)
#define CMD56_GEN_CMD (56)
#define ACMD6_SET_BUS_WIDTH (6)
#define ACMD13_STATUS (13)
#define ACMD22_SET_NUM_WR_BLOCKS (22)
#define ACMD23_SET_WR_BLK_ERASE_COUNT (23)
#define ACMD41_SEND_OP_COND (41)
#define ACMD42_SET_CLR_CARD_DETECT (42)
#define ACMD51_SEND_SCR (51)

#define HOST_PRINTF printf
#define CLK_FREQ_FOR_IDENTIFICATION 400000

#define HS_TIMING_IDX 185
#define STROBE_SUPPORT_IDX 184
#define BUS_WIDTH_IDX 183

/**
 * @brief EMMC Bus modes
 */
#define EMMC_EXT_ESD_BUS_MODE_1BIT      (0U)
#define EMMC_EXT_ESD_BUS_MODE_4BITS     (1U)
#define EMMC_EXT_ESD_BUS_MODE_8BITS     (2U)
#define EMMC_EXT_ESD_BUS_MODE_4BITS_DDR (5U)
#define EMMC_EXT_ESD_BUS_MODE_8BITS_DDR (6U)

/**
 * @brief EMMC HS_TIMING modes
 */
#define EMMC_EXT_ESD_HS_TIMING_LEGACY       (0U)
#define EMMC_EXT_ESD_HS_TIMING_HIGH_SPEED   (1U)
#define EMMC_EXT_ESD_HS_TIMING_HS200        (2U)
#define EMMC_EXT_ESD_HS_TIMING_HS400        (3U)

#pragma pack(1)
typedef struct {
    uint8_t reserved[15];
    uint8_t cmdq_mode_en;                           //[15]
    uint8_t secure_removal_type;
    uint8_t product_state_awareness_enablement;
    uint32_t max_pre_loading_data_size;
    uint32_t pre_loading_data_size;
    uint8_t ffu_status; //[26]
    uint8_t reserved1[2];
    uint8_t mode_operation_codes;
    uint8_t mode_config;
    uint8_t barrier_ctrl;
    uint8_t flush_cache;
    uint8_t cache_ctrl;
    uint8_t power_off_notification;
    uint8_t packed_failure_index;
    uint8_t packed_command_status;
    uint8_t context_conf[15];
    uint16_t ext_partitions_attribute;
    uint16_t exception_events_status;
    uint16_t exception_events_ctrl;
    uint8_t dyncap_needed;
    uint8_t class_6_ctrl;
    uint8_t ini_timeout_emu;
    uint8_t data_sector_size;
    uint8_t use_native_sector;
    uint8_t native_sector_size; //63]
    uint8_t vendor_specific_field[64];
    uint8_t rserved2[2];
    uint8_t program_cid_csd_ddr_support;
    uint8_t periodic_wakeup;
    uint8_t tcase_support;
    uint8_t production_state_awareness;
    uint8_t sec_bad_blk_mgmnt;
    uint8_t reserved3;
    uint32_t enh_start_addr;
    uint8_t enh_size_mult[3];
    uint32_t gp_size_mult[3];
    uint8_t partition_setting_completed;
    uint8_t partitions_attribute;
    uint8_t max_enh_size_mult[3];
    uint8_t partitioning_support; //[160]
    uint8_t hpm_mgmt;
    uint8_t rst_n_function;
    uint8_t bkops_en;
    uint8_t bkops_start;
    uint8_t sanitize_start;
    uint8_t wr_rel_param;
    uint8_t wr_rel_set;
    uint8_t rpmb_size_mult;
    uint8_t fw_config;
    uint8_t reserved4;
    uint8_t user_wp;
    uint8_t reserved5;
    uint8_t boot_wp;
    uint8_t boot_wp_status; //[174]
    uint8_t erase_group_def;
    uint8_t reserved6;
    uint8_t boot_bus_conditions;
    uint8_t boot_config_prot;
    uint8_t partition_config;
    uint8_t reserved7;
    uint8_t erased_mem_cont;
    uint8_t reserved8;
    uint8_t bus_width;
    uint8_t strobe_support; //[184]
    uint8_t hs_timing;
    uint8_t reserved9;
    uint8_t power_class;
    uint8_t reserved10;
    uint8_t cmd_set_rev;
    uint8_t reserved11;
    uint8_t cmd_set;
    uint8_t ext_csd_rev; //[192]
    uint8_t reserved12;
    uint8_t csd_structure;
    uint8_t rserved12;
    uint8_t device_type;
    uint8_t driver_strength;
    uint8_t out_of_interrupt_time;
    uint8_t partition_switch_time;
    uint8_t pwr_cl_52_195;
    uint8_t pwr_cl_26_195;
    uint8_t pwr_cl_52_360;
    uint8_t pwr_cl_26_360;
    uint8_t reserved13;
    uint8_t min_perf_r_4_26;
    uint8_t min_perf_w_4_26;
    uint8_t min_perf_r_8_26_4_52;
    uint8_t min_perf_w_8_256_4_52;
    uint8_t min_perf_r_8_52;
    uint8_t min_perf_w_8_52; //[210]
    uint8_t secure_wp_info;
    uint32_t sec_count;
    uint8_t sleep_notification_time;
    uint8_t s_a_timeout;
    uint8_t production_state_awareness_timeout;
    uint8_t s_c_vccq;
    uint8_t s_c_vcc;
    uint8_t hc_wp_grp_size;
    uint8_t rel_wr_sec_c;
    uint8_t erase_timeout_mult;
    uint8_t hc_erase_grp_size;
    uint8_t acc_size;
    uint8_t boot_size_mult;
    uint8_t reserved14;
    uint8_t boot_info;
    uint8_t sec_trim_mult;
    uint8_t sec_erase_mult;
    uint8_t sec_feature_support;
    uint8_t trim_mult;
    uint8_t reserved15; //[233]
    uint8_t min_perf_ddr_r_8_52;
    uint8_t min_perf_ddr_w_8_52;
    uint8_t pwr_cl_200_130;
    uint8_t pwr_cl_200_195;
    uint8_t pwr_cl_ddr_52_195;
    uint8_t pwr_cl_ddr_52_360;
    uint8_t cache_flush_policy;
    uint8_t ini_timeout_ap;
    uint32_t correctly_prg_sectors_num;
    uint8_t bkops_status;
    uint8_t power_off_long_time;
    uint8_t generic_cmd6_time;
    uint32_t cache_size;
    uint8_t pwr_cl_ddr_200_360;
    uint8_t firmware_version[8];
    uint8_t device_version[2];
    uint8_t optimal_trim_unit_size; //[264]
    uint8_t optimal_write_size;
    uint8_t optimal_read_size;
    uint8_t pre_eol_info;
    uint8_t device_life_time_est_typ_a;
    uint8_t device_life_time_est_typ_b;
    uint8_t vendor_proprietary_health_report[32];
    uint32_t number_of_fw_sectors_correctly_programmed;
    uint8_t reserved16;
    uint8_t cmdq_depth;
    uint8_t cmdq_support; //[308]
    uint8_t reserved17[177];
    uint8_t barrier_support;
    uint32_t ffu_arg;
    uint8_t operation_code_timeout;
    uint8_t ffu_features;
    uint8_t supported_modes;
    uint8_t ext_support;
    uint8_t large_unit_size_m1;
    uint8_t context_capabilities;
    uint8_t tag_res_size;
    uint8_t tag_unit_size;
    uint8_t data_tag_support;
    uint8_t max_packet_writes;
    uint8_t max_packed_reads;
    uint8_t bkops_support;
    uint8_t hpm_features;
    uint8_t s_cmd_set;
    uint8_t ext_security_err;
    uint8_t reserved18[6];
} emmc_ext_csd_t;
#pragma pack()

typedef union { ;
    struct {
        uint64_t : 1;
        uint64_t crc7: 7;
        uint64_t mdt: 12;
        uint64_t : 4;
        uint64_t psn: 32;
        uint64_t prv: 8;
        uint64_t pnm: 40;
        uint64_t oid: 16;
        uint64_t mid: 8;
    } sd_cid;
    struct {
        uint8_t reserved;
        uint8_t md;
        uint8_t psn[3];
        uint8_t prv;
        uint8_t pnm[6];
        uint8_t od;
        uint8_t cbx;
        uint8_t mid;
    } emmc_cid;
} sdmmc_cid_t;

typedef struct {
    bool erase_block_enable;
    bool write_group_enable;
    bool support_write_block_partial;
    bool support_read_block_partial;
    bool support_data_strobe;
    uint8_t spec_version;
    uint32_t max_freq;
    uint32_t read_block_len;
    uint32_t write_block_len;
    uint32_t sector_size;
    uint32_t write_group_size;
    uint64_t card_size_in_bytes;
    uint32_t erasable_unit_size;
} emmc_info_t;

typedef union {
    uint32_t ocr_word;
    struct {
        uint32_t : 7;
        uint32_t low_voltage_range: 1;
        uint32_t : 7;
        uint32_t support_2v7_2v8: 1;
        uint32_t support_2v8_2v9: 1;
        uint32_t support_2v9_3v0: 1;
        uint32_t support_3v0_3v1: 1;
        uint32_t support_3v1_3v2: 1;
        uint32_t support_3v2_3v3: 1;
        uint32_t support_3v3_3v4: 1;
        uint32_t support_3v5_3v6: 1;
        uint32_t switching_to_1v8_accepted: 1;
        uint32_t : 2;
        uint32_t over_2tb_support: 1;
        uint32_t : 1;
        uint32_t uhs2_card_status: 1;
        uint32_t card_capacity_status: 1;
        uint32_t card_power_up_status: 1;
    } sd_ocr;
    struct {
        uint32_t : 7;
        uint32_t low_voltage_range: 1;
        uint32_t : 7;
        uint32_t support_2v7_3v6: 9;
        uint32_t : 5;
        uint32_t access_mode: 2;
        uint32_t card_power_up_status: 1;
    } emmc_ocr;

} sdmmc_ocr_t;

typedef union {
    uint32_t status;
    struct {
        uint32_t : 3;
        uint32_t ake_seq_error: 1;
        uint32_t : 1;
        uint32_t app_cmd: 1;
        uint32_t fx_event: 1;
        uint32_t : 1;
        uint32_t ready_for_data: 1;
        uint32_t current_state: 4;
        uint32_t erase_reset: 1;
        uint32_t card_ecc_disabled: 1;
        uint32_t wp_erase_skip: 1;
        uint32_t csd_overwrite: 1;
        uint32_t : 2;
        uint32_t error: 1;
        uint32_t cc_error: 1;
        uint32_t card_ecc_failed: 1;
        uint32_t illegal_command: 1;
        uint32_t com_crc_error: 1;
        uint32_t lock_unlock_failed: 1;
        uint32_t card_is_locked: 1;
        uint32_t wp_violation: 1;
        uint32_t erase_param: 1;
        uint32_t erase_seq_error: 1;
        uint32_t block_len_error: 1;
        uint32_t address_error: 1;
        uint32_t out_of_range: 1;
    };
} sdcard_status_t;

typedef union {
    struct {
        uint32_t cmd_support: 4;
        uint32_t : 2;
        uint32_t sd_specx: 4;
        uint32_t sd_spec4: 1;
        uint32_t ex_security: 4;
        uint32_t sd_spec3: 1;
        uint32_t sd_bus_widths: 4;
        uint32_t sd_security: 3;
        uint32_t data_stat_after_erase: 1;
        uint32_t sd_spec: 4;
        uint32_t scr_structure: 4;
        uint32_t reserved;
    };
    uint32_t scr_word[2];
} sdcard_scr_t;

#endif /*EMMC_DEFS_H*/
