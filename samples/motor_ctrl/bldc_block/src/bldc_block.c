/*
 * Copyright (c) 2021 hpmicro
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
#include "hpm_hall_drv.h"
#include "hpm_qei_drv.h"
#include "hpm_gptmr_drv.h"

#include "hpm_clock_drv.h"
#include "freemaster.h"
#include "hpm_uart_drv.h"

#include "hpm_bldc_define.h"
#include "bldc_block_cfg.h"
#include "hpm_bldc_block_func.h"

/*motor_speed set*/
#define MOTOR0_SPD                  (20.0)  /*r/s   deta:0.1r/s    -40-40r/s */
/*USER define*/
#define MOTOR_PWM_DUTY_INIT_VAL     (10)  /*percentage such: 70 mean  700*/
#define QEI_WDOG_TIMEOUT            (200000)      

uint8_t motor_dir = (MOTOR0_SPD > 0 ? BLDC_MOTOR_DIR_FORWARD:BLDC_MOTOR_DIR_REVERSE);
int32_t pwm_out_global = 0;
float  current_speed_global = 0;
/*
*freemaster use global
*/
float fre_pid_p = PI_P_VAL;
float fre_pid_i = PI_I_VAL;
float fre_setspeed = MOTOR0_SPD;
float fre_curspeed = 0.0;
uint16_t fre_set_times = 1;
float motor_pi_ctrl_mem = 0.0;
int32_t qei_clock_hz = 0;

/*speed filter*/
int32_t speed_filter_tbl[SPEED_FILT_NUM] ={0} ; /*累加*/
int32_t speed_filter_sum = 0;
uint8_t speed_filter_num = 0;
uint8_t speed_filter_pos = 0;

/*speed Filter*/
int32_t bldc_block_speed_filter(int32_t input_speed)
{
    
    if(speed_filter_num >= SPEED_FILT_NUM){
        speed_filter_sum -= speed_filter_tbl[speed_filter_pos];
        speed_filter_num--;
    }

    speed_filter_tbl[speed_filter_pos++] = input_speed;
    if(speed_filter_pos >= SPEED_FILT_NUM){
        speed_filter_pos = 0;
    }
    speed_filter_sum += input_speed;

    speed_filter_num++;
    return (speed_filter_sum/speed_filter_num);
}

/*HALL ISR*/
void isr_hall(void)
{
    /*在这个中断内换向*/
    uint8_t hall_stat;
    uint8_t motor_step;
    uint8_t u, v, w;
    
    hall_clear_status(BOARD_BLDC_HALL_BASE, hall_get_status(BOARD_BLDC_HALL_BASE));

    /* the following u,v,w count value read out on read event at u singal toggle */
    hall_stat = hall_get_current_uvw_stat(BOARD_BLDC_HALL_BASE);
    w = ((hall_stat >> 0)& 0x01);
    v = ((hall_stat >> 1)& 0x01);
    u = ((hall_stat >> 2)& 0x01);
    motor_step = bldc_block_step_get(MOTOR0_HALL_ANGLE,u,v,w);

    bldc_block_ctrl(BLDC_MOTOR0_INDEX,motor_dir,motor_step);
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_HALL_IRQ, isr_hall)

int hall_init(void)
{

    trgm_output_t config = {0};
    init_hall_trgm_pins();
    config.invert = false;

    config.input = BOARD_BLDC_HALL_TRGM_HALL_U_SRC;
    trgm_output_config(BOARD_BLDC_HALL_TRGM, TRGM_TRGOCFG_HALL_U, &config);

    config.input = BOARD_BLDC_HALL_TRGM_HALL_V_SRC;
    trgm_output_config(BOARD_BLDC_HALL_TRGM, TRGM_TRGOCFG_HALL_V, &config);

    config.input = BOARD_BLDC_HALL_TRGM_HALL_W_SRC;
    trgm_output_config(BOARD_BLDC_HALL_TRGM, TRGM_TRGOCFG_HALL_W, &config);

    intc_m_enable_irq_with_priority(BOARD_BLDC_HALL_IRQ, 1);

    hall_counter_reset_assert(BOARD_BLDC_HALL_BASE);
    hall_phase_config(BOARD_BLDC_HALL_BASE, 1, hall_count_delay_start_after_uvw_toggle);
    hall_load_read_trigger_event_enable(BOARD_BLDC_HALL_BASE, HALL_EVENT_PHUPT_FLAG_MASK);
    hall_irq_enable(BOARD_BLDC_HALL_BASE, HALL_EVENT_PHUPT_FLAG_MASK);
    hall_counter_reset_release(BOARD_BLDC_HALL_BASE);
    return 0;
}

