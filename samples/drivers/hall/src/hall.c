/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_hall_drv.h"
#include "hpm_trgm_drv.h"

#ifndef BOARD_APP_HALL_BASE
#ifdef BOARD_BLDC_HALL_BASE
#define BOARD_APP_HALL_BASE BOARD_BLDC_HALL_BASE
#define BOARD_APP_HALL_TRGM BOARD_BLDC_HALL_TRGM
#define BOARD_APP_HALL_IRQ BOARD_BLDC_HALL_IRQ
#define BOARD_APP_HALL_TRGM_HALL_U_SRC BOARD_BLDC_HALL_TRGM_HALL_U_SRC
#define BOARD_APP_HALL_TRGM_HALL_V_SRC BOARD_BLDC_HALL_TRGM_HALL_V_SRC
#define BOARD_APP_HALL_TRGM_HALL_W_SRC BOARD_BLDC_HALL_TRGM_HALL_W_SRC
#define BOARD_APP_HALL_MOTOR_PHASE_COUNT_PER_REV (1000U)
#else
#define BOARD_APP_HALL_BASE HPM_HALL2
#define BOARD_APP_HALL_TRGM HPM_TRGM2
#define BOARD_APP_HALL_IRQ IRQn_HALL2
#define BOARD_APP_HALL_TRGM_HALL_U_SRC HPM_TRGM2_INPUT_SRC_TRGM2_P6
#define BOARD_APP_HALL_TRGM_HALL_V_SRC HPM_TRGM2_INPUT_SRC_TRGM2_P7
#define BOARD_APP_HALL_TRGM_HALL_W_SRC HPM_TRGM2_INPUT_SRC_TRGM2_P8
#define BOARD_APP_HALL_MOTOR_PHASE_COUNT_PER_REV (1000U)
#endif
#endif

volatile uint32_t u, v, w, tmr;
volatile bool data_ready = false;

SDK_DECLARE_EXT_ISR_M(BOARD_APP_HALL_IRQ, isr_hall)
void isr_hall(void)
{
    hall_clear_status(BOARD_APP_HALL_BASE, hall_get_status(BOARD_APP_HALL_BASE));
    data_ready = true;
}

int main(void)
{
    board_init();
    init_hall_trgm_pins();

    trgm_output_t config = {0};
    config.invert = false;

    config.input = BOARD_APP_HALL_TRGM_HALL_U_SRC;
    trgm_output_config(BOARD_APP_HALL_TRGM, TRGM_TRGOCFG_HALL_U, &config);

    config.input = BOARD_APP_HALL_TRGM_HALL_V_SRC;
    trgm_output_config(BOARD_APP_HALL_TRGM, TRGM_TRGOCFG_HALL_V, &config);

    config.input = BOARD_APP_HALL_TRGM_HALL_W_SRC;
    trgm_output_config(BOARD_APP_HALL_TRGM, TRGM_TRGOCFG_HALL_W, &config);

    intc_m_enable_irq_with_priority(BOARD_APP_HALL_IRQ, 1);

    hall_counter_reset_assert(BOARD_APP_HALL_BASE);
    hall_phase_config(BOARD_APP_HALL_BASE, 1000, hall_count_delay_start_after_uvw_toggle);
    hall_load_read_trigger_event_enable(BOARD_APP_HALL_BASE,
            HALL_EVENT_U_FLAG_MASK);
    hall_irq_enable(BOARD_APP_HALL_BASE, HALL_EVENT_U_FLAG_MASK);
    hall_counter_reset_release(BOARD_APP_HALL_BASE);

    printf("hall example\n");
    while(1) {
        while(!data_ready);
        data_ready = false;
        /* the following u,v,w count value read out on read event at u signal toggle */
        u = hall_get_count_on_read_event(BOARD_APP_HALL_BASE, hall_counter_type_u);
        v = hall_get_count_on_read_event(BOARD_APP_HALL_BASE, hall_counter_type_v);
        w = hall_get_count_on_read_event(BOARD_APP_HALL_BASE, hall_counter_type_w);
        tmr = hall_get_count_on_read_event(BOARD_APP_HALL_BASE, hall_counter_type_timer);

        printf("u: 0x%x\n", u);
        printf("v: 0x%x\n", v);
        printf("w: 0x%x\n", w);
        printf("timer: 0x%x\n", tmr);
    }
    return 0;
}
