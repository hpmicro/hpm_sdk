/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_sdmmc_emmc.h"
#include "hpm_l1c_drv.h"

#define SPEED_1Kbps (1000U)
#define SPEED_1Mbps (1000UL * 1000UL)
#define MAX_BLOCK_COUNT ((1UL << 26) / 512 - 1U)
#define WRITE_BLOCK_TIMEOUT_IN_MS (1000U)
#define READ_BLOCK_TIMEOUT_IN_MS (100U)

#define EMMC_SECTOR_SIZE_DEFAULT (512U)

#define SIZE_128KB (128UL * SIZE_1KB)
#define SIZE_512KB (512UL * SIZE_1KB)

#define EMMC_RCA_DEFAULT (1U)

#define EMMC_CSD_VERSION_MAX (2U)
#define EMMC_EXT_CSD_REV_MAX (8U)
#define EMMC_CMD_SET_REV_MAX (0U)

#define EXT_CSD_SECURE_WP_INFO_SECURE_WP_SUPPORT_MASK (1U)
#define EXT_CSD_SECURE_WP_INFO_SECURE_WP_EN_STATUS (2U)

#define EXT_CSD_BOOT_INFO_ALT_BOOT_MODE_MASK (1U)
#define EXT_CSD_BOOT_INFO_DDR_BOOT_MODE_MASK (2U)
#define EXT_CSD_BOOT_INFO_HS_BOOT_MODE_MASK (4U)

#define EXT_CSD_SEC_FEATURE_SUPPORT_SECURE_ER_EN_MASK (1UL << 0)
#define EXT_CSD_SEC_FEATURE_SUPPORT_SEC_BD_BLK_EN_MASK (1UL << 2)
#define EXT_CSD_SEC_FEATURE_SUPPORT_SEC_GB_CL_EN_MASK (1UL << 4)
#define EXT_CSD_SEC_FEATURE_SUPPORT_SEC_SANITIZE_MASK (1UL << 6)

#define EXT_CSD_ERASE_GROUP_DEF_ENABLE_MASK (1UL << 0)


#define EXT_CSD_HS_TIMING_COMPATIBLE    (0U)
#define EXT_CSD_HS_TIMING_HIGH_SPEED    (1U)
#define EXT_CSD_HS_TIMING_HS200         (2U)
#define EXT_CSD_HS_TIMING_HS400         (3U)

/**
 * @brief Set Relative Address
 */
static hpm_stat_t emmc_set_rca(emmc_card_t *card, uint16_t relative_addr);

/**
 * @brief Send CSD register
 */
static hpm_stat_t emmc_send_csd(emmc_card_t *card);

/**
 * @brief Send Extend CSD register
 */
static hpm_stat_t emmc_send_ext_csd(emmc_card_t *card);

/* Decode Extend CSD */
static hpm_stat_t emmc_decode_ext_csd(emmc_card_t *card, emmc_ext_csd_t *ext_csd);

/* Polling Card Status register when the card is busy */
static hpm_stat_t emmc_polling_card_status_busy(emmc_card_t *card, uint32_t timeout_ms);

/* Send OP Condition command */
static hpm_stat_t emmc_send_op_cond(emmc_card_t *card, emmc_ocr_t ocr);

/* Decode CSD register */
static void emmc_decode_csd(emmc_card_t *card, uint32_t *raw_csd);

/* Set eMMC Bus Width */
static hpm_stat_t emmc_set_bus_width(emmc_card_t *card, emmc_bus_mode_t bus_mode);

/* Set eMMC HS Timing */
static hpm_stat_t emmc_set_hs_timing(emmc_card_t *card, emmc_hs_timing_t timing);

/* Check  */
static hpm_stat_t emmc_check_card_parameters(emmc_card_t *card);

static hpm_stat_t emmc_error_recovery(emmc_card_t *card);

static hpm_stat_t emmc_send_cmd(emmc_card_t *card, sdmmchost_cmd_t *cmd);
static hpm_stat_t emmc_transfer(emmc_card_t *card, sdmmchost_xfer_t *content);

static hpm_stat_t emmc_send_cmd(emmc_card_t *card, sdmmchost_cmd_t *cmd)
{
    hpm_stat_t status = sdmmchost_send_command(card->host, cmd);

    if ((status >= status_sdxc_busy) && (status <= status_sdxc_tuning_failed)) {
        hpm_stat_t error_recovery_status = emmc_error_recovery(card);
        if (error_recovery_status != status_success) {
            status = error_recovery_status;
        }
    }
    return status;
}

static hpm_stat_t emmc_transfer(emmc_card_t *card, sdmmchost_xfer_t *content)
{
    hpm_stat_t status = sdmmchost_transfer(card->host, content);

    if ((status >= status_sdxc_busy) && (status <= status_sdxc_tuning_failed)) {
        hpm_stat_t error_recovery_status = emmc_error_recovery(card);
        if (error_recovery_status != status_success) {
            status = error_recovery_status;
        }
    }
    return status;
}

