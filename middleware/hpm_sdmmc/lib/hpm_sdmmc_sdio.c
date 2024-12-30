/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_sdmmc_common.h"
#include "hpm_sdmmc_sdio.h"
#include "hpm_l1c_drv.h"

static hpm_stat_t sdio_send_if_cond(sdio_card_t *card);

static hpm_stat_t sdio_io_send_op_if_cond(sdio_card_t *card, uint32_t arg);

static hpm_stat_t sdio_send_rca(sdio_card_t *card);

static hpm_stat_t sdio_probe_sdio_card(sdio_card_t *card);

static hpm_stat_t sdio_read_cccr(sdio_card_t *card);

static hpm_stat_t sdio_set_timing(sdio_card_t *card);

static hpm_stat_t sdio_set_bus_width(sdio_card_t *card);

static void sdio_switch_to_1v8(sdio_card_t *card);


static hpm_stat_t sdio_send_command(sdio_card_t *card, sdmmchost_cmd_t *cmd);

static hpm_stat_t sdio_transfer(sdio_card_t *card, sdmmchost_xfer_t *xfer);

static hpm_stat_t sdio_error_recovery(sdio_card_t *card);


static hpm_stat_t sdio_error_recovery(sdio_card_t *card)
{
    uint32_t cmd_arg = card->host->cmd.cmd_argument;
    uint32_t cmd_index = card->host->cmd.cmd_index;
    sdxc_command_t *cmd;
    if (cmd_index == sdio_cmd_io_rw_direct) {
        /* Abort function transfer */
        const sdio_cmd52_arg_t *cmd52_arg = (const sdio_cmd52_arg_t *) &cmd_arg;
        cmd = &card->host->cmd;
        cmd->cmd_index = sdio_cmd_io_rw_direct;
        sdio_cmd52_arg_t *abort_arg = (sdio_cmd52_arg_t *) &cmd->cmd_argument;
        abort_arg->func_num = 0;
        abort_arg->reg_addr = 6;
        abort_arg->wr_data = cmd52_arg->func_num;
        abort_arg->write = 1;
        cmd->cmd_type = sdxc_cmd_type_abort_cmd;
    } else if (cmd_index == sdio_cmd_io_rw_extend) {
        /* Abort function transfer */
        const sdio_cmd53_arg_t *cmd53_arg = (const sdio_cmd53_arg_t *) &cmd_arg;
        cmd = &card->host->cmd;
        cmd->cmd_index = sdio_cmd_io_rw_direct;
        sdio_cmd52_arg_t *abort_arg = (sdio_cmd52_arg_t *) &cmd->cmd_argument;
        abort_arg->func_num = 0;
        abort_arg->reg_addr = 6;
        abort_arg->wr_data = cmd53_arg->func_num;
        abort_arg->write = 1;
        cmd->cmd_type = sdxc_cmd_type_abort_cmd;
    } else {
        cmd = NULL;
    }

    return sdmmchost_error_recovery(card->host, cmd);
}

static hpm_stat_t sdio_send_command(sdio_card_t *card, sdmmchost_cmd_t *cmd)
{
    hpm_stat_t status = sdmmchost_send_command(card->host, cmd);
    if (status != status_success) {
        hpm_stat_t recover_status = sdio_error_recovery(card);
        if (recover_status != status_success) {
            status = recover_status;
        }
    }
    return status;
}

static hpm_stat_t sdio_transfer(sdio_card_t *card, sdmmchost_xfer_t *xfer)
{
    hpm_stat_t status = sdmmchost_transfer(card->host, xfer);
    if (status != status_success) {
        hpm_stat_t recover_status = sdio_error_recovery(card);
        if (recover_status != status_success) {
            status = recover_status;
        }
    }
    return status;
}

static hpm_stat_t sdio_send_if_cond(sdio_card_t *card)
{
    sdmmc_host_t *host = card->host;
    sdmmchost_cmd_t *cmd = &host->cmd;

    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));
    cmd->cmd_index = sd_cmd_send_if_cond;
    cmd->cmd_argument = 0x1aa;
    cmd->resp_type = (sdxc_dev_resp_type_t) sdmmc_resp_r7;
    hpm_stat_t status = sdio_send_command(card, cmd);
    if (status != status_success) {
        return status;
    }

    if ((cmd->response[0] & 0xFFU) != 0xAAU) {
        return status_sdmmc_card_not_support;
    }

    return status_success;
}

