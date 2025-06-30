/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include <stdio.h>
#include <math.h>
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_qei_drv.h"
#include "hpm_gptmr_drv.h"
#include "hpm_adc12_drv.h"

#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"

#include "hpm_bldc_define.h"
#include "bldc_foc_cfg.h"
#include "hpm_foc.h"
#include "hpm_smc.h"

#include "hpm_lcdc_drv.h"

#include "lvgl.h"
#include "lv_adapter.h"
#include "hpm_gpio_drv.h"
#include "hpm_adc.h"

/*motor_speed set*/
#define MOTOR0_SPD                  (0)  /*r/s   delta:0.1r/s    1-40r/s */
/*USER define*/
#define BLDC_ANGLE_SET_TIME_MS      (2000) /*角度对中时间  单位ms*/
#define BLDC_CURRENT_SET_TIME_MS    (200) /*电流对中时间  单位ms，禁止大于250*/
#if (BLDC_CURRENT_SET_TIME_MS > 250)
#error "BLDC_CURRENT_SET_TIME_MS must be smaller than 250"
#endif

#define LV_TICK (5)

#ifndef TIMER_CLK_NAME
#define TIMER_CLK_NAME clock_mchtmr0
#endif

uint32_t mchtmr_freq_in_khz = 0;

int32_t bldc_foc_get_pos(void);
extern void set_value_pos( int32_t v);
extern void lv_disp_current_speed(int16_t val);
extern void lv_ex_motor_speed(void);

volatile ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT) uint32_t adc_buff[3][BOARD_BLDC_ADC_PMT_DMA_SIZE_IN_4BYTES];

void motor0_highspeed_loop(void);
uint8_t smc_start_flag = 0; /*滑模启动标志*/
int32_t qei_clock_hz = 0;
float fre_setspeed = MOTOR0_SPD;
/*1ms 时间参考*/
volatile uint8_t timer_flag = 0;
float fre_set_angle = 0.0;
int32_t fre_set_pos = 0;
int32_t fre_last_pos = 0;
/*mode choose*/
int8_t fre_user_mode = 0; /*0 : pos     1:  speed*/
int32_t fre_record_now_pos = 0; /*record position */
/**adc hal struct*/
adc_type hpm_adc_u = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_U_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_U_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE};
adc_type hpm_adc_v = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_V_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_V_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE};
adc_type hpm_adc_w = {
    #if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    .adc_base.adc12 = BOARD_BLDC_ADC_W_BASE,
    #else
    .adc_base.adc16 = BOARD_BLDC_ADC_W_BASE,
    #endif
    .module = BOARD_BLDC_ADC_MODULE};
typedef struct motor0_par{
    BLDC_CONTROL_FOC_PARA       foc_para;
    BLDC_CONTRL_PID_PARA        speedloop_para;
    BLDC_CONTRL_PID_PARA        position_para;
    void (*adc_trig_event_callback)(void);
}MOTOR0_PARA;
MOTOR0_PARA motor0 = {  BLDC_CONTROL_FOC_PARA_DEFAULTS,
                        BLDC_CONTRL_PID_PARA_DEFAULTS, BLDC_CONTRL_PID_PARA_DEFAULTS,
                        NULL
                    };