static hpm_stat_t emmc_send_card_status(emmc_card_t *card)
{
    hpm_stat_t status;

    sdmmchost_cmd_t *cmd = &card->host->cmd;
    memset(cmd, 0, sizeof(*cmd));
    cmd->cmd_index = sdmmc_cmd_send_status;
    cmd->resp_type = sdmmc_resp_r1;
    cmd->cmd_argument = (uint32_t) card->relative_addr << 16;
    status = emmc_send_cmd(card, cmd);
    if (status != status_success) {
        return status;
    }

    card->current_r1_status.card_status = cmd->response[0];

    return status;
}

static hpm_stat_t emmc_send_op_cond(emmc_card_t *card, emmc_ocr_t ocr)
{
    sdmmchost_cmd_t *cmd = &card->host->cmd;
    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));

    cmd->cmd_index = emmc_cmd_send_op_cond;
    cmd->resp_type = sdmmc_resp_r3;
    cmd->cmd_argument = ocr.ocr_word;

    hpm_stat_t status = emmc_send_cmd(card, cmd);
    if (status != status_success) {
        return status;
    }

    return status_success;
}

static void emmc_decode_csd(emmc_card_t *card, uint32_t *raw_csd)
{
    emmc_csd_t *csd = &card->csd;

    csd->sector_size = EMMC_SECTOR_SIZE_DEFAULT;
    csd->csd_structure = extract_csd_field(raw_csd, 127, 126);
    csd->spec_version = (uint8_t) extract_csd_field(raw_csd, 125, 122);
    uint8_t taac = (uint8_t) extract_csd_field(raw_csd, 119, 112);
    uint8_t nsac = (uint8_t) extract_csd_field(raw_csd, 111, 104);

    uint32_t taac_time_unit_ns[8] = { 1UL, 10UL, 100UL, 1000UL, 10000UL, 100000UL, 1000000UL, 10000000UL };
    uint32_t taac_mult_factor[16] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
    uint8_t taac_time_unit = taac & 0x07;
    uint8_t taac_mult_fact = (taac >> 3) & 0xFU;
    csd->taac_ns = taac_time_unit_ns[taac_time_unit] * taac_mult_factor[taac_mult_fact];
    csd->nsac_cycles = 100UL * nsac;

    uint32_t tran_speed = (uint8_t) extract_csd_field(raw_csd, 103, 96);
    uint32_t bitrate_unit = tran_speed & 0x7U;
    uint32_t time_value = (tran_speed >> 3) & 0xFU;
    const uint32_t bitrate_unit_list[8] = {
        100UL * SPEED_1Kbps, SPEED_1Mbps, 10U * SPEED_1Mbps, 100U * SPEED_1Mbps, 0, 0, 0, 0
    };
    const uint32_t time_value_list[16] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
    csd->max_freq = bitrate_unit_list[bitrate_unit] / 10U * time_value_list[time_value];

    csd->card_command_class = (uint16_t) extract_csd_field(raw_csd, 95, 84);

    uint32_t read_bl_len = (uint8_t) extract_csd_field(raw_csd, 83, 80);
    csd->max_read_block_len = 1UL << read_bl_len;

    csd->support_read_block_partial = (extract_csd_field(raw_csd, 79, 79) != 0);
    csd->support_write_block_misalignment = (extract_csd_field(raw_csd, 78, 78) != 0);
    csd->support_read_block_misalignment = (extract_csd_field(raw_csd, 77, 77) != 0);
    csd->is_dsr_implemented = (extract_csd_field(raw_csd, 76, 76) != 0);

    /* TODO */
    uint32_t c_size = (uint32_t) extract_csd_field(raw_csd, 73, 62);
    uint32_t c_size_mult = (uint8_t) extract_csd_field(raw_csd, 49, 47);

    uint32_t num_of_blocks = (c_size + 1U) * (4UL << c_size_mult);
    csd->device_size_in_bytes = (uint64_t) num_of_blocks * csd->max_read_block_len;

    csd->read_current_vdd_min = (emmc_csd_vdd_current_min_t) extract_csd_field(raw_csd, 61, 59);
    csd->read_current_vdd_max = (emmc_csd_vdd_current_max_t) extract_csd_field(raw_csd, 58, 56);
    csd->write_current_vdd_min = (emmc_csd_vdd_current_min_t) extract_csd_field(raw_csd, 55, 53);
    csd->write_current_vdd_max = (emmc_csd_vdd_current_max_t) extract_csd_field(raw_csd, 52, 50);

    uint32_t erase_group_size = extract_csd_field(raw_csd, 46, 42);
    uint32_t erase_group_multiplier = extract_csd_field(raw_csd, 41, 37);
    uint32_t wp_group_size = extract_csd_field(raw_csd, 36, 32);

    /* TODO: check the unit later */
    csd->erase_group_size = (erase_group_size + 1U) * (erase_group_multiplier + 1U);
    csd->write_protect_group_size = (1U + wp_group_size) * csd->erase_group_size;

    csd->is_write_protection_group_enabled = (extract_csd_field(raw_csd, 31, 31) != 0);

    uint32_t r2w_factor = extract_csd_field(raw_csd, 28, 26);
    uint32_t write_bl_len = extract_csd_field(raw_csd, 25, 22);
    csd->max_write_block_len = 1UL << write_bl_len;
    csd->write_speed_factor = (1U << r2w_factor);

    csd->support_write_block_partial = (extract_csd_field(raw_csd, 21, 21) != 0);
    csd->support_content_protect_app = (extract_csd_field(raw_csd, 16, 16) != 0);
    csd->is_predefined_file_format = (extract_csd_field(raw_csd, 15, 15) != 0);
    csd->support_copy = (extract_csd_field(raw_csd, 14, 14) != 0);
    csd->support_permanent_write_protect = (extract_csd_field(raw_csd, 13, 13) != 0);
    csd->support_temporary_write_protect = (extract_csd_field(raw_csd, 12, 12) != 0);

    csd->file_format = (uint8_t) extract_csd_field(raw_csd, 11, 10);

    csd->ecc_type = (uint8_t) extract_csd_field(raw_csd, 9, 8);
}

