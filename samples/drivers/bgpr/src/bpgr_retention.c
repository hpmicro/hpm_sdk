/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_bpor_drv.h"
#include "hpm_bgpr_drv.h"

static const uint32_t gpr_value_list[] = { 0x33221100, 0x77665544, 0xbbaa9988, 0xffeeddcc, 0x12345678, 0x5AA555AA, 0x98765432, 0xFF0000FF,
                                           0x65472135, 0x9ABCDEF0, 0x887799AA, 0xCCDDEEFF, 0x87654321, 0x778899AA, 0x65879412, 0x11111111 };

int main(void)
{
    uint32_t bgpr_val;
    bool rlt_ok = true;

    board_init();
    board_init_led_pins();

    printf("bgpr retention example\n");
    board_led_write(board_get_led_gpio_off_level());

    if (!bpor_is_reg_value_retention_enable(HPM_BPOR)) {
        bpor_enable_reg_value_retention(HPM_BPOR);
        for (uint32_t i = 0; i < sizeof(BGPR_Type) / sizeof(uint32_t); i++) {
            bgpr_write32(BOARD_BGPR, i, gpr_value_list[i]);
        }
        printf("bgpr retention enable and data initialized, please keep vbat and power down!\n");
    } else {
        for (uint32_t i = 0; i < sizeof(BGPR_Type) / sizeof(uint32_t); i++) {
            bgpr_read32(BOARD_BGPR, i, &bgpr_val);
            if (bgpr_val != gpr_value_list[i]) {
                rlt_ok = false;
                printf("bgpr data retention failure, index: %d!\n", i);
            }
        }

        if (rlt_ok) {
            board_led_write(!board_get_led_gpio_off_level());
            printf("bgpr data retention success!\n");
        } else {
            for (uint32_t i = 0; i < sizeof(BGPR_Type) / sizeof(uint32_t); i++) {
                bgpr_write32(BOARD_BGPR, i, gpr_value_list[i]);
            }
            printf("Because bgpr data compare failure, so re-initialize bgpr data, please keep vbat and power down again!\n");
        }
    }

    while (1) {
        ;
    }
    return 0;
}
