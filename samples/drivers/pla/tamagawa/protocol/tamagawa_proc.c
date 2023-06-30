/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tamagawa_proc.h"
#include "hpm_math.h"
#include "hpm_l1c_drv.h"

uint16_t tmgw_cmd_sendcf[TMGW_CMD_NUM] = {
    0x81,
    0xa3,
    0x93,
    0xb1,
    0xbb,
    0x87,
    0x8d,
    0x99,
    0xaf
};

void tmgw_cmd_send_write_eeprom(uint8_t addr, uint8_t data, tmgw_cmd_cfg_t *cfg)
{
    uint8_t crc = 0;

    cfg->send_buf[0] = tmgw_cmd_sendcf[tmgw_cmd_wr_eeprom];
    cfg->send_buf[1] = (((uint16_t)hpm_math_sw_reverse_bit8_msb_to_lsb(addr & 0x7f)) << 1) | 0x01;
    cfg->send_buf[2] = (((uint16_t)hpm_math_sw_reverse_bit8_msb_to_lsb(data & 0x7f)) << 1) | 0x01;
    for (uint8_t i = 0; i < cfg->tx_num; i++) {
        crc ^= (cfg->send_buf[i] >> 1);
    }
    cfg->send_buf[3] = ((uint16_t)crc << 1) | 0x01;
    cfg->send_buf[4] = 0x3ff; /* END */
}

void tmgw_cmd_send_read_eeprom(uint8_t addr, tmgw_cmd_cfg_t *cfg)
{
    uint8_t crc = 0;

    cfg->send_buf[0] = tmgw_cmd_sendcf[tmgw_cmd_rd_eeprom];
    cfg->send_buf[1] = (((uint16_t)hpm_math_sw_reverse_bit8_msb_to_lsb(addr & 0x7f)) << 1) | 0x01;
    for (uint8_t i = 0; i < cfg->tx_num; i++) {
        crc ^= (cfg->send_buf[i] >> 1);
    }
    cfg->send_buf[2] = ((uint16_t)crc << 1) | 0x01;
    cfg->send_buf[3] = 0x3ff;
}

bool tmgw_cmd_init_data(tmgw_cmd_cfg_t *cfg)
{
    uint8_t i;

    for (i = 0; i < TMGW_CMD_NUM; i++) {
        if (cfg[i].send_buf == NULL) {
            return false;
        }
        cfg[i].send_buf[0] = tmgw_cmd_sendcf[i];
        cfg[i].send_buf[1] = 0x3ff;
    }
    if (l1c_dc_is_enabled()) {
        l1c_dc_flush_all();
    }
    return true;
}

void tmgw_cmd_qei_phcmp_init(uint32_t (*qei_buf)[2], tmgw_cmd_cfg_t *cfg)
{
    uint8_t i;

    for (i = 0; i < TMGW_CMD_NUM; i++) {
        qei_buf[i][0] = 0xc0000000 + cfg[i].tx_num * 10 * 2;
        qei_buf[i][1] = 0xc0000000 + (cfg[i].tx_num + cfg[i].rx_num) * 10 * 2;
    }
    if (l1c_dc_is_enabled()) {
        l1c_dc_flush_all();
    }
}

uint8_t tmgw_cmd_crc8_calibration(uint16_t *data, tmgw_cmd_cfg_t *cfg)
{
    uint8_t crc;
    uint8_t num;

    crc = 0;
    num = cfg->rx_num - 1;
    for (uint8_t i = 0; i < num; i++) {
        crc ^= *data;
        data++;
    }
    return crc;
}

bool tmgw_message_process(volatile uint32_t *status, tmgw_dev_cfg_t *dev, tmgw_proc_message_t *msg)
{
    uint8_t rxnum, txnum;
    uint8_t j;
    uint16_t crc_recv, crc_cal;

    dev->comm_time_delay_config = dev->comm_time_delay_init;
    if (*status) {
        *status = 0;
        dev->comm_time_delay = *dev->hall_buf;
        *dev->hall_buf = 0;
        dev->comm_time_delay_config -= dev->comm_time_delay;
        rxnum = dev->buf_cfg[dev->run_cmd].rx_num;
        txnum = dev->buf_cfg[dev->run_cmd].tx_num;
        for (j = 0; j < rxnum; j++) {
            dev->copy_buf[j] = dev->buf_cfg[dev->run_cmd].receive_buf[(txnum + j)] >> 1;
        }
        if (dev->copy_buf[0] == tmgw_cmd_sendcf[dev->run_cmd] >> 1) {
            crc_recv = dev->copy_buf[rxnum - 1];
            crc_cal = tmgw_cmd_crc8_calibration(&dev->copy_buf[0], &dev->buf_cfg[dev->run_cmd]);
            if (crc_cal == crc_recv) {
                msg->is_completed = true;
                msg->is_crc_error = false;
                msg->cmd = dev->run_cmd;
            } else {
                msg->is_completed = false;
                msg->is_crc_error = true;
                return false;
            }
        } else {
            msg->is_completed = false;
            msg->is_cmd_error = true;
            return false;
        }

        for (j = 0; j < rxnum; j++) {
            dev->copy_buf[j] = hpm_math_sw_reverse_bit8_msb_to_lsb(dev->copy_buf[j]);
        }

        switch (msg->cmd) {
        case tmgw_cmd_id0:
        case tmgw_cmd_id7:
        case tmgw_cmd_id8:
        case tmgw_cmd_idc:
            msg->encoder_error = dev->copy_buf[1] & 0xf;
            msg->abs = (dev->copy_buf[4] << 16) | (dev->copy_buf[3] << 8) | (dev->copy_buf[2]);
        break;
        case tmgw_cmd_id1:
            msg->encoder_error = dev->copy_buf[1] & 0xf;
            msg->abm = (dev->copy_buf[4] << 16) | (dev->copy_buf[3] << 8) | (dev->copy_buf[2]);
        break;
        case tmgw_cmd_id2:
            msg->encoder_error = dev->copy_buf[1] & 0xf;
            msg->id = dev->copy_buf[2];
        break;
        case tmgw_cmd_id3:
            msg->encoder_error = dev->copy_buf[1] & 0xf;
            msg->abs = (dev->copy_buf[4] << 16) | (dev->copy_buf[3] << 8) | (dev->copy_buf[2]);
            msg->id = dev->copy_buf[5];
            msg->abm = (dev->copy_buf[8] << 16) | (dev->copy_buf[7] << 8) | (dev->copy_buf[6]);
            msg->almc = dev->copy_buf[9];
        break;
        case tmgw_cmd_wr_eeprom:
        case tmgw_cmd_rd_eeprom:
            msg->encoder_error = 0;
            msg->is_eeprom_busy = 0;
            msg->is_eeprom_busy |= dev->copy_buf[1] >> 7;
            msg->is_eeprom_busy |= dev->copy_buf[2] >> 7;
            msg->eeprom_addr = dev->copy_buf[1] & 0x7f;
            msg->eeprom_data = dev->copy_buf[2] & 0x7f;
        break;
        default:
            for (j = 0; j < rxnum; j++) {
                dev->copy_buf[j] = hpm_math_sw_reverse_bit8_lsb_to_msb(dev->copy_buf[j]);
            }
            msg->encoder_error = 0;
            msg->abs = 0;
            msg->id = 0;
            msg->abm = 0;
            msg->almc = 0;
        break;
        }
    } else {
        msg->is_completed = false;
        return false;
    }
    return true;
}