static hpm_stat_t emmc_send_csd(emmc_card_t *card)
{
    sdmmchost_cmd_t *cmd = &card->host->cmd;
    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));

    cmd->cmd_index = emmc_cmd_send_csd;
    cmd->cmd_argument = (uint32_t) card->relative_addr << 16;
    cmd->resp_type = sdmmc_resp_r2;

    hpm_stat_t status = emmc_send_cmd(card, cmd);
    if (status != status_success) {
        return status;
    }

    uint32_t temp_buf[4];
    memcpy(temp_buf, cmd->response, sizeof(temp_buf));
    emmc_decode_csd(card, temp_buf);

    return status_success;
}

static uint32_t emmc_get_24bit_num_from_ext_csd(const uint8_t *array)
{
    uint32_t value = array[0] | (((uint32_t) array[1]) << 8) | (((uint32_t) array[2]) << 16);
    return value;
}

static hpm_stat_t emmc_decode_ext_csd(emmc_card_t *card, emmc_ext_csd_t *ext_csd)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(ext_csd->csd_structure > EMMC_CSD_VERSION_MAX);
        HPM_BREAK_IF(ext_csd->ext_csd_rev > EMMC_EXT_CSD_REV_MAX);
        HPM_BREAK_IF(ext_csd->cmd_set_revision > EMMC_CMD_SET_REV_MAX);

        emmc_device_attribute_t *attr = (emmc_device_attribute_t *) &card->device_attribute;

        attr->is_secure_write_protection_supported =
            ((ext_csd->secure_wp_info & EXT_CSD_SECURE_WP_INFO_SECURE_WP_SUPPORT_MASK) != 0);
        attr->write_protection_mode = ((ext_csd->secure_wp_info & EXT_CSD_SECURE_WP_INFO_SECURE_WP_EN_STATUS) != 0)
                                      ? emmc_write_protection_mode_secure : emmc_write_protection_mode_legacy;


        attr->boot_info.is_alt_boot_mode_supported =
            IS_HPM_BITMASK_SET(ext_csd->boot_info, EXT_CSD_BOOT_INFO_ALT_BOOT_MODE_MASK);
        attr->boot_info.is_ddr_boot_mode_supported =
            IS_HPM_BITMASK_SET(ext_csd->boot_info, EXT_CSD_BOOT_INFO_DDR_BOOT_MODE_MASK);
        attr->boot_info.is_hs_boot_mode_supported =
            IS_HPM_BITMASK_SET(ext_csd->boot_info, EXT_CSD_BOOT_INFO_HS_BOOT_MODE_MASK);

        attr->secure_feature_info.is_secure_erase_supported =
            IS_HPM_BITMASK_SET(ext_csd->secure_feature_support, EXT_CSD_SEC_FEATURE_SUPPORT_SECURE_ER_EN_MASK);
        attr->secure_feature_info.is_secure_bad_block_management_supported =
            IS_HPM_BITMASK_SET(ext_csd->secure_feature_support, EXT_CSD_SEC_FEATURE_SUPPORT_SEC_BD_BLK_EN_MASK);
        attr->secure_feature_info.is_secure_insecure_trim_supported =
            IS_HPM_BITMASK_SET(ext_csd->secure_feature_support, EXT_CSD_SEC_FEATURE_SUPPORT_SEC_GB_CL_EN_MASK);
        attr->secure_feature_info.is_secure_sanitize_supported =
            IS_HPM_BITMASK_SET(ext_csd->secure_feature_support, EXT_CSD_SEC_FEATURE_SUPPORT_SEC_SANITIZE_MASK);

        attr->boot_partition_size = SIZE_128KB * ext_csd->boot_partition_size_mult;
        attr->rpmb_partition_size = SIZE_128KB * ext_csd->rpmb_size;

        attr->max_enhanced_area_size =
            SIZE_512KB * emmc_get_24bit_num_from_ext_csd(ext_csd->enhanced_user_data_area_size);

        for (uint32_t i = 0; i < 4; i++) {
            attr->gp_partition_size[i] =
                ext_csd->high_capacity_write_protect_group_size * ext_csd->high_capacity_erase_unit_size * SIZE_512KB *
                    emmc_get_24bit_num_from_ext_csd(ext_csd->general_purpose_partition_size[i]);
        }

        attr->max_enhanced_area_size = SIZE_512KB * emmc_get_24bit_num_from_ext_csd(ext_csd->max_enh_size_mult) *
            ext_csd->high_capacity_write_protect_group_size * ext_csd->high_capacity_erase_unit_size;

        attr->super_page_size = 512 * (1UL << (ext_csd->access_size & 0x03));
        attr->write_protect_group_size =
            SIZE_512KB * ext_csd->high_capacity_erase_unit_size * ext_csd->high_capacity_write_protect_group_size;
        attr->erase_group_size = 512UL * ext_csd->high_capacity_erase_unit_size;
        attr->device_size_in_bytes = (uint64_t) 512UL * ext_csd->sector_count;
        attr->sector_count = ext_csd->sector_count;
        attr->sector_size = (ext_csd->data_sector_size == 0) ? 512U : 4U * SIZE_1KB;

        attr->device_type.device_type = ext_csd->device_type;

        attr->large_unit_size = SIZE_1MB * (ext_csd->large_unit_size + 1U);
        attr->optimal_read_size = 4U * SIZE_1KB * ext_csd->optimal_read_size;
        attr->optimal_write_size = 4U * SIZE_1KB * ext_csd->optimal_write_size;
        attr->optimal_trim_unit_size = 4U * SIZE_1KB * (1UL << ext_csd->optimal_trim_unit_size);

        attr->operation_codes_timeout_us = 100UL * (2U << ext_csd->operation_code_timeout);
        attr->switch_cmd_timeout_ms = 10UL * ext_csd->generic_cmd6_timeout;
        attr->power_off_timeout_ms = 10UL * ext_csd->power_off_long_timeout;
        attr->init_timeout_after_partition_ms = 100UL * ext_csd->init_timeout_after_partitioning;
        attr->trim_timeout_ms = 300UL * ext_csd->trim_mult;
        attr->secure_erase_timeout_ms = 300UL * ext_csd->secure_erase_mult;
        attr->secure_trim_timeout_ms = 300UL * ext_csd->secure_trim_mult;

        attr->erase_timeout_ms = 300UL * ext_csd->erase_timeout_mult;
        attr->partition_switch_timeout_ms = 100UL * ext_csd->partition_switch_timing;
        attr->out_of_interrupt_timeout_ms = 100UL * ext_csd->out_of_interrupt_timing;
        attr->sleep_notification_timeout_us = 10UL * (2UL << ext_csd->sleep_notification_timeout);
        attr->sleep_awake_timeout_ns = 100UL * (2UL << ext_csd->sleep_or_awake_timeout);
        attr->production_state_awareness_timeout_us = 100UL * (2UL << ext_csd->production_state_awareness_timeout);

        attr->sleep_current_vcc_ua = 2UL << ext_csd->sleep_current_vcc;
        attr->sleep_current_vccq_ua = 2UL << ext_csd->sleep_current_vccq;

        attr->use_high_capacity_erase_unit_size =
            IS_HPM_BITMASK_SET(ext_csd->erase_group_def, EXT_CSD_ERASE_GROUP_DEF_ENABLE_MASK);
        attr->is_all_0xffs_for_erased_region = IS_HPM_BITMASK_SET(ext_csd->erased_mem_content, 1);
        attr->is_enhanced_strobe_supported = IS_HPM_BITMASK_SET(ext_csd->strobe_support, 1);
        attr->is_cmd_queue_mode_enabled = IS_HPM_BITMASK_SET(ext_csd->cmdq_support, 1);

        status = status_success;

    } while (false);

    return status;
}

