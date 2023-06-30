/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef _LV_DRAW_HPMICRO_PDMA_H_
#define _LV_DRAW_HPMICRO_PDMA_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_HPMICRO_PDMA
#include "../../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_draw_sw_ctx_t lv_draw_hpmicro_pdma_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_hpmicro_pdma_ctx_init(struct _lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx);

void lv_draw_hpmicro_pdma_ctx_deinit(struct _lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_GPU_HPMICRO_PDMA */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_LV_DRAW_HPMICRO_PDMA_H_*/
