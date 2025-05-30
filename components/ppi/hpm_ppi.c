
/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------
 * Include
 *---------------------------------------------------------------------
 */
#include "hpm_ppi.h"
#include "hpm_clock_drv.h"

/* API */
void ppi_get_async_sram_defconfig(PPI_Type *ppi, ppi_async_sram_config_t *config)
{
    (void)ppi;

    config->base_address = 0xF8000000;
    config->size_in_byte = 128 * 1024;
    config->port_size = ppi_port_size_16bits;
    config->ad_mux_mode = true;
    config->cs_valid_polarity = false;
    config->dm_valid_polarity = false;
    config->addr_valid_polarity = true;
    config->adv_ctrl_pin = 0;
    config->rel_ctrl_pin = 1;
    config->wel_ctrl_pin = 2;
    config->as_in_ns = 25;
    config->ah_in_ns = 25;
    config->rel_in_ns = 50;
    config->reh_in_ns = 0;
    config->wel_in_ns = 50;
    config->weh_in_ns = 0;
    config->dq_sig_sel[0] = ppi_dq_pins_0_7;
    config->dq_sig_sel[1] = ppi_dq_pins_8_15;
    config->dq_sig_sel[2] = ppi_dq_pins_16_23;
    config->dq_sig_sel[3] = ppi_dq_pins_24_31;
}