/*初始化foc变量*/
void bldc_init_par(void)
{
    BLDC_CONTROL_FOC_PARA *par = &motor0.foc_para;

    par->motorpar.i_lstator_h = 2.63;
    par->motorpar.i_maxspeed_rs = 35;
    par->motorpar.i_phasecur_a = 0.125;
    par->motorpar.i_phasevol_v = 24;
    par->motorpar.i_poles_n = 2;
    par->motorpar.i_rstator_ohm = 1.1;
    par->motorpar.i_samplingper_s = 0.00005;

    par->speedcalpar.i_speedacq = 20;
    par->speedcalpar.i_speedfilter = HPM_MOTOR_MATH_FL_MDF(0.02);
    par->speedcalpar.i_speedlooptime_s = HPM_MOTOR_MATH_FL_MDF(0.00005*20);
    par->speedcalpar.i_motorpar = &par->motorpar;
    par->speedcalpar.func_getspd = (void(*)(void *))&hpm_mcl_bldc_foc_al_speed;

    par->currentdpipar.i_kp = HPM_MOTOR_MATH_FL_MDF(10);
    par->currentdpipar.i_ki = HPM_MOTOR_MATH_FL_MDF(0.01);
    par->currentdpipar.i_max = HPM_MOTOR_MATH_FL_MDF(4000);
    par->currentdpipar.func_pid = (void(*)(void *))&hpm_mcl_bldc_foc_pi_contrl;

    par->currentqpipar.i_kp = HPM_MOTOR_MATH_FL_MDF(10);
    par->currentqpipar.i_ki = HPM_MOTOR_MATH_FL_MDF(0.01);
    par->currentqpipar.i_max = HPM_MOTOR_MATH_FL_MDF(4000);
    par->currentqpipar.func_pid = (void(*)(void *))&hpm_mcl_bldc_foc_pi_contrl;

    par->pwmpar.func_spwm = (void(*)(void *))&hpm_mcl_bldc_foc_svpwm;
    par->pwmpar.i_pwm_reload_max = PWM_RELOAD*0.95;
    par->pwmpar.pwmout.func_set_pwm = (void(*)(void *))&hpm_mcl_bldc_foc_pwmset;
    par->pwmpar.pwmout.i_pwm_reload = PWM_RELOAD;
    par->pwmpar.pwmout.i_motor_id = BLDC_MOTOR0_INDEX;

    par->samplcurpar.func_sampl = (void(*)(void *))&hpm_mcl_bldc_foc_current_cal;
    par->func_dqsvpwm =  (void (*)(void *, void *, void *, void *))&hpm_mcl_bldc_foc_ctrl_dq_to_pwm;

    /*速度环参数*/
    motor0.speedloop_para.func_pid  = (void(*)(void *))&hpm_mcl_bldc_foc_pi_contrl;
    motor0.speedloop_para.i_kp      = HPM_MOTOR_MATH_FL_MDF(60);
    motor0.speedloop_para.i_ki      = HPM_MOTOR_MATH_FL_MDF(0.01);
    motor0.speedloop_para.i_max     = HPM_MOTOR_MATH_FL_MDF(300);
    /*位置环参数*/
    motor0.position_para.func_pid   = (void(*)(void *))&hpm_mcl_bldc_foc_pi_contrl;
    motor0.position_para.i_kp       = HPM_MOTOR_MATH_FL_MDF(0.0095);
    motor0.position_para.i_ki       = 0;
    motor0.position_para.i_max      = HPM_MOTOR_MATH_FL_MDF(25);

    motor0.adc_trig_event_callback = &motor0_highspeed_loop;
    par->pos_estimator_par.func = NULL;
}

void reset_pwm_counter(void)
{
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
    trgm_output_update_source(BOARD_BLDCPWM_TRGM, TRGM_TRGOCFG_PWM_SYNCI, 1);
    trgm_output_update_source(BOARD_BLDCPWM_TRGM, TRGM_TRGOCFG_PWM_SYNCI, 0);
}

void enable_all_pwm_output(void)
{
   /*force pwm*/
    pwm_disable_sw_force(MOTOR0_BLDCPWM);
}