static hpm_stat_t sdio_io_send_op_if_cond(sdio_card_t *card, uint32_t arg)
{
    sdmmc_host_t *host = card->host;
    sdmmchost_cmd_t *cmd = &host->cmd;

    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));
    /* Probe the presence of SDIO */
    cmd->cmd_index = sdio_cmd_io_send_op_cond;
    cmd->cmd_argument = arg;
    cmd->resp_type = (sdxc_dev_resp_type_t) sdmmc_resp_r4;

    hpm_stat_t status = sdio_send_command(card, cmd);
    if (status != status_success) {
        return status;
    }

    return status_success;
}

static hpm_stat_t sdio_send_rca(sdio_card_t *card)
{
    sdmmc_host_t *host = card->host;
    sdmmchost_cmd_t *cmd = &host->cmd;

    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));
    cmd->cmd_index = sd_cmd_send_relative_addr;
    cmd->resp_type = (sdxc_dev_resp_type_t) sdmmc_resp_r6;

    hpm_stat_t status = sdio_send_command(card, cmd);
    if (status != status_success) {
        return status;
    }
    card->relative_addr = cmd->response[0] >> 16;

    return status;
}

static void sdio_switch_to_1v8(sdio_card_t *card)
{
    sdmmc_host_t *host = card->host;

    /* 1. Stop providing clock to the card */
    sdxc_enable_inverse_clock(host->host_param.base, false);
    sdxc_enable_sd_clock(host->host_param.base, false);

    /* 2. Switch signaling to 1.8v */
    sdmmchost_vsel_pin_control(host, hpm_sdmmc_io_voltage_1v8);
    sdmmchost_select_voltage(host, hpm_sdmmc_io_voltage_1v8);

    /* 3. Provide SD clock the card again */
    sdxc_enable_inverse_clock(host->host_param.base, true);
    sdxc_enable_sd_clock(host->host_param.base, true);

    /* 4. wait 1ms */
    sdmmchost_delay_ms(host, 1);
}

static hpm_stat_t sdio_probe_sdio_card(sdio_card_t *card)
{
    sdmmc_host_t *host = card->host;
    sdmmchost_cmd_t *cmd = &card->host->cmd;
    hpm_stat_t status = sdmmc_go_idle_state(card->host, 0);
    if (status != status_success) {
        return status;
    }

    /* Probe with CMD8 first */
    status = sdio_send_if_cond(card);
    if ((status != status_sdxc_cmd_timeout_error) && (status != status_sdxc_non_recoverable_error) && (
            status != status_success)) {
        return status;
    }

    /* Probe with CMD */
    sdio_cmd5_resp_t cmd5_resp;
    status = sdio_io_send_op_if_cond(card, 0);
    if (status != status_success) {
        return status;
    }
    cmd5_resp.value = cmd->response[0];


    uint32_t cmd5_arg = (0x3FUL << 18); /* Voltage 3.0 - 3.6 */
    bool switch_voltage = false;
    if (sdmmchost_is_voltage_switch_supported(host) && card->operation_voltage == sdmmc_operation_voltage_3v3) {
        cmd5_arg |= (1UL << 24); /* S18R */
        switch_voltage = true;
    }
    do {
        sdmmchost_delay_ms(host, 1);
        status = sdio_io_send_op_if_cond(card, cmd5_arg);
        if (status != status_success) {
            return status;
        }
        cmd5_resp.value = cmd->response[0];
    } while (!cmd5_resp.ready);

    card->num_of_functions = cmd5_resp.num_func;

    if (switch_voltage && (cmd5_resp.s18r == 1)) {
        sdio_switch_to_1v8(card);
        card->operation_voltage = sdmmc_operation_voltage_1v8;
    }

    return status;
}

static hpm_stat_t sdio_read_cccr(sdio_card_t *card)
{
    hpm_stat_t status;
    sdmmc_host_t *host = card->host;
    sdmmchost_cmd_t *cmd = &host->cmd;
    (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));
    uint8_t *pdata = (uint8_t *) &card->cccr;
    for (uint32_t reg = SDIO_REG_CCCR_START; reg <= SDIO_REG_CCCR_END; reg++) {
        status = sdio_io_read_byte(card, 0, reg, pdata);
        if (status != status_success) {
            break;
        }
        ++pdata;
    }
    return status;
}

