/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_TRGMMUX_SRC_H
#define HPM_TRGMMUX_SRC_H

/* trgm0_input mux definitions */
#define HPM_TRGM0_INPUT_SRC_VSS                            (0x0UL)   /* Low level */
#define HPM_TRGM0_INPUT_SRC_VDD                            (0x1UL)   /* High level */
#define HPM_TRGM0_INPUT_SRC_DEBUG_FLAG                     (0x2UL)   /* the flag bit of debug mode enters */
#define HPM_TRGM0_INPUT_SRC_USB0_SOF                       (0x3UL)   /* USB0 frame start */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP0                      (0x4UL)   /* PTPC output comparison 0 */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP1                      (0x5UL)   /* PTPC output comparison 1 */
#define HPM_TRGM0_INPUT_SRC_ACMP0_OUT                      (0x6UL)   /* Comparator 0 output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_OUT                      (0x7UL)   /* Comparator 1 output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT2                    (0x8UL)   /* GPTMR0 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT3                    (0x9UL)   /* GPTMR0 channel 3 */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT2                    (0xAUL)   /* GPTMR1 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT3                    (0xBUL)   /* GPTMR1 channel 3 */
#define HPM_TRGM0_INPUT_SRC_GPTMR2_OUT2                    (0xCUL)   /* GPTMR2 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR2_OUT3                    (0xDUL)   /* GPTMR2 channel 3 */
#define HPM_TRGM0_INPUT_SRC_GPTMR3_OUT2                    (0xEUL)   /* GPTMR3 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR3_OUT3                    (0xFUL)   /* GPTMR3 channel 3 */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P0                       (0x10UL)  /* TRGM0 Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P1                       (0x11UL)  /* TRGM0 Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P2                       (0x12UL)  /* TRGM0 Input 2 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P3                       (0x13UL)  /* TRGM0 Input 3 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P4                       (0x14UL)  /* TRGM0 Input 4 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P5                       (0x15UL)  /* TRGM0 Input 5 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P6                       (0x16UL)  /* TRGM0 Input 6 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P7                       (0x17UL)  /* TRGM0 Input 7 (from IO) */
#define HPM_TRGM0_INPUT_SRC_SYNT0_CH0                      (0x18UL)  /* SYNT0 Channel 0 */
#define HPM_TRGM0_INPUT_SRC_SYNT0_CH1                      (0x19UL)  /* SYNT0 Channel 1 */
#define HPM_TRGM0_INPUT_SRC_SYNT0_CH2                      (0x1AUL)  /* SYNT0 Channel 2 */
#define HPM_TRGM0_INPUT_SRC_SYNT0_CH3                      (0x1BUL)  /* SYNT0 channel 3 */
#define HPM_TRGM0_INPUT_SRC_MMC0_TRGO_0                    (0x1CUL)  /* MMC0 triggers output 0 */
#define HPM_TRGM0_INPUT_SRC_MMC0_TRGO_1                    (0x1DUL)  /* MMC0 triggers output 1 */
#define HPM_TRGM0_INPUT_SRC_MMC1_TRGO_0                    (0x1EUL)  /* MMC1 triggers output 0 */
#define HPM_TRGM0_INPUT_SRC_MMC1_TRGO_1                    (0x1FUL)  /* MMC1 triggers output 1 */
#define HPM_TRGM0_INPUT_SRC_QEO0_TRGO_0                    (0x20UL)  /* QEO0 triggers output 0 */
#define HPM_TRGM0_INPUT_SRC_QEO0_TRGO_1                    (0x21UL)  /* QEO0 triggers output 1 */
#define HPM_TRGM0_INPUT_SRC_QEO0_TRGO_2                    (0x22UL)  /* QEO0 triggers output 2 */
#define HPM_TRGM0_INPUT_SRC_QEO0_TRGO_3                    (0x23UL)  /* QEO0 triggers output 3 */
#define HPM_TRGM0_INPUT_SRC_QEO0_TRGO_4                    (0x24UL)  /* QEO0 triggers output 4 */
#define HPM_TRGM0_INPUT_SRC_QEO0_TRGO_5                    (0x25UL)  /* QEO0 triggers output 5 */
#define HPM_TRGM0_INPUT_SRC_QEO0_TRGO_6                    (0x26UL)  /* QEO0 triggers output 6 */
#define HPM_TRGM0_INPUT_SRC_QEO0_TRGO_7                    (0x27UL)  /* QEO0 triggers output 7 */
#define HPM_TRGM0_INPUT_SRC_QEO1_TRGO_0                    (0x28UL)  /* QEO1 triggers output 0 */
#define HPM_TRGM0_INPUT_SRC_QEO1_TRGO_1                    (0x29UL)  /* QEO1 triggers output 1 */
#define HPM_TRGM0_INPUT_SRC_QEO1_TRGO_2                    (0x2AUL)  /* QEO1 triggers output 2 */
#define HPM_TRGM0_INPUT_SRC_QEO1_TRGO_3                    (0x2BUL)  /* QEO1 triggers output 3 */
#define HPM_TRGM0_INPUT_SRC_QEO1_TRGO_4                    (0x2CUL)  /* QEO1 triggers output 4 */
#define HPM_TRGM0_INPUT_SRC_QEO1_TRGO_5                    (0x2DUL)  /* QEO1 triggers output 5 */
#define HPM_TRGM0_INPUT_SRC_QEO1_TRGO_6                    (0x2EUL)  /* QEO1 triggers output 6 */
#define HPM_TRGM0_INPUT_SRC_QEO1_TRGO_7                    (0x2FUL)  /* QEO1 triggers output 7 */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH8REF                    (0x30UL)  /* PWM timer 0 channel 8 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH9REF                    (0x31UL)  /* PWM timer 0 channel 9 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH10REF                   (0x32UL)  /* PWM timer 0 channel 10 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH11REF                   (0x33UL)  /* PWM timer 0 channel 11 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH12REF                   (0x34UL)  /* PWM timer 0 channel 12 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH13REF                   (0x35UL)  /* PWM timer 0 channel 13 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH14REF                   (0x36UL)  /* PWM timer 0 channel 14 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH15REF                   (0x37UL)  /* PWM timer 0 channel 15 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM1_CH8REF                    (0x38UL)  /* PWM timer 1 channel 8 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM1_CH9REF                    (0x39UL)  /* PWM timer 1 channel 9 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM1_CH10REF                   (0x3AUL)  /* PWM timer 1 channel 10 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM1_CH11REF                   (0x3BUL)  /* PWM timer 1 channel 11 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM1_CH12REF                   (0x3CUL)  /* PWM timer 1 channel 12 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM1_CH13REF                   (0x3DUL)  /* PWM timer 1 channel 13 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM1_CH14REF                   (0x3EUL)  /* PWM timer 1 channel 14 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM1_CH15REF                   (0x3FUL)  /* PWM timer 1 channel 15 reference output */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT00                      (0x40UL)  /* PLB module output 0 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT01                      (0x41UL)  /* PLB module output 1 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT02                      (0x42UL)  /* PLB module output 2 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT03                      (0x43UL)  /* PLB module output 3 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT04                      (0x44UL)  /* PLB module output 4 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT05                      (0x45UL)  /* PLB module output 5 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT06                      (0x46UL)  /* PLB module output 6 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT07                      (0x47UL)  /* PLB module output 7 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT08                      (0x48UL)  /* PLB module output 8 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT09                      (0x49UL)  /* PLB module output 9 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT10                      (0x4AUL)  /* PLB module output 10 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT11                      (0x4BUL)  /* PLB module output 11 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT12                      (0x4CUL)  /* PLB module output 12 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT13                      (0x4DUL)  /* PLB module output 13 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT14                      (0x4EUL)  /* PLB module output 14 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT15                      (0x4FUL)  /* PLB module output 15 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT16                      (0x50UL)  /* PLB module output 16 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT17                      (0x51UL)  /* PLB module output 17 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT18                      (0x52UL)  /* PLB module output 18 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT19                      (0x53UL)  /* PLB module output 19 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT20                      (0x54UL)  /* PLB module output 20 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT21                      (0x55UL)  /* PLB module output 21 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT22                      (0x56UL)  /* PLB module output 22 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT23                      (0x57UL)  /* PLB module output 23 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT24                      (0x58UL)  /* PLB module output 24 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT25                      (0x59UL)  /* PLB module output 25 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT26                      (0x5AUL)  /* PLB module output 26 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT27                      (0x5BUL)  /* PLB module output 27 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT28                      (0x5CUL)  /* PLB module output 28 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT29                      (0x5DUL)  /* PLB module output 29 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT30                      (0x5EUL)  /* PLB module output 30 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT31                      (0x5FUL)  /* PLB module output 31 */
#define HPM_TRGM0_INPUT_SRC_RDC_TRGO_0                     (0x60UL)  /* RDC triggers output 0 */
#define HPM_TRGM0_INPUT_SRC_RDC_TRGO_1                     (0x61UL)  /* RDC triggers output 1 */
#define HPM_TRGM0_INPUT_SRC_QEI1_TRGO                      (0x62UL)  /* QEI1 triggers output */
#define HPM_TRGM0_INPUT_SRC_QEI0_TRGO                      (0x63UL)  /* QEI0 triggers output */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_0                     (0x64UL)  /* SEI triggers output 0 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_1                     (0x65UL)  /* SEI triggers output 1 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_2                     (0x66UL)  /* SEI triggers output 2 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_3                     (0x67UL)  /* SEI triggers output 3 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_4                     (0x68UL)  /* SEI triggers output 4 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_5                     (0x69UL)  /* SEI triggers output 5 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_6                     (0x6AUL)  /* SEI triggers output 6 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_7                     (0x6BUL)  /* SEI triggers output 7 */
#define HPM_TRGM0_INPUT_SRC_PWM0_FAULT0                    (0x6CUL)  /* PWM0 fault Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_FAULT1                    (0x6DUL)  /* PWM0 fault Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_FAULT0                    (0x6EUL)  /* PWM1 fault Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_FAULT1                    (0x6FUL)  /* PWM1 fault Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN0                    (0x70UL)  /* PWM0 Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN1                    (0x71UL)  /* PWM0 Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN2                    (0x72UL)  /* PWM0 Input 2 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN3                    (0x73UL)  /* PWM0 Input 3 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN4                    (0x74UL)  /* PWM0 Input 4 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN5                    (0x75UL)  /* PWM0 Input 5 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN6                    (0x76UL)  /* PWM0 Input 6 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN7                    (0x77UL)  /* PWM0 Input 7 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN0                    (0x78UL)  /* PWM1 Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN1                    (0x79UL)  /* PWM1 Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN2                    (0x7AUL)  /* PWM1 Input 2 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN3                    (0x7BUL)  /* PWM1 Input 3 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN4                    (0x7CUL)  /* PWM1 Input 4 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN5                    (0x7DUL)  /* PWM1 Input 5 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN6                    (0x7EUL)  /* PWM1 Input 6 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN7                    (0x7FUL)  /* PWM1 Input 7 (from IO) */

