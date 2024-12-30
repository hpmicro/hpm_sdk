/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <lvgl.h>
#include <demos/lv_demos.h>


typedef struct demo_info {
    char *name;
    void (*entry)(void);
} demo_info_t;

static demo_info_t demo_infos[] = {
    {"stress", lv_demo_stress},
    {"benchmark", lv_demo_benchmark},
    {"music", lv_demo_music},
    {"widgets", lv_demo_widgets},
    {"flex_layout", lv_demo_flex_layout},
};

static lv_color_t def_color;

static void hpm_lvgl_add_logo(void)
{
    LV_IMAGE_DECLARE(hpmlogo);
    lv_obj_t *screen;
    int screen_h;
    int screen_w;
    int logo_size;
    lv_obj_t *obj_logo;

    screen = lv_screen_active();
    screen_h = lv_obj_get_height(screen);
    screen_w = lv_obj_get_width(screen);

    if (screen_h < screen_w)
        logo_size = screen_h * 30 / 100;
    else
        logo_size = screen_w * 30 / 100;

    obj_logo = lv_image_create(lv_screen_active());

    lv_obj_set_size(obj_logo, logo_size, logo_size);
    lv_image_set_src(obj_logo, &hpmlogo);
    lv_obj_set_align(obj_logo, LV_ALIGN_TOP_LEFT);
    lv_image_set_inner_align(obj_logo, LV_IMAGE_ALIGN_STRETCH);

    def_color = lv_obj_get_style_bg_color(screen, LV_PART_MAIN);
    lv_obj_set_style_bg_color(screen, lv_color_black(), LV_PART_MAIN);
}

static void btn_event_cb(lv_event_t *e)
{
    demo_info_t *demo_info;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) {
        demo_info = lv_obj_get_user_data(btn);
        lv_obj_clean(lv_screen_active());
        lv_obj_set_style_bg_color(lv_screen_active(), def_color, LV_PART_MAIN);
        demo_info->entry();
    }
}

static void hpm_lvgl_add_button(void)
{
    lv_obj_t *obj_button;
    lv_obj_t *obj_text;
    lv_obj_t *obj_grid;
    static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_style_t style_btn;

    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, lv_color_hex(0x0));
    lv_style_set_bg_opa(&style_btn, LV_OPA_TRANSP);
    lv_style_set_border_width(&style_btn, 2);
    lv_style_set_border_color(&style_btn, lv_color_hex(0xFFFFFF));
    lv_style_set_radius(&style_btn, LV_RADIUS_CIRCLE);

    obj_grid = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj_grid);
    lv_obj_set_layout(obj_grid, LV_LAYOUT_GRID);
    lv_obj_set_size(obj_grid, lv_pct(100), lv_pct(30));
    lv_obj_center(obj_grid);
    lv_obj_set_grid_dsc_array(obj_grid, col_dsc, row_dsc);

    for (int i = 0; i < sizeof(demo_infos) / sizeof(demo_infos[0]); i++) {
        obj_button = lv_button_create(obj_grid);
        lv_obj_set_user_data(obj_button, &demo_infos[i]);
        lv_obj_add_style(obj_button, &style_btn, 0);
        lv_obj_add_event_cb(obj_button, btn_event_cb, LV_EVENT_ALL, NULL);
        lv_obj_set_grid_cell(obj_button, LV_GRID_ALIGN_CENTER, i, 1,
                             LV_GRID_ALIGN_CENTER, 0, 1);

        obj_text = lv_label_create(obj_button);
        lv_label_set_text(obj_text, demo_infos[i].name);
        lv_obj_set_size(obj_text, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_center(obj_text);
    }
}

void hpm_lvgl_demos(void)
{
    hpm_lvgl_add_logo();
    hpm_lvgl_add_button();
}
