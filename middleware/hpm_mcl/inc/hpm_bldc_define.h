/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_BLDC_DEFINE_H
#define HPM_BLDC_DEFINE_H
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
#ifdef CONFIG_BLDC_HAS_EXTRA_CONFIG
#include CONFIG_BLDC_HAS_EXTRA_CONFIG
#endif

#include "hpm_motor_math.h"

#define BLDC_MOTOR_DIR_FORWARD 0
#define BLDC_MOTOR_DIR_REVERSE 1

/*pwm_pin define*/
#define BLDC_PWM_PIN_UH     0
#define BLDC_PWM_PIN_UL     1
#define BLDC_PWM_PIN_VH     2
#define BLDC_PWM_PIN_VL     3
#define BLDC_PWM_PIN_WH     4
#define BLDC_PWM_PIN_WL     5

#define BLDC_MOTOR0_INDEX                (1)
#define BLDC_MOTOR1_INDEX                (2)
#define BLDC_MOTOR2_INDEX                (3)
#define BLDC_MOTOR3_INDEX                (4)

/*pwm 通道定义  foc算法中 它对应 pwm_u 、pwm_v 、pwm_w*/
#define BLDC_PWM_U          0
#define BLDC_PWM_V          1
#define BLDC_PWM_W          2
extern void bldc_nullcallback_func(void);
typedef enum bldc_hall_phase{
    bldc_hall_phase_60, /*60 °*/
    bldc_hall_phase_120 /*120 °*/
}bldc_hall_phase_t;

/*电机相关结构体定义,请遵守命名规则，物理性性质描述定义：输入（I）/输出（O）_物理量描述_物理量单位*/
typedef struct bldc_motor_par{
    /*输入参数*/
    float  I_Rstator_ohm; 	        /* 例：输入：定子电阻 (单位ohm)（下同省略） */
    float  I_Poles_n;               /*极对数*/
    float  I_MaxSpeed_rs;           /*最大转速  r/s*/
    float  I_Lstator_h;				      /* 定子电感 */	  			      
    float  I_PhaseCur_a; 			      /* 额定电流 */
    float  I_PhaseVol_v;			      /* 额定电压 */
    float  I_SamplingPer_s;			    /* 电流采样周期*/
    
    /*输出参数*/
    HPM_MOTOR_MATH_TYPE  O_smc_f;			                /* 滑膜控制系数1 */
    HPM_MOTOR_MATH_TYPE  O_smc_g;			                /* 滑膜控制系数2*/
    /*电机参数相关的函数*/
    void   (*func_smc_const)();	    	/* 计算滑膜控制系数的函数 */ 
}BLDC_MOTOR_PARA;

#define BLDC_MOTOR_PARA_DEFAULTS {0,0,0,\
                                        0,0,0,\
                                        0,0,0,\
                                        &bldc_nullcallback_func}

/*速度计算参数*/
typedef struct  bldc_contrl_spd_par{
    uint16_t            I_speedacq;               /*采集n次角度数据后，更新一次速度数据*/
    uint16_t            num;                    /*计数用内部计算*/
    HPM_MOTOR_MATH_TYPE       I_speedLooptime_s;        /*循环n次的时间 s*/
    HPM_MOTOR_MATH_TYPE       speedtheta;             /*计算速度使用的角度*/
    HPM_MOTOR_MATH_TYPE       speedlasttheta;          /*存储上次的角度值*/
    HPM_MOTOR_MATH_TYPE       speedthetaLastN;        /*累加theta，初始化为0即可，用户不需要关心*/
    HPM_MOTOR_MATH_TYPE       I_speedfilter;            /*速度滤波系数，低通滤波器*/
    HPM_MOTOR_MATH_TYPE       O_speedout_filter;        /*滤波后速度输出*/
    HPM_MOTOR_MATH_TYPE       O_speedout;               /*直接速度输出*/
    BLDC_MOTOR_PARA     *I_motorpar;                /*电机参数*/
    void    (*func_getspd)();                     /*速度运算函数*/
}BLDC_CONTRL_SPD_PARA;