/* trgm0_output mux definitions */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP0_0                  (0x0UL)   /* OPAMP0 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP0_1                  (0x1UL)   /* OPAMP0 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP0_2                  (0x2UL)   /* OPAMP0 triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP0_3                  (0x3UL)   /* OPAMP0 triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP0_4                  (0x4UL)   /* OPAMP0 triggers input 4 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP0_5                  (0x5UL)   /* OPAMP0 triggers input 5 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP0_6                  (0x6UL)   /* OPAMP0 triggers input 6 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP0_7                  (0x7UL)   /* OPAMP0 triggers input 7 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP1_0                  (0x8UL)   /* OPAMP1 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP1_1                  (0x9UL)   /* OPAMP1 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP1_2                  (0xAUL)   /* OPAMP1 triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP1_3                  (0xBUL)   /* OPAMP1 triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP1_4                  (0xCUL)   /* OPAMP1 triggers input 4 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP1_5                  (0xDUL)   /* OPAMP1 triggers input 5 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP1_6                  (0xEUL)   /* OPAMP1 triggers input 6 */
#define HPM_TRGM0_OUTPUT_SRC_MOT2OPAMP1_7                  (0xFUL)   /* OPAMP1 triggers input 7 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN2                    (0x10UL)  /* GPTMR0 channel 2 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN3                    (0x11UL)  /* GPTMR0 channel 3 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_SYNCI                  (0x12UL)  /* GPTMR0 counter synchronous input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN2                    (0x13UL)  /* GPTMR1 channel 2 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN3                    (0x14UL)  /* GPTMR1 channel 3 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_SYNCI                  (0x15UL)  /* GPTMR1 counter synchronous input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_IN2                    (0x16UL)  /* GPTMR2 channel 2 input; This bit is also activated as DAC0 ladder mode 1 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_IN3                    (0x17UL)  /* GPTMR2 channel 3 input; This bit is also activated as DAC0 ladder mode 2 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_SYNCI                  (0x18UL)  /* GPTMR2 counter synchronous input; This bit is also activated as DAC0 ladder mode 3 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_IN2                    (0x19UL)  /* GPTMR3 channel 2 input; This bit is also activated as DAC1 ladder mode 1 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_IN3                    (0x1AUL)  /* GPTMR3 channel 3 input; This bit is also activated as DAC1 ladder mode 2 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_SYNCI                  (0x1BUL)  /* GPTMR3 counter synchronous input; This bit is also activated as DAC1 ladder mode 3 */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_WIN                     (0x1CUL)  /* Comparator 0 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP1_WIN                     (0x1DUL)  /* Comparator 1 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_DAC0_BUFTRG                   (0x1EUL)  /* DAC0 buffer mode starts to trigger; This bit is also activated as DAC0 ladder mode 0 */
#define HPM_TRGM0_OUTPUT_SRC_DAC1_BUFTRG                   (0x1FUL)  /* DAC1 buffer mode starts to trigger; This bit is also activated as DAC1 ladder mode 0 */
#define HPM_TRGM0_OUTPUT_SRC_ADC0_STRGI                    (0x20UL)  /* The sequence conversion of ADC0 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_ADC1_STRGI                    (0x21UL)  /* The sequence conversion of ADC1 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0A                  (0x22UL)  /* The preemption conversion of ADC0, 1 triggers input 0A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0B                  (0x23UL)  /* The preemption conversion of ADC0, 1 triggers input 0B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0C                  (0x24UL)  /* The preemption conversion of ADC0, 1 triggers input 0C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1A                  (0x25UL)  /* The preemption conversion of ADC0, 1 triggers input 1A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1B                  (0x26UL)  /* The preemption conversion of ADC0, 1 triggers input 1B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1C                  (0x27UL)  /* The preemption conversion of ADC0, 1 triggers input 1C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2A                  (0x28UL)  /* The preemption conversion of ADC0, 1 triggers input 2A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2B                  (0x29UL)  /* The preemption conversion of ADC0, 1 triggers input 2B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2C                  (0x2AUL)  /* The preemption conversion of ADC0, 1 triggers input 2C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3A                  (0x2BUL)  /* The preemption conversion of ADC0, 1 triggers input 3A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3B                  (0x2CUL)  /* The preemption conversion of ADC0, 1 triggers input 3B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3C                  (0x2DUL)  /* The preemption conversion of ADC0, 1 triggers input 3C */
#define HPM_TRGM0_OUTPUT_SRC_MCAN_PTPC0_CAP                (0x2EUL)  /* PTPC input capture 0 */
#define HPM_TRGM0_OUTPUT_SRC_MCAN_PTPC1_CAP                (0x2FUL)  /* PTPC input capture 1 */
#define HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN0                 (0x30UL)  /* QEO0 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1                 (0x31UL)  /* QEO0 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_QEO1_TRIG_IN0                 (0x32UL)  /* QEO1 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO1_TRIG_IN1                 (0x33UL)  /* QEO1 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN0                  (0x34UL)  /* SEI triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN1                  (0x35UL)  /* SEI triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN2                  (0x36UL)  /* SEI triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN3                  (0x37UL)  /* SEI triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN4                  (0x38UL)  /* SEI triggers input 4 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN5                  (0x39UL)  /* SEI triggers input 5 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN6                  (0x3AUL)  /* SEI triggers input 6 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN7                  (0x3BUL)  /* SEI triggers input 7 */
#define HPM_TRGM0_OUTPUT_SRC_MMC0_TRIG_IN0                 (0x3CUL)  /* MMC0 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_MMC0_TRIG_IN1                 (0x3DUL)  /* MMC0 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_MMC1_TRIG_IN0                 (0x3EUL)  /* MMC1 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_MMC1_TRIG_IN1                 (0x3FUL)  /* MMC1 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_00                     (0x40UL)  /* PLB module input 00 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_01                     (0x41UL)  /* PLB module input 01 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_02                     (0x42UL)  /* PLB module input 02 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_03                     (0x43UL)  /* PLB module input 03 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_04                     (0x44UL)  /* PLB module input 04 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_05                     (0x45UL)  /* PLB module input 05 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_06                     (0x46UL)  /* PLB module input 06 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_07                     (0x47UL)  /* PLB module input 07 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_08                     (0x48UL)  /* PLB module input 08 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_09                     (0x49UL)  /* PLB module input 09 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_10                     (0x4AUL)  /* PLB module input 10 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_11                     (0x4BUL)  /* PLB module input 11 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_12                     (0x4CUL)  /* PLB module input 12 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_13                     (0x4DUL)  /* PLB module input 13 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_14                     (0x4EUL)  /* PLB module input 14 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_15                     (0x4FUL)  /* PLB module input 15 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_16                     (0x50UL)  /* PLB module input 16 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_17                     (0x51UL)  /* PLB module input 17 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_18                     (0x52UL)  /* PLB module input 18 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_19                     (0x53UL)  /* PLB module input 19 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_20                     (0x54UL)  /* PLB module input 20 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_21                     (0x55UL)  /* PLB module input 21 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_22                     (0x56UL)  /* PLB module input 22 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_23                     (0x57UL)  /* PLB module input 23 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_24                     (0x58UL)  /* PLB module input 24 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_25                     (0x59UL)  /* PLB module input 25 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_26                     (0x5AUL)  /* PLB module input 26 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_27                     (0x5BUL)  /* PLB module input 27 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_28                     (0x5CUL)  /* PLB module input 28 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_29                     (0x5DUL)  /* PLB module input 29 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_30                     (0x5EUL)  /* PLB module input 30 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_31                     (0x5FUL)  /* PLB module input 31 */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO0                     (0x60UL)  /* TRGM Output 0 (output to IO) (dropped macro  please use macro TRGM0_P0) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P0                      (0x60UL)  /* TRGM Output 0 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO1                     (0x61UL)  /* TRGM Output 1 (output to IO) (dropped macro  please use macro TRGM0_P1) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P1                      (0x61UL)  /* TRGM Output 1 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO2                     (0x62UL)  /* TRGM Output 2 (output to IO) (dropped macro  please use macro TRGM0_P2) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P2                      (0x62UL)  /* TRGM Output 2 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO3                     (0x63UL)  /* TRGM Output 3 (output to IO) (dropped macro  please use macro TRGM0_P3) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P3                      (0x63UL)  /* TRGM Output 3 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO4                     (0x64UL)  /* TRGM Output 4 (output to IO) (dropped macro  please use macro TRGM0_P4) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P4                      (0x64UL)  /* TRGM Output 4 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO5                     (0x65UL)  /* TRGM Output 5 (output to IO) (dropped macro  please use macro TRGM0_P5) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P5                      (0x65UL)  /* TRGM Output 5 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO6                     (0x66UL)  /* TRGM Output 6 (output to IO) (dropped macro  please use macro TRGM0_P6) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P6                      (0x66UL)  /* TRGM Output 6 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO7                     (0x67UL)  /* TRGM Output 7 (output to IO) (dropped macro  please use macro TRGM0_P7) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P7                      (0x67UL)  /* TRGM Output 7 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_PWM_IN8                       (0x68UL)  /* PWM timer x capture input 8 */
#define HPM_TRGM0_OUTPUT_SRC_PWM_IN9                       (0x69UL)  /* PWM timer x capture input 9 */
#define HPM_TRGM0_OUTPUT_SRC_PWM_IN10                      (0x6AUL)  /* PWM timer x capture input 10 */
#define HPM_TRGM0_OUTPUT_SRC_PWM_IN11                      (0x6BUL)  /* PWM timer x capture input 11 */
#define HPM_TRGM0_OUTPUT_SRC_PWM_IN12                      (0x6CUL)  /* PWM timer x capture input 12 */
#define HPM_TRGM0_OUTPUT_SRC_PWM_IN13                      (0x6DUL)  /* PWM timer x capture input 13 */
#define HPM_TRGM0_OUTPUT_SRC_PWM_IN14                      (0x6EUL)  /* PWM timer x capture input 14 */
#define HPM_TRGM0_OUTPUT_SRC_PWM_IN15                      (0x6FUL)  /* PWM timer x capture input 15 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FRCI                     (0x70UL)  /* the input value for PWM timer 0 forces control */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FRCSYNCI                 (0x71UL)  /* the synchronous input for PWM timer 0 forces control */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_SYNCI                    (0x72UL)  /* PWM timer 0 counter synchronously triggers input */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_SHRLDSYNCI               (0x73UL)  /* PWM timer 0 Shadow register to activate trigger input */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI0                  (0x74UL)  /* PWM timer 0 Fault protection input 0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI1                  (0x75UL)  /* PWM timer 0 Fault protection input 1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_FRCI                     (0x76UL)  /* the input value for PWM timer 1 forces control */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_FRCSYNCI                 (0x77UL)  /* the synchronous input for PWM timer 1 forces control */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_SYNCI                    (0x78UL)  /* PWM timer 1 Counter synchronizes trigger input */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_SHRLDSYNCI               (0x79UL)  /* PWM timer 1 Shadow register kicks in trigger input */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_FAULTI0                  (0x7AUL)  /* PWM timer 1 Fault protection input 0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_FAULTI1                  (0x7BUL)  /* PWM timer 1 Fault protection Input 1 */
#define HPM_TRGM0_OUTPUT_SRC_RDC_TRIG_IN0                  (0x7CUL)  /* RDC triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_RDC_TRIG_IN1                  (0x7DUL)  /* RDC triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_SYNCTIMER_TRIG                (0x7EUL)  /* SYNT triggers input */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_TRIG_IN                  (0x7FUL)  /* QEI0 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_QEI1_TRIG_IN                  (0x80UL)  /* QEI1 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_PAUSE                    (0x81UL)  /* QEI0 Pause input */
#define HPM_TRGM0_OUTPUT_SRC_QEI1_PAUSE                    (0x82UL)  /* QEI1 Pause input */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG0                    (0x83UL)  /* UART0/1/2/3 Trigger signal */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG1                    (0x84UL)  /* UART4/5/6/7 Trigger signal */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ0                     (0x85UL)  /* TRGM interrupt signal 0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ1                     (0x86UL)  /* TRGM interrupt signal 1 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA0                     (0x87UL)  /* TRGM DMA request 0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA1                     (0x88UL)  /* TRGM DMA request 1 */

