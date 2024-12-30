/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_crc_drv.h"


uint8_t crc_buf[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

struct crc_test {
    char *info;
    uint8_t *buf;
    uint32_t len;
    crc_channel_config_t cfg;
    uint32_t crc_result;
};

struct crc_test crc_test_list[] = {
    {
        .info = "crc_preset_crc32",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc32,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0xCECEE288,
    }, {
        .info = "crc32",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0xFFFFFFFF,
            .poly = 0x04C11DB7,
            .poly_width = CRC_POLY_WIDTH_32,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0xFFFFFFFF,
        },
        .crc_result = 0xCECEE288,
    }, {
        .info = "crc_preset_crc32_autosar",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc32_autosar,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0xF02784A3,
    }, {
        .info = "crc32_autosar",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0xFFFFFFFF,
            .poly = 0xF4ACFB13,
            .poly_width = CRC_POLY_WIDTH_32,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0xFFFFFFFF,
        },
        .crc_result = 0xF02784A3,
    }, {
        .info = "crc_preset_crc16_ccitt",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc16_ccitt,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0xBC40,
    }, {
        .info = "crc16_ccitt",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x0000,
            .poly = 0x1021,
            .poly_width = CRC_POLY_WIDTH_16,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0x0000,
        },
        .crc_result = 0xBC40,
    }, {
        .info = "crc_preset_crc16_xmodem",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc16_xmodem,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x513D,
    }, {
        .info = "crc16_xmodem",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x0000,
            .poly = 0x1021,
            .poly_width = CRC_POLY_WIDTH_16,
            .refout = crc_refout_false,
            .refin = crc_refin_false,
            .xorout = 0x0000,
        },
        .crc_result = 0x513D,
    }, {
        .info = "crc_preset_crc16_modbus",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc16_modbus,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0xE7B4,
    }, {
        .info = "crc16_modbus",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0xFFFF,
            .poly = 0x8005,
            .poly_width = CRC_POLY_WIDTH_16,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0x0000,
        },
        .crc_result = 0xE7B4,
    }, {
        .info = "crc_preset_crc16_dnp",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc16_dnp,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x10EC,
    }, {
        .info = "crc16_dnp",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x0000,
            .poly = 0x3D65,
            .poly_width = CRC_POLY_WIDTH_16,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0xFFFF,
        },
        .crc_result = 0x10EC,
    }, {
        .info = "crc_preset_crc16_x25",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc16_x25,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x13E9,
    }, {
        .info = "crc16_x25",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0xFFFF,
            .poly = 0x1021,
            .poly_width = CRC_POLY_WIDTH_16,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0xFFFF,
        },
        .crc_result = 0x13E9,
    }, {
        .info = "crc_preset_crc16_usb",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc16_usb,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x184B,
    }, {
        .info = "crc16_usb",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0xFFFF,
            .poly = 0x8005,
            .poly_width = CRC_POLY_WIDTH_16,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0xFFFF,
        },
        .crc_result = 0x184B,
    }, {
        .info = "crc_preset_crc16_maxim",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc16_maxim,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0xE8F5,
    }, {
        .info = "crc16_maxim",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x0000,
            .poly = 0x8005,
            .poly_width = CRC_POLY_WIDTH_16,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0xFFFF,
        },
        .crc_result = 0xE8F5,
    }, {
        .info = "crc_preset_crc16_ibm",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc16_ibm,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x170A,
    }, {
        .info = "crc16_ibm",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x0000,
            .poly = 0x8005,
            .poly_width = CRC_POLY_WIDTH_16,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0x0000,
        },
        .crc_result = 0x170A,
    }, {
        .info = "crc_preset_crc8_maxim",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc8_maxim,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x3C,
    }, {
        .info = "crc8_maxim",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x00,
            .poly = 0x31,
            .poly_width = CRC_POLY_WIDTH_8,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0x00,
        },
        .crc_result = 0x3C,
    }, {
        .info = "crc_preset_crc8_rohc",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc8_rohc,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x5A,
    }, {
        .info = "crc8_rohc",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0xFF,
            .poly = 0x07,
            .poly_width = CRC_POLY_WIDTH_8,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0x00,
        },
        .crc_result = 0x5A,
    }, {
        .info = "crc_preset_crc8_itu",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc8_itu,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x14,
    }, {
        .info = "crc8_itu",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x00,
            .poly = 0x07,
            .poly_width = CRC_POLY_WIDTH_8,
            .refout = crc_refout_false,
            .refin = crc_refin_false,
            .xorout = 0x55,
        },
        .crc_result = 0x14,
    }, {
        .info = "crc_preset_crc8",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc8,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x41,
    }, {
        .info = "crc8",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x00,
            .poly = 0x07,
            .poly_width = CRC_POLY_WIDTH_8,
            .refout = crc_refout_false,
            .refin = crc_refin_false,
            .xorout = 0x00,
        },
        .crc_result = 0x41,
    }, {
        .info = "crc_preset_crc5_usb",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_crc5_usb,
            .in_byte_order = crc_in_byte_order_lsb,
        },
        .crc_result = 0x1E,
    }, {
        .info = "crc5_usb",
        .buf = crc_buf,
        .len = 16,
        .cfg = {
            .preset = crc_preset_none,
            .in_byte_order = crc_in_byte_order_lsb,
            .init = 0x1F,
            .poly = 0x05,
            .poly_width = CRC_POLY_WIDTH_5,
            .refout = crc_refout_true,
            .refin = crc_refin_true,
            .xorout = 0x1F,
        },
        .crc_result = 0x1E,
    },
};

