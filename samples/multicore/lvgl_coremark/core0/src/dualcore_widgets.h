/**
 * @file lv_demo_widgets.h
 *
 */

#ifndef DUALCORE_WIDGETS_H
#define DUALCORE_WIDGETS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
#define COREMARK_MAIN main

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t core_freq[2];
    uint32_t bus_freq[2];
    uint32_t coremark[2];
    float coremarkmhz[2];
    bool result_ready[2];
} lv_coremark_ctx_t;

#define COREMARK_MAIN_ENTRY (SEC_CORE_IMG_START)
#define COREMARK_BIN_ARRAY ((void *)SEC_CORE_IMG_START)

extern lv_coremark_ctx_t g_lv_cm_ctx;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_dualcore_coremark_demo(void);

void init_coremark_result(lv_coremark_ctx_t *cm_ctx);

void load_coremark_bin_for_cpu(void);

void init_coremark_context(void);

void refresh_coremark_info(void);

void run_coremark_for_cpu(uint32_t index);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*DUALCORE_WIDGETS_H */
