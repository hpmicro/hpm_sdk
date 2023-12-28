/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include <stdio.h>
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_qei_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_adc12_drv.h"

#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#include "hpm_lcdc_drv.h"
#include "hpm_mchtmr_drv.h"

#include "lvgl.h"
#include "lv_adapter.h"
#include <stdlib.h>

static lv_obj_t *obj_btnm_keyboard;
static lv_obj_t *obj_text_pos_speed;
static lv_obj_t *obj_switch_pos_speed;
static lv_obj_t *obj_main;
static lv_obj_t *obj_meter_pos;
static lv_meter_indicator_t * indic_meter_pos;
static lv_obj_t *obj_chart_speed;
static lv_chart_series_t * chart_ser_cur_speed;

extern int8_t fre_user_mode; /*0 : pos     1:  speed*/
extern int32_t fre_set_pos ;
extern float fre_setspeed;

void lv_meter_disp(void);
void lv_speed_disp(void);
void lv_logo_img(void);

/*键盘回调*/
static void event_handler_keyboard(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    const char * txt = lv_textarea_get_text(obj);
    lv_event_code_t code = lv_event_get_code(e);
    lv_textarea_t * ta = (lv_textarea_t *)obj;
    if(code == LV_EVENT_READY){/*按下回车，处理数据*/
        float disp_val;
        disp_val = atof(txt);
        if(fre_user_mode == 0){
            fre_set_pos = (int32_t)(-disp_val)*11.12; 
        }
        else{
            if(disp_val > 2800){
                disp_val = 2800;
                lv_label_set_text(ta->label, "2800");
            }
            else if(disp_val < -2800){
                disp_val = -2800;
                lv_label_set_text(ta->label, "-2800");
            }
            if((disp_val < 0.1)&&(disp_val >0)){
                disp_val = 0.1;
                lv_label_set_text(ta->label, "0.1");
            }
            else if((disp_val > -0.1)&&(disp_val <0)){
                disp_val = -0.1;
                lv_label_set_text(ta->label, "-0.1");
            }
            fre_setspeed = -disp_val/60;
        }
        
        printf("val:%f.\r\n",disp_val);
    }
    else if(code == LV_EVENT_VALUE_CHANGED){
    
        /*起始输入.变成0.*/
        if(txt[0] == '-' ){
            if(txt[1] == '.' ){
                lv_label_set_text(ta->label, "-0.");
                lv_textarea_set_cursor_pos(obj, 3);
            }
        }
        if(txt[0] == '.' ){
            lv_label_set_text(ta->label, "0.");
            lv_textarea_set_cursor_pos(obj, 2);
        }
        /*禁止两个..*/
        unsigned char pos = lv_textarea_get_cursor_pos(obj);
        if(txt[pos-1] == '.'){
            for(unsigned char i= 0; i< lv_textarea_get_max_length(obj); i++)
            {
                if((txt[i] == '.')&&(i != pos-1)){
                    lv_textarea_del_char(obj);
                }
                
            }
            
        }
    }
    
}
/*键盘重绘*/
static void event_handler_redraw_keyboard(lv_event_t *e)
{
  (void)e;
}
/*速度位置切换按钮回调*/
static void event_handler_switch_speed_pos(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    LV_UNUSED(obj);
    if (code == LV_EVENT_VALUE_CHANGED) {
        fre_user_mode = lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0;
        fre_setspeed = 0;
    }
}

