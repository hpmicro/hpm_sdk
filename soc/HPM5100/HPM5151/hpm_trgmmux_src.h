/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_TRGMMUX_SRC_H
#define HPM_TRGMMUX_SRC_H

/* trgm0_input mux definitions */
#define HPM_TRGM0_INPUT_SRC_VSS                            (0x0UL)   /* low level voltage */
#define HPM_TRGM0_INPUT_SRC_VDD                            (0x1UL)   /* high level voltage */
#define HPM_TRGM0_INPUT_SRC_PTPC_COMP_0                    (0x2UL)   /* PTPC compare output0 */
#define HPM_TRGM0_INPUT_SRC_PTPC_COMP_1                    (0x3UL)   /* PTPC compare output1 */
#define HPM_TRGM0_INPUT_SRC_USB0_SOF                       (0x4UL)   /* USB0 start of frame marker */
#define HPM_TRGM0_INPUT_SRC_DEBUG_FLAG                     (0x5UL)   /* debug mode flag */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH00                      (0x6UL)   /* SYNT channel0 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH01                      (0x7UL)   /* SYNT channel1 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH02                      (0x8UL)   /* SYNT channel2 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH03                      (0x9UL)   /* SYNT channel3 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH04                      (0xAUL)   /* SYNT channel4 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH05                      (0xBUL)   /* SYNT channel5 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH06                      (0xCUL)   /* SYNT channel6 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH07                      (0xDUL)   /* SYNT channel7 pulse output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT2                    (0xEUL)   /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT3                    (0xFUL)   /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT2                    (0x10UL)  /* GPTMR1 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT3                    (0x11UL)  /* GPTMR1 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_PWM0_IN_0                      (0x12UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM0_IN_1                      (0x13UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM0_IN_2                      (0x14UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM0_IN_3                      (0x15UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM0_IN_4                      (0x16UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM0_IN_5                      (0x17UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM0_IN_6                      (0x18UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM0_IN_7                      (0x19UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM1_IN_0                      (0x1AUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM1_IN_1                      (0x1BUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM1_IN_2                      (0x1CUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM1_IN_3                      (0x1DUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM1_IN_4                      (0x1EUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM1_IN_5                      (0x1FUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM1_IN_6                      (0x20UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM1_IN_7                      (0x21UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM2_IN_0                      (0x22UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM2_IN_1                      (0x23UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM2_IN_2                      (0x24UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM2_IN_3                      (0x25UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM2_IN_4                      (0x26UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM2_IN_5                      (0x27UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM2_IN_6                      (0x28UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM2_IN_7                      (0x29UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM3_IN_0                      (0x2AUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM3_IN_1                      (0x2BUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM3_IN_2                      (0x2CUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM3_IN_3                      (0x2DUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM3_IN_4                      (0x2EUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM3_IN_5                      (0x2FUL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM3_IN_6                      (0x30UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM3_IN_7                      (0x31UL)  /*  */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRIGO_8                   (0x32UL)  /* PWM0 trigger out8 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRIGO_9                   (0x33UL)  /* PWM0 trigger out9 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRIGO_10                  (0x34UL)  /* PWM0 trigger out10 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRIGO_11                  (0x35UL)  /* PWM0 trigger out11 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRIGO_12                  (0x36UL)  /* PWM0 trigger out12 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRIGO_13                  (0x37UL)  /* PWM0 trigger out13 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRIGO_14                  (0x38UL)  /* PWM0 trigger out14 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRIGO_15                  (0x39UL)  /* PWM0 trigger out15 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRIGO_8                   (0x3AUL)  /* PWM1 trigger out8 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRIGO_9                   (0x3BUL)  /* PWM1 trigger out9 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRIGO_10                  (0x3CUL)  /* PWM1 trigger out10 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRIGO_11                  (0x3DUL)  /* PWM1 trigger out11 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRIGO_12                  (0x3EUL)  /* PWM1 trigger out12 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRIGO_13                  (0x3FUL)  /* PWM1 trigger out13 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRIGO_14                  (0x40UL)  /* PWM1 trigger out14 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRIGO_15                  (0x41UL)  /* PWM1 trigger out15 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRIGO_8                   (0x42UL)  /* PWM2 trigger out8 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRIGO_9                   (0x43UL)  /* PWM2 trigger out9 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRIGO_10                  (0x44UL)  /* PWM2 trigger out10 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRIGO_11                  (0x45UL)  /* PWM2 trigger out11 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRIGO_12                  (0x46UL)  /* PWM2 trigger out12 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRIGO_13                  (0x47UL)  /* PWM2 trigger out13 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRIGO_14                  (0x48UL)  /* PWM2 trigger out14 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRIGO_15                  (0x49UL)  /* PWM2 trigger out15 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRIGO_8                   (0x4AUL)  /* PWM3 trigger out8 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRIGO_9                   (0x4BUL)  /* PWM3 trigger out9 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRIGO_10                  (0x4CUL)  /* PWM3 trigger out10 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRIGO_11                  (0x4DUL)  /* PWM3 trigger out11 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRIGO_12                  (0x4EUL)  /* PWM3 trigger out12 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRIGO_13                  (0x4FUL)  /* PWM3 trigger out13 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRIGO_14                  (0x50UL)  /* PWM3 trigger out14 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRIGO_15                  (0x51UL)  /* PWM3 trigger out15 */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P00                      (0x52UL)  /* TRGM input data0(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P01                      (0x53UL)  /* TRGM input data1(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P02                      (0x54UL)  /* TRGM input data2(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P03                      (0x55UL)  /* TRGM input data3(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P04                      (0x56UL)  /* TRGM input data4(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P05                      (0x57UL)  /* TRGM input data5(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P06                      (0x58UL)  /* TRGM input data6(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P07                      (0x59UL)  /* TRGM input data7(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P08                      (0x5AUL)  /* TRGM input data8(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P09                      (0x5BUL)  /* TRGM input data9(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P10                      (0x5CUL)  /* TRGM input data10(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P11                      (0x5DUL)  /* TRGM input data11(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P12                      (0x5EUL)  /* TRGM input data12(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P13                      (0x5FUL)  /* TRGM input data13(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P14                      (0x60UL)  /* TRGM input data14(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P15                      (0x61UL)  /* TRGM input data15(from IO) */
#define HPM_TRGM0_INPUT_SRC_ADC0_TRIGO                     (0x62UL)  /* ADC0 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC1_TRIGO                     (0x63UL)  /* ADC1 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC2_TRIGO                     (0x64UL)  /* ADC2 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC3_TRIGO                     (0x65UL)  /* ADC3 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC0_ADC_VALID                 (0x66UL)  /* ADC0 ADC data valid */
#define HPM_TRGM0_INPUT_SRC_ADC1_ADC_VALID                 (0x67UL)  /* ADC1 ADC data valid */
#define HPM_TRGM0_INPUT_SRC_ADC2_ADC_VALID                 (0x68UL)  /* ADC2 ADC data valid */
#define HPM_TRGM0_INPUT_SRC_ADC3_ADC_VALID                 (0x69UL)  /* ADC3 ADC data valid */
#define HPM_TRGM0_INPUT_SRC_ACMP0_CH0_OUT                  (0x6AUL)  /* ACMP0 CH0 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP0_CH1_OUT                  (0x6BUL)  /* ACMP0 CH1 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_CH0_OUT                  (0x6CUL)  /* ACMP1 CH0 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_CH1_OUT                  (0x6DUL)  /* ACMP1 CH1 compare output */