hpm_stat_t sdio_read_cis(sdio_card_t *card, uint32_t cis_ptr, sdio_cis_t *cis, bool keep_tpl_raw_data)
{
    hpm_stat_t status = status_invalid_argument;
    uint8_t tpl_code;
    uint8_t tpl_link;
    struct sdio_tpl_block *tpl_blk = NULL;
    struct sdio_tpl_block *current_tpl = NULL;

    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL) || (cis == NULL));

        do {
            uint8_t tmp_buf[256] = { 0 };
            status = sdio_io_read_byte(card, 0, cis_ptr++, &tpl_code);
            HPM_BREAK_IF(status != status_success);
            status = sdio_io_read_byte(card, 0, cis_ptr++, &tpl_link);
            HPM_BREAK_IF(status != status_success);
            if ((tpl_code == CISTPL_END) || (tpl_link == 0xFF)) {
                if (keep_tpl_raw_data && (tpl_blk != NULL)) {
                    tpl_blk->next = NULL;
                }
                break;
            }
            if (tpl_code == CISTPL_NULL) {
                continue;
            }

            uint8_t *pdata = tmp_buf;
            for (uint32_t i = 0; i < tpl_link; i++) {
                status = sdio_io_read_byte(card, 0, cis_ptr++, pdata++);
                if (status != status_success) {
                    return status;
                }
            }

            if (keep_tpl_raw_data) {
                tpl_blk = (struct sdio_tpl_block *) HPM_SDMMC_MALLOC(sizeof(struct sdio_tpl_block));
                if (tpl_blk == NULL) {
                    status = status_fail;
                    break;
                }
                tpl_blk->tpl_code = tpl_code;
                tpl_blk->tpl_size = tpl_link;
                if (tpl_link > 0) {
                    tpl_blk->tpl_data = (uint8_t *) HPM_SDMMC_MALLOC(tpl_link);
                    if (tpl_blk->tpl_data == NULL) {
                        HPM_SDMMC_FREE(tpl_blk);
                        status = status_fail;
                        break;
                    }
                    (void) memcpy(tpl_blk->tpl_data, tmp_buf, tpl_link);
                }
                if (current_tpl == NULL) {
                    cis->tpl_link = tpl_blk;
                    current_tpl = cis->tpl_link;
                } else {
                    current_tpl->next = tpl_blk;
                    current_tpl = current_tpl->next;
                }
            }

            const uint8_t *tpl_data_buf = tmp_buf;
            sdio_cis_t *p_cis = cis;
            switch (tpl_code) {
            case CISTPL_MANFID:
                if (tpl_link < 4) {
                    return status_invalid_argument;
                }
                p_cis->manufacturer = (((uint16_t) tpl_data_buf[1]) << 8) | tpl_data_buf[0];
                p_cis->product = (((uint16_t) tpl_data_buf[3]) << 8) | tpl_data_buf[2];
                break;
            case CISTPL_FUNCE:
                if (tpl_data_buf[0] == 0x00) {
                    p_cis->func0_blk_size = (((uint16_t) tpl_data_buf[2]) << 8) | tpl_data_buf[1];
                    p_cis->max_tran_speed = tpl_data_buf[3];
                } else if (tpl_data_buf[0] == 0x01) {
                    sdio_func_funce_t *pfunce = (sdio_func_funce_t *) &tpl_data_buf[1];
                    p_cis->std_io_rev = pfunce->std_io_rev;
                    p_cis->support_wakeup = pfunce->support_wakeup;
                    p_cis->max_blk_size = pfunce->max_blk_size;
                    p_cis->csa_size = pfunce->csa_size;
                    p_cis->serial_num = pfunce->psn;
                    p_cis->timeout_val_in_10ms = pfunce->enable_timeout_val;
                    p_cis->csa_wp = pfunce->csa_wp;
                    p_cis->csa_nf = pfunce->csa_nf;
                    p_cis->ocr = pfunce->ocr;
                } else {
                    /* Skip data */
                }
                break;
            case CISTPL_FUNCID:
                /* Skip */
                break;
            case CISTPL_SDIO_STD:
                p_cis->std_id = tpl_data_buf[0];
                p_cis->std_type = tpl_data_buf[1];
                break;
            case CISTPL_SDIO_EXT:
                /* Skip */
                break;
            default:
                /* Skip */
                break;
            }
        } while (tpl_code != CISTPL_END);
    } while (false);


    return status;
}

hpm_stat_t sdio_read_fbr(sdio_card_t *card, uint32_t func_idx, sdio_fbr_t *fbr)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL) || (fbr == NULL));
        uint32_t reg_addr = SDIO_FBR_REG_BASE(func_idx);
        uint8_t *pdata = (uint8_t *) fbr;
        for (uint32_t i = 0; i < sizeof(sdio_fbr_t); i++) {
            status = sdio_io_read_byte(card, 0, reg_addr++, pdata++);
            if (status != status_success) {
                return status;
            }
        }
    } while (false);
    return status;
}


