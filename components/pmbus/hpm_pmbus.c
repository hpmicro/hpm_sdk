/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_pmbus.h"
#include "hpm_pmbus_private.h"
#include "hpm_smbus.h"

typedef struct {
    I2C_Type *i2c_ptr;
    uint8_t slave_addr;
    bool cmd_found;
    uint8_t command_code;
    uint32_t data_length;
    hpm_pmbus_slave_cfg_t slave_conf[255];
} hpm_pmbus_cfg_t;

extern const hpm_pmbus_cmd_param_t pmbus_cmd_param_table[];

static hpm_pmbus_cfg_t hpm_pmbus_cfg_table[] = {
#if defined(HPM_I2C0)
    {
        .i2c_ptr = HPM_I2C0,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

#if defined(HPM_I2C1)
    {
        .i2c_ptr = HPM_I2C1,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

#if defined(HPM_I2C2)
    {
        .i2c_ptr = HPM_I2C2,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

#if defined(HPM_I2C3)
    {
        .i2c_ptr = HPM_I2C3,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

#if defined(HPM_I2C4)
    {
        .i2c_ptr = HPM_I2C4,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

#if defined(HPM_I2C5)
    {
        .i2c_ptr = HPM_I2C5,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

#if defined(HPM_I2C6)
    {
        .i2c_ptr = HPM_I2C6,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

#if defined(HPM_I2C7)
    {
        .i2c_ptr = HPM_I2C7,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

#if defined(HPM_I2C8)
    {
        .i2c_ptr = HPM_I2C8,
        .cmd_found = false,
        .data_length = 0,
    },
#endif

};

static hpm_pmbus_cfg_t *hpm_pmbus_get_cfg_obj(I2C_Type *ptr)
{
    hpm_pmbus_cfg_t *obj;
    uint8_t i = 0;
    for (i = 0; i < (sizeof(hpm_pmbus_cfg_table) / sizeof(hpm_pmbus_cfg_t)); i++) {
        obj = &hpm_pmbus_cfg_table[i];
        if (obj->i2c_ptr == ptr) {
            return obj;
        }
    }
    return NULL;
}

static uint8_t hpm_smbus_pec_crc8_in_command(uint8_t slave_addr, uint8_t command, uint8_t *data, uint32_t len, bool read, bool block)
{
    /* The PEC is a CRC-8 error-checking byte, calculated on all the message bytes (including addresses and read/write bits) */
    uint32_t i;
    uint8_t crc = 0x00;
    crc ^= (slave_addr << 1);
    for (i = 0; i < 8; i++) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0x07;
        } else {
            crc <<= 1;
        }
    }

    crc ^= command;
    for (i = 0; i < 8; i++) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0x07;
        } else {
            crc <<= 1;
        }
    }

    if (read == true) {
        crc ^= ((slave_addr << 1) + 1);
        for (i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
        if (block == true) {
            crc ^= len;
            for (i = 0; i < 8; i++) {
                if (crc & 0x80) {
                    crc = (crc << 1) ^ 0x07;
                } else {
                    crc <<= 1;
                }
            }
        }
    }

    while (len--) {
        crc ^= *data++;
        for (i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void hpm_pmbus_isr_handler(I2C_Type *ptr)
{
    volatile uint32_t status, irq;
    uint8_t pec = 0;
    uint8_t size;
    uint32_t send_size;
    uint8_t cmd_code;
    hpm_pmbus_complete_cb_cfg_t cfg;
    status = i2c_get_status(ptr);
    irq = i2c_get_irq_setting(ptr);
    hpm_pmbus_cfg_t *obj = hpm_pmbus_get_cfg_obj(ptr);

    if (I2C_SETUP_MASTER_GET(ptr) == false) {
        /* address hit */
        if (status & I2C_EVENT_ADDRESS_HIT) {
            if (I2C_DIR_SLAVE_WRITE == i2c_get_direction(ptr)) {
                i2c_enable_irq(ptr, I2C_EVENT_FIFO_EMPTY);
            }
            i2c_clear_status(ptr, I2C_EVENT_ADDRESS_HIT);
            obj->data_length = 0;
        }
    }

    /* receive */
    if ((status & I2C_EVENT_BYTE_RECEIVED) && (irq & I2C_EVENT_BYTE_RECEIVED)) {
        i2c_clear_status(ptr, I2C_EVENT_BYTE_RECEIVED);
        if (obj->cmd_found == false) {
            cmd_code = i2c_read_byte(ptr);
            /* check if the command is valid */
            if (obj->slave_conf[cmd_code].is_valid == true) {
                /* Mark that the command has been found, set the current command code */
                obj->command_code = cmd_code;
                obj->cmd_found = true;
                ptr->CMD = I2C_CMD_ACK;
            } else {
                /* Send a NACK response, indicating an invalid command */
                ptr->CMD = I2C_CMD_NACK;
            }
        } else {
            /* If the command has been found and the data length is within the expected range */
            if ((obj->cmd_found == true) && (obj->data_length < pmbus_cmd_param_table[obj->command_code].data_length)) {
                if (obj->slave_conf[obj->command_code].rdata) {
                    obj->slave_conf[obj->command_code].rdata[obj->data_length++] = i2c_read_byte(ptr);
                    /* Send an ACK response, indicating successful data reception */
                    ptr->CMD = I2C_CMD_ACK;
                } else {
                    /* Send a NACK response, indicating a failure in storing data */
                    ptr->CMD = I2C_CMD_NACK;
                    obj->data_length = 0;
                }
            } else {
                /* has pec byte*/
                if (obj->data_length >= pmbus_cmd_param_table[obj->command_code].data_length) {
                    obj->slave_conf[obj->command_code].rdata[obj->data_length++] = i2c_read_byte(ptr);
                    /* Send an ACK response, indicating successful data reception */
                    ptr->CMD = I2C_CMD_ACK;
                } else {
                    i2c_read_byte(ptr);
                    ptr->CMD = I2C_CMD_NACK;
                }
            }
        }
    }

    /* transmit */
    if ((status & I2C_EVENT_FIFO_EMPTY) && (irq & I2C_EVENT_FIFO_EMPTY)) {
        if ((obj->data_length >= pmbus_cmd_param_table[obj->command_code].data_length) ||
            (obj->cmd_found == false)) {
            i2c_disable_irq(ptr, I2C_EVENT_FIFO_EMPTY);
        } else {
            while (!i2c_fifo_is_full(ptr)) {
                if ((obj->data_length == 0) && (pmbus_cmd_param_table[obj->command_code].read_transaction_type == read_block)) {
                    i2c_write_byte(ptr, obj->slave_conf[obj->command_code].data_length);
                }
                if (pmbus_cmd_param_table[obj->command_code].read_transaction_type == read_block) {
                    send_size = obj->slave_conf[obj->command_code].data_length;
                } else {
                    send_size = pmbus_cmd_param_table[obj->command_code].data_length;
                }
                if (obj->data_length < send_size) {
                    i2c_write_byte(ptr, obj->slave_conf[obj->command_code].wdata[obj->data_length++]);
                } else {
                    if (obj->data_length == send_size) {
                        if (pmbus_cmd_param_table[obj->command_code].read_transaction_type == read_block) {
                            /* For read block, pec check must contain length */
                            pec = hpm_smbus_pec_crc8_in_command(obj->slave_addr, obj->command_code, &obj->slave_conf[obj->command_code].wdata[0],
                                                                    obj->data_length, true, true);
                        } else {
                            pec = hpm_smbus_pec_crc8_in_command(obj->slave_addr, obj->command_code, &obj->slave_conf[obj->command_code].wdata[0],
                                                                    obj->data_length, true, false);
                        }
                        i2c_write_byte(ptr, pec);
                        obj->data_length++;
                    } else {
                        i2c_disable_irq(ptr, I2C_EVENT_FIFO_EMPTY);
                        break;
                    }
                }
            }
        }
    }

    /* complete */
    if (status & I2C_EVENT_TRANSACTION_COMPLETE) {
        i2c_disable_irq(ptr, I2C_EVENT_FIFO_EMPTY);
        i2c_clear_status(ptr, I2C_EVENT_TRANSACTION_COMPLETE);
        /* For the slave mode, write-related commands are read operations */
        if (I2C_DIR_SLAVE_READ == i2c_get_direction(ptr)) {
            if (!obj->data_length) {
                return;
            }
            if (pmbus_cmd_param_table[obj->command_code].write_transaction_type == write_block) {
                /* need read length for write_block */
                if (obj->slave_conf[obj->command_code].rdata == NULL) {
                    return;
                }
                size = obj->slave_conf[obj->command_code].rdata[0];
                if (obj->data_length == (uint32_t)(size + 2)) {
                    pec = hpm_smbus_pec_crc8_in_command(obj->slave_addr, obj->command_code, &obj->slave_conf[obj->command_code].rdata[0], size + 1, false, true);
                    memcpy(&obj->slave_conf[obj->command_code].rdata[0], &obj->slave_conf[obj->command_code].rdata[1], size);
                    if (pec != obj->slave_conf[obj->command_code].rdata[size + 1]) {
                        cfg.pec_check = false;
                    } else {
                        cfg.pec_check = true;
                    }
                    cfg.len = size;
                }
            } else {
                switch (pmbus_cmd_param_table[obj->command_code].write_transaction_type) {
                case write_word:
                    cfg.len = 2;
                    break;
                case write_byte:
                    cfg.len = 1;
                    break;
                default:
                    cfg.len = 0;
                    break;
                }
                pec = hpm_smbus_pec_crc8_in_command(obj->slave_addr, obj->command_code, &obj->slave_conf[obj->command_code].rdata[0], cfg.len, false, false);
                if (obj->slave_conf[obj->command_code].rdata == NULL) {
                    return;
                }
                if (pec != obj->slave_conf[obj->command_code].rdata[obj->data_length - 1]) {
                    cfg.pec_check = false;
                } else {
                    cfg.pec_check = true;
                }
            }
            cfg.data = obj->slave_conf[obj->command_code].rdata;
            cfg.read = true;
        } else {
            if (!obj->data_length) {
                return;
            }
            if (pmbus_cmd_param_table[obj->command_code].read_transaction_type == read_block) {
                cfg.len = obj->slave_conf[obj->command_code].data_length;
            } else {
                cfg.len = pmbus_cmd_param_table[obj->command_code].data_length;
            }
            cfg.data = obj->slave_conf[obj->command_code].wdata;
            cfg.read = false;
            cfg.pec_check = true;
        }
        cfg.command = obj->command_code;
        if (obj->slave_conf[obj->command_code].callback != NULL) {
            obj->slave_conf[obj->command_code].callback(ptr, &cfg);
        }
        obj->cmd_found = false;
    }
}

hpm_stat_t hpm_pmbus_slave_init(I2C_Type *ptr, uint8_t slave_addr)
{
    hpm_stat_t sta = status_success;
    hpm_pmbus_cfg_t *obj = hpm_pmbus_get_cfg_obj(ptr);
    if (obj == NULL) {
        return status_invalid_argument;
    }
    memset(obj->slave_conf, 0, sizeof(obj->slave_conf));
    obj->slave_addr = slave_addr;
    i2c_enable_irq(ptr, I2C_EVENT_ADDRESS_HIT | I2C_EVENT_TRANSACTION_COMPLETE);
    i2c_enable_auto_ack(ptr);
    return sta;
}

hpm_stat_t hpm_pmbus_master_read(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t *len)
{
    hpm_pmbus_cmd_param_t para;
    hpm_stat_t sta = status_success;
    para = pmbus_cmd_param_table[command];
    switch (para.read_transaction_type) {
    case read_byte:
        sta = hpm_smbus_master_read_byte_in_command(ptr, slave_address, command, data);
        *len = 1;
        break;
    case read_word:
        sta = hpm_smbus_master_read_word_in_command(ptr, slave_address, command, (uint16_t *)data);
        *len = 2;
        break;
    case read_block:
        if (para.data_length != 0xFFFFFFFF) {
            sta = hpm_smbus_master_read_block_in_command(ptr, slave_address, command, data, para.data_length);
            *len = para.data_length;
        } else {
            sta = hpm_smbus_master_read_block_in_command(ptr, slave_address, command, data, *len);
        }
        break;
    case mfr_defined:
    case extended_command:
        sta = status_pmbus_not_support_cmd;
        break;
    default:
        sta = status_pmbus_not_transaction_type;
        break;
    }
    return sta;
}

hpm_stat_t hpm_pmbus_master_write(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t len)
{
    hpm_pmbus_cmd_param_t para;
    hpm_stat_t sta = status_success;
    para = pmbus_cmd_param_table[command];
    switch (para.write_transaction_type) {
    case send_byte:
        sta = hpm_smbus_master_write_byte(ptr, slave_address, command);
        break;
    case write_byte:
        sta = hpm_smbus_master_write_byte_in_command(ptr, slave_address, command, *data);
        break;
    case write_word:
        sta = hpm_smbus_master_write_word_in_command(ptr, slave_address, command, *(uint16_t *)data);
        break;
    case write_block:
        if (para.data_length != 0xFFFFFFFF) {
            sta = hpm_smbus_master_write_block_in_command(ptr, slave_address, command, data, para.data_length);
        } else {
            sta = hpm_smbus_master_write_block_in_command(ptr, slave_address, command, data, len);
        }
        break;
    case mfr_defined:
    case extended_command:
        sta = status_pmbus_not_support_cmd;
        break;
    default:
        sta = status_pmbus_not_transaction_type;
        break;
    }
    return sta;
}

hpm_stat_t hpm_pmbus_slave_command_transaction_install(I2C_Type *ptr, uint8_t command, uint8_t *wdata, uint8_t *rdata, uint32_t len, hpm_pmbus_complete_cb_t callback)
{
    hpm_stat_t stat = status_success;
    hpm_pmbus_cfg_t *obj = hpm_pmbus_get_cfg_obj(ptr);
    if ((obj == NULL) ||
        ((pmbus_cmd_param_table[command].write_transaction_type != none) && (rdata == NULL)) ||
        ((pmbus_cmd_param_table[command].read_transaction_type != none) && (wdata == NULL)) ||
        ((pmbus_cmd_param_table[command].data_length != 0xFFFFFFFF) && (len < (pmbus_cmd_param_table[command].data_length + 1)))) {
        /* included pec byte, So the length needs to be increased by 1 */
        return status_invalid_argument;
    }
    if ((pmbus_cmd_param_table[command].write_transaction_type == mfr_defined) ||
        (pmbus_cmd_param_table[command].read_transaction_type == mfr_defined) ||
        (pmbus_cmd_param_table[command].write_transaction_type == extended_command) ||
        (pmbus_cmd_param_table[command].read_transaction_type == extended_command)) {
        return status_pmbus_not_support_cmd;
    }
    obj->slave_conf[command].is_valid = true;
    obj->slave_conf[command].wdata = wdata;
    obj->slave_conf[command].rdata = rdata;
    obj->slave_conf[command].data_length = len;
    obj->slave_conf[command].callback = callback;
    return stat;
}

hpm_stat_t hpm_pmbus_slave_command_transaction_uninstall(I2C_Type *ptr, uint8_t command)
{
    hpm_stat_t stat = status_success;
    hpm_pmbus_cfg_t *obj = hpm_pmbus_get_cfg_obj(ptr);
    if (obj == NULL) {
        return status_invalid_argument;
    }
    obj->slave_conf[command].is_valid = false;
    return stat;
}