static hpm_stat_t emmc_send_ext_csd(emmc_card_t *card)
{
    hpm_stat_t status;
    sdmmchost_cmd_t *cmd = &card->host->cmd;
    sdmmchost_data_t *data = &card->host->data;
    sdmmchost_xfer_t *content = &card->host->xfer;
    memset(cmd, 0, sizeof(*cmd));
    memset(data, 0, sizeof(*data));
    memset(content, 0, sizeof(*content));
    do {
        cmd->cmd_index = emmc_cmd_send_ext_csd;
        cmd->resp_type = sdmmc_resp_r1;
        cmd->cmd_argument = 0;
        data->block_size = sizeof(emmc_ext_csd_t);
        data->block_cnt = 1;
        data->rx_data = (uint32_t *) sdmmc_get_sys_addr(card->host, (uint32_t) card->host->buffer);
        content->data = data;
        content->command = cmd;
        status = emmc_transfer(card, content);
        if (status != status_success) {
            break;
        }

        (void) memcpy(&card->ext_csd, card->host->buffer, sizeof(emmc_ext_csd_t));

        status = emmc_decode_ext_csd(card, &card->ext_csd);

    } while (false);

    return status;
}

static hpm_stat_t emmc_all_send_cid(emmc_card_t *card)
{
    sdmmchost_cmd_t *cmd = &card->host->cmd;
    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));

    cmd->cmd_index = emmc_cmd_all_send_cid;
    cmd->resp_type = sdmmc_resp_r2;

    hpm_stat_t status = emmc_send_cmd(card, cmd);
    if (status != status_success) {
        return status;
    }
    for (uint32_t i = 0; i < 3; i++) {
        card->cid.cid_words[i] = cmd->response[i];
    }

    return status;
}