/* trgm0_output mux definitions */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P00                     (0x0UL)   /* TRGM output data0(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P01                     (0x1UL)   /* TRGM output data1(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P02                     (0x2UL)   /* TRGM output data2(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P03                     (0x3UL)   /* TRGM output data3(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P04                     (0x4UL)   /* TRGM output data4(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P05                     (0x5UL)   /* TRGM output data5(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P06                     (0x6UL)   /* TRGM output data6(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P07                     (0x7UL)   /* TRGM output data7(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P08                     (0x8UL)   /* TRGM output data8(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P09                     (0x9UL)   /* TRGM output data9(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P10                     (0xAUL)   /* TRGM output data10(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P11                     (0xBUL)   /* TRGM output data11(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P12                     (0xCUL)   /* TRGM output data12(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P13                     (0xDUL)   /* TRGM output data13(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P14                     (0xEUL)   /* TRGM output data14(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P15                     (0xFUL)   /* TRGM output data15(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN_0                     (0x10UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN_1                     (0x11UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN_2                     (0x12UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN_3                     (0x13UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN_4                     (0x14UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN_5                     (0x15UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN_6                     (0x16UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN_7                     (0x17UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FORCE                    (0x18UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FORCE_SYNC_PULSE         (0x19UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_RELOAD_SYNC_PULSE        (0x1AUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_SHADOW_SYNC_PULSE        (0x1BUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_IN_0                     (0x1CUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_IN_1                     (0x1DUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_IN_2                     (0x1EUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_IN_3                     (0x1FUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_IN_4                     (0x20UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_IN_5                     (0x21UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_IN_6                     (0x22UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_IN_7                     (0x23UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_FORCE                    (0x24UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_FORCE_SYNC_PULSE         (0x25UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_RELOAD_SYNC_PULSE        (0x26UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_SHADOW_SYNC_PULSE        (0x27UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_IN_0                     (0x28UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_IN_1                     (0x29UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_IN_2                     (0x2AUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_IN_3                     (0x2BUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_IN_4                     (0x2CUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_IN_5                     (0x2DUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_IN_6                     (0x2EUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_IN_7                     (0x2FUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_FORCE                    (0x30UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_FORCE_SYNC_PULSE         (0x31UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_RELOAD_SYNC_PULSE        (0x32UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_SHADOW_SYNC_PULSE        (0x33UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_IN_0                     (0x34UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_IN_1                     (0x35UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_IN_2                     (0x36UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_IN_3                     (0x37UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_IN_4                     (0x38UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_IN_5                     (0x39UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_IN_6                     (0x3AUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_IN_7                     (0x3BUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_FORCE                    (0x3CUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_FORCE_SYNC_PULSE         (0x3DUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_RELOAD_SYNC_PULSE        (0x3EUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_SHADOW_SYNC_PULSE        (0x3FUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_ADC0_STRGI                    (0x40UL)  /* ADC0 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC1_STRGI                    (0x41UL)  /* ADC1 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC2_STRGI                    (0x42UL)  /* ADC2 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC3_STRGI                    (0x43UL)  /* ADC3 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0A                  (0x44UL)  /* ADC preemption trigger0A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0B                  (0x45UL)  /* ADC preemption trigger0B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0C                  (0x46UL)  /* ADC preemption trigger0C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1A                  (0x47UL)  /* ADC preemption trigger1A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1B                  (0x48UL)  /* ADC preemption trigger1B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1C                  (0x49UL)  /* ADC preemption trigger1C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2A                  (0x4AUL)  /* ADC preemption trigger2A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2B                  (0x4BUL)  /* ADC preemption trigger2B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2C                  (0x4CUL)  /* ADC preemption trigger2C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3A                  (0x4DUL)  /* ADC preemption trigger3A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3B                  (0x4EUL)  /* ADC preemption trigger3B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3C                  (0x4FUL)  /* ADC preemption trigger3C */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_CH0_WIN                 (0x50UL)  /* ACMP0 CH0 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_CH1_WIN                 (0x51UL)  /* ACMP0 CH1 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP1_CH0_WIN                 (0x52UL)  /* ACMP1 CH0 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP1_CH1_WIN                 (0x53UL)  /* ACMP1 CH1 window */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN2                    (0x54UL)  /* GPTMR0 channel2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN3                    (0x55UL)  /* GPTMR0 channel3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN2                    (0x56UL)  /* GPTMR1 channel2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN3                    (0x57UL)  /* GPTMR1 channel3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_SYNCI                  (0x58UL)  /* GPTMR0 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_SYNCI                  (0x59UL)  /* GPTMR1 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_PTPC_CAPT0                    (0x5AUL)  /* PTPC capture in0 */
#define HPM_TRGM0_OUTPUT_SRC_PTPC_CAPT1                    (0x5BUL)  /* PTPC capture in1 */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG0                    (0x5CUL)  /* UART0 ~ UART1 trigger event */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG1                    (0x5DUL)  /* UART2 ~ UART3 trigger event */
#define HPM_TRGM0_OUTPUT_SRC_SYNCTIMER_TRIG                (0x5EUL)  /* SYNT sync trigger */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ0                     (0x5FUL)  /* TRGM interrupt0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ1                     (0x60UL)  /* TRGM interrupt1 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA0                     (0x61UL)  /* TRGM dma request0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA1                     (0x62UL)  /* TRGM dma request1 */
#define HPM_TRGM0_OUTPUT_SRC_OPA0_TRIGGER_0                (0x63UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA0_TRIGGER_1                (0x64UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA0_TRIGGER_2                (0x65UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA0_TRIGGER_3                (0x66UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA0_TRIGGER_4                (0x67UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA0_TRIGGER_5                (0x68UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA0_TRIGGER_6                (0x69UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA0_TRIGGER_7                (0x6AUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA1_TRIGGER_0                (0x6BUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA1_TRIGGER_1                (0x6CUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA1_TRIGGER_2                (0x6DUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA1_TRIGGER_3                (0x6EUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA1_TRIGGER_4                (0x6FUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA1_TRIGGER_5                (0x70UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA1_TRIGGER_6                (0x71UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA1_TRIGGER_7                (0x72UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA2_TRIGGER_0                (0x73UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA2_TRIGGER_1                (0x74UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA2_TRIGGER_2                (0x75UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA2_TRIGGER_3                (0x76UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA2_TRIGGER_4                (0x77UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA2_TRIGGER_5                (0x78UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA2_TRIGGER_6                (0x79UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA2_TRIGGER_7                (0x7AUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA3_TRIGGER_0                (0x7BUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA3_TRIGGER_1                (0x7CUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA3_TRIGGER_2                (0x7DUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA3_TRIGGER_3                (0x7EUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA3_TRIGGER_4                (0x7FUL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA3_TRIGGER_5                (0x80UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA3_TRIGGER_6                (0x81UL)  /*  */
#define HPM_TRGM0_OUTPUT_SRC_OPA3_TRIGGER_7                (0x82UL)  /*  */