/*
* return r/s
*/
float qei_get_speed(bool zero)
{
    int32_t spd;
    float fspd;
    uint8_t dir;
    if(zero == 0){
        spd =0;
    }
    else{
        spd = qei_get_speed_history(BOARD_BLDC_QEI_BASE,0)+\
            qei_get_speed_history(BOARD_BLDC_QEI_BASE,1)+\
            qei_get_speed_history(BOARD_BLDC_QEI_BASE,2)+\
            qei_get_speed_history(BOARD_BLDC_QEI_BASE,3);
    }
    /*get dir*/
    dir = qei_get_count_on_read_event(BOARD_BLDC_QEI_BASE,qei_counter_type_speed) >> QEI_COUNT_SPD_DIR_SHIFT;
    if(dir == 0){
        spd = -spd;
    }

    spd = bldc_block_speed_filter(spd);
    if(spd == 0){
        fspd = 0.0;
    }
    else{
        fspd = spd;
        fspd = qei_clock_hz/spd;//hz
        fspd = (fspd*QEI_A_PHASE_ANGLE_CYCLE)/360;//r/s
    }

    return fspd;
}
/*pi --->>>> pwm*/
int32_t pival_to_pwmoutput(float pi)
{
    float duty; 
    uint32_t  pwm_out;

    duty = (PWM_RELOAD / PI_PWM_RANGE)*pi;
    pwm_out_global -= duty;
    if(pwm_out_global > PI_PWM_OUT_MAX){
        pwm_out_global = PI_PWM_OUT_MAX;
    }
    if(pwm_out_global < -PI_PWM_OUT_MAX){
        pwm_out_global = -PI_PWM_OUT_MAX;
    }
    if(pwm_out_global < 0){
        pwm_out = -pwm_out_global;
        motor_dir = BLDC_MOTOR_DIR_REVERSE;
    }
    else{
        pwm_out = pwm_out_global;
        motor_dir = BLDC_MOTOR_DIR_FORWARD;
    }
    
    if(pwm_out < PI_PWM_OUT_MIN){
        pwm_out = PI_PWM_OUT_MIN;
    }
    return pwm_out;
}

void reset_pwm_counter(void)
{
    pwm_enable_reload_at_synci(MOTOR0_BLDCPWM);
    trgm_output_update_source(BOARD_BLDCPWM_TRGM, TRGM_TRGOCFG_PWM_SYNCI, 1);
    trgm_output_update_source(BOARD_BLDCPWM_TRGM, TRGM_TRGOCFG_PWM_SYNCI, 0);
}

/*PWMISR*/
void isr_pwm(void)
{
    pwm_clear_status(MOTOR0_BLDCPWM, pwm_get_status(MOTOR0_BLDCPWM));

}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDCAPP_PWM_IRQ, isr_pwm)

void pwm_init(void)
{
    uint8_t cmp_index = BOARD_BLDCPWM_CMP_INDEX_0;
    pwm_cmp_config_t cmp_config[2] = {0};
    pwm_config_t pwm_config = {0};

    pwm_stop_counter(MOTOR0_BLDCPWM);
    reset_pwm_counter();
    pwm_get_default_pwm_config(MOTOR0_BLDCPWM, &pwm_config);

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
    cmp_config[0].update_trigger = pwm_shadow_register_update_on_shlk;

    cmp_config[1].mode = pwm_cmp_mode_output_compare;
    cmp_config[1].cmp = PWM_RELOAD + 1;
    cmp_config[1].update_trigger = pwm_shadow_register_update_on_shlk;

    pwm_config.enable_output = true;
    pwm_config.dead_zone_in_half_cycle = 100;
    pwm_config.invert_output = false;

/*
     * config pwm
     */
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }
    if (status_success != pwm_setup_waveform(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN, &pwm_config, cmp_index, cmp_config, 2)) {
        printf("failed to setup waveform\n");
        while(1);
    }



    /*force pwm*/
    pwm_config_force_cmd_timing(MOTOR0_BLDCPWM, pwm_force_immediately);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwm_disable_pwm_sw_force_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);
    pwm_set_force_output(MOTOR0_BLDCPWM,
                        PWM_FORCE_OUTPUT(BOARD_BLDC_UH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_UL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_VL_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WH_PWM_OUTPIN, pwm_output_0)
                        | PWM_FORCE_OUTPUT(BOARD_BLDC_WL_PWM_OUTPIN, pwm_output_0));
    pwm_enable_sw_force(MOTOR0_BLDCPWM);
    pwm_start_counter(MOTOR0_BLDCPWM);

}