hpm_stat_t sdio_io_rw_direct(sdio_card_t *card,
                             bool write,
                             uint32_t func_idx,
                             uint32_t reg_addr,
                             uint8_t *pdata,
                             bool read_after_write)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF(card == NULL);
        sdmmc_host_t *host = card->host;
        sdmmchost_cmd_t *cmd = &host->cmd;
        (void) memset(cmd, 0, sizeof(sdmmchost_cmd_t));

        sdio_cmd52_arg_t cmd52_arg = { .value = 0 };

        cmd52_arg.write = write ? 1UL : 0U;
        cmd52_arg.read_afer_write = read_after_write ? 1UL : 0U;
        cmd52_arg.func_num = func_idx;
        cmd52_arg.reg_addr = reg_addr;

        if (write) {
            cmd52_arg.wr_data = *pdata;
        }

        cmd->cmd_index = sdio_cmd_io_rw_direct;
        cmd->resp_type = (sdxc_dev_resp_type_t) sdmmc_resp_r5;
        cmd->cmd_argument = cmd52_arg.value;


        status = sdio_send_command(card, cmd);
        if (status != status_success) {
            break;
        }

        sdio_resp_r5_t cmd52_resp;
        cmd52_resp.value = cmd->response[0];

        if (!write || read_after_write) {
            *pdata = (uint8_t) cmd52_resp.rw_data;
        }
    } while (false);

    return status;
}

hpm_stat_t sdio_set_block_size(sdio_card_t *card, uint32_t func_idx, uint32_t block_size)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if (card == NULL) {
            break;
        }
        if (block_size == 0) {
            block_size = MIN(SDMMC_BLOCK_SIZE_DEFAULT, block_size);
        }
        status = sdio_io_write_byte(card,
                                    0,
                                    SDIO_FBR_REG_BASE(func_idx) + SDIO_REG_CCCR_FN0_BLK_SIZE,
                                    block_size & 0xFF);
        if (status != status_success) {
            break;
        }
        status = sdio_io_write_byte(card,
                                    0,
                                    SDIO_FBR_REG_BASE(func_idx) + SDIO_REG_CCCR_FN0_BLK_SIZE + 1,
                                    (block_size >> 8) & 0xFF);
        if (status != status_success) {
            break;
        }

        card->sdio_func[func_idx]->current_block_size = block_size;

    } while (false);

    return status;
}

hpm_stat_t sdio_io_read_byte(sdio_card_t *card, uint32_t func_num, uint32_t reg, uint8_t *pdata)
{
    return sdio_io_rw_direct(card, false, func_num, reg, pdata, false);
}

hpm_stat_t sdio_io_write_byte(sdio_card_t *card, uint32_t func_num, uint32_t reg, uint8_t byte_data)
{
    return sdio_io_rw_direct(card, true, func_num, reg, &byte_data, false);
}

hpm_stat_t sdio_io_rw_extend(sdio_card_t *card,
                             bool write,
                             uint32_t func_idx,
                             uint32_t reg_addr,
                             uint32_t op_code,
                             uint8_t *pbuf,
                             uint32_t blocks,
                             uint32_t block_size)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL));

        if (!card->host->card_init_done) {
            status = status_sdmmc_device_init_required;
            break;
        }

        sdmmc_host_t *host = card->host;
        sdmmchost_cmd_t *cmd = &host->cmd;
        sdmmchost_data_t *data = &host->data;
        sdmmchost_xfer_t *content = &host->xfer;
        memset(cmd, 0, sizeof(*cmd));
        memset(data, 0, sizeof(*data));
        memset(content, 0, sizeof(*content));

        sdio_cmd53_arg_t cmd53_arg = { .value = 0 };

        cmd53_arg.rw_flag = write ? 1UL : 0;
        cmd53_arg.func_num = func_idx;
        cmd53_arg.reg_addr = reg_addr;
        cmd53_arg.block_mode = 0;
        cmd53_arg.op_code = op_code;

        if ((blocks == 1) && (block_size <= 512)) {
            cmd53_arg.count = (block_size == 512) ? 0 : block_size;
        } else {
            cmd53_arg.count = blocks;
            cmd53_arg.block_mode = 1;
        }

        cmd->cmd_index = sdio_cmd_io_rw_extend;
        cmd->cmd_argument = cmd53_arg.value;
        cmd->cmd_timeout_ms = SDIO_CMD_TIMEOUT_MS;
        cmd->resp_type = (sdxc_dev_resp_type_t) sdmmc_resp_r5;

        data->block_size = block_size;
        data->block_cnt = blocks;

        if (write) {
            data->tx_data = (uint32_t *) sdmmc_get_sys_addr(host, (uint32_t)pbuf);
        } else {
            data->rx_data = (uint32_t *) sdmmc_get_sys_addr(host, (uint32_t)pbuf);
        }

        content->data = data;
        content->command = cmd;