void disable_all_pwm_output(void)
{
   /*force pwm*/
    pwm_config_force_cmd_timing(MOTOR0_BLDCPWM, pwm_force_immediately);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwm_enable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
    pwm_set_force_output(MOTOR0_BLDCPWM,
                        PWM_FORCE_OUTPUT(BOARD_BLDC_UH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_UL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WL_PWM_OUTPIN, pwm_output_0));
    pwm_enable_sw_force(MOTOR0_BLDCPWM);

}
/*
*中心对齐互补 pwm
*/
void pwm_init(void)
{
    uint8_t cmp_index = BOARD_BLDCPWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[4] = {0};
    pwm_pair_config_t pwm_pair_config = {0};
    pwm_output_channel_t pwm_output_ch_cfg;

    pwm_stop_counter(MOTOR0_BLDCPWM);
    reset_pwm_counter();
    /*
     * reload and start counter
     */
    pwm_set_reload(MOTOR0_BLDCPWM, 0, PWM_RELOAD);
    pwm_set_start_count(MOTOR0_BLDCPWM, 0, 0);
    /*
     * config cmp1 and cmp2
     */
    cmp_config[0].mode = pwm_cmp_mode_output_compare;
    cmp_config[0].cmp = PWM_RELOAD + 1;
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = PWM_RELOAD + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_hw_event;

    cmp_config[2].enable_ex_cmp  = false;
    cmp_config[2].mode           = pwm_cmp_mode_output_compare;
    cmp_config[2].cmp = 5;
    cmp_config[2].update_trigger = pwm_shadow_register_update_on_shlk;

    cmp_config[3].mode = pwm_cmp_mode_output_compare;
    cmp_config[3].cmp = PWM_RELOAD;
    cmp_config[3].update_trigger = pwm_shadow_register_update_on_modify;

    pwm_get_default_pwm_pair_config(MOTOR0_BLDCPWM, &pwm_pair_config);
    pwm_pair_config.pwm[0].enable_output = true;
    pwm_pair_config.pwm[0].dead_zone_in_half_cycle = 100;
    pwm_pair_config.pwm[0].invert_output = false;

    pwm_pair_config.pwm[1].enable_output = true;
    pwm_pair_config.pwm[1].dead_zone_in_half_cycle = 100;
    pwm_pair_config.pwm[1].invert_output = false;

    /* Set comparator channel for trigger a */
    pwm_output_ch_cfg.cmp_start_index = BOARD_BLDC_PWM_TRIG_CMP_INDEX;  /* start channel 8 */
    pwm_output_ch_cfg.cmp_end_index   = BOARD_BLDC_PWM_TRIG_CMP_INDEX;  /* end channel 8 */
    pwm_output_ch_cfg.invert_output   = false;
    pwm_config_output_channel(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &pwm_output_ch_cfg);

    /*
     * config pwm
     */
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, &pwm_pair_config, cmp_index, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_pair_config, cmp_index+2, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform_in_pair(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_pair_config, cmp_index+4, &cmp_config[0], 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    pwm_load_cmp_shadow_on_match(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_TRIG_CMP, &cmp_config[3]);

    pwm_config_cmp(MOTOR0_BLDCPWM, BOARD_BLDC_PWM_TRIG_CMP_INDEX, &cmp_config[2]);

    pwm_start_counter(MOTOR0_BLDCPWM);
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
}
/*
*   qei初始化
 */
int qei_init(void)
{
    trgm_output_t trgm_config = {0};
    qei_mode_config_t mode_config = {0};

    init_qei_trgm_pins();

    /* trgm config */
    trgm_config.invert = false;
    trgm_config.input = BOARD_BLDC_QEI_TRGM_QEI_A_SRC;
    trgm_output_config(BOARD_BLDC_QEI_TRGM, TRGM_TRGOCFG_QEI_A, &trgm_config);
    trgm_config.input = BOARD_BLDC_QEI_TRGM_QEI_B_SRC;
    trgm_output_config(BOARD_BLDC_QEI_TRGM, TRGM_TRGOCFG_QEI_B, &trgm_config);

    /* mode config */
    mode_config.work_mode = qei_work_mode_abz;
    mode_config.z_count_inc_mode = qei_z_count_inc_on_phase_count_max;
    mode_config.phcnt_max = BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV;
    mode_config.z_cali_enable = false;
    mode_config.phcnt_idx = 0;
    qei_config_mode(BOARD_BLDC_QEI_BASE, &mode_config);

    qei_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);

    return 0;
}

/*1ms*/
static volatile uint32_t gui_times = 0;
volatile uint8_t gui_begin = 0;
volatile int32_t lvgl_fre_get_pos;
volatile int32_t lvgl_fre_is_update;

void lvgl_pos_update(void)
{
    if (lvgl_fre_is_update) {
        lvgl_fre_is_update = 0;
        int32_t fre_get_pos = lvgl_fre_get_pos;
        if (-fre_get_pos > 0) {
            set_value_pos(((-fre_get_pos) % BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV) * 0.09);
        } else {
            set_value_pos((BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV + (-(fre_get_pos % BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV))) * 0.09);
        }
    }
}