void ppi_config_async_sram(PPI_Type *ppi, uint8_t cs_index, uint8_t cmd_start_index, ppi_async_sram_config_t *config)
{
    assert((config->ad_mux_mode) || (config->port_size != ppi_port_size_32bits));
    assert(((config->base_address & 0xFFFFF) == 0) && (config->size_in_byte > 0));    /* Addr should be aligned by 1MB */
    assert(((cmd_start_index & 0x07) == 0) && (cmd_start_index < 64));
    assert(cs_index < 4);

    uint32_t ppi_freq = clock_get_frequency(clock_ppi0);
    ppi_cs_pin_config_t cs_config = { 0 };
    ppi_cmd_config_t cmd_config = { 0 };

    /*
     * Pin polarity Config
     */
    if (config->cs_valid_polarity) {
        ppi_config_cs_pin_polarity(ppi, cs_index, ppi_cs_idle_pol_low);
    } else {
        ppi_config_cs_pin_polarity(ppi, cs_index, ppi_cs_idle_pol_high);
    }

    if (config->ad_mux_mode) {
        ppi_set_ctrl_pin_dir(ppi, config->adv_ctrl_pin, ppi_ctrl_pin_dir_output);
        ppi_config_ctrl_pin_polarity(ppi, config->adv_ctrl_pin, ppi_ctrl_pol_low);
    }
    ppi_set_ctrl_pin_dir(ppi, config->rel_ctrl_pin, ppi_ctrl_pin_dir_output);
    ppi_config_ctrl_pin_polarity(ppi, config->rel_ctrl_pin, ppi_ctrl_pol_low);
    ppi_set_ctrl_pin_dir(ppi, config->wel_ctrl_pin, ppi_ctrl_pin_dir_output);
    ppi_config_ctrl_pin_polarity(ppi, config->wel_ctrl_pin, ppi_ctrl_pol_low);

    /*
     * Read Cmd
     */
    /* common */
    cmd_config.cs_pin_value = config->cs_valid_polarity;
    cmd_config.clk_output = false;
    if (config->ad_mux_mode) {
        cmd_config.byte_sel[config->dq_sig_sel[0]] = ppi_byte_sel_0_7_bits;
        cmd_config.byte_sel[config->dq_sig_sel[1]] = ppi_byte_sel_8_15_bits;
        cmd_config.byte_sel[config->dq_sig_sel[2]] = ppi_byte_sel_16_23_bits;
        cmd_config.byte_sel[config->dq_sig_sel[3]] = ppi_byte_sel_24_31_bits;
    } else {
        if (config->port_size == ppi_port_size_8bits) {
            cmd_config.ad_func_sel[config->dq_sig_sel[0]] = ppi_ad_func_data;
            cmd_config.ad_pin_dir[config->dq_sig_sel[0]] = ppi_ad_pin_dir_input;
            cmd_config.byte_sel[config->dq_sig_sel[0]] = ppi_byte_sel_0_7_bits;
            for (uint8_t i = 1; i < 4; i++) {
                cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_addr;
                cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
            }
            cmd_config.byte_sel[config->dq_sig_sel[1]] = ppi_byte_sel_0_7_bits;
            cmd_config.byte_sel[config->dq_sig_sel[2]] = ppi_byte_sel_8_15_bits;
            cmd_config.byte_sel[config->dq_sig_sel[3]] = ppi_byte_sel_16_23_bits;
        } else if (config->port_size == ppi_port_size_16bits) {
            for (uint8_t i = 0; i < 2; i++) {
                cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_data;
                cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_input;
            }
            cmd_config.byte_sel[config->dq_sig_sel[0]] = ppi_byte_sel_0_7_bits;
            cmd_config.byte_sel[config->dq_sig_sel[1]] = ppi_byte_sel_8_15_bits;
            for (uint8_t i = 2; i < 4; i++) {
                cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_addr;
                cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
            }
            cmd_config.byte_sel[config->dq_sig_sel[2]] = ppi_byte_sel_0_7_bits;
            cmd_config.byte_sel[config->dq_sig_sel[3]] = ppi_byte_sel_8_15_bits;
        } else {
            ;    /* Not Support */
        }
    }
    cmd_config.ctrl_pin_value[config->wel_ctrl_pin] = true;

    /* AS Stage */
    cmd_config.cmd_cycle = ppi_ns2cycle(ppi_freq, config->as_in_ns);
    if (config->ad_mux_mode) {
        for (uint8_t i = 0; i < 4; i++) {
            cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_addr;
            cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
        }
        cmd_config.ctrl_pin_value[config->adv_ctrl_pin] = config->addr_valid_polarity;
    }
    cmd_config.ctrl_pin_value[config->rel_ctrl_pin] = true;
    ppi_config_cmd(ppi, cmd_start_index, &cmd_config);

    /* AH Stage */
    cmd_config.cmd_cycle = ppi_ns2cycle(ppi_freq, config->ah_in_ns);
    if (config->ad_mux_mode) {
        for (uint8_t i = 0; i < 4; i++) {
            cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_addr;
            cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
        }
        cmd_config.ctrl_pin_value[config->adv_ctrl_pin] = !config->addr_valid_polarity;
    }
    cmd_config.ctrl_pin_value[config->rel_ctrl_pin] = true;
    ppi_config_cmd(ppi, cmd_start_index + 1, &cmd_config);

    /* REL Stage */
    cmd_config.cmd_cycle = ppi_ns2cycle(ppi_freq, config->rel_in_ns);
    if (config->ad_mux_mode) {
        for (uint8_t i = 0; i < (1u << config->port_size); i++) {
            cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_data;
            cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_input;
        }
        cmd_config.ctrl_pin_value[config->adv_ctrl_pin] = !config->addr_valid_polarity;
    }
    cmd_config.ctrl_pin_value[config->rel_ctrl_pin] = false;
    ppi_config_cmd(ppi, cmd_start_index + 2, &cmd_config);

    /* REH Stage */
    cmd_config.cmd_cycle = ppi_ns2cycle(ppi_freq, config->reh_in_ns);
    if (config->ad_mux_mode) {
        for (uint8_t i = 0; i < (1u << config->port_size); i++) {
            cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_data;
            cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_input;
        }
        cmd_config.ctrl_pin_value[config->adv_ctrl_pin] = !config->addr_valid_polarity;
    }
    cmd_config.ctrl_pin_value[config->rel_ctrl_pin] = true;
    ppi_config_cmd(ppi, cmd_start_index + 3, &cmd_config);

    /*
     * Write Cmd
     */
    /* common */
    cmd_config.cs_pin_value = config->cs_valid_polarity;
    cmd_config.clk_output = false;
    if (config->ad_mux_mode) {
        cmd_config.byte_sel[config->dq_sig_sel[0]] = ppi_byte_sel_0_7_bits;
        cmd_config.byte_sel[config->dq_sig_sel[1]] = ppi_byte_sel_8_15_bits;
        cmd_config.byte_sel[config->dq_sig_sel[2]] = ppi_byte_sel_16_23_bits;
        cmd_config.byte_sel[config->dq_sig_sel[3]] = ppi_byte_sel_24_31_bits;
    } else {
        if (config->port_size == ppi_port_size_8bits) {
            cmd_config.ad_func_sel[config->dq_sig_sel[0]] = ppi_ad_func_data;
            cmd_config.ad_pin_dir[config->dq_sig_sel[0]] = ppi_ad_pin_dir_output;
            cmd_config.byte_sel[config->dq_sig_sel[0]] = ppi_byte_sel_0_7_bits;
            for (uint8_t i = 1; i < 4; i++) {
                cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_addr;
                cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
            }
            cmd_config.byte_sel[config->dq_sig_sel[1]] = ppi_byte_sel_0_7_bits;
            cmd_config.byte_sel[config->dq_sig_sel[2]] = ppi_byte_sel_8_15_bits;
            cmd_config.byte_sel[config->dq_sig_sel[3]] = ppi_byte_sel_16_23_bits;
        } else if (config->port_size == ppi_port_size_16bits) {
            for (uint8_t i = 0; i < 2; i++) {
                cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_data;
                cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
            }
            cmd_config.byte_sel[config->dq_sig_sel[0]] = ppi_byte_sel_0_7_bits;
            cmd_config.byte_sel[config->dq_sig_sel[1]] = ppi_byte_sel_8_15_bits;
            for (uint8_t i = 2; i < 4; i++) {
                cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_addr;
                cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
            }
            cmd_config.byte_sel[config->dq_sig_sel[2]] = ppi_byte_sel_0_7_bits;
            cmd_config.byte_sel[config->dq_sig_sel[3]] = ppi_byte_sel_8_15_bits;
        } else {
            ;    /* Not Support */
        }
    }
    cmd_config.ctrl_pin_value[config->rel_ctrl_pin] = true;

    /* AS Stage */
    cmd_config.cmd_cycle = ppi_ns2cycle(ppi_freq, config->as_in_ns);
    if (config->ad_mux_mode) {
        for (uint8_t i = 0; i < 4; i++) {
            cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_addr;
            cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
        }
        cmd_config.ctrl_pin_value[config->adv_ctrl_pin] = config->addr_valid_polarity;
    }
    cmd_config.ctrl_pin_value[config->wel_ctrl_pin] = true;
    ppi_config_cmd(ppi, cmd_start_index + 4, &cmd_config);

    /* AH Stage */
    cmd_config.cmd_cycle = ppi_ns2cycle(ppi_freq, config->ah_in_ns);
    if (config->ad_mux_mode) {
        for (uint8_t i = 0; i < 4; i++) {
            cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_addr;
            cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
        }
        cmd_config.ctrl_pin_value[config->adv_ctrl_pin] = !config->addr_valid_polarity;
    }
    cmd_config.ctrl_pin_value[config->wel_ctrl_pin] = true;
    ppi_config_cmd(ppi, cmd_start_index + 5, &cmd_config);

    /* WEL Stage */
    cmd_config.cmd_cycle = ppi_ns2cycle(ppi_freq, config->wel_in_ns);
    if (config->ad_mux_mode) {
        for (uint8_t i = 0; i < (1u << config->port_size); i++) {
            cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_data;
            cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
        }
        cmd_config.ctrl_pin_value[config->adv_ctrl_pin] = !config->addr_valid_polarity;
    }
    cmd_config.ctrl_pin_value[config->wel_ctrl_pin] = false;
    ppi_config_cmd(ppi, cmd_start_index + 6, &cmd_config);

    /* WEH Stage */
    cmd_config.cmd_cycle = ppi_ns2cycle(ppi_freq, config->weh_in_ns);
    if (config->ad_mux_mode) {
        for (uint8_t i = 0; i < (1u << config->port_size); i++) {
            cmd_config.ad_func_sel[config->dq_sig_sel[i]] = ppi_ad_func_data;
            cmd_config.ad_pin_dir[config->dq_sig_sel[i]] = ppi_ad_pin_dir_output;
        }
        cmd_config.ctrl_pin_value[config->adv_ctrl_pin] = !config->addr_valid_polarity;
    }
    cmd_config.ctrl_pin_value[config->wel_ctrl_pin] = true;
    ppi_config_cmd(ppi, cmd_start_index + 7, &cmd_config);

    /*
     * CS Config
     */
    cs_config.addr_start_high_12bits = (config->base_address & 0xFFF00000u) >> 20u;
    cs_config.addr_end_high_12bits = ((config->base_address + (config->size_in_byte - 1u)) & 0xFFF00000u) >> 20u;
    cs_config.port_size = config->port_size;
    cs_config.addr_mask = 0xFFFFu;
    cs_config.sync_clk_en = false;
    cs_config.sync_clk_sel = 0;
    cs_config.interval_cycle = 2;
    cs_config.rcmd_start0 = cmd_start_index;
    cs_config.rcmd_end0 = cmd_start_index + 3;
    cs_config.rcmd_start1 = cmd_start_index;
    cs_config.rcmd_end1 = cmd_start_index + 3;
    cs_config.wcmd_start0 = cmd_start_index + 4;
    cs_config.wcmd_end0 = cmd_start_index + 7;
    cs_config.wcmd_start1 = cmd_start_index + 4;
    cs_config.wcmd_end1 = cmd_start_index + 7;
#if defined(HPM_IP_FEATURE_PPI_DM_POLARITY_EACH_CS) && HPM_IP_FEATURE_PPI_DM_POLARITY_EACH_CS
    if (config->dm_valid_polarity) {
        cs_config.dm_polarity = ppi_dm_valid_pol_high;
    } else {
        cs_config.dm_polarity = ppi_dm_valid_pol_low;
    }
#else
    if (config->dm_valid_polarity) {
        ppi_config_dm_pin_polarity(ppi, cs_index, ppi_dm_valid_pol_high);
    } else {
        ppi_config_dm_pin_polarity(ppi, cs_index, ppi_dm_valid_pol_low);
    }
#endif
    ppi_config_cs_pin(ppi, cs_index, &cs_config);
}