#if !defined(HPM_SDMMC_ENABLE_CACHE_MAINTENANCE) || (HPM_SDMMC_ENABLE_CACHE_MAINTENANCE == 1)
        uint32_t buf_start = write ? (uint32_t) data->tx_data : (uint32_t) data->rx_data;
        uint32_t buf_size = blocks * block_size;
        uint32_t end_addr = buf_start + buf_size;
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(buf_start);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(end_addr);
        uint32_t aligned_size = aligned_end - aligned_start;
        if (write) {
            l1c_dc_writeback(aligned_start, aligned_size);
        } else {
            /* FLUSH un-cacheline aligned memory region */
            if ((buf_start % HPM_L1C_CACHELINE_SIZE) != 0) {
                l1c_dc_writeback(aligned_start, HPM_L1C_CACHELINE_SIZE);
            }
            if ((end_addr % HPM_L1C_CACHELINE_SIZE) != 0) {
                uint32_t aligned_tail = HPM_L1C_CACHELINE_ALIGN_DOWN(end_addr);
                l1c_dc_writeback(aligned_tail, HPM_L1C_CACHELINE_SIZE);
            }
        }
#endif
        status = sdio_transfer(card, content);
        HPM_BREAK_IF(status != status_success);

#if !defined(HPM_SDMMC_ENABLE_CACHE_MAINTENANCE) || (HPM_SDMMC_ENABLE_CACHE_MAINTENANCE == 1)
        if (!write) {
            l1c_dc_invalidate(aligned_start, aligned_size);
        }
#endif
    } while (false);

    return status;
}

hpm_stat_t sdio_io_write_multi_bytes_to_fifo(sdio_card_t *card,
                                             uint32_t func_idx,
                                             uint32_t reg_addr,
                                             uint8_t *pbuf,
                                             uint32_t length)
{
    if ((card == NULL) || (card->host == NULL) || (length < 1U)) {
        return status_invalid_argument;
    }
    uint32_t block_size = MIN(card->sdio_func[func_idx]->current_block_size, length);
    uint32_t num_blocks = (length + block_size - 1U) / block_size;
    return sdio_io_rw_extend(card,
                             true,
                             func_idx,
                             reg_addr,
                             SDIO_CMD53_OP_MODE_FIXED_ADDR,
                             pbuf,
                             num_blocks,
                             block_size);
}


hpm_stat_t sdio_io_read_multi_bytes_from_fifo(sdio_card_t *card,
                                              uint32_t func_idx,
                                              uint32_t reg_addr,
                                              uint8_t *pbuf,
                                              uint32_t length)
{
    if ((card == NULL) || (card->host == NULL) || (length < 1U)) {
        return status_invalid_argument;
    }
    uint32_t block_size = MIN(card->sdio_func[func_idx]->current_block_size, length);
    uint32_t num_blocks = (length + block_size - 1U) / block_size;
    return sdio_io_rw_extend(card,
                             false,
                             func_idx,
                             reg_addr,
                             SDIO_CMD53_OP_MODE_FIXED_ADDR,
                             pbuf,
                             num_blocks,
                             block_size);
}

hpm_stat_t sdio_io_write_incr_multi_bytes(sdio_card_t *card,
                                          uint32_t func_idx,
                                          uint32_t reg_addr,
                                          uint8_t *pbuf,
                                          uint32_t length)
{
    if ((card == NULL) || (card->host == NULL)) {
        return status_invalid_argument;
    }
    uint32_t block_size = MIN(card->sdio_func[func_idx]->current_block_size, length);
    uint32_t num_blocks = (length + block_size - 1U) / block_size;
    return sdio_io_rw_extend(card,
                             true,
                             func_idx,
                             reg_addr,
                             SDIO_CMD53_OP_MODE_INCR_ADDR,
                             pbuf,
                             num_blocks,
                             block_size);
}