void lv_ex_motor_speed(void)
{
    obj_main = lv_obj_create(lv_scr_act());
    lv_obj_t *obj = lv_obj_create(obj_main);

    lv_obj_set_size(obj_main, BOARD_LCD_WIDTH+20, BOARD_LCD_HEIGHT+50);
    lv_obj_center(obj_main);
    lv_obj_set_style_bg_color(obj_main,lv_palette_main(LV_PALETTE_GREY),0);
    lv_obj_clear_flag(obj_main, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(obj_main, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_set_size(obj, 770, 240);
    lv_obj_center(obj);
    lv_obj_set_style_bg_color(obj,lv_palette_darken(LV_PALETTE_GREY, 1),0);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_LEFT, 0, -15);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    /*按钮样式*/
    static lv_style_t style_btn_default;
    lv_style_init(&style_btn_default);
    lv_style_set_bg_color(&style_btn_default,lv_palette_darken(LV_PALETTE_GREY, 2));	//设置背景颜色

    /*矩阵键盘文本框*/
    obj_text_pos_speed = lv_textarea_create(obj);
    lv_obj_add_event_cb(obj_text_pos_speed, event_handler_keyboard, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(obj_text_pos_speed, event_handler_keyboard, LV_EVENT_READY, NULL);
    lv_textarea_set_accepted_chars(obj_text_pos_speed, "0123456789.-");
    lv_textarea_set_max_length(obj_text_pos_speed, 10);
    lv_textarea_set_one_line(obj_text_pos_speed, true);
    lv_textarea_set_text(obj_text_pos_speed, "0.0");
    lv_obj_align(obj_text_pos_speed, LV_ALIGN_TOP_LEFT, 300,-13);
    lv_obj_set_width(obj_text_pos_speed, 400);
    lv_obj_set_height(obj_text_pos_speed,45);

    /*键盘*/
    obj_btnm_keyboard = lv_keyboard_create(obj);
    lv_obj_set_size(obj_btnm_keyboard,  700, 180);
    lv_keyboard_set_mode(obj_btnm_keyboard, LV_KEYBOARD_MODE_NUMBER);
    lv_keyboard_set_textarea(obj_btnm_keyboard, obj_text_pos_speed);
    lv_obj_align(obj_btnm_keyboard, LV_ALIGN_TOP_MID, 0, 32);
    lv_obj_add_event_cb(obj_btnm_keyboard, event_handler_redraw_keyboard, LV_EVENT_DRAW_PART_BEGIN, NULL);
    /*键盘样式*/
    static lv_style_t style_keyboard_default;
    lv_style_init(&style_keyboard_default);
    lv_style_set_bg_color(&style_keyboard_default,lv_palette_darken(LV_PALETTE_GREY, 1));	//设置背景颜色
    lv_obj_add_style(obj_btnm_keyboard, &style_keyboard_default,LV_STATE_DEFAULT);   

    /*开关*/  
    obj_switch_pos_speed = lv_switch_create(obj);
    lv_obj_set_size(obj_switch_pos_speed,  200, 40);
    lv_obj_align(obj_switch_pos_speed, LV_ALIGN_TOP_LEFT, 50, -13);
    lv_obj_add_style(obj_switch_pos_speed, &style_btn_default,LV_STATE_DEFAULT);   
    lv_obj_add_event_cb(obj_switch_pos_speed, event_handler_switch_speed_pos, LV_EVENT_VALUE_CHANGED, NULL);
 
    lv_obj_t *label; 
    label = lv_label_create(obj);
    lv_label_set_text(label, "POS");
    lv_obj_set_style_text_color(label,lv_color_white() ,0);
    lv_obj_set_size(label,  50, 40);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 0);

    label = lv_label_create(obj);
    lv_label_set_text(label, "SPD");
    lv_obj_set_style_text_color(label,lv_color_white() ,0);
    lv_obj_set_size(label,  50, 40);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 255, 0);

    // /*表盘标签显示*/
    lv_meter_disp();
    // /*速度显示标签*/
    lv_speed_disp();
    lv_logo_img();
}
/*设置表盘值*/
void set_value_pos( int32_t v)
{
    lv_meter_set_indicator_value(obj_meter_pos, indic_meter_pos, v);
}

/*位置表盘重绘*/
static void event_handler_redraw_meter_pos(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_DRAW_PART_BEGIN){
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);    
        if((dsc->text[0] == '3')&&(dsc->text[1] == '6')){
            dsc->text[0] = '0';
            dsc->text[1] = '\0';
        }
    }
}

void lv_meter_disp(void)
{
    lv_obj_t *obj1 = lv_obj_create(obj_main);

    lv_obj_set_size(obj1, 190, 200);
    lv_obj_set_style_bg_color(obj1,lv_palette_darken(LV_PALETTE_GREY, 1),0);
    lv_obj_align(obj1, LV_ALIGN_TOP_LEFT, 180, 25);
    lv_obj_set_style_border_width(obj1, 1, 0);
    lv_obj_clear_flag(obj1, LV_OBJ_FLAG_SCROLLABLE);
    
    obj_meter_pos = lv_meter_create(obj1);
    lv_obj_align(obj_meter_pos, LV_ALIGN_BOTTOM_MID, 0, 30);
    lv_obj_set_size(obj_meter_pos, 200, 200);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(obj_meter_pos);
    lv_meter_set_scale_ticks(obj_meter_pos, scale, 73, 2, 10, lv_color_white());
    lv_meter_set_scale_major_ticks(obj_meter_pos, scale, 12, 4, 15, lv_color_black(), 10);
    lv_meter_set_scale_range(obj_meter_pos,scale,0,360,360,0);

    lv_obj_add_event_cb(obj_meter_pos, event_handler_redraw_meter_pos, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_set_style_bg_color(obj_meter_pos, lv_palette_darken(LV_PALETTE_GREY, 1), 0);
    lv_obj_set_style_bg_grad_color(obj_meter_pos, lv_palette_darken(LV_PALETTE_GREY, 1), 0);
    lv_obj_set_style_line_color(obj_meter_pos, lv_palette_darken(LV_PALETTE_GREY, 1), 0);
    lv_obj_set_style_outline_color(obj_meter_pos, lv_palette_darken(LV_PALETTE_GREY, 1), 0);
    lv_obj_set_style_shadow_color(obj_meter_pos, lv_palette_darken(LV_PALETTE_GREY, 1), 0);
    lv_obj_set_style_border_color(obj_meter_pos, lv_palette_darken(LV_PALETTE_GREY, 1), 0);/*边框颜色*/
    lv_obj_set_style_border_width(obj_meter_pos,1,0);

    /*创建指针*/
    indic_meter_pos = lv_meter_add_needle_line(obj_meter_pos, scale, 4, lv_palette_main(LV_PALETTE_RED), -10);

    lv_obj_t *label; 
    label = lv_label_create(obj1);
    lv_label_set_text(label, "POSITION");
    lv_obj_set_style_text_color(label,lv_color_black() ,0);
    lv_obj_set_size(label,  90, 40);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, -15);

}