hpm_stat_t emmc_send_cid(emmc_card_t *card)
{
    sdmmchost_cmd_t *cmd = &card->host->cmd;
    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));

    cmd->cmd_index = emmc_cmd_send_cid;
    cmd->cmd_argument = card->relative_addr << 16;
    cmd->resp_type = sdmmc_resp_r2;

    hpm_stat_t status = emmc_send_cmd(card, cmd);
    if (status != status_success) {
        return status;
    }
    for (uint32_t i = 0; i < 4; i++) {
        card->cid.cid_words[i] = cmd->response[i];
    }

    return status;
}

static hpm_stat_t emmc_set_rca(emmc_card_t *card, uint16_t relative_addr)
{
    sdmmchost_cmd_t *cmd = &card->host->cmd;
    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));

    cmd->cmd_index = emmc_cmd_set_relative_addr;
    cmd->cmd_argument = relative_addr << 16;
    cmd->resp_type = sdmmc_resp_r1;

    hpm_stat_t status = emmc_send_cmd(card, cmd);
    if (status != status_success) {
        return status;
    }
    card->relative_addr = relative_addr;

    return status;
}

static hpm_stat_t emmc_error_recovery(emmc_card_t *card)
{
    sdmmchost_cmd_t *cmd = &card->host->cmd;
    cmd->cmd_index = sdmmc_cmd_stop_transmission;
    cmd->cmd_type = sdxc_cmd_type_abort_cmd;
    cmd->resp_type = sdmmc_resp_r1b;

    return sdmmchost_error_recovery(card->host, cmd);
}

static hpm_stat_t emmc_check_card_parameters(emmc_card_t *card)
{
    hpm_stat_t status;
    if ((card == NULL) || (card->host == NULL) || (card->host->host_param.base == NULL)) {
        status = status_invalid_argument;
    } else {
        status = status_success;
    }
    return status;
}

static hpm_stat_t emmc_set_bus_width(emmc_card_t *card, emmc_bus_mode_t bus_mode)
{
    emmc_switch_cmd_arg_t switch_arg = { .argument = 0U };
    switch_arg.access = emmc_switch_cmd_access_mode_write_byte;
    switch_arg.cmd_set = 0;
    switch_arg.index = EMMC_EXT_CSD_INDEX_BUS_WDITH;
    switch_arg.value = (uint8_t) bus_mode;

    hpm_stat_t status = emmc_switch_function(card, switch_arg, card->device_attribute.switch_cmd_timeout_ms * 1000U);

    sdmmc_buswidth_t bus_width;

    if (status == status_success) {
        switch (bus_mode) {
        default:
            bus_width = sdmmc_bus_width_1bit;
            break;
        case emmc_bus_mode_x4_sdr:
        case emmc_bus_mode_x4_ddr:
            bus_width = sdmmc_bus_width_4bit;
            break;
        case emmc_bus_mode_x8_sdr:
        case emmc_bus_mode_x8_ddr:
        case emmc_bus_mode_x8_ddr_ds:
            bus_width = sdmmc_bus_width_8bit;
            break;
        }
        sdmmchost_set_card_bus_width(card->host, bus_width);
    }

    return status;
}

