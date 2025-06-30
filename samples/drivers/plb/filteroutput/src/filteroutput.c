/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_filter.h"
#include "hpm_trgm_drv.h"

#define FILTER_TIME_MS (50)

int main(void)
{
    char option;
    filter_mode_t mode;
    filter_config_t filter_cfg;
    const char menu_str[] =
        "\r\nPlease Select Filter Mode:\r\n"
        "  0 - Filter rapid change mode\r\n"
        "  1 - Filter delay mode\r\n"
        "  2 - Filter stable low mode\r\n"
        "  3 - Filter stable high mode\r\n\r\n";

    board_init();

    printf("PLB Filiter Demo.\r\n");
    printf(menu_str);

    init_plb_filter_pins();
    trgm_enable_io_output(HPM_TRGM0, 1 << BOARD_PLB_FILTER_IO_TRG_SHIFT);

    while (1) {
        option = getchar();
        if (('0' <= option) && (option <= '3')) {
            mode = option - '0';

            switch (mode) {
            case filter_mode_delay:
                printf("  1 - Filter delay mode\r\n");
                break;

            case filter_mode_stable_low:
                printf("  2 - Filter stable low mode\r\n");
                break;

            case filter_mode_stable_high:
                printf("  3 - Filter stable high mode\r\n");
                break;

            case filter_mode_rapid_change:
            default:
                printf("  0 - Filter rapid change mode\r\n");
                break;
            }

            printf("  You can press 0 - 3 to change filter mode...\r\n\r\n");

            filter_cfg.plb = HPM_PLB;
            filter_cfg.trgm = BOARD_PLB_TRGM;
            filter_cfg.mode = mode;
            filter_cfg.plb_type_a_chn = plb_chn0;
            filter_cfg.plb_type_b_chn = plb_chn0;
            filter_cfg.filter_len = FILTER_TIME_MS * (clock_get_frequency(BOARD_PLB_CLOCK_NAME) / 1000);
            filter_cfg.input_trgmux_pin = BOARD_PLB_FILTER_SIG_INPUT_SOURCE;
            filter_cfg.output_trgmux_pin = BOARD_PLB_FILTER_SIG_OUTUPT_SOURCE;
            plb_filter_init(&filter_cfg);
        } else {
            printf("Invalid input, please try again.\r\n");
        }
    }

    return 0;
}