hpm_stat_t sdio_io_read_incr_multi_bytes(sdio_card_t *card,
                                         uint32_t func_idx,
                                         uint32_t reg_addr,
                                         uint8_t *pbuf,
                                         uint32_t length)
{
    if ((card == NULL) || (card->host == NULL)) {
        return status_invalid_argument;
    }
    uint32_t block_size = MIN(card->sdio_func[func_idx]->current_block_size, length);
    uint32_t num_blocks = (length + block_size - 1U) / block_size;
    return sdio_io_rw_extend(card,
                             false,
                             func_idx,
                             reg_addr,
                             SDIO_CMD53_OP_MODE_INCR_ADDR,
                             pbuf,
                             num_blocks,
                             block_size);
}


hpm_stat_t sdio_init(sdio_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF(card == NULL);

        status = sdio_host_init(card);
        HPM_BREAK_IF(status != status_success);

        card->host->card_init_done = false;
        card->relative_addr = 0;
        card->current_timing = sdmmc_sd_speed_normal;
        card->host->card_inserted = true;
        sdmmchost_delay_ms(card->host, 100); /* Wait a while in case the card connection is still not stable */

        status = sdio_card_init(card);
    } while (false);

    return status;
}


void sdio_deinit(sdio_card_t *card)
{
    if ((card != NULL) && (card->host != NULL) && (card->host->card_init_done)) {
        card->host->card_init_done = false;

        sdmmchost_deinit(card->host);
    }
}

static hpm_stat_t sdio_set_bus_width(sdio_card_t *card)
{
    hpm_stat_t status = status_success;

    do {
        /* If the card is a full-speed card, it shall support both 1-bit and 4-bit  bus widths */
        bool support_4bit = IS_HPM_BITMASK_SET(card->host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_4BIT);
        if ((card->cccr.low_speed_card == 0) && support_4bit) {
            uint8_t cccr_bus_if_ctrl = 0;
            status = sdio_io_read_byte(card, 0, SDIO_REG_CCCR_BUS_IF_CTRL, &cccr_bus_if_ctrl);
            HPM_BREAK_IF(status != status_success);
            cccr_bus_if_ctrl |= SDIO_CCCR_BUS_WIDTH_4BIT;
            status = sdio_io_write_byte(card, 0, SDIO_REG_CCCR_BUS_IF_CTRL, cccr_bus_if_ctrl);
            HPM_BREAK_IF(status != status_success);
            sdmmchost_set_card_bus_width(card->host, sdmmc_bus_width_4bit);
        }
    } while (false);
    return status;
}

static hpm_stat_t sdio_set_timing(sdio_card_t *card)
{
    hpm_stat_t status = status_success;

    do {
        /* Change Speed */
        if (card->cccr.support_highspeed) {
            uint8_t bus_speed_sel = 0;
            bool need_reverse = true;
            uint32_t clock_option = SD_CLOCK_25MHZ;
            status = sdio_io_read_byte(card, 0, SDIO_REG_CCCR_BUS_SPEED_SEL, &bus_speed_sel);
            HPM_BREAK_IF(status != status_success);

            bool support_ddr = IS_HPM_BITMASK_SET(card->host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_DDR);
            bool support_sdr104 = IS_HPM_BITMASK_SET(card->host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_SDR104);
            bool support_sdr50 = IS_HPM_BITMASK_SET(card->host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_SDR50);

            sdmmc_speed_mode_t speed_mode;
            if (card->cccr.support_sdr104 && support_sdr104) {
                speed_mode = sdmmc_sd_speed_sdr104;
                clock_option = SD_CLOCK_208MHZ;
                bus_speed_sel = SDIO_CCCR_BUS_SPEED_SUPPORT_HIGHSPEED | SDIO_CCCR_BUS_SPEED_SDR104;
            } else if ((card->cccr.support_sdr50 || (card->max_tran_speed_in_kbps > 25000)) && support_sdr50) {
                speed_mode = sdmmc_sd_speed_sdr50;
                clock_option = SD_CLOCK_100MHZ;
                bus_speed_sel = SDIO_CCCR_BUS_SPEED_SUPPORT_HIGHSPEED | SDIO_CCCR_BUS_SPEED_SDR50;
            } else if (card->cccr.support_ddr50 && support_ddr) {
                speed_mode = sdmmc_sd_speed_ddr50;
                clock_option = SD_CLOCK_50MHZ;
                need_reverse = false;
                bus_speed_sel = SDIO_CCCR_BUS_SPEED_SUPPORT_HIGHSPEED | SDIO_CCCR_BUS_SPEED_DDR50;
            } else {
                speed_mode = sdmmc_sd_speed_high;
                bus_speed_sel = SDIO_CCCR_BUS_SPEED_SUPPORT_HIGHSPEED | SDIO_CCCR_BUS_SPEED_HIGH;
                clock_option = SD_CLOCK_50MHZ;
            }

            status = sdio_io_write_byte(card, 0, SDIO_REG_CCCR_BUS_SPEED_SEL, bus_speed_sel);
            HPM_BREAK_IF(status != status_success);

            sdmmchost_set_speed_mode(card->host, speed_mode);
            card->host->clock_freq = sdmmchost_set_card_clock(card->host, clock_option, need_reverse);
        }
    } while (false);
    return status;
}

