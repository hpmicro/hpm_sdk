/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_spi_drv.h"
#include "hpm_clock_drv.h"

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

   printf("SPI-Master transfer mode:%s\n", trans_mode_table[trans_mode]);
}

void spi_master_command_dump(spi_control_config_t *config, uint8_t *cmd)
{
    if (config->master_config.cmd_enable == true && cmd != NULL) {
        printf("SPI-Master write command: 0x%02x\n", *cmd);
    }
}

void spi_master_address_dump(spi_control_config_t *config, uint32_t *addr)
{
    if (config->master_config.addr_enable == true && addr != NULL) {
        printf("SPI-Master write address:0x%x\n", *addr);
    }
}

void spi_master_data_dump(spi_op_t op, uint32_t datalen, uint8_t *buff,  uint32_t size)
{
    uint32_t i;

    if (op == spi_op_no_data) {
        printf("SPI-Master no data transfer.\n");
        return;
    }

    if (buff == NULL || size == 0) {
        return;
    }

    printf("SPI-Master %s", op == spi_op_write ? "write data: " : "read  data: ");

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

void spi_master_frame_dump(uint32_t datalen,
                           spi_control_config_t *config,
                           uint8_t *cmd, uint32_t *addr,
                           uint8_t *wbuff,  uint32_t wsize, uint8_t *rbuff,  uint32_t rsize)
{
    uint8_t trans_mode = config->common_config.trans_mode;

    spi_master_command_dump(config, cmd);
    spi_master_address_dump(config, addr);

    if (trans_mode == spi_trans_write_read_together || trans_mode == spi_trans_write_read || trans_mode == spi_trans_write_dummy_read) {
        spi_master_data_dump(spi_op_write, datalen, wbuff, wsize);
        spi_master_data_dump(spi_op_read, datalen, rbuff, rsize);
    } else if (trans_mode == spi_trans_read_write ||  trans_mode == spi_trans_read_dummy_write) {
        spi_master_data_dump(spi_op_read, datalen, rbuff, rsize);
        spi_master_data_dump(spi_op_write, datalen, wbuff, wsize);
    } else if (trans_mode == spi_trans_write_only || trans_mode == spi_trans_dummy_write) {
        spi_master_data_dump(spi_op_write, datalen, wbuff, wsize);
    } else if (trans_mode == spi_trans_read_only || trans_mode == spi_trans_dummy_read) {
        spi_master_data_dump(spi_op_read, datalen, rbuff, rsize);
    } else {
        spi_master_data_dump(spi_op_no_data, 0, NULL, 0);
    }
}

int main(void)
{
    uint8_t wbuff[10] = {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9};
    uint8_t rbuff[10] = {0};
    spi_timing_config_t timing_config = {0};
    spi_format_config_t format_config = {0};
    spi_control_config_t control_config = {0};
    hpm_stat_t stat;
    uint32_t spi_clcok;

    /* bsp initialization */
    board_init();
    spi_clcok = board_init_spi_clock(BOARD_APP_SPI_BASE);
    board_init_spi_pins(BOARD_APP_SPI_BASE);
    printf("SPI-Master Polling Transfer Example\n");

    /* set SPI sclk frequency for master */
    spi_master_get_default_timing_config(&timing_config);
    timing_config.master_config.clk_src_freq_in_hz = spi_clcok;
    timing_config.master_config.sclk_freq_in_hz = BOARD_APP_SPI_SCLK_FREQ;
    if (status_success != spi_master_timing_init(BOARD_APP_SPI_BASE, &timing_config)) {
        printf("SPI master timming init failed\n");
        while (1) {
        }
    }
    printf("SPI-Master transfer timing is configured.\n");
    printf("SPI-Master transfer source clock frequency: %dHz\n", timing_config.master_config.clk_src_freq_in_hz);
    printf("SPI-Master transfer sclk frequency: %dHz\n", timing_config.master_config.sclk_freq_in_hz);

    /* set SPI format config for master */
    spi_master_get_default_format_config(&format_config);
    format_config.common_config.data_len_in_bits = BOARD_APP_SPI_DATA_LEN_IN_BITS;
    format_config.common_config.mode = spi_master_mode;
    format_config.common_config.cpol = spi_sclk_high_idle;
    format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges;
    spi_format_init(BOARD_APP_SPI_BASE, &format_config);
    printf("SPI-Master transfer format is configured.\n");

    /* set SPI control config for master */
    spi_master_get_default_control_config(&control_config);
    control_config.master_config.cmd_enable = false;  /* cmd phase control for master */
    control_config.master_config.addr_enable = false; /* address phase control for master */
    control_config.common_config.trans_mode = spi_trans_write_read_together;
    spi_transfer_mode_print(&control_config);

    printf("SPI-Master transfer starts.\n");
    stat = spi_transfer(BOARD_APP_SPI_BASE,
                &control_config,
                NULL, NULL,
                (uint8_t *)wbuff, ARRAY_SIZE(wbuff), (uint8_t *)rbuff, ARRAY_SIZE(rbuff));

    if (stat == status_success) {
        spi_master_frame_dump(BOARD_APP_SPI_DATA_LEN_IN_BITS,
                                &control_config,
                                NULL, NULL,
                                (uint8_t *)wbuff, ARRAY_SIZE(wbuff), (uint8_t *)rbuff, ARRAY_SIZE(rbuff));

        printf("SPI-Master transfer ends.\n");
    } else {
        printf("SPI-Master transfer error[%d]!\n", stat);
    }

    while(1) {
    }

    return 0;
}
