/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_soc_feature.h"
#include "hpm_trgm_drv.h"
#include "hpm_qeiv2_drv.h"
#include "hpm_enc_pos_drv.h"

#ifndef APP_QEI_BASE
#define APP_QEI_BASE BOARD_BLDC_QEIV2_BASE
#endif

#ifndef APP_USE_STRUCT_CONFIG
#define APP_USE_STRUCT_CONFIG 1
#endif


/* API for extern */

/* Static function declaration */
static void qeiv2_init(void);

/* Function definition */
int main(void)
{
    uint32_t pos;

    board_init();
    init_qeiv2_uvw_pins(APP_QEI_BASE);
    qeiv2_init();

    printf("qeiv2 uvw encoder example\n");

    for (uint32_t i = 0; i < 100; i++) {
        pos = qeiv2_get_postion(APP_QEI_BASE);
        printf("pos:%#x\n", pos);
    }

    while (1) {
        ;
    }
    return 0;
}


static void qeiv2_init(void)
{
    qeiv2_uvw_config_t *ptr;

    qeiv2_reset_counter(APP_QEI_BASE);

    qeiv2_set_work_mode(APP_QEI_BASE, qeiv2_work_mode_uvw);
    qeiv2_config_abz_uvw_signal_edge(APP_QEI_BASE, true, true, true, true, true);

#if defined(APP_USE_STRUCT_CONFIG) && (APP_USE_STRUCT_CONFIG == 1)
    qeiv2_get_uvw_position_defconfig(ptr);
    ptr->pos_opt = qeiv2_uvw_pos_opt_next;
    ptr->u_pos_sel[0] = qeiv2_uvw_pos_sel_high;
    ptr->v_pos_sel[0] = qeiv2_uvw_pos_sel_low;
    ptr->w_pos_sel[0] = qeiv2_uvw_pos_sel_high;

    ptr->u_pos_sel[1] = qeiv2_uvw_pos_sel_high;
    ptr->v_pos_sel[1] = qeiv2_uvw_pos_sel_low;
    ptr->w_pos_sel[1] = qeiv2_uvw_pos_sel_low;

    ptr->u_pos_sel[2] = qeiv2_uvw_pos_sel_high;
    ptr->v_pos_sel[2] = qeiv2_uvw_pos_sel_high;
    ptr->w_pos_sel[2] = qeiv2_uvw_pos_sel_low;

    ptr->u_pos_sel[3] = qeiv2_uvw_pos_sel_low;
    ptr->v_pos_sel[3] = qeiv2_uvw_pos_sel_high;
    ptr->w_pos_sel[3] = qeiv2_uvw_pos_sel_low;

    ptr->u_pos_sel[4] = qeiv2_uvw_pos_sel_low;
    ptr->v_pos_sel[4] = qeiv2_uvw_pos_sel_high;
    ptr->w_pos_sel[4] = qeiv2_uvw_pos_sel_high;

    ptr->u_pos_sel[5] = qeiv2_uvw_pos_sel_low;
    ptr->v_pos_sel[5] = qeiv2_uvw_pos_sel_low;
    ptr->w_pos_sel[5] = qeiv2_uvw_pos_sel_high;

    ptr->pos_cfg[0] = encoder_deg_to_position(30);
    ptr->pos_cfg[1] = encoder_deg_to_position(90);
    ptr->pos_cfg[2] = encoder_deg_to_position(150);
    ptr->pos_cfg[3] = encoder_deg_to_position(210);
    ptr->pos_cfg[4] = encoder_deg_to_position(270);
    ptr->pos_cfg[5] = encoder_deg_to_position(330);
    (void)qeiv2_config_uvw_position(APP_QEI_BASE, ptr);
#else
    qeiv2_set_uvw_position_opt(APP_QEI_BASE, qeiv2_uvw_pos_opt_next);
    qeiv2_set_uvw_position_sel(APP_QEI_BASE, qeiv2_uvw_pos0, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, true);
    qeiv2_set_uvw_position_sel(APP_QEI_BASE, qeiv2_uvw_pos1, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, true);
    qeiv2_set_uvw_position_sel(APP_QEI_BASE, qeiv2_uvw_pos2, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, true);
    qeiv2_set_uvw_position_sel(APP_QEI_BASE, qeiv2_uvw_pos3, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, true);
    qeiv2_set_uvw_position_sel(APP_QEI_BASE, qeiv2_uvw_pos4, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, true);
    qeiv2_set_uvw_position_sel(APP_QEI_BASE, qeiv2_uvw_pos5, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, QEIV2_UVW_POS_OPT_NEX_SEL_LOW, QEIV2_UVW_POS_OPT_NEX_SEL_HIGH, true);

    qeiv2_set_uvw_position(APP_QEI_BASE, qeiv2_uvw_pos0, encoder_deg_to_position(30));  /* 30 deg */
    qeiv2_set_uvw_position(APP_QEI_BASE, qeiv2_uvw_pos1, encoder_deg_to_position(90));  /* 90 deg */
    qeiv2_set_uvw_position(APP_QEI_BASE, qeiv2_uvw_pos2, encoder_deg_to_position(150));  /* 150 deg */
    qeiv2_set_uvw_position(APP_QEI_BASE, qeiv2_uvw_pos3, encoder_deg_to_position(210));  /* 210 deg */
    qeiv2_set_uvw_position(APP_QEI_BASE, qeiv2_uvw_pos4, encoder_deg_to_position(270));  /* 270 deg */
    qeiv2_set_uvw_position(APP_QEI_BASE, qeiv2_uvw_pos5, encoder_deg_to_position(330));  /* 330 deg */
#endif

    qeiv2_release_counter(APP_QEI_BASE);
}