void lv_disp_current_speed(int16_t val)
{
    static int16_t y_pos = 0;
    static int16_t point_num = 0;
    static int16_t max = 20;
    static int16_t min = -20;
    static int16_t same_speed = 0;
    static uint16_t same_num = 0;
    if(same_speed == val){
        same_num++;
    }
    else{
        same_num = 0;
    }
    if(same_num > 100){ /*速度相同不刷新*/
        same_num = 100;
    }
    else{
        point_num++;
        if(point_num > 70){
            point_num = 0;
            max = 20;
            min = -20;
            y_pos = val;
            lv_chart_set_range(obj_chart_speed, LV_CHART_AXIS_PRIMARY_Y, y_pos+ max, y_pos+ min);
            lv_obj_refresh_ext_draw_size(obj_chart_speed);      
        }
        if((val - y_pos > max)||(val - y_pos < min)){
            if(val - y_pos > max){
                max = val - y_pos ;
            }
            else{
                min = val - y_pos ;
            }
            point_num = 0;
            lv_chart_set_range(obj_chart_speed, LV_CHART_AXIS_PRIMARY_Y, y_pos+ max, y_pos+ min);
            lv_obj_refresh_ext_draw_size(obj_chart_speed);       
        }
        lv_chart_set_next_value(obj_chart_speed, chart_ser_cur_speed, val); 
    }
    
    
}

static void event_handler_redraw_speed_chart(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t *dsc = lv_event_get_param(e);
        if (dsc->part == LV_PART_TICKS) {
            if (dsc->label_dsc != NULL) {
                dsc->label_dsc->color = lv_color_black();
            }
        }
    }
}

/*速度显示*/
void lv_speed_disp(void)
{
    lv_obj_t *obj = lv_obj_create(obj_main);

    lv_obj_set_size(obj, 390, 200);
    lv_obj_set_style_bg_color(obj,lv_palette_darken(LV_PALETTE_GREY, 1),0);
    lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 380, 25);
    lv_obj_set_style_border_width(obj, 1, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    /*创建一个表格*/
    obj_chart_speed = lv_chart_create(obj);
    lv_obj_set_size(obj_chart_speed, 310, 150);
    lv_obj_align(obj_chart_speed, LV_ALIGN_RIGHT_MID, 0, 10);
    lv_chart_set_type(obj_chart_speed, LV_CHART_TYPE_LINE);  
    lv_chart_set_point_count(obj_chart_speed,50);

    lv_chart_set_axis_tick(obj_chart_speed, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true,70);
    lv_chart_set_axis_tick(obj_chart_speed, LV_CHART_AXIS_PRIMARY_X, 10, 5, 6, 10, false, 30);
    lv_chart_set_range(obj_chart_speed, LV_CHART_AXIS_PRIMARY_Y,-20,+20);
    lv_obj_add_event_cb(obj_chart_speed, event_handler_redraw_speed_chart, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_refresh_ext_draw_size(obj_chart_speed);

    chart_ser_cur_speed = lv_chart_add_series(obj_chart_speed, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_CHART_AXIS_PRIMARY_Y);

    lv_chart_set_zoom_x(obj_chart_speed, 250);

    lv_obj_t *label; 
    label = lv_label_create(obj);
    lv_label_set_text(label, "SPEED (r/min)");
    lv_obj_set_style_text_color(label,lv_color_black() ,0);
    lv_obj_set_size(label,  150, 40);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, -15);

}

void lv_logo_img(void)
{
    LV_IMG_DECLARE(hp_micro_logo_alpha_181x100);
    lv_obj_t * img_logo = lv_img_create(lv_scr_act());
    lv_img_set_src(img_logo, &hp_micro_logo_alpha_181x100);
    lv_obj_align(img_logo, LV_ALIGN_TOP_LEFT, 0, 10);
    lv_obj_set_size(img_logo, 181, 100);

    lv_obj_t *lable = lv_label_create(lv_scr_act());
    lv_label_set_text(lable, "MOTOR\r DEMO");
    lv_obj_set_style_text_color(lable,lv_color_white() ,0);
    lv_obj_align(lable, LV_ALIGN_TOP_LEFT, 30, 130);
    lv_obj_set_style_text_font(lable, &lv_font_montserrat_32, 0);
}