hpm_stat_t sdio_card_init(sdio_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL));

        sdmmchost_set_card_bus_width(card->host, sdmmc_bus_width_1bit);
        sdmmchost_set_card_clock(card->host, SDMMC_CLOCK_400KHZ, true);
        status = sdio_probe_sdio_card(card);
        card->host->operation_mode = hpm_sdmmc_operation_mode_identification;
        sdmmchost_init_io(card->host, card->host->operation_mode);
        HPM_BREAK_IF(status != status_success);

        card->host->dev_type = sdmmc_dev_type_sdio;

        /* Send CMD3 */
        status = sdio_send_rca(card);
        HPM_BREAK_IF(status != status_success);

        /* Send CMD7 */
        status = sdmmc_select_card(card->host, card->relative_addr, true);
        HPM_BREAK_IF(status != status_success);

        sdmmchost_set_card_clock(card->host, SD_CLOCK_25MHZ, true);
        card->host->operation_mode = hpm_sdmmc_operation_mode_transfer;
        sdmmchost_init_io(card->host, card->host->operation_mode);

        status = sdio_read_cccr(card);
        HPM_BREAK_IF(status != status_success);

        uint32_t common_cis_ptr = sdio_cis_csr_addr(card->cccr.common_cis_pointer);
        status = sdio_read_cis(card, common_cis_ptr, &card->common_cis, false);
        HPM_BREAK_IF(status != status_success);

        /* Change Bus Speed */
        status = sdio_set_timing(card);
        HPM_BREAK_IF(status != status_success);

        /* Change Bus width */
        status = sdio_set_bus_width(card);
        HPM_BREAK_IF(status != status_success);

        sdmmchost_init_io(card->host, card->host->operation_mode);

        sdio_get_max_transfer_speed(card);

        /* Update CCCR */
        status = sdio_read_cccr(card);
    } while (false);

    if (status == status_success) {
        card->host->card_init_done = true;
    }

    return status;
}

hpm_stat_t sdio_card_deinit(sdio_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL));
        if (!card->host->card_init_done) {
            status = status_sdmmc_device_init_required;
            break;
        }
        /* De-initialize functions */
       status = sdio_deinit_funcs(card);
    } while (false);

    if (status == status_success) {
        card->host->card_init_done = false;
    }

    return status;
}

hpm_stat_t sdio_get_max_transfer_speed(sdio_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL));

        uint32_t speed_units[8] = { 100UL, 1000UL, 10UL * 1000UL, 100UL * 1000UL, 0, 0, 0, 0 };
        uint32_t factor_mul10[16] = { 0, 10, 12, 13, 14, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

        uint32_t speed_unit_idx = card->common_cis.max_tran_speed & 0x7;
        uint32_t fact_idx = (card->common_cis.max_tran_speed >> 3) & 0xF;

        card->max_tran_speed_in_kbps = speed_units[speed_unit_idx] * factor_mul10[fact_idx] / 10;

        status = status_success;
    } while (false);

    return status;
}

hpm_stat_t sdio_init_funcs(sdio_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL));
        for (uint32_t i = 0; i <= card->num_of_functions; i++) {
            sdio_func_t *sdio_func = (sdio_func_t *) HPM_SDMMC_MALLOC(sizeof(sdio_func_t));
            if (sdio_func == NULL) {
                return status_fail;
            }
            card->sdio_func[i] = sdio_func;
        }

        for (uint32_t i = 1; i <= card->num_of_functions; i++) {
            sdio_func_t *sdio_func = card->sdio_func[i];
            status = sdio_read_fbr(card, i, &sdio_func->fbr);
            HPM_BREAK_IF(status != status_success);
            uint32_t cis_ptr = sdio_cis_csr_addr(sdio_func->fbr.cis_ptr);
            status = sdio_read_cis(card, cis_ptr, &sdio_func->cis, true);
            HPM_BREAK_IF(status != status_success);
            sdio_func->func_idx = i;
            sdio_func->max_block_size = sdio_func->cis.max_blk_size;
            sdio_func->current_block_size = sdio_func->cis.max_blk_size;
        }

        if (status != status_success) {
            for (uint32_t i = 0; i <= card->num_of_functions; i++) {
                HPM_SDMMC_FREE(card->sdio_func[i]);
                card->sdio_func[i] = NULL;
            }
        }
    } while (false);
    return status;
}