static hpm_stat_t emmc_set_hs_timing(emmc_card_t *card, emmc_hs_timing_t timing)
{
    hpm_stat_t status;
    uint32_t clock_option = MMC_CLOCK_26MHz;

    emmc_switch_cmd_arg_t switch_arg = { .argument = 0U };
    switch_arg.access = emmc_switch_cmd_access_mode_write_byte;
    switch_arg.index = EMMC_EXT_CSD_INDEX_HS_TIMING;
    switch_arg.value = (uint8_t) timing;
    status = emmc_switch_function(card, switch_arg, card->device_attribute.switch_cmd_timeout_ms * 1000U);
    if (status != status_success) {
        return status;
    }

    card->current_hs_timing = timing;

    bool clock_inverse = true;
    switch (card->current_hs_timing) {
    case emmc_timing_legacy:
        sdmmchost_set_speed_mode(card->host, sdmmc_emmc_speed_legacy);
        break;
    case emmc_timing_high_speed:
        sdmmchost_set_speed_mode(card->host, sdmmc_emmc_speed_high_speed_sdr);
        clock_option = MMC_CLOCK_52MHz;
        break;
    case emmc_timing_hs200:
        sdmmchost_set_speed_mode(card->host, sdmmc_emmc_speed_hs200);
        clock_option = MMC_CLOCK_HS200;
        break;
    case emmc_timing_hs400:
        clock_inverse = false;
        sdmmchost_set_speed_mode(card->host, sdmmc_emmc_speed_hs400);
        clock_option = MMC_CLOCK_HS400;
        break;
    }

    sdmmchost_set_card_clock(card->host, clock_option, clock_inverse);
    return status;
}

void emmc_card_deinit(emmc_card_t *card);

hpm_stat_t emmc_host_init(emmc_card_t *card)
{
    hpm_stat_t status = status_success;
    assert(card != NULL);

    if (!card->is_host_ready) {
        card->host = sdmmchost_get_host();
        if (card->host == NULL) {
            return status_invalid_argument;
        }
        status = sdmmchost_init(card->host);
        if (status == status_success) {
            card->is_host_ready = true;
            card->operation_voltage = sdmmc_operation_voltage_3v3;
        }
    } else {
        status = sdmmchost_init(card->host);
        if (status == status_success) {
            card->operation_voltage = sdmmc_operation_voltage_3v3;
        }
    }

    return status_success;
}

hpm_stat_t emmc_init(emmc_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF(card == NULL);
        if ((card->host == NULL) || (!card->host->card_init_done)) {
            status = emmc_host_init(card);
            HPM_BREAK_IF(status != status_success);
            sdmmchost_reset(card->host);
            sdmmchost_enable_emmc_support(card->host, true);

            card->relative_addr = 0;
            card->current_hs_timing = emmc_timing_legacy;

            card->host->host_param.delay_ms(1); /* Wait a while in case the card connection is still not stable */

            status = emmc_card_init(card);

            if (status == status_success) {
                card->host->card_init_done = true;
            }
        } else {
            status = status_success;
        }
    } while (false);

    return status;
}

void emmc_deinit(emmc_card_t *card)
{

}

hpm_stat_t emmc_probe_device(emmc_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        status = sdmmc_go_idle_state(card->host, (uint32_t) emmc_idle_option_go_idle_state);
        HPM_BREAK_IF(status != status_success);

        emmc_ocr_t ocr = { .ocr_word = 0xc0ff8080 };

        /* Query OCR */
        status = emmc_send_op_cond(card, ocr);
        HPM_BREAK_IF(status != status_success);
        sdmmchost_cmd_t *cmd = &card->host->cmd;

        emmc_ocr_t recv_ocr = { .ocr_word = 0 };
        do {
            status = emmc_send_op_cond(card, ocr);
            HPM_BREAK_IF(status != status_success);
            recv_ocr.ocr_word = cmd->response[0];
        } while (recv_ocr.powerup_status == 0);

        card->ocr.ocr_word = recv_ocr.ocr_word;

    } while (false);

    return status;
}

hpm_stat_t emmc_card_init(emmc_card_t *card)
{
    hpm_stat_t status = emmc_check_card_parameters(card);
    do {
        HPM_BREAK_IF(status != status_success);

        sdmmchost_set_card_bus_width(card->host, sdxc_bus_width_1bit);
        sdmmchost_set_card_clock(card->host, SDMMC_CLOCK_400KHZ, true);
        status = emmc_probe_device(card);
        HPM_BREAK_IF(status != status_success);

        /* Send CMD2 */
        status = emmc_all_send_cid(card);
        HPM_BREAK_IF(status != status_success);
        /* Send CMD3 */
        status = emmc_set_rca(card, EMMC_RCA_DEFAULT);
        HPM_BREAK_IF(status != status_success);
        /* Send CMD9 (SEND_CSD) */
        status = emmc_send_csd(card);
        HPM_BREAK_IF(status != status_success);
        /* Send CMD10 (SEND_CID) */
        status = emmc_send_cid(card);
        HPM_BREAK_IF(status != status_success);

        /* Send CMD7 */
        status = emmc_select_card(card, true);
        HPM_BREAK_IF(status != status_success);

        sdmmchost_set_card_clock(card->host, SD_CLOCK_25MHZ, true);

        /*  Send CMD8 */
        status = emmc_send_ext_csd(card);
        HPM_BREAK_IF(status != status_success);

        /* Send CMD16: Set block size */
        status = sdmmc_set_block_size(card->host, SDMMC_BLOCK_SIZE_DEFAULT);
        if (status != status_success) {
            return status;
        }

        emmc_bus_mode_t bus_mode;
        sdmmc_speed_mode_t speed_mode;
        bus_mode = emmc_bus_mode_x4_sdr;
        speed_mode = sdmmc_emmc_speed_high_speed_sdr;
        status = emmc_set_bus_width(card, bus_mode);
        if (status != status_success) {
            return status;
        }

        /* Send CMD6: Set bus timing  */
        status = emmc_set_hs_timing(card, emmc_timing_high_speed);
        HPM_BREAK_IF(status != status_success);

        card->current_hs_timing = emmc_timing_high_speed;

        card->current_bus_mode = bus_mode;

        sdmmchost_set_speed_mode(card->host, speed_mode);

        sdmmchost_set_card_clock(card->host, SD_CLOCK_50MHZ, false);

        /* Perform Tuning process if necessary */
        if (card->current_hs_timing >= emmc_timing_high_speed) {
            status = sdmmc_enable_auto_tuning(card->host);
            HPM_BREAK_IF(status != status_success);
        }
    } while (false);

    return status;
}