/* trgm0_filter mux definitions */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN0                      (0x0UL)   /* PWM timer 0 capture Input 0 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN1                      (0x1UL)   /* PWM timer 0 capture Input 1 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN2                      (0x2UL)   /* PWM timer 0 capture Input 2 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN3                      (0x3UL)   /* PWM timer 0 capture Input 3 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN4                      (0x4UL)   /* PWM timer 0 capture Input 4 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN5                      (0x5UL)   /* PWM timer 0 capture Input 5 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN6                      (0x6UL)   /* PWM timer 0 capture Input 6 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN7                      (0x7UL)   /* PWM timer 0 capture Input 7 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN0                      (0x8UL)   /* PWM timer 1 capture Input 0 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN1                      (0x9UL)   /* PWM timer 1 capture Input 1 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN2                      (0xAUL)   /* PWM timer 1 capture Input 2 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN3                      (0xBUL)   /* PWM timer 1 capture Input 3 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN4                      (0xCUL)   /* PWM timer 1 capture Input 4 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN5                      (0xDUL)   /* PWM timer 1 capture Input 5 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN6                      (0xEUL)   /* PWM timer 1 capture Input 6 */
#define HPM_TRGM0_FILTER_SRC_PWM1_IN7                      (0xFUL)   /* PWM timer 1 capture Input 7 */
#define HPM_TRGM0_FILTER_SRC_TRGM_IN0                      (0x10UL)  /* TRGM input 0 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM_P00                      (0x10UL)  /* TRGM input 0 from IO */
#define HPM_TRGM0_FILTER_SRC_TRGM_IN1                      (0x11UL)  /* TRGM input 1 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM_P01                      (0x11UL)  /* TRGM input 1 from IO */
#define HPM_TRGM0_FILTER_SRC_TRGM_IN2                      (0x12UL)  /* TRGM input 2 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM_P02                      (0x12UL)  /* TRGM input 2 from IO */
#define HPM_TRGM0_FILTER_SRC_TRGM_IN3                      (0x13UL)  /* TRGM input 3 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM_P03                      (0x13UL)  /* TRGM input 3 from IO */
#define HPM_TRGM0_FILTER_SRC_TRGM_IN4                      (0x14UL)  /* TRGM input 4 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM_P04                      (0x14UL)  /* TRGM input 4 from IO */
#define HPM_TRGM0_FILTER_SRC_TRGM_IN5                      (0x15UL)  /* TRGM input 5 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM_P05                      (0x15UL)  /* TRGM input 5 from IO */
#define HPM_TRGM0_FILTER_SRC_TRGM_IN6                      (0x16UL)  /* TRGM input 6 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM_P06                      (0x16UL)  /* TRGM input 6 from IO */
#define HPM_TRGM0_FILTER_SRC_TRGM_IN7                      (0x17UL)  /* TRGM input 7 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM_P07                      (0x17UL)  /* TRGM input 7 from IO */
#define HPM_TRGM0_FILTER_SRC_PWM0_FAULT0                   (0x18UL)  /* PWM timer 0 Input fault0 */
#define HPM_TRGM0_FILTER_SRC_PWM0_FAULT1                   (0x19UL)  /* PWM timer 0 Input fault1 */
#define HPM_TRGM0_FILTER_SRC_PWM1_FAULT0                   (0x1AUL)  /* PWM timer 1 Input fault0 */
#define HPM_TRGM0_FILTER_SRC_PWM1_FAULT1                   (0x1BUL)  /* PWM timer 1 Input fault1 */