hpm_stat_t sdio_deinit_funcs(sdio_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        HPM_BREAK_IF((card == NULL) || (card->host == NULL));
        /* Free all buffers */
        for (uint32_t i = 0; i <= card->num_of_functions; i++) {
            if (card->sdio_func[i] == NULL) {
                continue;
            }
            /* Free TPL data if necessary */
            if (card->sdio_func[i]->cis.tpl_link != NULL) {
                sdio_tpl_blk_t current_tpl = card->sdio_func[i]->cis.tpl_link;
                while (current_tpl != NULL) {
                    sdio_tpl_blk_t tmp_tpl = current_tpl->next;
                    HPM_SDMMC_FREE(current_tpl);
                    current_tpl = tmp_tpl;
                }
            }
            HPM_SDMMC_FREE(card->sdio_func[i]);
            card->sdio_func[i] = NULL;
        }
        status = status_success;
    } while (false);

    return status;
}

hpm_stat_t sdio_host_init(sdio_card_t *card)
{
    hpm_stat_t status = status_success;
    assert(card != NULL);

    status = sdmmchost_init(card->host);
    if (status == status_success) {
        card->is_host_ready = true;
        if (IS_HPM_BITMASK_SET(card->host->host_param.host_flags, HPM_SDMMC_HOST_SUPPORT_3V3)) {
            card->operation_voltage = sdmmc_operation_voltage_3v3;
        } else {
            card->operation_voltage = sdmmc_operation_voltage_1v8;
        }
    }

    return status_success;
}

hpm_stat_t sdio_host_deinit(sdio_card_t *card)
{
    (void) card;
    return status_success;
}

const char *sdio_get_func_if_code_str(uint8_t std_func_if_code)
{
    const char *code_str;
    switch (std_func_if_code) {
    case STD_SDIO_FUNC_IF_CODE_NON_STD:
        code_str = "No Standard SDIO interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_UART:
        code_str = "SDIO Standard UART";
        break;
    case STD_SDIO_FUNC_IF_CODE_BT_TYPEA:
        code_str = "SDIO Bluetooth Type-A standard interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_BT_TYPEB:
        code_str = "SDIO Bluetooth Type-B standard interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_GPS:
        code_str = "SDIO GPS standard interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_CAMERA:
        code_str = "SDIO Camera standard interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_PHS:
        code_str = "SDIO PHS standard interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_WLAN:
        code_str = "SDIO WLAN interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_ATA:
        code_str = "Embedded SDIO-ATA standard interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_BT_TYPEA_AMP:
        code_str = "SDIO Bluetooth Type-A AMP standard interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_ISDIO:
        code_str = "iSDIO standard interface";
        break;
    case STD_SDIO_FUNC_IF_CODE_EXT_STD:
        code_str = "Extended SDIO Function Interface";
        break;
    default:
        code_str = "UnKnown Interface Code";
        break;
    }
    return code_str;
}

hpm_stat_t sdio_enable_interrupt(sdio_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((card == NULL) || (card->host == NULL)) {
            break;
        }
        sdmmchost_enable_sdio_interrupt(card->host, true);
        status = status_success;
    } while (false);
    return status;
}

hpm_stat_t sdio_disable_interrupt(sdio_card_t *card)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((card == NULL) || (card->host == NULL)) {
            break;
        }
        sdmmchost_enable_sdio_interrupt(card->host, false);
        status = status_success;
    } while (false);
    return status;
}

hpm_stat_t sdio_register_irq_callback(sdio_card_t *card, void (*sdio_irq_callback)(void *param), void *sdio_irq_param)
{
    hpm_stat_t status = status_invalid_argument;
    do {
        if ((card == NULL) || (card->host == NULL)) {
            break;
        }
        sdmmchost_register_sdio_callback(card->host, sdio_irq_callback, sdio_irq_param);
        status = status_success;
    } while (false);
    return status;
}