bool emmc_is_card_present(emmc_card_t *card)
{
    return sdmmchost_is_card_detected(card->host);
}

hpm_stat_t emmc_select_card(emmc_card_t *card, bool is_selected)
{
    uint16_t rca = (is_selected) ? card->relative_addr : 0;
    return sdmmc_select_card(card->host, rca, is_selected);
}

hpm_stat_t emmc_read_blocks(emmc_card_t *card, uint8_t *buffer, uint32_t start_block, uint32_t block_count)
{
    hpm_stat_t status = emmc_check_card_parameters(card);
    do {
        HPM_BREAK_IF(status != status_success);

        if (!card->host->card_init_done) {
            status = status_sdmmc_device_init_required;
            break;
        }

        sdmmchost_cmd_t *cmd = &card->host->cmd;
        sdmmchost_data_t *data = &card->host->data;
        sdmmchost_xfer_t *content = &card->host->xfer;
        memset(cmd, 0, sizeof(*cmd));
        memset(data, 0, sizeof(*data));
        memset(content, 0, sizeof(*content));

        while (block_count > 0) {
            uint32_t read_block_count = (block_count >= MAX_BLOCK_COUNT) ? MAX_BLOCK_COUNT : block_count;
            if (read_block_count > 1) {
                cmd->cmd_index = sdmmc_cmd_read_multiple_block;
                data->enable_auto_cmd23 = true;
            } else {
                cmd->cmd_index = sdmmc_cmd_read_single_block;
            }
            cmd->resp_type = sdmmc_resp_r1;
            cmd->cmd_argument = start_block;
            data->block_size = SDMMC_BLOCK_SIZE_DEFAULT;
            data->block_cnt = read_block_count;
            data->rx_data = (uint32_t *) sdmmc_get_sys_addr(card->host, (uint32_t) buffer);
            content->data = data;
            content->command = cmd;
            uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) data->rx_data);
            uint32_t end_addr = (uint32_t) data->rx_data + card->device_attribute.sector_size * block_count;
            uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(end_addr);
            uint32_t aligned_size = aligned_end - aligned_start;
            l1c_dc_flush(aligned_start, aligned_size);
            status = emmc_transfer(card, content);
            l1c_dc_invalidate(aligned_start, aligned_size);
            if (status != status_success) {
                break;
            }
            block_count -= read_block_count;
            start_block += read_block_count;
            buffer += SDMMC_BLOCK_SIZE_DEFAULT * read_block_count;
        }
    } while (false);

    return status;
}

hpm_stat_t emmc_write_blocks(emmc_card_t *card, const uint8_t *buffer, uint32_t start_block, uint32_t block_count)
{
    hpm_stat_t status = emmc_check_card_parameters(card);
    do {
        HPM_BREAK_IF(status != status_success);

        if (!card->host->card_init_done) {
            status = status_sdmmc_device_init_required;
            break;
        }

        sdmmchost_cmd_t *cmd = &card->host->cmd;
        sdmmchost_data_t *data = &card->host->data;
        sdmmchost_xfer_t *content = &card->host->xfer;
        memset(cmd, 0, sizeof(*cmd));
        memset(data, 0, sizeof(*data));
        memset(content, 0, sizeof(*content));

        status = emmc_polling_card_status_busy(card, WRITE_BLOCK_TIMEOUT_IN_MS);
        HPM_BREAK_IF(status != status_success);
        while (block_count > 0) {
            uint32_t write_block_count = (block_count >= MAX_BLOCK_COUNT) ? MAX_BLOCK_COUNT : block_count;
            if (write_block_count > 1) {
                cmd->cmd_index = sdmmc_cmd_write_multiple_block;
                data->enable_auto_cmd23 = true;
            } else {
                cmd->cmd_index = sdmmc_cmd_write_single_block;
                data->enable_auto_cmd12 = false;
            }
            cmd->resp_type = sdmmc_resp_r1;
            cmd->cmd_argument = start_block;
            data->block_size = SDMMC_BLOCK_SIZE_DEFAULT;
            data->block_cnt = write_block_count;
            data->tx_data = (const uint32_t *) sdmmc_get_sys_addr(card->host, (uint32_t) buffer);
            content->data = data;
            content->command = cmd;
            uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t) data->tx_data);
            uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(
                (uint32_t) data->tx_data + card->device_attribute.sector_size * write_block_count);
            uint32_t aligned_size = aligned_end - aligned_start;
            l1c_dc_flush(aligned_start, aligned_size);
            status = emmc_transfer(card, content);
            if (status != status_success) {
                break;
            }
            status = emmc_polling_card_status_busy(card, WRITE_BLOCK_TIMEOUT_IN_MS);
            HPM_BREAK_IF(status != status_success);
            block_count -= write_block_count;
            start_block += write_block_count;
            buffer += SDMMC_BLOCK_SIZE_DEFAULT * write_block_count;
        }

    } while (false);

    return status;
}

