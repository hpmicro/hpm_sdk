/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_spi_drv.h"

typedef enum {
    spi_op_write = 0,
    spi_op_read,
    spi_op_no_data
} spi_op_t;

void spi_transfer_mode_print(spi_control_config_t *config)
{
    uint8_t trans_mode = config->common_config.trans_mode;

    char trans_mode_table[][50] = { "write-read-together",
                                    "write-only",
                                    "read-only",
                                    "write-read",
                                    "read-write",
                                    "write-dummy-read",
                                    "read-dummy-write",
                                    "no-data",
                                    "dummy-write",
                                    "dummy-read"
                                };

   printf("SPI-Slave transfer mode:%s\n", trans_mode_table[trans_mode]);
}

void spi_slave_comand_dump(spi_control_config_t *config, uint8_t *cmd)
{
    if (config->slave_config.slave_data_only == false &&  cmd != NULL) {
        printf("SPI-Slave read command:0x%02x\n", *cmd);
    }
}

void spi_slave_address_dump(spi_control_config_t *config)
{
    if (config->slave_config.slave_data_only == false) {
        printf("SPI-Slave read address:dummy\n");
    }
}

void spi_slave_data_dump(spi_op_t op, uint32_t datalen,  uint8_t *buff,  uint32_t size)
{
    uint32_t i;

    if (op == spi_op_no_data) {
        printf("SPI-Slave no data transfer.\n");
        return;
    }

    if (buff == NULL || size == 0) {
        return;
    }

    printf("SPI-Slave %s", op == spi_op_write ? "write data: " : "read  data: ");

    for (i = 0; i < size; i++) {
        if (datalen <= 8) {
            printf("0x%02x ", *(uint8_t *)buff);
             buff += 1;
        } else if (datalen <= 16) {
            printf("0x%02x ", *(uint16_t *)buff);
            buff += 2;
        } else {
            printf("0x%02x ", *(uint32_t *)buff);
            buff += 4;
        }
    }
    printf("\n");
}

void spi_slave_frame_dump(uint32_t datalen,
                          spi_control_config_t *config,
                          uint8_t *cmd,
                          uint8_t *wbuff,  uint32_t wsize, uint8_t *rbuff,  uint32_t rsize)
{
    uint8_t trans_mode = config->common_config.trans_mode;

    spi_slave_comand_dump(config, cmd);
    spi_slave_address_dump(config);

    if (trans_mode == spi_trans_write_read || trans_mode == spi_trans_write_dummy_read) {
        spi_slave_data_dump(spi_op_write, datalen, wbuff, wsize);
        spi_slave_data_dump(spi_op_read, datalen, rbuff, rsize);
    } else if (trans_mode == spi_trans_write_read_together || trans_mode == spi_trans_read_write ||  trans_mode == spi_trans_read_dummy_write) {
        spi_slave_data_dump(spi_op_read, datalen, rbuff, rsize);
        spi_slave_data_dump(spi_op_write, datalen, wbuff, wsize);
    } else if (trans_mode == spi_trans_write_only || trans_mode == spi_trans_dummy_write) {
        spi_slave_data_dump(spi_op_write, datalen, wbuff, wsize);
    } else if (trans_mode == spi_trans_read_only || trans_mode == spi_trans_dummy_read) {
         spi_slave_data_dump(spi_op_read, datalen, rbuff, rsize);
    } else {
        spi_slave_data_dump(spi_op_no_data, 0, NULL, 0);
    }
}

int main(void)
{
    uint8_t wbuff[10] = {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9};
    uint8_t rbuff[10] = {0};
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    hpm_stat_t stat;

    /* bsp initialization */
    board_init();
    board_init_spi_clock(BOARD_APP_SPI_BASE);
    board_init_spi_pins(BOARD_APP_SPI_BASE);
    printf("SPI-Slave Polling Transfer Example\n");

    /* set SPI format config for slave */
    spi_slave_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = BOARD_APP_SPI_DATA_LEN_IN_BITS;
    format_config.common_config.mode = spi_slave_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(BOARD_APP_SPI_BASE, &format_config);
    printf("SPI-Slave transfer format is configured.\n");

    /* set SPI control config for slave */
    spi_slave_get_default_control_config(&control_config);
    control_config.slave_config.slave_data_only = true; /* raw data mode for slave */
    /* data only mode, trans_mode have to be spi_trans_write_read_together */
    control_config.common_config.trans_mode = spi_trans_write_read_together;
    spi_transfer_mode_print(&control_config);

    printf("SPI-Slave transfer waits.\n");

    do {
        stat = spi_transfer(BOARD_APP_SPI_BASE,
                        &control_config,
                        NULL, NULL,
                        (uint8_t *)wbuff, ARRAY_SIZE(wbuff), (uint8_t *)rbuff, ARRAY_SIZE(rbuff));
    } while (stat == status_timeout);

    if (stat == status_success) {
        spi_slave_frame_dump(BOARD_APP_SPI_DATA_LEN_IN_BITS,
                                &control_config,
                                NULL,
                                (uint8_t *)wbuff, ARRAY_SIZE(wbuff), (uint8_t *)rbuff, ARRAY_SIZE(rbuff));

        printf("SPI-Slave transfer ends.\n");
    } else {
        printf("SPI-Slave transfer error[%d]!\n", stat);
    }

    while (1) {

    }

    return 0;
}
