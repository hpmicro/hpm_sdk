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
#define HPM_TRGM0_INPUT_SRC_ACMP0_OUT                      (0x6UL)   /* Comparator 0 output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_OUT                      (0x7UL)   /* Comparator 1 output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT2                    (0x8UL)   /* GPTMR0 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT3                    (0x9UL)   /* GPTMR0 channel 3 */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT2                    (0xAUL)   /* GPTMR1 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT3                    (0xBUL)   /* GPTMR1 channel 3 */
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
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_WIN                     (0x1CUL)  /* Comparator 0 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP1_WIN                     (0x1DUL)  /* Comparator 1 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ADC0_STRGI                    (0x20UL)  /* The sequence conversion of ADC0 triggers input */
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
#define HPM_TRGM0_OUTPUT_SRC_SYNCTIMER_TRIG                (0x7EUL)  /* SYNT triggers input */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG0                    (0x83UL)  /* UART0/1/2/3 Trigger signal */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG1                    (0x84UL)  /* UART4/5/6/7 Trigger signal */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ0                     (0x85UL)  /* TRGM interrupt signal 0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ1                     (0x86UL)  /* TRGM interrupt signal 1 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA0                     (0x87UL)  /* TRGM DMA request 0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA1                     (0x88UL)  /* TRGM DMA request 1 */

/* trgm0_filter mux definitions */
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

/* trgm0_dma mux definitions */
#define HPM_TRGM0_DMA_SRC_TRGM0                            (0x3CUL)  /* DMA request 0 for TRGM (from TRGM output 135) */
#define HPM_TRGM0_DMA_SRC_TRGM1                            (0x3DUL)  /* DMA request 1 for TRGM (from TRGM output 136) */



#endif /* HPM_TRGMMUX_SRC_H */
