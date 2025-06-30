/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_gptmr_drv.h"

#define APP_BOARD_GPTMR_QEI               BOARD_GPTMR_QEI
#define APP_BOARD_GPTMR_QEI_CLOCK         BOARD_GPTMR_QEI_CLK_NAME
#define APP_BOARD_GPTMR_QEI_CH_GROUP      BOARD_GPTMR_QEI_CH_GROUP
#define APP_BOARD_GPTMR_QEI_PHMAX         BOARD_GPTMR_QEI_PHMAX

#define APP_BOARD_GPTMR                   BOARD_GPTMR
#define APP_BOARD_GPTMR_IRQ               BOARD_GPTMR_IRQ
#define APP_BOARD_GPTMR_CLOCK             BOARD_GPTMR_CLK_NAME
#define APP_BOARD_GPTMR_CH                BOARD_GPTMR_CHANNEL
#define APP_TICK_MS                      (1000)

typedef struct {
    gptmr_qei_type_t num;
    char *num_str;
} test_number_t;

static void show_help(void);
static void base_timer_config(void);

const test_number_t test_table[] = {
    {gptmr_qei_ud_mode,  "* 0 - ud mode                                                 *\n"},
    {gptmr_qei_ab_mode,  "* 1 - ab mode                                                 *\n"},
    {gptmr_qei_pd_mode,  "* 2 - pd mode                                                 *\n"},
};

SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr);
void tick_ms_isr(void)
{
    if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH))) {
        gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
        printf("qei_phcnt: %d\n", gptmr_get_qei_phcnt(APP_BOARD_GPTMR_QEI, APP_BOARD_GPTMR_QEI_CH_GROUP));
    }
}

int main(void)
{
    uint8_t i;
    uint8_t table_count = sizeof(test_table) / sizeof(test_number_t);
    char option;
    uint8_t num;
    gptmr_qei_config_t qei_config;

    board_init();
    board_init_gptmr_clock(APP_BOARD_GPTMR);
    board_init_gptmr_clock(APP_BOARD_GPTMR_QEI);
    board_init_gptmr_channel_pin(APP_BOARD_GPTMR_QEI, APP_BOARD_GPTMR_QEI_CH_GROUP, false);
    board_init_gptmr_channel_pin(APP_BOARD_GPTMR_QEI, APP_BOARD_GPTMR_QEI_CH_GROUP + 1, false);
    show_help();
    base_timer_config();
    while (1) {
        option = getchar();
        num = (uint8_t)(option - '0');
        for (i = 0; i < table_count; i++) {
            if (num == test_table[i].num) {
                printf("%s", test_table[num].num_str);
                qei_config.type = (gptmr_qei_type_t)num;
                qei_config.ch_group = APP_BOARD_GPTMR_QEI_CH_GROUP;
                qei_config.phmax = APP_BOARD_GPTMR_QEI_PHMAX;
                gptmr_config_qei(APP_BOARD_GPTMR_QEI, &qei_config);
                gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
                break;
            }
        }
        if (i >= table_count) {
            gptmr_stop_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            gptmr_channel_reset_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            show_help();
        }
    }
    return 0;
}

static void base_timer_config(void)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
    gptmr_freq = clock_get_frequency(APP_BOARD_GPTMR_CLOCK);
    config.reload = gptmr_freq / 1000 * APP_TICK_MS;
    gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);

    gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
    intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
}

static void show_help(void)
{
    static const char help_info[] = "\n"
                                    "---------------------------------------------------------------\n"
                                    "*                                                             *\n"
                                    "*                       GPTMR QEI DEMO                        *\n"
                                    "*                                                             *\n";

    static const char help_info_ending[] = "*-------------------------------------------------------------*\n";

    printf("%s", help_info);
    for (uint32_t i = 0; i < sizeof(test_table) / sizeof(test_number_t); i++) {
        printf("%s", test_table[i].num_str);
    }
    printf("%s", help_info_ending);
}
