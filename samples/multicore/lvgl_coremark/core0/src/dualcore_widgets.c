/**
 * @file lv_demo_widgets.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hpm_common.h"
#include "hpm_soc.h"
#include "board.h"
#include "hpm_l1c_drv.h"
#include "hpm_pllctl_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_pcfg_drv.h"
#include "coremark.h"
#include "demos/lv_demos.h"
#include "dualcore_widgets.h"

extern coremark_context_t g_coremark_ctx[2];
coremark_context_t *g_cm_ctx[2] = { (coremark_context_t *)&g_coremark_ctx[0], (coremark_context_t *)&g_coremark_ctx[1] };

bool start_coremark;

uint32_t countdown;

static uint32_t freq_idx;

typedef struct {
    uint32_t freq;
    uint32_t voltage;
} clock_setting_t;

static const clock_setting_t k_clock_setting_list[] = {{ 816000000UL, 1275U }, { 720000000UL, 1200U }, { 648000000UL, 1150U }};

#if LV_USE_DEMO_WIDGETS

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    DISP_SMALL, DISP_MEDIUM, DISP_LARGE,
} disp_size_t;

typedef struct {
    lv_obj_t *core_freq_tips_label[2];
    lv_obj_t *core_freq_result_label[2];
    lv_obj_t *bus_freq_tips_label[2];
    lv_obj_t *bus_freq_result_label[2];
    lv_obj_t *coremark_tips_label[2];
    lv_obj_t *coremark_result_label[2];
    lv_obj_t *coremarkmhz_tips_label[2];
    lv_obj_t *coremarkmhz_result_label[2];
    lv_obj_t *run_btn;
    lv_obj_t *run_btn_label;
    lv_obj_t *freqswitch_btn;
    lv_obj_t *freqswitch_btn_label;
} ui_component_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void profile_create(lv_obj_t *parent, ui_component_t *ui);

/**********************
 *  STATIC VARIABLES
 **********************/
static disp_size_t disp_size;

static lv_obj_t *tv;

static lv_style_t style_text_muted;

static lv_style_t style_title;

static lv_style_t style_icon;

static lv_style_t style_bullet;

static const lv_font_t *font_large;

static const lv_font_t *font_normal;

static ui_component_t s_ui;

static void run_button_click_cb(lv_event_t *e);

static void freqswitch_button_click_cb(lv_event_t *e);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_dualcore_coremark_demo(void)
{
    if (LV_HOR_RES <= 320) {
        disp_size = DISP_SMALL;
    } else if (LV_HOR_RES < 720) {
        disp_size = DISP_MEDIUM;
    } else {
        disp_size = DISP_LARGE;
    }

    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

    int32_t tab_h;
    if (disp_size == DISP_LARGE) {
        tab_h = 50;
#if LV_FONT_MONTSERRAT_20
        font_large     =  &lv_font_montserrat_20;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_20 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_16
        font_normal    =  &lv_font_montserrat_16;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_16 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    } else if (disp_size == DISP_MEDIUM) {
        tab_h = 45;
#if LV_FONT_MONTSERRAT_20
        font_large     =  &lv_font_montserrat_20;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_20 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_14
        font_normal = &lv_font_montserrat_14;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_14 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    } else { /* disp_size == DISP_SMALL */
        tab_h = 45;
#if LV_FONT_MONTSERRAT_18
        font_large     =  &lv_font_montserrat_18;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_18 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_12
        font_normal    =  &lv_font_montserrat_12;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_12 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }

#if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL,
                          lv_palette_main(LV_PALETTE_BLUE),
                          lv_palette_main(LV_PALETTE_RED),
                          LV_THEME_DEFAULT_DARK,
                          font_normal);