/* trgm0_filter mux definitions */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN0                      (0x0UL)   /* PWM0 capture in0 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN1                      (0x1UL)   /* PWM0 capture in1 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN2                      (0x2UL)   /* PWM0 capture in2 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN3                      (0x3UL)   /* PWM0 capture in3 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN4                      (0x4UL)   /* PWM0 capture in4 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN5                      (0x5UL)   /* PWM0 capture in5 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN6                      (0x6UL)   /* PWM0 capture in6 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN7                      (0x7UL)   /* PWM0 capture in7 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN0                      (0x8UL)   /* PWM1 capture in0 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN1                      (0x9UL)   /* PWM1 capture in1 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN2                      (0xAUL)   /* PWM1 capture in2 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN3                      (0xBUL)   /* PWM1 capture in3 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN4                      (0xCUL)   /* PWM1 capture in4 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN5                      (0xDUL)   /* PWM1 capture in5 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN6                      (0xEUL)   /* PWM1 capture in6 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN7                      (0xFUL)   /* PWM1 capture in7 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN0                      (0x10UL)  /* PWM2 capture in0 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN1                      (0x11UL)  /* PWM2 capture in1 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN2                      (0x12UL)  /* PWM2 capture in2 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN3                      (0x13UL)  /* PWM2 capture in3 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN4                      (0x14UL)  /* PWM2 capture in4 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN5                      (0x15UL)  /* PWM2 capture in5 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN6                      (0x16UL)  /* PWM2 capture in6 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN7                      (0x17UL)  /* PWM2 capture in7 */
#define HPM_TRGM0_FILTER_SRC_PWM3_IN0                      (0x18UL)  /* PWM3 capture in0 */
#define HPM_TRGM0_FILTER_SRC_PWM3_IN1                      (0x19UL)  /* PWM3 capture in1 */
#define HPM_TRGM0_FILTER_SRC_PWM3_IN2                      (0x1AUL)  /* PWM3 capture in2 */
#define HPM_TRGM0_FILTER_SRC_PWM3_IN3                      (0x1BUL)  /* PWM3 capture in3 */
#define HPM_TRGM0_FILTER_SRC_PWM3_IN4                      (0x1CUL)  /* PWM3 capture in4 */
#define HPM_TRGM0_FILTER_SRC_PWM3_IN5                      (0x1DUL)  /* PWM3 capture in5 */
#define HPM_TRGM0_FILTER_SRC_PWM3_IN6                      (0x1EUL)  /* PWM3 capture in6 */
#define HPM_TRGM0_FILTER_SRC_PWM3_IN7                      (0x1FUL)  /* PWM3 capture in7 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P00                      (0x20UL)  /* TRGM IO input 00 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P01                      (0x21UL)  /* TRGM IO input 01 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P02                      (0x22UL)  /* TRGM IO input 02 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P03                      (0x23UL)  /* TRGM IO input 03 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P04                      (0x24UL)  /* TRGM IO input 04 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P05                      (0x25UL)  /* TRGM IO input 05 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P06                      (0x26UL)  /* TRGM IO input 06 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P07                      (0x27UL)  /* TRGM IO input 07 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P08                      (0x28UL)  /* TRGM IO input 08 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P09                      (0x29UL)  /* TRGM IO input 09 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P10                      (0x2AUL)  /* TRGM IO input 10 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P11                      (0x2BUL)  /* TRGM IO input 11 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P12                      (0x2CUL)  /* TRGM IO input 12 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P13                      (0x2DUL)  /* TRGM IO input 13 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P14                      (0x2EUL)  /* TRGM IO input 14 */
#define HPM_TRGM0_FILTER_SRC_TRGM_P15                      (0x2FUL)  /* TRGM IO input 15 */