void lvgl_pos_set(int32_t pos)
{
    lvgl_fre_get_pos = pos;
    lvgl_fre_is_update = 1;
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)
void isr_gptmr(void)
{
    volatile uint32_t s = BOARD_BLDC_TMR_1MS->SR;
    int32_t pos_ctrl = 0;
    int32_t pos_err = 0;
    int32_t fre_get_pos = 0 ;

    BOARD_BLDC_TMR_1MS->SR = s;
    if (s & GPTMR_CH_CMP_STAT_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP)) {

        if(gui_begin == 1){
            gui_times++;
            if(gui_times >= LV_TICK){
                gui_times = 0;
                lv_tick_inc(LV_TICK);
                lv_disp_current_speed(-fre_setspeed*60);
            }
        }
        timer_flag = !timer_flag;

        /*速度控制*/
        motor0.speedloop_para.target = HPM_MOTOR_MATH_FL_MDF(fre_setspeed);
        motor0.speedloop_para.cur = motor0.foc_para.speedcalpar.o_speedout;
        motor0.speedloop_para.func_pid(&motor0.speedloop_para); /*速度控制函数*/
        motor0.foc_para.currentqpipar.target =  motor0.speedloop_para.outval;
        motor0.foc_para.currentdpipar.target =  0;
        if(fre_user_mode == 0){/*pos*/

            fre_get_pos = bldc_foc_get_pos()+ fre_record_now_pos;
            if(fre_set_pos != fre_last_pos){
                pos_err =  fre_set_pos - fre_get_pos;
                if((pos_err <= 5 )&&(pos_err >= -5)){
                    fre_last_pos = fre_get_pos;
                    pos_ctrl = fre_get_pos;
                }
                else{
                    pos_ctrl = fre_set_pos;
                }
                    if(gui_begin == 1){
                        /*位置显示*/
                        lvgl_pos_set(fre_get_pos);
                    }
            }
            else{
                pos_err =  fre_last_pos - fre_get_pos;
                if((pos_err <= 100 )&&(pos_err >= -100)){
                    pos_ctrl = fre_get_pos;
                }
                else{
                    pos_ctrl = fre_set_pos;
                        if(gui_begin == 1){
                            /*位置显示*/
                            lvgl_pos_set(fre_get_pos);
                        }
                    }
                }
            motor0.position_para.cur = HPM_MOTOR_MATH_FL_MDF(fre_get_pos);
            motor0.position_para.target = HPM_MOTOR_MATH_FL_MDF(pos_ctrl);
            motor0.position_para.func_pid(&motor0.position_para);
            fre_setspeed = HPM_MOTOR_MATH_MDF_FL(motor0.position_para.outval);
        } else if (fre_user_mode == 1) {
            fre_record_now_pos = -bldc_foc_get_pos();
        }
    }

}

/*Timer init 1ms_isr*/
static void timer_init(void)
{
    gptmr_channel_config_t config;

    clock_add_to_group(BOARD_BLDC_TMR_CLOCK, 0);
    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_TMR_RELOAD+1;
    config.cmp[0] = BOARD_BLDC_TMR_RELOAD;
    config.cmp[1] = config.reload;
    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 1);
}