#endif

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

    tv = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tv, tab_h);

    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);

    if (disp_size == DISP_LARGE) {
        lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tv);
        lv_obj_set_style_pad_left(tab_btns, LV_HOR_RES / 2, 0);
        lv_obj_t *logo = lv_image_create(tab_btns);
        lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
        LV_IMAGE_DECLARE(img_lvgl_logo);
        lv_image_set_src(logo, &img_lvgl_logo);
        lv_obj_align(logo, LV_ALIGN_TOP_LEFT, -LV_HOR_RES / 2 + 25, 0);

        lv_obj_t *label = lv_label_create(tab_btns);
        lv_obj_add_style(label, &style_title, 0);
        lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_label_set_text(label, "HPMicro");
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

        label = lv_label_create(tab_btns);
        lv_label_set_text(label, "Coremark demo");
        lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_add_style(label, &style_text_muted, 0);
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
    }

    lv_obj_t *t1 = lv_tabview_add_tab(tv, "CoreMark");
    profile_create(t1, &s_ui);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void profile_create(lv_obj_t *parent, ui_component_t *ui)
{
    char str_buf[100];
    uint32_t cpu_freq_in_hz;

    /* Top panel */
    lv_obj_t *panel1 = lv_obj_create(parent);
    lv_obj_set_height(panel1, LV_SIZE_CONTENT);

    LV_IMAGE_DECLARE(hpmlogo_180x120);
    lv_obj_t *avatar = lv_image_create(panel1);
    lv_image_set_src(avatar, &hpmlogo_180x120);

    lv_obj_t *name = lv_label_create(panel1);
    lv_label_set_text(name, "HPMICRO Semiconductor Co, Ltd.");
    lv_obj_add_style(name, &style_title, 0);

    lv_obj_t *dsc = lv_label_create(panel1);
    lv_obj_add_style(dsc, &style_text_muted, 0);
    lv_label_set_text(dsc, "An innovative embedded system solution provider");
    lv_label_set_long_mode(dsc, LV_LABEL_LONG_WRAP);

    lv_obj_t *email_icn = lv_label_create(panel1);
    lv_obj_add_style(email_icn, &style_icon, 0);
    lv_label_set_text(email_icn, LV_SYMBOL_ENVELOPE);

    lv_obj_t *email_label = lv_label_create(panel1);
    lv_label_set_text(email_label, "info@hpmicro.com");

    lv_obj_t *call_icn = lv_label_create(panel1);
    lv_obj_add_style(call_icn, &style_icon, 0);
    lv_label_set_text(call_icn, LV_SYMBOL_CALL);

    lv_obj_t *call_label = lv_label_create(panel1);
    lv_label_set_text(call_label, "+86-21-58993108");

    ui->run_btn = lv_button_create(panel1);
    lv_obj_set_height(ui->run_btn, LV_SIZE_CONTENT);
    lv_obj_add_event_cb(ui->run_btn, run_button_click_cb, LV_EVENT_CLICKED, NULL);

    ui->run_btn_label = lv_label_create(ui->run_btn);
    lv_label_set_text(ui->run_btn_label, "Start");
    lv_obj_center(ui->run_btn_label);

    ui->freqswitch_btn = lv_button_create(panel1);
    lv_obj_set_height(ui->freqswitch_btn, LV_SIZE_CONTENT);
    lv_obj_add_event_cb(ui->freqswitch_btn, freqswitch_button_click_cb, LV_EVENT_CLICKED, NULL);

    ui->freqswitch_btn_label = lv_label_create(ui->freqswitch_btn);

    cpu_freq_in_hz = clock_get_frequency(clock_cpu0);
    sprintf(str_buf, "%dMHz", cpu_freq_in_hz / 1000000U);
    lv_label_set_text(ui->freqswitch_btn_label, str_buf);
    lv_obj_center(ui->freqswitch_btn_label);

    /*Create the second panel*/
    lv_obj_t *panel2 = lv_obj_create(parent);
    lv_obj_set_height(panel2, LV_SIZE_CONTENT);

    lv_obj_t *panel2_title = lv_label_create(panel2);
    lv_label_set_text(panel2_title, "Core0 Info");
    lv_obj_add_style(panel2_title, &style_title, 0);

    ui->core_freq_tips_label[0] = lv_label_create(panel2);
    lv_label_set_text(ui->core_freq_tips_label[0], "Core Frequency");
    lv_obj_add_style(ui->core_freq_tips_label[0], &style_text_muted, 0);

    ui->core_freq_result_label[0] = lv_label_create(panel2);
    lv_label_set_text(ui->core_freq_result_label[0], "816MHz");
    lv_obj_add_style(ui->core_freq_result_label[0], &style_text_muted, 0);

    ui->bus_freq_tips_label[0] = lv_label_create(panel2);
    lv_label_set_text(ui->bus_freq_tips_label[0], "Bus Frequency");
    lv_obj_add_style(ui->bus_freq_tips_label[0], &style_text_muted, 0);

    ui->bus_freq_result_label[0] = lv_label_create(panel2);
    lv_label_set_text(ui->bus_freq_result_label[0], "200MHz");
    lv_obj_add_style(ui->bus_freq_result_label[0], &style_text_muted, 0);

    ui->coremark_tips_label[0] = lv_label_create(panel2);
    lv_label_set_text(ui->coremark_tips_label[0], "CoreMark");
    lv_obj_add_style(ui->coremark_tips_label[0], &style_text_muted, 0);


    ui->coremark_result_label[0] = lv_label_create(panel2);
    lv_label_set_text(ui->coremark_result_label[0], "--");
    lv_obj_add_style(ui->coremark_result_label[0], &style_text_muted, 0);


    ui->coremarkmhz_tips_label[0] = lv_label_create(panel2);
    lv_label_set_text(ui->coremarkmhz_tips_label[0], "CoreMark / MHz");
    lv_obj_add_style(ui->coremarkmhz_tips_label[0], &style_text_muted, 0);

    ui->coremarkmhz_result_label[0] = lv_label_create(panel2);
    lv_label_set_text(ui->coremarkmhz_result_label[0], "--");
    lv_obj_add_style(ui->coremarkmhz_result_label[0], &style_text_muted, 0);

    /*Create the third panel*/
    lv_obj_t *panel3 = lv_obj_create(parent);
    lv_obj_t *panel3_title = lv_label_create(panel3);
    lv_label_set_text(panel3_title, "Core1 Info");
    lv_obj_add_style(panel3_title, &style_title, 0);

    ui->core_freq_tips_label[1] = lv_label_create(panel3);
    lv_label_set_text(ui->core_freq_tips_label[1], "Core Frequency");
    lv_obj_add_style(ui->core_freq_tips_label[1], &style_text_muted, 0);

    ui->core_freq_result_label[1] = lv_label_create(panel3);
    lv_label_set_text(ui->core_freq_result_label[1], "816MHz");
    lv_obj_add_style(ui->core_freq_result_label[1], &style_text_muted, 0);

    ui->bus_freq_tips_label[1] = lv_label_create(panel3);
    lv_label_set_text(ui->bus_freq_tips_label[1], "Bus Frequency");
    lv_obj_add_style(ui->bus_freq_tips_label[1], &style_text_muted, 0);

    ui->bus_freq_result_label[1] = lv_label_create(panel3);
    lv_label_set_text(ui->bus_freq_result_label[1], "200MHz");
    lv_obj_add_style(ui->bus_freq_result_label[1], &style_text_muted, 0);

    ui->coremark_tips_label[1] = lv_label_create(panel3);
    lv_label_set_text(ui->coremark_tips_label[1], "CoreMark");
    lv_obj_add_style(ui->coremark_tips_label[1], &style_text_muted, 0);

    ui->coremark_result_label[1] = lv_label_create(panel3);
    lv_label_set_text(ui->coremark_result_label[1], "--");
    lv_obj_add_style(ui->coremark_result_label[1], &style_text_muted, 0);

    ui->coremarkmhz_tips_label[1] = lv_label_create(panel3);
    lv_label_set_text(ui->coremarkmhz_tips_label[1], "CoreMark / MHz");
    lv_obj_add_style(ui->coremarkmhz_tips_label[1], &style_text_muted, 0);

    ui->coremarkmhz_result_label[1] = lv_label_create(panel3);
    lv_label_set_text(ui->coremarkmhz_result_label[1], "--");
    lv_obj_add_style(ui->coremarkmhz_result_label[1], &style_text_muted, 0);

    if (disp_size == DISP_LARGE) {
        static int32_t grid_main_col_dsc[] = {
            LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST
        };
        static int32_t grid_main_row_dsc[] = {
            LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST
        };

        /*Create the top panel*/
        static int32_t grid_1_col_dsc[] = {
            LV_GRID_CONTENT, 5, LV_GRID_CONTENT, LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST
        };
        static int32_t grid_1_row_dsc[] = {
            LV_GRID_CONTENT, LV_GRID_CONTENT, 1, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST
        };

        static int32_t grid_2_col_dsc[] = {
            LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST
        };
        static int32_t grid_2_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            1,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            1,                /*Boxes*/
            1,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            1,                /*Boxes*/
            LV_GRID_TEMPLATE_LAST
        };


        lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

        lv_obj_set_grid_cell(panel1, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);

        lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);
        lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 5);
        lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 2, 4, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(call_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(call_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(ui->run_btn, LV_GRID_ALIGN_STRETCH, 4, 1, LV_GRID_ALIGN_CENTER, 3, 2);
        lv_obj_set_grid_cell(ui->freqswitch_btn, LV_GRID_ALIGN_STRETCH, 5, 1, LV_GRID_ALIGN_CENTER, 3, 2);

        lv_obj_set_grid_cell(panel2, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_dsc_array(panel2, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_cell(panel2_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(ui->core_freq_result_label[0], LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(ui->core_freq_tips_label[0], LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(ui->bus_freq_result_label[0], LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(ui->bus_freq_tips_label[0], LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(ui->coremarkmhz_result_label[0], LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);
        lv_obj_set_grid_cell(ui->coremarkmhz_tips_label[0], LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(ui->coremark_result_label[0], LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
        lv_obj_set_grid_cell(ui->coremark_tips_label[0], LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);


        lv_obj_set_grid_cell(panel3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
        lv_obj_set_grid_dsc_array(panel3, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_cell(panel3_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(ui->core_freq_result_label[1], LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(ui->core_freq_tips_label[1], LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(ui->bus_freq_result_label[1], LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(ui->bus_freq_tips_label[1], LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(ui->coremarkmhz_result_label[1], LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);
        lv_obj_set_grid_cell(ui->coremarkmhz_tips_label[1], LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(ui->coremark_result_label[1], LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
        lv_obj_set_grid_cell(ui->coremark_tips_label[1], LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);
    } else if (disp_size == DISP_MEDIUM) {
        static int32_t grid_main_col_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        static int32_t grid_main_row_dsc[] = { LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };


        /*Create the top panel*/
        static int32_t
            grid_1_col_dsc[] = { LV_GRID_CONTENT, 1, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        static int32_t grid_1_row_dsc[] = {
            LV_GRID_CONTENT, /*Name*/
            LV_GRID_CONTENT, /*Description*/
            LV_GRID_CONTENT, /*Email*/
            -20,
            LV_GRID_CONTENT, /*Phone*/
            LV_GRID_CONTENT, /*Buttons*/
            LV_GRID_TEMPLATE_LAST
        };

        static int32_t grid_2_col_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        static int32_t grid_2_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            5,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_TEMPLATE_LAST
        };


        lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);
        lv_obj_set_grid_cell(panel1, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);

        lv_obj_set_width(ui->run_btn, 120);
        lv_obj_set_width(ui->freqswitch_btn, 120);

        lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);
        lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 4);
        lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(call_icn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(call_label, LV_GRID_ALIGN_START, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(ui->run_btn, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);
        lv_obj_set_grid_cell(ui->freqswitch_btn, LV_GRID_ALIGN_END, 3, 1, LV_GRID_ALIGN_CENTER, 5, 1);

        lv_obj_set_grid_cell(panel2, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_dsc_array(panel2, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_cell(panel2_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(ui->core_freq_tips_label[0], LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(ui->core_freq_result_label[0], LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(ui->bus_freq_tips_label[0], LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(ui->bus_freq_result_label[0], LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(ui->coremarkmhz_result_label[0], LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 6, 1);
        lv_obj_set_grid_cell(ui->coremarkmhz_tips_label[0], LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 7, 1);
        lv_obj_set_grid_cell(ui->coremark_tips_label[0], LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 8, 1);
        lv_obj_set_grid_cell(ui->coremark_result_label[0], LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 9, 1);

        lv_obj_set_grid_cell(panel3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
        lv_obj_set_grid_dsc_array(panel3, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_cell(panel3_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    } else if (disp_size == DISP_SMALL) {
        static int32_t grid_main_col_dsc[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        static int32_t
            grid_main_row_dsc[] = { LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST };
        lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);


        /*Create the top panel*/
        static int32_t grid_1_col_dsc[] = { LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        static int32_t grid_1_row_dsc[] = {
            LV_GRID_CONTENT, /*Avatar*/
            LV_GRID_CONTENT, /*Name*/
            LV_GRID_CONTENT, /*Description*/
            LV_GRID_CONTENT, /*Email*/
            LV_GRID_CONTENT, /*Phone number*/
            LV_GRID_CONTENT, /*Button1*/
            LV_GRID_CONTENT, /*Button2*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);


        static int32_t grid_2_col_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
        static int32_t grid_2_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            5,                /*Separator*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40,               /*Box*/
            LV_GRID_CONTENT,  /*Box title*/
            40, LV_GRID_TEMPLATE_LAST               /*Box*/
        };

        lv_obj_set_grid_dsc_array(panel2, grid_2_col_dsc, grid_2_row_dsc);
        lv_obj_set_grid_dsc_array(panel3, grid_2_col_dsc, grid_2_row_dsc);

        lv_obj_set_grid_cell(panel1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

        lv_obj_set_style_text_align(dsc, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(name, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_CENTER, 1, 1);
        lv_obj_set_grid_cell(dsc, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(email_icn, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(email_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(call_icn, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(call_label, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(ui->run_btn, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 5, 1);
        lv_obj_set_grid_cell(ui->freqswitch_btn, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 6, 1);

        lv_obj_set_grid_cell(panel2, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(panel2_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(ui->core_freq_tips_label[0], LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(ui->core_freq_result_label[0], LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(ui->bus_freq_tips_label[0], LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(ui->bus_freq_result_label[0], LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 5, 1);
        lv_obj_set_grid_cell(ui->coremarkmhz_result_label[0], LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 6, 1);
        lv_obj_set_grid_cell(ui->coremarkmhz_tips_label[0], LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 7, 1);
        lv_obj_set_grid_cell(ui->coremark_tips_label[0], LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 8, 1);
        lv_obj_set_grid_cell(ui->coremark_result_label[0], LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 9, 1);

        lv_obj_set_height(panel3, LV_SIZE_CONTENT);
        lv_obj_set_grid_cell(panel3, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(panel3_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);
    }
}

void update_coremark_result(lv_coremark_ctx_t *cm_ctx)
{
    char str_buf[100];
    for (uint32_t i = 0; i < 2; i++) {
        clock_name_t cpu_clk_name = (i == 0) ? clock_cpu0 : clock_cpu1;
        uint32_t cpu_freq_in_hz = clock_get_frequency(cpu_clk_name);
        float core_voltage = (HPM_PCFG->DCDC_MODE & PCFG_DCDC_MODE_VOLT_MASK) / 1000.0;
        sprintf(str_buf, "%dMHz (@%.3fV)", cpu_freq_in_hz / 1000000U, core_voltage);
        lv_label_set_text(s_ui.core_freq_result_label[i], str_buf);
        sprintf(str_buf, "%dMHz", cm_ctx->bus_freq[i] / 1000000U);
        lv_label_set_text(s_ui.bus_freq_result_label[i], str_buf);
        if (cm_ctx->result_ready[i]) {
            sprintf(str_buf, "%d", cm_ctx->coremark[i]);
            lv_label_set_text(s_ui.coremark_result_label[i], str_buf);
            sprintf(str_buf, "%.2f", cm_ctx->coremarkmhz[i]);
            lv_label_set_text(s_ui.coremarkmhz_result_label[i], str_buf);
        } else {
            lv_label_set_text(s_ui.bus_freq_result_label[i], str_buf);
            lv_label_set_text(s_ui.coremark_result_label[i], "--");
            lv_label_set_text(s_ui.coremarkmhz_result_label[i], "--");
        }
    }
}

void run_button_click_cb(lv_event_t *e)
{
    (void)e;

    lv_obj_add_state(s_ui.run_btn, LV_STATE_DISABLED);
    lv_obj_add_state(s_ui.freqswitch_btn, LV_STATE_DISABLED);
    lv_label_set_text(s_ui.run_btn_label, "Running...");
    init_coremark_result(&g_lv_cm_ctx);
    init_coremark_context();
    start_coremark = true;
    countdown = 2;
}

#endif

void init_coremark_cpu_freq(void)
{
    pcfg_dcdc_set_voltage(HPM_PCFG, k_clock_setting_list[0].voltage);
    pllctl_init_int_pll_with_freq(HPM_PLLCTL, 0, k_clock_setting_list[0].freq);
    clock_update_core_clock();
}

void init_coremark_context(void)
{
    memset(&g_lv_cm_ctx, 0, sizeof(g_lv_cm_ctx));

    for (uint32_t i = 0; i < ARRAY_SIZE(g_cm_ctx); i++) {
        memset(g_cm_ctx[i], 0, sizeof(*g_cm_ctx[i]));
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(g_cm_ctx[i]);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t) g_cm_ctx[i] + sizeof(*g_cm_ctx[i]));
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_flush(aligned_start, aligned_size);
    }
    g_lv_cm_ctx.bus_freq[0] = clock_get_frequency(clock_axi0);
    g_lv_cm_ctx.bus_freq[1] = clock_get_frequency(clock_axi0);
}

void load_coremark_bin_for_cpu(void)
{
    extern const uint32_t sec_core_img_size;
    extern const uint8_t sec_core_img[];
    uint32_t core1_img_sys_addr = core_local_mem_to_sys_address(HPM_CORE1, (uint32_t) COREMARK_BIN_ARRAY);
    memcpy((void *) core1_img_sys_addr, sec_core_img, sec_core_img_size);
    printf("Loading core1 image to destination 0x%08x...\n", core1_img_sys_addr);
    l1c_dc_flush((uint32_t) core1_img_sys_addr, HPM_L1C_CACHELINE_ALIGN_UP(sec_core_img_size));
}

void refresh_coremark_info(void)
{
    static const char coremark_str_hdrs[] = "CoreMark 1.0 : ";
    char *str_start;
    uint32_t temp;
    uint32_t cpu_freq;
    uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(g_cm_ctx[1]);
    uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t) g_cm_ctx[1] + sizeof(*g_cm_ctx[1]));
    uint32_t aligned_size = aligned_end - aligned_start;
    l1c_dc_invalidate(aligned_start, aligned_size);

    for (uint32_t i = 0; i < 2; i++) {
        if (g_cm_ctx[i]->has_done) {
            g_lv_cm_ctx.result_ready[i] = true;
            str_start = strstr(g_cm_ctx[i]->s_buffer, coremark_str_hdrs);
            if (str_start != NULL) {
                str_start += strlen(coremark_str_hdrs);
                /* temp = strtoul(str_start, NULL, 0); */
                sscanf(str_start, "%d", &temp);
                clock_name_t cpu_clk_name = (i == 0) ? clock_cpu0 : clock_cpu1;
                cpu_freq = clock_get_frequency(cpu_clk_name);
                g_lv_cm_ctx.coremark[i] = temp;
                g_lv_cm_ctx.coremarkmhz[i] = g_lv_cm_ctx.coremark[i] * 1.0 / (cpu_freq / 1000000U);
            }
        }
    }
    if (g_cm_ctx[0]->has_done && g_cm_ctx[1]->has_done) {
        lv_obj_clear_state(s_ui.run_btn, LV_STATE_DISABLED);
        lv_obj_clear_state(s_ui.freqswitch_btn, LV_STATE_DISABLED);
        lv_label_set_text(s_ui.run_btn_label, "Start");
    }

    update_coremark_result(&g_lv_cm_ctx);

    if (countdown > 0) {
        countdown--;
        if (countdown == 0) {
            run_coremark_for_cpu(1);
            run_coremark_for_cpu(0);
        }
    }
}

void run_coremark_for_cpu(uint32_t index)
{
    if (index == 0) {
        extern void COREMARK_MAIN(void);
        disable_global_irq(CSR_MSTATUS_MIE_MASK);
        printf("Run Coremark on Core0...\n");
        COREMARK_MAIN();
        enable_global_irq(CSR_MSTATUS_MIE_MASK);
        g_lv_cm_ctx.result_ready[0] = false;
    } else if (index == 1) {
        if (HPM_SYSCTL->CPU[1].GPR[1] != 0xc1bef1a9) {
            load_coremark_bin_for_cpu();
            HPM_SYSCTL->CPU[1].GPR[0] = (uint32_t) COREMARK_BIN_ARRAY;
            HPM_SYSCTL->CPU[1].GPR[1] = 0xc1bef1a9;
            HPM_SYSCTL->CPU[1].LP &= ~SYSCTL_CPU_LP_HALT_MASK;
            printf("Released Core1!\n");
        } else {
            HPM_SYSCTL->CPU[1].GPR[3] = 1;
        }
        g_lv_cm_ctx.result_ready[1] = false;
        printf("Run Coremark on Core1...\n");
    } else {

    }
}

void freqswitch_button_click_cb(lv_event_t *e)
{
    (void)e;

    freq_idx++;
    if (freq_idx >= ARRAY_SIZE(k_clock_setting_list)) {
        freq_idx = 0;
    }

    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    pcfg_dcdc_set_voltage(HPM_PCFG, k_clock_setting_list[freq_idx].voltage);
    pllctl_init_int_pll_with_freq(HPM_PLLCTL, 0, k_clock_setting_list[freq_idx].freq);
    clock_update_core_clock();
    enable_global_irq(CSR_MSTATUS_MIE_MASK);

    char label_str[100];
    sprintf(label_str, "%dMHz", k_clock_setting_list[freq_idx].freq / 1000000U);
    lv_label_set_text(s_ui.freqswitch_btn_label, label_str);

    g_lv_cm_ctx.core_freq[0] = clock_get_frequency(clock_cpu0);
    g_lv_cm_ctx.core_freq[1] = clock_get_frequency(clock_cpu1);
    g_lv_cm_ctx.bus_freq[0] = clock_get_frequency(clock_axi0);
    g_lv_cm_ctx.bus_freq[1] = clock_get_frequency(clock_axi0);

    init_coremark_context();
}
