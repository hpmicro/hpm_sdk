/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef BLDC_BLOCK_CFG_H
#define BLDC_BLOCK_CFG_H

extern  int32_t qei_clock_hz;
#define PWM_FREQUENCY               (20000) /*PWM 频率  单位HZ*/
#define PWM_RELOAD                  (qei_clock_hz/PWM_FREQUENCY) /*20K hz  = 200 000 000/PWM_RELOAD */

/*PI Control define*/
   
#define PI_PWM_RANGE           1000 /* PI output will from PI_PWM_RANGE to -PI_PWM_RANGE */

#define PI_PWM_OUT_MAX        (PWM_RELOAD *0.97)
#define PI_PWM_OUT_MIN        (PWM_RELOAD *0.01) 
#define PI_P_VAL                (0.37) /*PI p Val*/
#define PI_I_VAL                (0.0000002) /*PI i Val*/

/*speed Filter define */
#define SPEED_FILT_NUM (3) /*0.36.   must smaller than 200*/

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

#endif

#ifndef BOARD_BLDC_HALL_BASE

#define BOARD_BLDC_HALL_BASE                 HPM_HALL2
#define BOARD_BLDC_HALL_TRGM                 HPM_TRGM2
#define BOARD_BLDC_HALL_IRQ                  IRQn_HALL2
#define BOARD_BLDC_HALL_TRGM_HALL_U_SRC      HPM_TRGM2_INPUT_SRC_TRGM0_P6
#define BOARD_BLDC_HALL_TRGM_HALL_V_SRC      HPM_TRGM2_INPUT_SRC_TRGM0_P7
#define BOARD_BLDC_HALL_TRGM_HALL_W_SRC      HPM_TRGM2_INPUT_SRC_TRGM0_P8
#define BOARD_BLDC_HALL_MOTOR_PHASE_COUNT_PER_REV        (1000U)

#endif

#ifndef BOARD_BLDC_QEI_BASE

#define BOARD_BLDC_QEI_BASE              HPM_QEI2
#define BOARD_BLDC_QEI_IRQ               IRQn_QEI2
#define BOARD_BLDC_QEI_TRGM              HPM_TRGM2
#define BOARD_BLDC_QEI_TRGM_QEI_A_SRC    HPM_TRGM2_INPUT_SRC_TRGM0_P9
#define BOARD_BLDC_QEI_TRGM_QEI_B_SRC    HPM_TRGM2_INPUT_SRC_TRGM0_P10
#define BOARD_BLDC_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_BLDC_QEI_CLOCK_SOURCE      clock_mot2

#endif

#ifndef BOARD_FREEMASTER_UART_BASE
#define BOARD_FREEMASTER_UART_BASE HPM_UART0
#define BOARD_FREEMASTER_UART_IRQ IRQn_UART0
#define BOARD_FREEMASTER_UART_CLK_NAME clock_uart0
#endif

#define MOTOR0_POLE_PAIR            (2)
#define MOTOR0_HALL_ANGLE           bldc_hall_phase_120  /*60   or  120  */
#define MOTOR0_BLDCPWM              BOARD_BLDCPWM

#define QEI_A_PHASE_ANGLE_CYCLE     (0.36) /* = 360 /(The number of encoder lines) */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*call back func*/
void bldc_pwm_enable(uint8_t motor_index,uint8_t pin_name);
void bldc_pwm_disable(uint8_t motor_index,uint8_t pin_name);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