/* trgm0_dma mux definitions */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP0                        (0x0UL)   /* The capture input or matches output of PWM timer 0 comparator 0 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP1                        (0x1UL)   /* The capture input or matches output of PWM timer 0 comparator 1 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP2                        (0x2UL)   /* The capture input or matches output of PWM timer 0 comparator 2 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP3                        (0x3UL)   /* The capture input or matches output of PWM timer 0 comparator 3 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP4                        (0x4UL)   /* The capture input or matches output of PWM timer 0 comparator 4 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP5                        (0x5UL)   /* The capture input or matches output of PWM timer 0 comparator 5 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP6                        (0x6UL)   /* The capture input or matches output of PWM timer 0 comparator 6 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP7                        (0x7UL)   /* The capture input or matches output of PWM timer 0 comparator 7 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP8                        (0x8UL)   /* The capture input or matches output of PWM timer 0 comparator 8 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP9                        (0x9UL)   /* The capture input or matches output of PWM timer 0 comparator 9 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP10                       (0xAUL)   /* The capture input or matches output of PWM timer 0 comparator 10 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP11                       (0xBUL)   /* The capture input or matches output of PWM timer 0 comparator 11 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP12                       (0xCUL)   /* The capture input or matches output of PWM timer 0 comparator 12 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP13                       (0xDUL)   /* The capture input or matches output of PWM timer 0 comparator 13 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP14                       (0xEUL)   /* The capture input or matches output of PWM timer 0 comparator 14 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP15                       (0xFUL)   /* The capture input or matches output of PWM timer 0 comparator 15 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP16                       (0x10UL)  /* The capture input or matches output of PWM timer 0 comparator 16 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP17                       (0x11UL)  /* The capture input or matches output of PWM timer 0 comparator 17 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP18                       (0x12UL)  /* The capture input or matches output of PWM timer 0 comparator 18 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP19                       (0x13UL)  /* The capture input or matches output of PWM timer 0 comparator 19 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP20                       (0x14UL)  /* The capture input or matches output of PWM timer 0 comparator 20 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP21                       (0x15UL)  /* The capture input or matches output of PWM timer 0 comparator 21 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP22                       (0x16UL)  /* The capture input or matches output of PWM timer 0 comparator 22 */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP23                       (0x17UL)  /* The capture input or matches output of PWM timer 0 comparator 23 */
#define HPM_TRGM0_DMA_SRC_PWM0_RLD                         (0x18UL)  /* PWM timer 0 counter reload */
#define HPM_TRGM0_DMA_SRC_PWM0_HALFRLD                     (0x19UL)  /* PWM timer 0 half cycle reload */
#define HPM_TRGM0_DMA_SRC_PWM0_XRLD                        (0x1AUL)  /* PWM timer 0 extended counter reload */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP0                        (0x1BUL)  /* The capture input or matches output of PWM timer 1 comparator 0 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP1                        (0x1CUL)  /* The capture input or matches output of PWM timer 1 comparator 1 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP2                        (0x1DUL)  /* The capture input or matches output of PWM timer 1 comparator 2 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP3                        (0x1EUL)  /* The capture input or matches output of PWM timer 1 comparator 3 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP4                        (0x1FUL)  /* The capture input or matches output of PWM timer 1 comparator 4 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP5                        (0x20UL)  /* The capture input or matches output of PWM timer 1 comparator 5 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP6                        (0x21UL)  /* The capture input or matches output of PWM timer 1 comparator 6 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP7                        (0x22UL)  /* The capture input or matches output of PWM timer 1 comparator 7 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP8                        (0x23UL)  /* The capture input or matches output of PWM timer 1 comparator 8 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP9                        (0x24UL)  /* The capture input or matches output of PWM timer 1 comparator 9 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP10                       (0x25UL)  /* The capture input or matches output of PWM timer 1 comparator 10 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP11                       (0x26UL)  /* The capture input or matches output of PWM timer 1 comparator 11 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP12                       (0x27UL)  /* The capture input or matches output of PWM timer 1 comparator 12 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP13                       (0x28UL)  /* The capture input or matches output of PWM timer 1 comparator 13 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP14                       (0x29UL)  /* The capture input or matches output of PWM timer 1 comparator 14 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP15                       (0x2AUL)  /* The capture input or matches output of PWM timer 1 comparator 15 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP16                       (0x2BUL)  /* The capture input or matches output of PWM timer 1 comparator 16 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP17                       (0x2CUL)  /* The capture input or matches output of PWM timer 1 comparator 17 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP18                       (0x2DUL)  /* The capture input or matches output of PWM timer 1 comparator 18 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP19                       (0x2EUL)  /* The capture input or matches output of PWM timer 1 comparator 19 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP20                       (0x2FUL)  /* The capture input or matches output of PWM timer 1 comparator 20 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP21                       (0x30UL)  /* The capture input or matches output of PWM timer 1 comparator 21 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP22                       (0x31UL)  /* The capture input or matches output of PWM timer 1 comparator 22 */
#define HPM_TRGM0_DMA_SRC_PWM1_CMP23                       (0x32UL)  /* The capture input or matches output of PWM timer 1 comparator 23 */
#define HPM_TRGM0_DMA_SRC_PWM1_RLD                         (0x33UL)  /* PWM timer 1 Counter reload */
#define HPM_TRGM0_DMA_SRC_PWM1_HALFRLD                     (0x34UL)  /* PWM timer 1 half cycle reload */
#define HPM_TRGM0_DMA_SRC_PWM1_XRLD                        (0x35UL)  /* PWM timer 1 Extended counter reload */
#define HPM_TRGM0_DMA_SRC_QEI0                             (0x36UL)  /* DMA request for QEI0 */
#define HPM_TRGM0_DMA_SRC_QEI1                             (0x37UL)  /* DMA request for QEI1 */
#define HPM_TRGM0_DMA_SRC_MMC0                             (0x38UL)  /* DMA request for MMC0 */
#define HPM_TRGM0_DMA_SRC_MMC1                             (0x39UL)  /* DMA request for MMC1 */
#define HPM_TRGM0_DMA_SRC_SEI0                             (0x3AUL)  /* DMA request 0 for SEI0 */
#define HPM_TRGM0_DMA_SRC_SEI1                             (0x3BUL)  /* DMA request 0 for SEI1 */
#define HPM_TRGM0_DMA_SRC_TRGM0                            (0x3CUL)  /* DMA request 0 for TRGM (from TRGM output 135) */
#define HPM_TRGM0_DMA_SRC_TRGM1                            (0x3DUL)  /* DMA request 1 for TRGM (from TRGM output 136) */



#endif /* HPM_TRGMMUX_SRC_H */