void disable_all_pwm_output(void)
{
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_UH_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_UL_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_VH_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_VL_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_WH_PWM_OUTPIN);
    pwm_disable_output(MOTOR0_BLDCPWM, BOARD_BLDC_WL_PWM_OUTPIN);

}
/*qei*/
void isr_qei(void)
{
    if(qei_get_bit_status(BOARD_BLDC_QEI_BASE,QEI_EVENT_WDOG_FLAG_MASK)){
        current_speed_global = qei_get_speed(0);            
    }
    if(qei_get_bit_status(BOARD_BLDC_QEI_BASE,QEI_EVENT_POSITIVE_COMPARE_FLAG_MASK)){
        current_speed_global = qei_get_speed(1);
    }
    qei_clear_status(BOARD_BLDC_QEI_BASE, qei_get_status(BOARD_BLDC_QEI_BASE));
}

SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_QEI_IRQ, isr_qei)

int qei_init(void)
{
    init_qei_trgm_pins();

    trgm_output_t config = {0};
    config.invert = false;
    config.input = BOARD_BLDC_QEI_TRGM_QEI_A_SRC;

    trgm_output_config(BOARD_BLDC_QEI_TRGM, TRGM_TRGOCFG_QEI_A, &config);
    config.input = BOARD_BLDC_QEI_TRGM_QEI_B_SRC;
    trgm_output_config(BOARD_BLDC_QEI_TRGM, TRGM_TRGOCFG_QEI_B, &config);

    intc_m_enable_irq_with_priority(BOARD_BLDC_QEI_IRQ, 1);

    qei_wdog_config(BOARD_BLDC_QEI_BASE,QEI_WDOG_TIMEOUT,1);
    qei_wdog_enable(BOARD_BLDC_QEI_BASE);

    qei_counter_reset_assert(BOARD_BLDC_QEI_BASE);
    qei_phase_config(BOARD_BLDC_QEI_BASE, BOARD_BLDC_QEI_MOTOR_PHASE_COUNT_PER_REV,
            qei_z_count_inc_on_phase_count_max, false);
    qei_phase_cmp_set(BOARD_BLDC_QEI_BASE, (BOARD_BLDC_QEI_MOTOR_PHASE_COUNT_PER_REV>>2),
            false, qei_rotation_dir_cmp_ignore);
    qei_load_read_trigger_event_enable(BOARD_BLDC_QEI_BASE,
            QEI_EVENT_POSITIVE_COMPARE_FLAG_MASK);
    qei_irq_enable(BOARD_BLDC_QEI_BASE, QEI_EVENT_POSITIVE_COMPARE_FLAG_MASK|QEI_EVENT_WDOG_FLAG_MASK);
    qei_counter_reset_release(BOARD_BLDC_QEI_BASE);
    qei_clock_hz = clock_get_frequency(BOARD_BLDC_QEI_CLOCK_SOURCE);
    return 0;
}

/*1ms*/
uint8_t timer_times = 0;

static float last_set_speed = 10.0;
static uint32_t  speed_cal_times =0;
void isr_gptmr(void)
{
    volatile uint32_t s = BOARD_BLDC_TMR_1MS->SR;
    BOARD_BLDC_TMR_1MS->SR = s;
    if (s & GPTMR_CH_CMP_STAT_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP)) {
        timer_times++;
        if(timer_times >= fre_set_times){
            float pi;
            uint32_t block_pwm_out;
            float setspeed;
            timer_times = 0;
            setspeed = fre_setspeed;
            if((last_set_speed > 0)&&(fre_setspeed <0)){
                speed_cal_times++;
                if(speed_cal_times >= 10){
                    last_set_speed = fre_setspeed;
                    pwm_out_global = (PWM_DUTY_CYCLE_FP_MAX *0.4);
                   speed_cal_times = 0;
                }
            }
            if((last_set_speed < 0)&&(fre_setspeed >0)){
                speed_cal_times++;

                if(speed_cal_times >= 10){
                   last_set_speed = fre_setspeed;
                   pwm_out_global = -(PWM_DUTY_CYCLE_FP_MAX *0.4);
                   speed_cal_times = 0;
                }
            }
            
            pi = al_pi_ctrl_func(&motor_pi_ctrl_mem,setspeed,current_speed_global,fre_pid_p,fre_pid_i,PI_PWM_RANGE);
            fre_curspeed = current_speed_global;
            block_pwm_out = pival_to_pwmoutput(pi);
            pwm_update_raw_cmp_central_aligned(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_0, BOARD_BLDCPWM_CMP_INDEX_1,
                 (PWM_RELOAD - block_pwm_out)<<1, (PWM_RELOAD + block_pwm_out)<<1);
            pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);
        }
    }
}
SDK_DECLARE_EXT_ISR_M(BOARD_BLDC_TMR_IRQ, isr_gptmr)

/*Timer init 1ms_isr*/
static void timer_init(void)
{
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(BOARD_BLDC_TMR_1MS, &config);
    config.cmp[0] = BOARD_BLDC_TMR_RELOAD;
    config.debug_mode = 0;
    config.reload = BOARD_BLDC_TMR_RELOAD+1;

    gptmr_enable_irq(BOARD_BLDC_TMR_1MS, GPTMR_CH_CMP_IRQ_MASK(BOARD_BLDC_TMR_CH, BOARD_BLDC_TMR_CMP));
    gptmr_channel_config(BOARD_BLDC_TMR_1MS, BOARD_BLDC_TMR_CH, &config, true);
    intc_m_enable_irq_with_priority(BOARD_BLDC_TMR_IRQ, 1);

}

static void init_freemaster_uart(void)
{
    hpm_stat_t stat;
    uart_config_t config = {0};

    clock_set_source_divider(BOARD_FREEMASTER_UART_CLK_NAME, clk_src_osc24m, 1U);
    board_init_uart(BOARD_FREEMASTER_UART_BASE);
    uart_default_config(BOARD_FREEMASTER_UART_BASE, &config);
    config.fifo_enable = true;
    config.src_freq_in_hz = clock_get_frequency(BOARD_FREEMASTER_UART_CLK_NAME);

    stat = uart_init(BOARD_FREEMASTER_UART_BASE, &config);
    if (stat != status_success) {
        /* uart failed to be initialized */
        printf("failed to initialize uart\n");
        while(1);
    }

#if FMSTR_SHORT_INTR || FMSTR_LONG_INTR
    /* Enable UART interrupts. */
    uart_enable_irq(BOARD_FREEMASTER_UART_BASE, uart_intr_rx_data_avail_or_timeout, true);
    intc_m_enable_irq_with_priority(BOARD_FREEMASTER_UART_IRQ, 1);
#endif

}

int main(void)
{
    int32_t block_pwm_out = 0;
    uint8_t motor_step;  
    uint8_t hal_stat;
    uint8_t u, v, w;
    unsigned short nAppCmdCode;
    board_init();

    init_freemaster_uart();
    FMSTR_Init();


    init_pwm_pins(MOTOR0_BLDCPWM);
    hall_init();
    qei_init();
    printf("motor test example\n");

    pwm_init();
    timer_init();

    block_pwm_out = 0;
    pwm_update_raw_cmp_central_aligned(MOTOR0_BLDCPWM, BOARD_BLDCPWM_CMP_INDEX_0, BOARD_BLDCPWM_CMP_INDEX_1,
                 (PWM_RELOAD - block_pwm_out)<<1, (PWM_RELOAD + block_pwm_out)<<1);
    pwm_issue_shadow_register_lock_event(MOTOR0_BLDCPWM);

    /*start motor By Hall position ,Get motor step*/
    hal_stat = hall_get_current_uvw_stat(BOARD_BLDC_HALL_BASE);
    w = ((hal_stat >> 0)& 0x01);
    v = ((hal_stat >> 1)& 0x01);
    u = ((hal_stat >> 2)& 0x01);
    motor_step = bldc_block_step_get(MOTOR0_HALL_ANGLE,u,v,w);

    bldc_block_ctrl(BLDC_MOTOR0_INDEX,motor_dir,motor_step);

    while(1)
    {
        nAppCmdCode = FMSTR_GetAppCmd();

        if (nAppCmdCode != FMSTR_APPCMDRESULT_NOCMD){
            switch(nAppCmdCode){
                case 1:  FMSTR_AppCmdAck(1); break;
                case 2: FMSTR_AppCmdAck(0xcd); break;
                default: FMSTR_AppCmdAck(0); break;
            }
        }
        FMSTR_Recorder();
        FMSTR_Poll();
        
    }
    disable_all_pwm_output();    
    printf("test done\n");
    while(1);
    return 0;
}