uint32_t ch_list[] = {
    CRC_CHN_0,
    CRC_CHN_1,
    CRC_CHN_2,
    CRC_CHN_3,
    CRC_CHN_4,
    CRC_CHN_5,
    CRC_CHN_6,
    CRC_CHN_7,
};

int main(void)
{
    board_init();

    clock_add_to_group(clock_crc0, 0);

    struct crc_test *pcrc_test;
    crc_channel_config_t *pcfg;
    uint8_t *buf;
    uint32_t len;
    uint32_t result;
    uint32_t ch_index;
    uint32_t crc_test_list_index;
    uint32_t ch_list_index;
    uint32_t crc_test_list_size = sizeof(crc_test_list) / sizeof(crc_test_list[0]);
    uint32_t ch_list_size = sizeof(ch_list) / sizeof(ch_list[0]);

    for (ch_list_index = 0; ch_list_index < ch_list_size; ch_list_index++) {
        ch_index = ch_list[ch_list_index];

        for (crc_test_list_index = 0; crc_test_list_index < crc_test_list_size; crc_test_list_index++) {
            pcrc_test = &crc_test_list[crc_test_list_index];
            pcfg = &pcrc_test->cfg;
            buf = pcrc_test->buf;
            len = pcrc_test->len;

            crc_setup_channel_config(HPM_CRC, ch_index, pcfg);
            crc_calc_block_bytes(HPM_CRC, ch_index, buf, len);
            result = crc_get_result(HPM_CRC, ch_index);
            if (result != pcrc_test->crc_result) {
                printf("Failed: ch: [%u], info: [%s], api: [crc_calc_block_bytes]!!!\n", ch_index, pcrc_test->info);
                while (1) {
                }
            }

            crc_setup_channel_config(HPM_CRC, ch_index, pcfg);
            crc_calc_block_half_words(HPM_CRC, ch_index, (uint16_t *)buf, len / 2);
            result = crc_get_result(HPM_CRC, ch_index);
            if (result != pcrc_test->crc_result) {
                printf("Failed: ch: [%u], info: [%s], api: [crc_calc_block_half_words]!!!\n", ch_index, pcrc_test->info);
                while (1) {
                }
            }

            crc_setup_channel_config(HPM_CRC, ch_index, pcfg);
            crc_calc_block_words(HPM_CRC, ch_index, (uint32_t *)buf, len / 4);
            result = crc_get_result(HPM_CRC, ch_index);
            if (result != pcrc_test->crc_result) {
                printf("Failed: ch: [%u], info: [%s], api: [crc_calc_block_words]!!!\n", ch_index, pcrc_test->info);
                while (1) {
                }
            }

            crc_setup_channel_config(HPM_CRC, ch_index, pcfg);
            crc_calc_large_block_fast(HPM_CRC, ch_index, buf, len);
            result = crc_get_result(HPM_CRC, ch_index);
            if (result != pcrc_test->crc_result) {
                printf("Failed: ch: [%u], info: [%s], api: [crc_calc_large_block_fast]!!!\n", ch_index, pcrc_test->info);
                while (1) {
                }
            }
        }

        printf("ch: [%u] passed\n", ch_list_index);
    }

    printf("=============================================\n");
    printf("All cases are PASSED\n");
    printf("=============================================\n");


    while (1) {
    }
    return 0;
}