void init_trigger_mux(TRGM_Type * ptr)
{
    trgm_output_t trgm_output_cfg;

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = BOARD_BLDC_PWM_TRG_ADC;
    trgm_output_config(ptr, BOARD_BLDC_TRG_ADC, &trgm_output_cfg);
}
void motor0_current_loop(float angle)
{

    motor0.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4);
    motor0.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4);
    motor0.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4);
    motor0.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(angle);
    hpm_mcl_bldc_foc_ctrl_dq_to_pwm(&motor0.foc_para);
    motor0.foc_para.pwmpar.pwmout.func_set_pwm(&motor0.foc_para.pwmpar.pwmout);


}
void motor0_highspeed_loop(void)
{
    uint32_t  pos;
    float user_give_angle = 0;
    float fre_get_angle = 0.0;
    pos = qei_get_current_count(BOARD_BLDC_QEI_BASE, qei_counter_type_phase)&0x1fffff;
    pos = ((pos) % 2000)*18;
    fre_get_angle = pos;
    fre_get_angle = 360- (fre_get_angle /100);

    if(smc_start_flag == 1){
        user_give_angle = fre_set_angle;
    }
    else{
        user_give_angle = fre_get_angle;
        fre_set_angle = fre_get_angle;
    }
    motor0_current_loop(user_give_angle);
    motor0.foc_para.speedcalpar.speedtheta = motor0.foc_para.electric_angle;
    motor0.foc_para.speedcalpar.func_getspd(&motor0.foc_para.speedcalpar);
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_ADC_IRQn, isr_adc)
void isr_adc(void)
{
    uint32_t status;
    status = hpm_adc_get_status_flags(&hpm_adc_u);

    if ((status & BOARD_BLDC_ADC_TRIG_FLAG) != 0) {
        hpm_adc_clear_status_flags(&hpm_adc_u, BOARD_BLDC_ADC_TRIG_FLAG);
        motor0.adc_trig_event_callback();
    }
}