#define BLDC_CONTRL_SPD_PARA_DEFAULTS {0,0,0,0,0,\
                                        0,0,0,0,NULL,\
                                        &bldc_nullcallback_func}
/*针对所有pid控制*/
typedef struct bldc_contrl_pid_par{
    HPM_MOTOR_MATH_TYPE       I_kp;                   /*Kp*/
    HPM_MOTOR_MATH_TYPE       I_ki;                   /*Ki*/
    HPM_MOTOR_MATH_TYPE       I_kd;                   /*Kd*/
    HPM_MOTOR_MATH_TYPE       I_max;                  /*最大值  默认min = -max*/
    HPM_MOTOR_MATH_TYPE       target;                 /*目标*/
    HPM_MOTOR_MATH_TYPE       mem;                    /*内部运算使用*/
    HPM_MOTOR_MATH_TYPE       cur;                    /*采集*/
    HPM_MOTOR_MATH_TYPE       outval;                 /*输出*/
    void (*func_pid)();                         /*Pid控制程序*/
}BLDC_CONTRL_PID_PARA;
#define BLDC_CONTRL_PID_PARA_DEFAULTS {0,0,0,0,\
                                        0,0,0,0,\
                                       &bldc_nullcallback_func}
/*电流采样参数*/
typedef struct bldc_contrl_current_par{
    uint16_t            adc_u;                  /*u相电流ad字*/
    uint16_t            adc_v;                  /*v相电流ad字*/
    uint16_t            adc_w;                  /*W相电流ad字*/
    uint16_t            adc_u_middle;           /*u相中点ad字*/
    uint16_t            adc_v_middle;           /*v相中点ad字*/
    uint16_t            adc_w_middle;           /*w相中点ad字*/
    HPM_MOTOR_MATH_TYPE       cal_u;                  /*计算后的U相电流*/
    HPM_MOTOR_MATH_TYPE       cal_v;                  /*计算后的v相电流*/
    HPM_MOTOR_MATH_TYPE       cal_w;                  /*计算后的W相电流*/
    void*               userdata;               /*用户扩展数据*/
    void (*func_sampl)();                         /*电流采样*/
}BLDC_CONTROL_CURRENT_PARA;
#define BLDC_CONTROL_CURRENT_PARA_DEFAULTS {0,0,0,\
                                            0,0,0,\
                                            0,0,0,\
                                            NULL,&bldc_nullcallback_func}
/*pwm*/
typedef struct bldc_control_pwmout_par{
    uint8_t             I_motor_id;                 /*电机id*/
    uint8_t             I_sync_id;            /*同步id*/
    uint32_t            pwm_u;
    uint32_t            pwm_v;
    uint32_t            pwm_w;
    uint32_t            I_pwm_reload;
    void (*func_set_pwm)();                 /*指向输出pwm函数*/
}BLDC_CONTROL_PWMOUT_PARA;
#define BLDC_CONTROL_PWMOUT_PARA_DEFAULTS {0,0,0,\
                                            0,0,0,\
                                            &bldc_nullcallback_func}
typedef struct bldc_control_pwm_par{
    HPM_MOTOR_MATH_TYPE       target_alpha;
    HPM_MOTOR_MATH_TYPE       target_beta;
    int8_t             sector;             /*当前扇区，方便以后扩展功能*/
    uint32_t            I_pwm_reload_max;       /*最大占空比*/
    BLDC_CONTROL_PWMOUT_PARA    pwmout;
    void (*func_spwm)();                    /*指向处理函数*/
}BLDC_CONTROL_PWM_PARA;
#define BLDC_CONTROL_PWM_PARA_DEFAULTS {0,0,0,0,\
                                           BLDC_CONTROL_PWMOUT_PARA_DEFAULTS,\
                                            &bldc_nullcallback_func}