/**
 * @brief Calculate SD erase timeout value
 * Refer to SD_Specification_Part1_Physical_Layer_Specification_Version4.20.pdf, section 4.14 for more details.
 */
static uint32_t emmc_calculate_erase_timeout(emmc_card_t *card, uint32_t start_block, uint32_t block_count)
{
    uint32_t erase_timeout;
    if (card->device_attribute.erase_timeout_ms == 0) {
        erase_timeout = block_count * 250U;
    } else {
        erase_timeout = card->device_attribute.erase_timeout_ms;
    }
    return erase_timeout;
}

hpm_stat_t
emmc_erase_blocks(emmc_card_t *card, uint32_t start_block, uint32_t block_count, emmc_erase_option_t erase_option)
{
    hpm_stat_t status = emmc_check_card_parameters(card);
    do {
        HPM_BREAK_IF(status != status_success);

        if (!card->host->card_init_done) {
            status = status_sdmmc_device_init_required;
            break;
        }

        sdmmchost_cmd_t *cmd = &card->host->cmd;
        memset(cmd, 0, sizeof(*cmd));
        uint32_t erase_timeout = emmc_calculate_erase_timeout(card, start_block, block_count);
        /* Send erase start */
        cmd->cmd_index = emmc_cmd_erase_group_start;
        cmd->cmd_argument = start_block;
        cmd->resp_type = sdmmc_resp_r1;
        status = emmc_send_cmd(card, cmd);
        HPM_BREAK_IF(status != status_success);
        /* Send Erase end */
        cmd->cmd_index = emmc_cmd_erase_group_end;
        cmd->cmd_argument = start_block + block_count - 1U;
        status = emmc_send_cmd(card, cmd);
        HPM_BREAK_IF(status != status_success);

        /* Send erase command */
        uint32_t argument = 0;
        switch (erase_option) {
        default:
            argument = 0;
            break;
        case emmc_erase_option_discard:
            argument = 3;
            break;
        case emmc_erase_option_trim:
            argument = 1;
            break;
        }
        cmd->cmd_index = sdmmc_cmd_erase;
        cmd->cmd_argument = argument;
        cmd->resp_type = sdmmc_resp_r1b;
        cmd->cmd_timeout_ms = erase_timeout;
        status = emmc_send_cmd(card, cmd);
        HPM_BREAK_IF(status != status_success);

        /* Wait until erase completed. */
        status = emmc_polling_card_status_busy(card, erase_timeout);

    } while (false);

    return status;
}

hpm_stat_t emmc_polling_card_status_busy(emmc_card_t *card, uint32_t timeout_ms)
{
    hpm_stat_t status = status_invalid_argument;
    bool is_busy = true;
    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL));

        status = emmc_send_card_status(card);
        if (status != status_success) {
            break;
        }
        if ((card->current_r1_status.current_state == sdmmc_state_program) ||
            (card->current_r1_status.ready_for_data == 0U)) {
            is_busy = true;
            card->host->host_param.delay_ms(1);
            timeout_ms--;
        } else {
            is_busy = false;
        }

    } while (is_busy && (timeout_ms > 0));

    if ((status == status_success) && is_busy) {
        status = status_sdmmc_wait_busy_timeout;
    }

    return status;
}

hpm_stat_t emmc_switch_function(emmc_card_t *card, emmc_switch_cmd_arg_t arg, uint32_t timeout_us)
{
    hpm_stat_t error = status_invalid_argument;
    do {
        HPM_BREAK_IF(card == NULL);

        sdmmchost_cmd_t *cmd = &card->host->cmd;
        (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));

        cmd->cmd_index = emmc_cmd_switch;
        cmd->cmd_argument = arg.argument;
        cmd->resp_type = sdmmc_resp_r1b;
        cmd->cmd_timeout_ms = (timeout_us + 999) / 1000;
        error = emmc_send_cmd(card, cmd);

    } while (false);

    return error;
}