void init_trigger_cfg(uint8_t trig_ch, bool inten)
{
    adc_pmt_config_t pmt_cfg;

    pmt_cfg.module = BOARD_BLDC_ADC_MODULE;

#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.trig_ch   = trig_ch;
    pmt_cfg.config.adc12.trig_len  = BOARD_BLDC_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc12.adc_ch[0] = BOARD_BLDC_ADC_CH_W;
    pmt_cfg.config.adc12.inten[0] = inten;

    pmt_cfg.adc_base.adc12 = BOARD_BLDC_ADC_W_BASE;
#else
    pmt_cfg.config.adc16.trig_ch   = trig_ch;
    pmt_cfg.config.adc16.trig_len  = BOARD_BLDC_ADC_PREEMPT_TRIG_LEN;
    pmt_cfg.config.adc16.adc_ch[0] = BOARD_BLDC_ADC_CH_W;
    pmt_cfg.config.adc16.inten[0] = inten;

    pmt_cfg.adc_base.adc16 = BOARD_BLDC_ADC_W_BASE;
#endif
    hpm_adc_set_preempt_config(&pmt_cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.adc_ch[0] = BOARD_BLDC_ADC_CH_V;
    pmt_cfg.adc_base.adc12 = BOARD_BLDC_ADC_V_BASE;
#else
    pmt_cfg.config.adc16.adc_ch[0] = BOARD_BLDC_ADC_CH_V;
    pmt_cfg.adc_base.adc16 = BOARD_BLDC_ADC_V_BASE;
#endif
    hpm_adc_set_preempt_config(&pmt_cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    pmt_cfg.config.adc12.adc_ch[0] = BOARD_BLDC_ADC_CH_U;
    pmt_cfg.adc_base.adc12 = BOARD_BLDC_ADC_U_BASE;
#else
    pmt_cfg.config.adc16.adc_ch[0] = BOARD_BLDC_ADC_CH_U;
    pmt_cfg.adc_base.adc16 = BOARD_BLDC_ADC_U_BASE;
#endif
    hpm_adc_set_preempt_config(&pmt_cfg);
}

/*初始化adc*/
void adc_init(void)
{
    adc_config_t cfg;
    adc_channel_config_t ch_cfg;

    cfg.module = BOARD_BLDC_ADC_MODULE;
    hpm_adc_init_default_config(&cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    board_init_adc_clock(BOARD_BLDC_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_V_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_W_BASE, true);
    cfg.config.adc12.res            = adc12_res_12_bits;
    cfg.config.adc12.conv_mode      = adc12_conv_mode_preemption;
    cfg.config.adc12.diff_sel       = adc12_sample_signal_single_ended;
    cfg.config.adc12.adc_clk_div    = adc12_clock_divider_3;
    cfg.config.adc12.sel_sync_ahb   = false;
    cfg.config.adc12.adc_ahb_en = true;

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_U_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_V_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc12 = BOARD_BLDC_ADC_W_BASE;
    hpm_adc_init(&cfg);
#else
    board_init_adc_clock(BOARD_BLDC_ADC_U_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_V_BASE, true);
    board_init_adc_clock(BOARD_BLDC_ADC_W_BASE, true);
    cfg.config.adc16.res            = adc16_res_16_bits;
    cfg.config.adc16.conv_mode      = adc16_conv_mode_preemption;
    cfg.config.adc16.adc_clk_div    = adc16_clock_divider_4;
    cfg.config.adc16.sel_sync_ahb   = false;
    cfg.config.adc16.adc_ahb_en = true;

    cfg.adc_base.adc16 = BOARD_BLDC_ADC_U_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc16 = BOARD_BLDC_ADC_V_BASE;
    hpm_adc_init(&cfg);

    cfg.adc_base.adc16 = BOARD_BLDC_ADC_W_BASE;
    hpm_adc_init(&cfg);
#endif

    ch_cfg.module                        = BOARD_BLDC_ADC_MODULE;
    hpm_adc_init_channel_default_config(&ch_cfg);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC12
    ch_cfg.config.adc12_ch.diff_sel      = adc12_sample_signal_single_ended;
    ch_cfg.config.adc12_ch.sample_cycle  = 15;

    ch_cfg.adc_base.adc12                = BOARD_BLDC_ADC_U_BASE;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_CH_U;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc12                = BOARD_BLDC_ADC_V_BASE;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_CH_V;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc12                = BOARD_BLDC_ADC_W_BASE;
    ch_cfg.config.adc12_ch.ch            = BOARD_BLDC_ADC_CH_W;
    hpm_adc_channel_init(&ch_cfg);
#else
    ch_cfg.config.adc16_ch.sample_cycle  = 20;

    ch_cfg.adc_base.adc16                = BOARD_BLDC_ADC_U_BASE;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_CH_U;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc16                = BOARD_BLDC_ADC_V_BASE;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_CH_V;
    hpm_adc_channel_init(&ch_cfg);

    ch_cfg.adc_base.adc16                = BOARD_BLDC_ADC_W_BASE;
    ch_cfg.config.adc16_ch.ch            = BOARD_BLDC_ADC_CH_W;
    hpm_adc_channel_init(&ch_cfg);
#endif

    init_trigger_mux(BOARD_BLDCPWM_TRGM);
    init_trigger_cfg(BOARD_BLDC_ADC_TRG, true);
#if BOARD_BLDC_ADC_MODULE == ADCX_MODULE_ADC16
    adc16_enable_pmt_queue(BOARD_BLDC_ADC_U_BASE, BOARD_BLDC_ADC_TRG);
    adc16_enable_pmt_queue(BOARD_BLDC_ADC_V_BASE, BOARD_BLDC_ADC_TRG);
    adc16_enable_pmt_queue(BOARD_BLDC_ADC_W_BASE, BOARD_BLDC_ADC_TRG);
#endif
     /* Set DMA start address for preemption mode */
    hpm_adc_init_pmt_dma(&hpm_adc_u, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[0]));
    hpm_adc_init_pmt_dma(&hpm_adc_v, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[1]));
    hpm_adc_init_pmt_dma(&hpm_adc_w, core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)adc_buff[2]));
}
void motor0_angle_align_loop(void)
{
    motor0.foc_para.samplcurpar.adc_u = ((adc_buff[0][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.samplcurpar.adc_v = ((adc_buff[1][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.samplcurpar.adc_w = ((adc_buff[2][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4)&0xfff;
    motor0.foc_para.electric_angle = HPM_MOTOR_MATH_FL_MDF(0);
    motor0.foc_para.currentdpipar.target = HPM_MOTOR_MATH_FL_MDF(100);
    motor0.foc_para.currentqpipar.target = HPM_MOTOR_MATH_FL_MDF(0);
    hpm_mcl_bldc_foc_ctrl_dq_to_pwm(&motor0.foc_para);
    motor0.foc_para.pwmpar.pwmout.func_set_pwm(&motor0.foc_para.pwmpar.pwmout);
}
/*
* 转子角度对中，将编码器的中点值和实际物理角度的中点值对齐,默认当前的中点值为0角度所在的区域
*               该函数应当在已经可以输出pwm后调用，即电机可以根据给定角度输出力矩
*/
void bldc_foc_angle_align(void)
{
    uint16_t run_times = 0;
    motor0.adc_trig_event_callback = &motor0_angle_align_loop;
    do{
        if(timer_flag == 1){/*1ms 定时*/
            timer_flag = 0;
            run_times++;
            if(run_times >= BLDC_ANGLE_SET_TIME_MS){/*运行时间*/
                /*获取转子角度信息，标记为0 点*/
                qei_init();
                break;
            }
        }
    }while(1);
}
/*
*   获取转子角度实际的角度值 注意这里返回的是脉冲数不是实际角度，应当乘以编码器的1精度对应的角度才是实际角度。
*/
int32_t bldc_foc_get_pos(void)
{
    int32_t ph,z;
    ph = qei_get_current_count(BOARD_BLDC_QEI_BASE, qei_counter_type_phase)&0x1fffff;
    z = qei_get_current_count(BOARD_BLDC_QEI_BASE, qei_counter_type_z)&0x1fffff;
    /*将数据以0为中点  正负排列*/
    if(z >= (0x200000 >> 1)){
        return -(((z - 0x200000)*BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV)+ph);
    }
    else{
        return -((z*BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV)+ph);
    }

}

/*
    获取稳态情况下的电流值电流对中
*/
void lv_set_adval_middle(void)
{
    uint32_t adc_u_sum = 0;
    uint32_t adc_v_sum = 0;
    uint32_t adc_w_sum = 0;
    uint8_t times = 0;
    motor0.foc_para.pwmpar.pwmout.pwm_u = PWM_RELOAD >> 1;
    motor0.foc_para.pwmpar.pwmout.pwm_v = PWM_RELOAD >> 1;
    motor0.foc_para.pwmpar.pwmout.pwm_w = PWM_RELOAD >> 1;
    motor0.foc_para.pwmpar.pwmout.func_set_pwm(&motor0.foc_para.pwmpar.pwmout);
    do{
        if(timer_flag == 1){/*1ms*/

            adc_u_sum += ((adc_buff[0][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4)&0xfff;
            adc_v_sum += ((adc_buff[1][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4)&0xfff;
            adc_w_sum += ((adc_buff[2][BOARD_BLDC_ADC_TRG*4]&0xffff)>>4)&0xfff;
            times++;
            if(times >= BLDC_CURRENT_SET_TIME_MS){
                break;
            }
            timer_flag = 0;
        }
    }while(1);
    motor0.foc_para.samplcurpar.adc_u_middle = adc_u_sum / BLDC_CURRENT_SET_TIME_MS;
    motor0.foc_para.samplcurpar.adc_v_middle = adc_v_sum / BLDC_CURRENT_SET_TIME_MS;
    motor0.foc_para.samplcurpar.adc_w_middle = adc_w_sum / BLDC_CURRENT_SET_TIME_MS;
}

int main(void)
{
    board_init();
    init_adc_bldc_pins();
    init_pwm_pins(MOTOR0_BLDCPWM);
    qei_init();
    bldc_init_par();
    adc_init();
    pwm_init();
    timer_init();
    lv_set_adval_middle();
    board_init_cap_touch();
    board_init_lcd();
    lv_init();
    lv_adapter_init();
    printf("lvgl example\n");
    lv_ex_motor_speed();
	intc_m_enable_irq_with_priority(BOARD_BLDC_ADC_IRQn, 1);
    hpm_adc_enable_interrupts(&hpm_adc_u, BOARD_BLDC_ADC_TRIG_FLAG);
    /*角度对中*/
    bldc_foc_angle_align();
    /*开始转*/
    motor0.adc_trig_event_callback = &motor0_highspeed_loop;
    gui_begin = 1;
    while(1){
        lvgl_pos_update();
        lv_task_handler();
    }
}
