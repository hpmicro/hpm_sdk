/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_qei_drv.h"
#include "hpm_trgm_drv.h"

#ifndef BOARD_APP_QEI_BASE
#define BOARD_APP_QEI_BASE BOARD_BLDC_QEI_BASE
#define BOARD_APP_QEI_IRQ  BOARD_BLDC_QEI_IRQ
#define BOARD_APP_QEI_TRGM BOARD_BLDC_QEI_TRGM
#define BOARD_APP_QEI_TRGM_QEI_A_SRC BOARD_BLDC_QEI_TRGM_QEI_A_SRC
#define BOARD_APP_QEI_TRGM_QEI_B_SRC BOARD_BLDC_QEI_TRGM_QEI_B_SRC
#define BOARD_APP_QEI_MOTOR_PHASE_COUNT_PER_REV BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV
#endif

volatile uint32_t z, ph, spd, tmr;
volatile bool data_ready = false;

SDK_DECLARE_EXT_ISR_M(BOARD_APP_QEI_IRQ, isr_qei)
void isr_qei(void)
{
    qei_clear_status(BOARD_APP_QEI_BASE, qei_get_status(BOARD_APP_QEI_BASE));
    data_ready = true;
}

int main(void)
{
    trgm_output_t trgm_config = {0};
    qei_mode_config_t mode_config = {0};

    board_init();
    init_qei_trgm_pins();

    /* trgm config */
    trgm_config.invert = false;
    trgm_config.input = BOARD_APP_QEI_TRGM_QEI_A_SRC;
    trgm_output_config(BOARD_APP_QEI_TRGM, TRGM_TRGOCFG_QEI_A, &trgm_config);
    trgm_config.input = BOARD_APP_QEI_TRGM_QEI_B_SRC;
    trgm_output_config(BOARD_APP_QEI_TRGM, TRGM_TRGOCFG_QEI_B, &trgm_config);

    /* mode config */
    mode_config.work_mode = qei_work_mode_abz;
    mode_config.z_count_inc_mode = qei_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = BOARD_APP_QEI_MOTOR_PHASE_COUNT_PER_REV;
    mode_config.z_cali_enable = false;
    mode_config.phcnt_idx = 0;
    qei_config_mode(BOARD_APP_QEI_BASE, &mode_config);

    /* cmp config */
    qei_phase_cmp_set(BOARD_APP_QEI_BASE, BOARD_APP_QEI_MOTOR_PHASE_COUNT_PER_REV / 3,
            false, qei_rotation_dir_cmp_ignore);
    qei_load_read_trigger_event_enable(BOARD_APP_QEI_BASE, QEI_EVENT_POSITION_COMPARE_FLAG_MASK);

    /* irq config */
    qei_irq_enable(BOARD_APP_QEI_BASE, QEI_EVENT_POSITION_COMPARE_FLAG_MASK);
    intc_m_enable_irq_with_priority(BOARD_APP_QEI_IRQ, 1);

    printf("qei example\n");
    while(1) {
        while(!data_ready);
        data_ready = false;

        z = qei_get_count_on_read_event(BOARD_APP_QEI_BASE, qei_counter_type_z);
        ph = qei_get_count_on_read_event(BOARD_APP_QEI_BASE, qei_counter_type_phase);
        spd = qei_get_speed_history(BOARD_APP_QEI_BASE, qei_speed_his0) +
            qei_get_speed_history(BOARD_APP_QEI_BASE, qei_speed_his1) +
            qei_get_speed_history(BOARD_APP_QEI_BASE, qei_speed_his2) +
            qei_get_speed_history(BOARD_APP_QEI_BASE, qei_speed_his3);
        tmr = qei_get_count_on_read_event(BOARD_APP_QEI_BASE, qei_counter_type_timer);

        printf("z: 0x%x\n", z);
        printf("phase: 0x%x\n", ph);
        printf("speed: 0x%x\n", spd);
        printf("timer: 0x%x\n", tmr);
    }
    return 0;
}