/* trgm0_dma mux definitions */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP21                       (0x0UL)   /* The capture input or matches output of PWM timer 0 comparator 21 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP22                       (0x1UL)   /* The capture input or matches output of PWM timer 0 comparator 22 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP23                       (0x2UL)   /* The capture input or matches output of PWM timer 0 comparator 23 */
#define HPM_TRGM0_DMA_SRC_PWM0_RLD                         (0x3UL)   /* PWM timer 0 counter reload */
#define HPM_TRGM0_DMA_SRC_PWM0_HALFRLD                     (0x4UL)   /* PWM timer 0 half cycle reload */
#define HPM_TRGM0_DMA_SRC_PWM0_XRLD                        (0x5UL)   /* PWM timer 0 extended counter reload */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP21                       (0x6UL)   /* The capture input or matches output of PWM timer 1 comparator 21 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP22                       (0x7UL)   /* The capture input or matches output of PWM timer 1 comparator 22 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP23                       (0x8UL)   /* The capture input or matches output of PWM timer 1 comparator 23 */
#define HPM_TRGM0_DMA_SRC_PWM1_RLD                         (0x9UL)   /* PWM timer 1 counter reload */
#define HPM_TRGM0_DMA_SRC_PWM1_HALFRLD                     (0xAUL)   /* PWM timer 1 half cycle reload */
#define HPM_TRGM0_DMA_SRC_PWM1_XRLD                        (0xBUL)   /* PWM timer 1 extended counter reload */
#define HPM_TRGM0_DMA_SRC_PWM2_CMP21                       (0xCUL)   /* The capture input or matches output of PWM timer 2 comparator 21 */
#define HPM_TRGM0_DMA_SRC_PWM2_CMP22                       (0xDUL)   /* The capture input or matches output of PWM timer 2 comparator 22 */
#define HPM_TRGM0_DMA_SRC_PWM2_CMP23                       (0xEUL)   /* The capture input or matches output of PWM timer 2 comparator 23 */
#define HPM_TRGM0_DMA_SRC_PWM2_RLD                         (0xFUL)   /* PWM timer 2 counter reload */
#define HPM_TRGM0_DMA_SRC_PWM2_HALFRLD                     (0x10UL)  /* PWM timer 2 half cycle reload */
#define HPM_TRGM0_DMA_SRC_PWM2_XRLD                        (0x11UL)  /* PWM timer 2 extended counter reload */
#define HPM_TRGM0_DMA_SRC_PWM3_CMP21                       (0x12UL)  /* The capture input or matches output of PWM timer 3 comparator 21 */
#define HPM_TRGM0_DMA_SRC_PWM3_CMP22                       (0x13UL)  /* The capture input or matches output of PWM timer 3 comparator 22 */
#define HPM_TRGM0_DMA_SRC_PWM3_CMP23                       (0x14UL)  /* The capture input or matches output of PWM timer 3 comparator 23 */
#define HPM_TRGM0_DMA_SRC_PWM3_RLD                         (0x15UL)  /* PWM timer 3 counter reload */
#define HPM_TRGM0_DMA_SRC_PWM3_HALFRLD                     (0x16UL)  /* PWM timer 3 half cycle reload */
#define HPM_TRGM0_DMA_SRC_PWM3_XRLD                        (0x17UL)  /* PWM timer 3 extended counter reload */
#define HPM_TRGM0_DMA_SRC_TRGM0                            (0x18UL)  /* TRGM DMA0 */
#define HPM_TRGM0_DMA_SRC_TRGM1                            (0x19UL)  /* TRGM DMA1 */



#endif /* HPM_TRGMMUX_SRC_H */
