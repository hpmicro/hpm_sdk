/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef BLDC_FOC_CFG_H
#define BLDC_FOC_CFG_H

extern  int32_t qei_clock_hz;
/*PI Control define*/
#define PWM_FREQUENCY               (20000) /*PWM 频率  单位HZ*/
#define PWM_RELOAD                  (qei_clock_hz/PWM_FREQUENCY) /*20K hz  = 200 000 000/PWM_RELOAD */

#ifndef BOARD_BLDC_TMR_1MS

#define BOARD_BLDC_TMR_1MS                       HPM_GPTMR2
#define BOARD_BLDC_TMR_CH                        0
#define BOARD_BLDC_TMR_CMP                       0
#define BOARD_BLDC_TMR_IRQ                       IRQn_GPTMR2
#define BOARD_BLDC_TMR_RELOAD                    (100000U)

#endif

#ifndef BOARD_BLDCPWM
#define BOARD_BLDCPWM                     HPM_PWM2
#define BOARD_BLDC_UH_PWM_OUTPIN         (0U)
#define BOARD_BLDC_UL_PWM_OUTPIN         (1U)
#define BOARD_BLDC_VH_PWM_OUTPIN         (2U)
#define BOARD_BLDC_VL_PWM_OUTPIN         (3U)
#define BOARD_BLDC_WH_PWM_OUTPIN         (4U)
#define BOARD_BLDC_WL_PWM_OUTPIN         (5U)
#define BOARD_BLDCPWM_TRGM                HPM_TRGM2
#define BOARD_BLDCAPP_PWM_IRQ             IRQn_PWM2
#define BOARD_BLDCPWM_CMP_INDEX_0         (0U)
#define BOARD_BLDCPWM_CMP_INDEX_1         (1U)
#define BOARD_BLDCPWM_CMP_INDEX_2         (2U)
#define BOARD_BLDCPWM_CMP_INDEX_3         (3U)
#define BOARD_BLDCPWM_CMP_INDEX_4         (4U)
#define BOARD_BLDCPWM_CMP_INDEX_5         (5U)
#define BOARD_BLDCPWM_CMP_TRIG_CMP        (20U)

#endif

#ifndef BOARD_BLDC_QEI_BASE

#define BOARD_BLDC_QEI_BASE              HPM_QEI2
#define BOARD_BLDC_QEI_IRQ               IRQn_QEI2
#define BOARD_BLDC_QEI_TRGM              HPM_TRGM2
#define BOARD_BLDC_QEI_TRGM_QEI_A_SRC    HPM_TRGM2_INPUT_SRC_TRGM0_P9
#define BOARD_BLDC_QEI_TRGM_QEI_B_SRC    HPM_TRGM2_INPUT_SRC_TRGM0_P10
#define BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV     (4000U)
#define BOARD_BLDC_QEI_CLOCK_SOURCE      clock_mot2

#endif

#ifndef BOARD_FREEMASTER_UART_BASE
#define BOARD_FREEMASTER_UART_BASE HPM_UART0
#define BOARD_FREEMASTER_UART_IRQ IRQn_UART0
#define BOARD_FREEMASTER_UART_CLK_NAME clock_uart0
#endif

#ifndef BOARD_BLDC_ADC_U_BASE

#define BOARD_BLDC_ADC_MODULE                  ADCX_MODULE_ADC12
#define BOARD_BLDC_ADC_U_BASE                  HPM_ADC0
#define BOARD_BLDC_ADC_V_BASE                  HPM_ADC1
#define BOARD_BLDC_ADC_W_BASE                  HPM_ADC2
#define BOARD_BLDC_ADC_TRIG_FLAG               adc12_event_trig_complete

#define BOARD_BLDC_ADC_CH_U                    (7U)
#define BOARD_BLDC_ADC_CH_V                    (10U)
#define BOARD_BLDC_ADC_CH_W                    (11U)
#define BOARD_BLDC_ADC_IRQn                    IRQn_ADC0
#define BOARD_BLDC_ADC_PMT_DMA_SIZE_IN_4BYTES  (ADC_SOC_PMT_MAX_DMA_BUFF_LEN_IN_4BYTES)
#define BOARD_BLDC_ADC_TRG                    ADC12_CONFIG_TRG2A
#define BOARD_BLDC_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_BLDC_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_BLDC_PWM_TRG_ADC             HPM_TRGM2_INPUT_SRC_PWM2_CH8REF
#endif

#define MOTOR0_POLE_PAIR            (2)
#define MOTOR0_HALL_ANGLE           bldc_hall_phase_120  /*60   or  120  */
#define MOTOR0_BLDCPWM              BOARD_BLDCPWM

#define QEI_A_PHASE_ANGLE_CYCLE     (0.36) /* = 360 /(The number of encoder lines) */

#define MOTOR0_ADC_CH_NUM                  (3U)    /*adc转换的通道数量， u、v、w三相电流  所以默认应该是3，一般情况下不需要变动*/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*call back func*/
void hpm_mcl_bldc_pwm_enable(uint8_t motor_index, uint8_t pin_name);
void hpm_mcl_bldc_pwm_disable(uint8_t motor_index, uint8_t pin_name);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