/*sliding mode control(SMC)*/
typedef struct bldc_control_smc_par{
    HPM_MOTOR_MATH_TYPE       I_Ezero;    /*hua*/
    HPM_MOTOR_MATH_TYPE       I_ksmc;  
    HPM_MOTOR_MATH_TYPE       I_kfil;     
    HPM_MOTOR_MATH_TYPE       *ualpha;
    HPM_MOTOR_MATH_TYPE       *ubeta;     	       		
    HPM_MOTOR_MATH_TYPE       *ialpha;
    HPM_MOTOR_MATH_TYPE       *ibeta;  
    /*内部运算*/
    HPM_MOTOR_MATH_TYPE       ialpha_mem;
    HPM_MOTOR_MATH_TYPE       ibeta_mem;            
    HPM_MOTOR_MATH_TYPE       alpha_cal;   	
    HPM_MOTOR_MATH_TYPE       zalpha_cal; 
    HPM_MOTOR_MATH_TYPE       beta_cal;  	
    HPM_MOTOR_MATH_TYPE       zbeta_cal;  
    /*输出*/    
    HPM_MOTOR_MATH_TYPE       *theta;
    BLDC_MOTOR_PARA     *I_motorpar;                /*电机参数*/ 
    void (*func_smc)();
}BLDC_CONTROL_SMC_PARA;

#define BLDC_CONTROL_SMC_PARA_DEFAULTS {0,0,0,NULL,NULL,\
                                        NULL,NULL,0,0,0,\
                                        0,0,0,NULL,\
                                        NULL,\
                                         &bldc_nullcallback_func }
/*可以进行自主选择调用的函数*/
typedef struct bldc_func_cal{
    void *par;
    void (*func)();
}BLDC_FUNC_CAL;
#define BLDC_FUNC_CAL_DEFAULTS {NULL, &bldc_nullcallback_func}
/*foc控制*/
typedef struct bldc_contrl_foc_par{
    BLDC_CONTRL_PID_PARA        CurrentDPiPar;             /*D轴电流*/
    BLDC_CONTRL_PID_PARA        CurrentQPiPar;             /*Q轴电流*/
    BLDC_CONTRL_SPD_PARA        SpeedCalPar;                  /*速度参数*/
    HPM_MOTOR_MATH_TYPE               electric_angle;             /*当前电角度*/
    BLDC_CONTROL_CURRENT_PARA   samplCurpar;                    /*采样电流*/
    BLDC_MOTOR_PARA             motorpar;                   /*电机参数*/
    BLDC_CONTROL_PWM_PARA       pwmpar;                     /*pwm参数*/
    BLDC_FUNC_CAL               pos_estimator_par;         /*如果不进行位置估计,直接空指针，如果进行位置估计如滑膜控制，给指针赋值*/
    HPM_MOTOR_MATH_TYPE               u_alpha;
    HPM_MOTOR_MATH_TYPE               u_beta;
    HPM_MOTOR_MATH_TYPE               i_alpha;
    HPM_MOTOR_MATH_TYPE               i_beta;
    void (*func_dqsvpwm)();
}BLDC_CONTROL_FOC_PARA;
#define BLDC_CONTROL_FOC_PARA_DEFAULTS {BLDC_CONTRL_PID_PARA_DEFAULTS,BLDC_CONTRL_PID_PARA_DEFAULTS,\
                                        BLDC_CONTRL_SPD_PARA_DEFAULTS,0,\
                                        BLDC_CONTROL_CURRENT_PARA_DEFAULTS,BLDC_MOTOR_PARA_DEFAULTS,\
                                        BLDC_CONTROL_PWM_PARA_DEFAULTS,\
                                        BLDC_FUNC_CAL_DEFAULTS ,\
                                        0,0,0,0,&bldc_nullcallback_func}

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif