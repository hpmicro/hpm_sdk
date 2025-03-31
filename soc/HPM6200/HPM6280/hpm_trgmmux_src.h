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
#define HPM_TRGM0_INPUT_SRC_TRGM0_P0                       (0x2UL)   /* TRGM0 Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P1                       (0x3UL)   /* TRGM0 Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P2                       (0x4UL)   /* TRGM0 Input 2 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P3                       (0x5UL)   /* TRGM0 Input 3 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P4                       (0x6UL)   /* TRGM0 Input 4 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P5                       (0x7UL)   /* TRGM0 Input 5 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P6                       (0x8UL)   /* TRGM0 Input 6 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P7                       (0x9UL)   /* TRGM0 Input 7 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P8                       (0xAUL)   /* TRGM0 Input 8 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P9                       (0xBUL)   /* TRGM0 Input 9 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P10                      (0xCUL)   /* TRGM0 Input 10 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P11                      (0xDUL)   /* TRGM0 Input 11 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM3_OUTX0                    (0xEUL)   /* TRGM3 Output X0 */
#define HPM_TRGM0_INPUT_SRC_TRGM3_OUTX1                    (0xFUL)   /* TRGM3 Output X1 */
#define HPM_TRGM0_INPUT_SRC_TRGM2_OUTX0                    (0x10UL)  /* TRGM2 Output X0 */
#define HPM_TRGM0_INPUT_SRC_TRGM2_OUTX1                    (0x11UL)  /* TRGM2 Output X1 */
#define HPM_TRGM0_INPUT_SRC_TRGM1_OUTX0                    (0x12UL)  /* TRGM1 Output X0 */
#define HPM_TRGM0_INPUT_SRC_TRGM1_OUTX1                    (0x13UL)  /* TRGM1 Output X1 */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH8REF                    (0x14UL)  /* PWM timer 0 channel 8 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH9REF                    (0x15UL)  /* PWM timer 0 channel 9 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH10REF                   (0x16UL)  /* PWM timer 0 channel 10 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH11REF                   (0x17UL)  /* PWM timer 0 channel 11 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH12REF                   (0x18UL)  /* PWM timer 0 channel 12 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH13REF                   (0x19UL)  /* PWM timer 0 channel 13 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH14REF                   (0x1AUL)  /* PWM timer 0 channel 14 reference output */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH15REF                   (0x1BUL)  /* PWM timer 0 channel 15 reference output */
#define HPM_TRGM0_INPUT_SRC_QEI0_TRGO                      (0x1CUL)  /* QEI0 triggers output */
#define HPM_TRGM0_INPUT_SRC_HALL0_TRGO                     (0x1DUL)  /* HALL0 triggers output */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP0                      (0x1EUL)  /* PTPC output comparison 0 */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP1                      (0x1FUL)  /* PTPC output comparison 1 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH0                       (0x20UL)  /* SYNT0 Channel 0 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH1                       (0x21UL)  /* SYNT0 Channel 1 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH2                       (0x22UL)  /* SYNT0 Channel 2 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH3                       (0x23UL)  /* SYNT0 Channel 3 */
#define HPM_TRGM0_INPUT_SRC_USB0_SOF                       (0x24UL)  /* USB0 frame start */
#define HPM_TRGM0_INPUT_SRC_DEBUG_FLAG                     (0x25UL)  /* the flag bit of debug mode enters */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT2                    (0x26UL)  /* GPTMR0 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT3                    (0x27UL)  /* GPTMR0 channel 3 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPL0                      (0x28UL)  /* sdm0 amplitude lower threshold0 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPL1                      (0x29UL)  /* sdm0 amplitude lower threshold1 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPL2                      (0x2AUL)  /* sdm0 amplitude lower threshold2 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPL3                      (0x2BUL)  /* sdm0 amplitude lower threshold3 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPH0                      (0x2CUL)  /* sdm0 amplitude upper threshold0 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPH1                      (0x2DUL)  /* sdm0 amplitude upper threshold1 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPH2                      (0x2EUL)  /* sdm0 amplitude upper threshold2 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPH3                      (0x2FUL)  /* sdm0 amplitude upper threshold3 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPHZ0                     (0x30UL)  /* SDM0 amplitude zero-crossing threshold0 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPHZ1                     (0x31UL)  /* SDM0 amplitude zero-crossing threshold1 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPHZ2                     (0x32UL)  /* SDM0 amplitude zero-crossing threshold2 */
#define HPM_TRGM0_INPUT_SRC_SDM_CMPHZ3                     (0x33UL)  /* SDM0 amplitude zero-crossing threshold3 */
#define HPM_TRGM0_INPUT_SRC_ACMP0_OUT                      (0x34UL)  /* Comparator 0 output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_OUT                      (0x35UL)  /* Comparator 1 output */
#define HPM_TRGM0_INPUT_SRC_ACMP2_OUT                      (0x36UL)  /* Comparator 2 output */
#define HPM_TRGM0_INPUT_SRC_ACMP3_OUT                      (0x37UL)  /* Comparator 3 output */
#define HPM_TRGM0_INPUT_SRC_PLA0_OUT0                      (0x38UL)  /* PLA0 module output 0 */
#define HPM_TRGM0_INPUT_SRC_PLA0_OUT1                      (0x39UL)  /* PLA0 module output 1 */
#define HPM_TRGM0_INPUT_SRC_PLA0_OUT2                      (0x3AUL)  /* PLA0 module output 2 */
#define HPM_TRGM0_INPUT_SRC_PLA0_OUT3                      (0x3BUL)  /* PLA0 module output 3 */
#define HPM_TRGM0_INPUT_SRC_PLA0_OUT4                      (0x3CUL)  /* PLA0 module output 4 */
#define HPM_TRGM0_INPUT_SRC_PLA0_OUT5                      (0x3DUL)  /* PLA0 module output 5 */
#define HPM_TRGM0_INPUT_SRC_PLA0_OUT6                      (0x3EUL)  /* PLA0 module output 6 */
#define HPM_TRGM0_INPUT_SRC_PLA0_OUT7                      (0x3FUL)  /* PLA0 module output 7 */

/* trgm1_input mux definitions */
#define HPM_TRGM1_INPUT_SRC_VSS                            (0x0UL)   /* Low level */
#define HPM_TRGM1_INPUT_SRC_VDD                            (0x1UL)   /* High level */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P0                       (0x2UL)   /* TRGM1 Input 0 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P1                       (0x3UL)   /* TRGM1 Input 1 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P2                       (0x4UL)   /* TRGM1 Input 2 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P3                       (0x5UL)   /* TRGM1 Input 3 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P4                       (0x6UL)   /* TRGM1 Input 4 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P5                       (0x7UL)   /* TRGM1 Input 5 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P6                       (0x8UL)   /* TRGM1 Input 6 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P7                       (0x9UL)   /* TRGM1 Input 7 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P8                       (0xAUL)   /* TRGM1 Input 8 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P9                       (0xBUL)   /* TRGM1 Input 9 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P10                      (0xCUL)   /* TRGM1 Input 10 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P11                      (0xDUL)   /* TRGM1 Input 11 (from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM3_OUTX0                    (0xEUL)   /* TRGM3 Output X0 */
#define HPM_TRGM1_INPUT_SRC_TRGM3_OUTX1                    (0xFUL)   /* TRGM3 Output X1 */
#define HPM_TRGM1_INPUT_SRC_TRGM2_OUTX0                    (0x10UL)  /* TRGM2 Output X0 */
#define HPM_TRGM1_INPUT_SRC_TRGM2_OUTX1                    (0x11UL)  /* TRGM2 Output X1 */
#define HPM_TRGM1_INPUT_SRC_TRGM0_OUTX0                    (0x12UL)  /* TRGM0 Output X0 */
#define HPM_TRGM1_INPUT_SRC_TRGM0_OUTX1                    (0x13UL)  /* TRGM0 Output X1 */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH8REF                    (0x14UL)  /* PWM timer 1 channel 8 reference output */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH9REF                    (0x15UL)  /* PWM timer 1 channel 9 reference output */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH10REF                   (0x16UL)  /* PWM timer 1 channel 10 reference output */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH11REF                   (0x17UL)  /* PWM timer 1 channel 11 reference output */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH12REF                   (0x18UL)  /* PWM timer 1 channel 12 reference output */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH13REF                   (0x19UL)  /* PWM timer 1 channel 13 reference output */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH14REF                   (0x1AUL)  /* PWM timer 1 channel 14 reference output */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH15REF                   (0x1BUL)  /* PWM timer 1 channel 15 reference output */
#define HPM_TRGM1_INPUT_SRC_QEI1_TRGO                      (0x1CUL)  /* QEI1 triggers output */
#define HPM_TRGM1_INPUT_SRC_HALL1_TRGO                     (0x1DUL)  /* HALL1 triggers output */
#define HPM_TRGM1_INPUT_SRC_PTPC_CMP0                      (0x1EUL)  /* PTPC output comparison 0 */
#define HPM_TRGM1_INPUT_SRC_PTPC_CMP1                      (0x1FUL)  /* PTPC output comparison 1 */
#define HPM_TRGM1_INPUT_SRC_SYNT_CH0                       (0x20UL)  /* SYNT1 Channel 0 */
#define HPM_TRGM1_INPUT_SRC_SYNT_CH1                       (0x21UL)  /* SYNT1 Channel 1 */
#define HPM_TRGM1_INPUT_SRC_SYNT_CH2                       (0x22UL)  /* SYNT1 Channel 2 */
#define HPM_TRGM1_INPUT_SRC_SYNT_CH3                       (0x23UL)  /* SYNT1 Channel 3 */
#define HPM_TRGM1_INPUT_SRC_USB0_SOF                       (0x24UL)  /* USB0 frame start */
#define HPM_TRGM1_INPUT_SRC_DEBUG_FLAG                     (0x25UL)  /* the flag bit of debug mode enters */
#define HPM_TRGM1_INPUT_SRC_GPTMR1_OUT2                    (0x26UL)  /* GPTMR1 channel 2 */
#define HPM_TRGM1_INPUT_SRC_GPTMR1_OUT3                    (0x27UL)  /* GPTMR1 channel 3 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPL0                      (0x28UL)  /* sdm amplitude lower threshold0 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPL1                      (0x29UL)  /* sdm amplitude lower threshold1 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPL2                      (0x2AUL)  /* sdm amplitude lower threshold2 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPL3                      (0x2BUL)  /* sdm amplitude lower threshold3 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPH0                      (0x2CUL)  /* sdm amplitude upper threshold0 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPH1                      (0x2DUL)  /* sdm amplitude upper threshold1 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPH2                      (0x2EUL)  /* sdm amplitude upper threshold2 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPH3                      (0x2FUL)  /* sdm amplitude upper threshold3 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPHZ0                     (0x30UL)  /* SDM amplitude zero-crossing threshold0 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPHZ1                     (0x31UL)  /* SDM amplitude zero-crossing threshold1 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPHZ2                     (0x32UL)  /* SDM amplitude zero-crossing threshold2 */
#define HPM_TRGM1_INPUT_SRC_SDM_CMPHZ3                     (0x33UL)  /* SDM amplitude zero-crossing threshold3 */
#define HPM_TRGM1_INPUT_SRC_ACMP0_OUT                      (0x34UL)  /* Comparator 0 output */
#define HPM_TRGM1_INPUT_SRC_ACMP1_OUT                      (0x35UL)  /* Comparator 1 output */
#define HPM_TRGM1_INPUT_SRC_ACMP2_OUT                      (0x36UL)  /* Comparator 2 output */
#define HPM_TRGM1_INPUT_SRC_ACMP3_OUT                      (0x37UL)  /* Comparator 3 output */
#define HPM_TRGM1_INPUT_SRC_PLA1_OUT0                      (0x38UL)  /* PLA1 module output 0 */
#define HPM_TRGM1_INPUT_SRC_PLA1_OUT1                      (0x39UL)  /* PLA1 module output 1 */
#define HPM_TRGM1_INPUT_SRC_PLA1_OUT2                      (0x3AUL)  /* PLA1 module output 2 */
#define HPM_TRGM1_INPUT_SRC_PLA1_OUT3                      (0x3BUL)  /* PLA1 module output 3 */
#define HPM_TRGM1_INPUT_SRC_PLA1_OUT4                      (0x3CUL)  /* PLA1 module output 4 */
#define HPM_TRGM1_INPUT_SRC_PLA1_OUT5                      (0x3DUL)  /* PLA1 module output 5 */
#define HPM_TRGM1_INPUT_SRC_PLA1_OUT6                      (0x3EUL)  /* PLA1 module output 6 */
#define HPM_TRGM1_INPUT_SRC_PLA1_OUT7                      (0x3FUL)  /* PLA1 module output 7 */

/* trgm2_input mux definitions */
#define HPM_TRGM2_INPUT_SRC_VSS                            (0x0UL)   /* Low level */
#define HPM_TRGM2_INPUT_SRC_VDD                            (0x1UL)   /* High level */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P0                       (0x2UL)   /* TRGM2 Input 0 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P1                       (0x3UL)   /* TRGM2 Input 1 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P2                       (0x4UL)   /* TRGM2 Input 2 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P3                       (0x5UL)   /* TRGM2 Input 3 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P4                       (0x6UL)   /* TRGM2 Input 4 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P5                       (0x7UL)   /* TRGM2 Input 5 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P6                       (0x8UL)   /* TRGM2 Input 6 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P7                       (0x9UL)   /* TRGM2 Input 7 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P8                       (0xAUL)   /* TRGM2 Input 8 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P9                       (0xBUL)   /* TRGM2 Input 9 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P10                      (0xCUL)   /* TRGM2 Input 10 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P11                      (0xDUL)   /* TRGM2 Input 11 (from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM3_OUTX0                    (0xEUL)   /* TRGM3 Output X0 */
#define HPM_TRGM2_INPUT_SRC_TRGM3_OUTX1                    (0xFUL)   /* TRGM3 Output X1 */
#define HPM_TRGM2_INPUT_SRC_TRGM1_OUTX0                    (0x10UL)  /* TRGM1 Output X0 */
#define HPM_TRGM2_INPUT_SRC_TRGM1_OUTX1                    (0x11UL)  /* TRGM1 Output X1 */
#define HPM_TRGM2_INPUT_SRC_TRGM0_OUTX0                    (0x12UL)  /* TRGM0 Output X0 */
#define HPM_TRGM2_INPUT_SRC_TRGM0_OUTX1                    (0x13UL)  /* TRGM0 Output X1 */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH8REF                    (0x14UL)  /* PWM timer 2 channel 8 reference output */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH9REF                    (0x15UL)  /* PWM timer 2 channel 9 reference output */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH10REF                   (0x16UL)  /* PWM timer 2 channel 10 reference output */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH11REF                   (0x17UL)  /* PWM timer 2 channel 11 reference output */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH12REF                   (0x18UL)  /* PWM timer 2 channel 12 reference output */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH13REF                   (0x19UL)  /* PWM timer 2 channel 13 reference output */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH14REF                   (0x1AUL)  /* PWM timer 2 channel 14 reference output */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH15REF                   (0x1BUL)  /* PWM timer 2 channel 15 reference output */
#define HPM_TRGM2_INPUT_SRC_QEI2_TRGO                      (0x1CUL)  /* QEI2 triggers output */
#define HPM_TRGM2_INPUT_SRC_HALL2_TRGO                     (0x1DUL)  /* HALL2 triggers output */
#define HPM_TRGM2_INPUT_SRC_PTPC_CMP0                      (0x1EUL)  /* PTPC output comparison 0 */
#define HPM_TRGM2_INPUT_SRC_PTPC_CMP1                      (0x1FUL)  /* PTPC output comparison 1 */
#define HPM_TRGM2_INPUT_SRC_SYNT_CH0                       (0x20UL)  /* SYNT2 Channel 0 */
#define HPM_TRGM2_INPUT_SRC_SYNT_CH1                       (0x21UL)  /* SYNT2 Channel 1 */
#define HPM_TRGM2_INPUT_SRC_SYNT_CH2                       (0x22UL)  /* SYNT2 Channel 2 */
#define HPM_TRGM2_INPUT_SRC_SYNT_CH3                       (0x23UL)  /* SYNT2 Channel 3 */
#define HPM_TRGM2_INPUT_SRC_USB0_SOF                       (0x24UL)  /* USB0 frame start */
#define HPM_TRGM2_INPUT_SRC_DEBUG_FLAG                     (0x25UL)  /* the flag bit of debug mode enters */
#define HPM_TRGM2_INPUT_SRC_GPTMR2_OUT2                    (0x26UL)  /* GPTMR2 channel 2 */
#define HPM_TRGM2_INPUT_SRC_GPTMR2_OUT3                    (0x27UL)  /* GPTMR2 channel 3 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPL0                      (0x28UL)  /* sdm amplitude lower threshold0 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPL1                      (0x29UL)  /* sdm amplitude lower threshold1 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPL2                      (0x2AUL)  /* sdm amplitude lower threshold2 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPL3                      (0x2BUL)  /* sdm amplitude lower threshold3 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPH0                      (0x2CUL)  /* sdm amplitude upper threshold0 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPH1                      (0x2DUL)  /* sdm amplitude upper threshold1 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPH2                      (0x2EUL)  /* sdm amplitude upper threshold2 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPH3                      (0x2FUL)  /* sdm amplitude upper threshold3 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPHZ0                     (0x30UL)  /* SDM amplitude zero-crossing threshold0 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPHZ1                     (0x31UL)  /* SDM amplitude zero-crossing threshold1 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPHZ2                     (0x32UL)  /* SDM amplitude zero-crossing threshold2 */
#define HPM_TRGM2_INPUT_SRC_SDM_CMPHZ3                     (0x33UL)  /* SDM amplitude zero-crossing threshold3 */
#define HPM_TRGM2_INPUT_SRC_ACMP0_OUT                      (0x34UL)  /* Comparator 0 output */
#define HPM_TRGM2_INPUT_SRC_ACMP1_OUT                      (0x35UL)  /* Comparator 1 output */
#define HPM_TRGM2_INPUT_SRC_ACMP2_OUT                      (0x36UL)  /* Comparator 2 output */
#define HPM_TRGM2_INPUT_SRC_ACMP3_OUT                      (0x37UL)  /* Comparator 3 output */

/* trgm3_input mux definitions */
#define HPM_TRGM3_INPUT_SRC_VSS                            (0x0UL)   /* Low level */
#define HPM_TRGM3_INPUT_SRC_VDD                            (0x1UL)   /* High level */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P0                       (0x2UL)   /* TRGM3 Input 0 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P1                       (0x3UL)   /* TRGM3 Input 1 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P2                       (0x4UL)   /* TRGM3 Input 2 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P3                       (0x5UL)   /* TRGM3 Input 3 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P4                       (0x6UL)   /* TRGM3 Input 4 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P5                       (0x7UL)   /* TRGM3 Input 5 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P6                       (0x8UL)   /* TRGM3 Input 6 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P7                       (0x9UL)   /* TRGM3 Input 7 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P8                       (0xAUL)   /* TRGM3 Input 8 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P9                       (0xBUL)   /* TRGM3 Input 9 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P10                      (0xCUL)   /* TRGM3 Input 10 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P11                      (0xDUL)   /* TRGM3 Input 11 (from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM2_OUTX0                    (0xEUL)   /* TRGM2 Output X0 */
#define HPM_TRGM3_INPUT_SRC_TRGM2_OUTX1                    (0xFUL)   /* TRGM2 Output X1 */
#define HPM_TRGM3_INPUT_SRC_TRGM1_OUTX0                    (0x10UL)  /* TRGM1 Output X0 */
#define HPM_TRGM3_INPUT_SRC_TRGM1_OUTX1                    (0x11UL)  /* TRGM1 Output X1 */
#define HPM_TRGM3_INPUT_SRC_TRGM0_OUTX0                    (0x12UL)  /* TRGM0 Output X0 */
#define HPM_TRGM3_INPUT_SRC_TRGM0_OUTX1                    (0x13UL)  /* TRGM0 Output X1 */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH8REF                    (0x14UL)  /* PWM timer 3 channel 8 reference output */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH9REF                    (0x15UL)  /* PWM timer 3 channel 9 reference output */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH10REF                   (0x16UL)  /* PWM timer 3 channel 10 reference output */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH11REF                   (0x17UL)  /* PWM timer 3 channel 11 reference output */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH12REF                   (0x18UL)  /* PWM timer 3 channel 12 reference output */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH13REF                   (0x19UL)  /* PWM timer 3 channel 13 reference output */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH14REF                   (0x1AUL)  /* PWM timer 3 channel 14 reference output */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH15REF                   (0x1BUL)  /* PWM timer 3 channel 15 reference output */
#define HPM_TRGM3_INPUT_SRC_QEI3_TRGO                      (0x1CUL)  /* QEI3 triggers output */
#define HPM_TRGM3_INPUT_SRC_HALL3_TRGO                     (0x1DUL)  /* HALL3 triggers output */
#define HPM_TRGM3_INPUT_SRC_PTPC_CMP0                      (0x1EUL)  /* PTPC output comparison 0 */
#define HPM_TRGM3_INPUT_SRC_PTPC_CMP1                      (0x1FUL)  /* PTPC output comparison 1 */
#define HPM_TRGM3_INPUT_SRC_SYNT_CH0                       (0x20UL)  /* SYNT3 Channel 0 */
#define HPM_TRGM3_INPUT_SRC_SYNT_CH1                       (0x21UL)  /* SYNT3 Channel 1 */
#define HPM_TRGM3_INPUT_SRC_SYNT_CH2                       (0x22UL)  /* SYNT3 Channel 2 */
#define HPM_TRGM3_INPUT_SRC_SYNT_CH3                       (0x23UL)  /* SYNT3 Channel 3 */
#define HPM_TRGM3_INPUT_SRC_USB0_SOF                       (0x24UL)  /* USB0 frame start */
#define HPM_TRGM3_INPUT_SRC_DEBUG_FLAG                     (0x25UL)  /* the flag bit of debug mode enters */
#define HPM_TRGM3_INPUT_SRC_GPTMR3_OUT2                    (0x26UL)  /* GPTMR3 channel 2 */
#define HPM_TRGM3_INPUT_SRC_GPTMR3_OUT3                    (0x27UL)  /* GPTMR3 channel 3 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPL0                      (0x28UL)  /* sdm amplitude lower threshold0 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPL1                      (0x29UL)  /* sdm amplitude lower threshold1 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPL2                      (0x2AUL)  /* sdm amplitude lower threshold2 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPL3                      (0x2BUL)  /* sdm amplitude lower threshold3 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPH0                      (0x2CUL)  /* sdm amplitude upper threshold0 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPH1                      (0x2DUL)  /* sdm amplitude upper threshold1 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPH2                      (0x2EUL)  /* sdm amplitude upper threshold2 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPH3                      (0x2FUL)  /* sdm amplitude upper threshold3 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPHZ0                     (0x30UL)  /* SDM amplitude zero-crossing threshold0 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPHZ1                     (0x31UL)  /* SDM amplitude zero-crossing threshold1 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPHZ2                     (0x32UL)  /* SDM amplitude zero-crossing threshold2 */
#define HPM_TRGM3_INPUT_SRC_SDM_CMPHZ3                     (0x33UL)  /* SDM amplitude zero-crossing threshold3 */
#define HPM_TRGM3_INPUT_SRC_ACMP0_OUT                      (0x34UL)  /* Comparator 0 output */
#define HPM_TRGM3_INPUT_SRC_ACMP1_OUT                      (0x35UL)  /* Comparator 1 output */
#define HPM_TRGM3_INPUT_SRC_ACMP2_OUT                      (0x36UL)  /* Comparator 2 output */
#define HPM_TRGM3_INPUT_SRC_ACMP3_OUT                      (0x37UL)  /* Comparator 3 output */

/* trgm0_output mux definitions */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P0                      (0x0UL)   /* TRGM0 Output 0 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P1                      (0x1UL)   /* TRGM0 Output 1 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P2                      (0x2UL)   /* TRGM0 Output 2 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P3                      (0x3UL)   /* TRGM0 Output 3 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P4                      (0x4UL)   /* TRGM0 Output 4 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P5                      (0x5UL)   /* TRGM0 Output 5 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P6                      (0x6UL)   /* TRGM0 Output 6 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P7                      (0x7UL)   /* TRGM0 Output 7 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P8                      (0x8UL)   /* TRGM0 Output 8 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P9                      (0x9UL)   /* TRGM0 Output 9 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P10                     (0xAUL)   /* TRGM0 Output 10 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P11                     (0xBUL)   /* TRGM0 Output 11 (to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_OUTX0                   (0xCUL)   /* TRGM0 Output X0 (to another TRGM) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_OUTX1                   (0xDUL)   /* TRGM0 Output X1 (to another TRGM) */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_SYNCI                    (0xEUL)   /* PWM timer 0 counter synchronously triggers input */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FRCI                     (0xFUL)   /* the input value for PWM timer 0 forces control */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FRCSYNCI                 (0x10UL)  /* the synchronous input for PWM timer 0 forces control */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_SHRLDSYNCI               (0x11UL)  /* PWM timer 0 Shadow register to activate trigger input */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI0                  (0x12UL)  /* PWM timer 0 Fault protection input 0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI1                  (0x13UL)  /* PWM timer 0 Fault protection input 1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI2                  (0x14UL)  /* PWM timer 0 Fault protection input 2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI3                  (0x15UL)  /* PWM timer 0 Fault protection input 3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN8                      (0x16UL)  /* PWM timer 0 capture input 8 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN9                      (0x17UL)  /* PWM timer 0 capture input 9 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN10                     (0x18UL)  /* PWM timer 0 capture input 10 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN11                     (0x19UL)  /* PWM timer 0 capture input 11 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN12                     (0x1AUL)  /* PWM timer 0 capture input 12 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN13                     (0x1BUL)  /* PWM timer 0 capture input 13 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN14                     (0x1CUL)  /* PWM timer 0 capture input 14 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN15                     (0x1DUL)  /* PWM timer 0 capture input 15 */
#define HPM_TRGM0_OUTPUT_SRC_PLA0_IN0                      (0x1EUL)  /* PLA0 module input 0 */
#define HPM_TRGM0_OUTPUT_SRC_PLA0_IN1                      (0x1FUL)  /* PLA0 module input 1 */
#define HPM_TRGM0_OUTPUT_SRC_PLA0_IN2                      (0x20UL)  /* PLA0 module input 2 */
#define HPM_TRGM0_OUTPUT_SRC_PLA0_IN3                      (0x21UL)  /* PLA0 module input 3 */
#define HPM_TRGM0_OUTPUT_SRC_PLA0_IN4                      (0x22UL)  /* PLA0 module input 4 */
#define HPM_TRGM0_OUTPUT_SRC_PLA0_IN5                      (0x23UL)  /* PLA0 module input 5 */
#define HPM_TRGM0_OUTPUT_SRC_PLA0_IN6                      (0x24UL)  /* PLA0 module input 6 */
#define HPM_TRGM0_OUTPUT_SRC_PLA0_IN7                      (0x25UL)  /* PLA0 module input 7 */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_A                        (0x26UL)  /* QEI0 input of phase A */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_B                        (0x27UL)  /* QEI0 input of phase B */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_Z                        (0x28UL)  /* QEI0 input of phase Z */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_H                        (0x29UL)  /* QEI0 input of phase H */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_PAUSE                    (0x2AUL)  /* QEI0 Pause input */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_SNAPI                    (0x2BUL)  /* QEI0 Snap input */
#define HPM_TRGM0_OUTPUT_SRC_HALL0_U                       (0x2CUL)  /* HALL0 input of phase U */
#define HPM_TRGM0_OUTPUT_SRC_HALL0_V                       (0x2DUL)  /* HALL0 input of phase V */
#define HPM_TRGM0_OUTPUT_SRC_HALL0_W                       (0x2EUL)  /* HALL0 input of phase W */
#define HPM_TRGM0_OUTPUT_SRC_HALL0_SNAPI                   (0x2FUL)  /* HALL0 Snap input */
#define HPM_TRGM0_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* The sequence conversion of ADC0 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* The sequence conversion of ADC1 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* The sequence conversion of ADC2 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0A                  (0x34UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 0A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0B                  (0x35UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 0B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0C                  (0x36UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 0C */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_SYNCI                  (0x37UL)  /* GPTMR0 counter synchronous input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN2                    (0x38UL)  /* GPTMR0 channel 2 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN3                    (0x39UL)  /* GPTMR0 channel 3 input */
#define HPM_TRGM0_OUTPUT_SRC_DAC0_BUF_TRG                  (0x3AUL)  /* DAC0 buffer mode starts to trigger */
#define HPM_TRGM0_OUTPUT_SRC_DAC0_STP_TRG                  (0x3BUL)  /* DAC0 step mode starts to trigger */
#define HPM_TRGM0_OUTPUT_SRC_DAC1_STP_TRG                  (0x3CUL)  /* DAC1 step mode starts to trigger */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_WIN                     (0x3DUL)  /* Comparator 0 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_PTPC_CAP0                     (0x3EUL)  /* PTPC input capture 0 */
#define HPM_TRGM0_OUTPUT_SRC_PTPC_CAP1                     (0x3FUL)  /* PTPC input capture 1 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_TRG0                      (0x40UL)  /* SDM triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_TRG1                      (0x41UL)  /* SDM triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_TRG2                      (0x42UL)  /* SDM triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_TRG3                      (0x43UL)  /* SDM triggers input 3 */

/* trgm1_output mux definitions */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P0                      (0x0UL)   /* TRGM1 Output 0 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P1                      (0x1UL)   /* TRGM1 Output 1 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P2                      (0x2UL)   /* TRGM1 Output 2 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P3                      (0x3UL)   /* TRGM1 Output 3 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P4                      (0x4UL)   /* TRGM1 Output 4 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P5                      (0x5UL)   /* TRGM1 Output 5 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P6                      (0x6UL)   /* TRGM1 Output 6 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P7                      (0x7UL)   /* TRGM1 Output 7 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P8                      (0x8UL)   /* TRGM1 Output 8 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P9                      (0x9UL)   /* TRGM1 Output 9 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P10                     (0xAUL)   /* TRGM1 Output 10 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P11                     (0xBUL)   /* TRGM1 Output 11 (to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_OUTX0                   (0xCUL)   /* TRGM1 Output X0 (to another TRGM) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_OUTX1                   (0xDUL)   /* TRGM1 Output X1 (to another TRGM) */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_SYNCI                    (0xEUL)   /* PWM timer 1 counter synchronously triggers input */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FRCI                     (0xFUL)   /* the input value for PWM timer 1 forces control */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FRCSYNCI                 (0x10UL)  /* the synchronous input for PWM timer 1 forces control */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_SHRLDSYNCI               (0x11UL)  /* PWM timer 1 Shadow register to activate trigger input */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FAULTI0                  (0x12UL)  /* PWM timer 1 Fault protection input 0 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FAULTI1                  (0x13UL)  /* PWM timer 1 Fault protection input 1 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FAULTI2                  (0x14UL)  /* PWM timer 1 Fault protection input 2 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FAULTI3                  (0x15UL)  /* PWM timer 1 Fault protection input 3 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN8                      (0x16UL)  /* PWM timer 1 capture input 8 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN9                      (0x17UL)  /* PWM timer 1 capture input 9 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN10                     (0x18UL)  /* PWM timer 1 capture input 10 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN11                     (0x19UL)  /* PWM timer 1 capture input 11 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN12                     (0x1AUL)  /* PWM timer 1 capture input 12 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN13                     (0x1BUL)  /* PWM timer 1 capture input 13 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN14                     (0x1CUL)  /* PWM timer 1 capture input 14 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN15                     (0x1DUL)  /* PWM timer 1 capture input 15 */
#define HPM_TRGM1_OUTPUT_SRC_PLA1_IN0                      (0x1EUL)  /* PLA1 module input 0 */
#define HPM_TRGM1_OUTPUT_SRC_PLA1_IN1                      (0x1FUL)  /* PLA1 module input 1 */
#define HPM_TRGM1_OUTPUT_SRC_PLA1_IN2                      (0x20UL)  /* PLA1 module input 2 */
#define HPM_TRGM1_OUTPUT_SRC_PLA1_IN3                      (0x21UL)  /* PLA1 module input 3 */
#define HPM_TRGM1_OUTPUT_SRC_PLA1_IN4                      (0x22UL)  /* PLA1 module input 4 */
#define HPM_TRGM1_OUTPUT_SRC_PLA1_IN5                      (0x23UL)  /* PLA1 module input 5 */
#define HPM_TRGM1_OUTPUT_SRC_PLA1_IN6                      (0x24UL)  /* PLA1 module input 6 */
#define HPM_TRGM1_OUTPUT_SRC_PLA1_IN7                      (0x25UL)  /* PLA1 module input 7 */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_A                        (0x26UL)  /* QEI1 input of phase A */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_B                        (0x27UL)  /* QEI1 input of phase B */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_Z                        (0x28UL)  /* QEI1 input of phase Z */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_H                        (0x29UL)  /* QEI1 input of phase H */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_PAUSE                    (0x2AUL)  /* QEI1 Pause input */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_SNAPI                    (0x2BUL)  /* QEI1 Snap input */
#define HPM_TRGM1_OUTPUT_SRC_HALL1_U                       (0x2CUL)  /* HALL1 input of phase U */
#define HPM_TRGM1_OUTPUT_SRC_HALL1_V                       (0x2DUL)  /* HALL1 input of phase V */
#define HPM_TRGM1_OUTPUT_SRC_HALL1_W                       (0x2EUL)  /* HALL1 input of phase W */
#define HPM_TRGM1_OUTPUT_SRC_HALL1_SNAPI                   (0x2FUL)  /* HALL1 Snap input */
#define HPM_TRGM1_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* The sequence conversion of ADC0 triggers input */
#define HPM_TRGM1_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* The sequence conversion of ADC1 triggers input */
#define HPM_TRGM1_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* The sequence conversion of ADC2 triggers input */
#define HPM_TRGM1_OUTPUT_SRC_ADCX_PTRGI1A                  (0x34UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 1A */
#define HPM_TRGM1_OUTPUT_SRC_ADCX_PTRGI1B                  (0x35UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 1B */
#define HPM_TRGM1_OUTPUT_SRC_ADCX_PTRGI1C                  (0x36UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 1C */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR1_SYNCI                  (0x37UL)  /* GPTMR1 counter synchronous input */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR1_IN2                    (0x38UL)  /* GPTMR1 channel 2 input */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR1_IN3                    (0x39UL)  /* GPTMR1 channel 3 input */
#define HPM_TRGM1_OUTPUT_SRC_DAC1_BUF_TRG                  (0x3AUL)  /* DAC1 buffer mode starts to trigger */
#define HPM_TRGM1_OUTPUT_SRC_DAC0_STP_TRG                  (0x3BUL)  /* DAC0 step mode starts to trigger */
#define HPM_TRGM1_OUTPUT_SRC_DAC1_STP_TRG                  (0x3CUL)  /* DAC1 step mode starts to trigger */
#define HPM_TRGM1_OUTPUT_SRC_ACMP1_WIN                     (0x3DUL)  /* Comparator 1 window mode input */
#define HPM_TRGM1_OUTPUT_SRC_PTPC_CAP0                     (0x3EUL)  /* PTPC input capture 0 */
#define HPM_TRGM1_OUTPUT_SRC_PTPC_CAP1                     (0x3FUL)  /* PTPC input capture 1 */
#define HPM_TRGM1_OUTPUT_SRC_SDM_TRG4                      (0x40UL)  /* SDM triggers input 4 */
#define HPM_TRGM1_OUTPUT_SRC_SDM_TRG5                      (0x41UL)  /* SDM triggers input 5 */
#define HPM_TRGM1_OUTPUT_SRC_SDM_TRG6                      (0x42UL)  /* SDM triggers input 6 */
#define HPM_TRGM1_OUTPUT_SRC_SDM_TRG7                      (0x43UL)  /* SDM triggers input 7 */

/* trgm2_output mux definitions */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P0                      (0x0UL)   /* TRGM2 Output 0 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P1                      (0x1UL)   /* TRGM2 Output 1 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P2                      (0x2UL)   /* TRGM2 Output 2 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P3                      (0x3UL)   /* TRGM2 Output 3 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P4                      (0x4UL)   /* TRGM2 Output 4 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P5                      (0x5UL)   /* TRGM2 Output 5 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P6                      (0x6UL)   /* TRGM2 Output 6 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P7                      (0x7UL)   /* TRGM2 Output 7 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P8                      (0x8UL)   /* TRGM2 Output 8 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P9                      (0x9UL)   /* TRGM2 Output 9 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P10                     (0xAUL)   /* TRGM2 Output 10 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P11                     (0xBUL)   /* TRGM2 Output 11 (to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_OUTX0                   (0xCUL)   /* TRGM2 Output X0 (to another TRGM) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_OUTX1                   (0xDUL)   /* TRGM2 Output X1 (to another TRGM) */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_SYNCI                    (0xEUL)   /* PWM timer 2 counter synchronously triggers input */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FRCI                     (0xFUL)   /* the input value for PWM timer 2 forces control */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FRCSYNCI                 (0x10UL)  /* the synchronous input for PWM timer 2 forces control */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_SHRLDSYNCI               (0x11UL)  /* PWM timer 2 Shadow register to activate trigger input */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FAULTI0                  (0x12UL)  /* PWM timer 2 Fault protection input 0 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FAULTI1                  (0x13UL)  /* PWM timer 2 Fault protection input 1 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FAULTI2                  (0x14UL)  /* PWM timer 2 Fault protection input 2 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FAULTI3                  (0x15UL)  /* PWM timer 2 Fault protection input 3 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN8                      (0x16UL)  /* PWM timer 2 capture input 8 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN9                      (0x17UL)  /* PWM timer 2 capture input 9 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN10                     (0x18UL)  /* PWM timer 2 capture input 10 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN11                     (0x19UL)  /* PWM timer 2 capture input 11 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN12                     (0x1AUL)  /* PWM timer 2 capture input 12 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN13                     (0x1BUL)  /* PWM timer 2 capture input 13 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN14                     (0x1CUL)  /* PWM timer 2 capture input 14 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN15                     (0x1DUL)  /* PWM timer 2 capture input 15 */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_A                        (0x26UL)  /* QEI2 input of phase A */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_B                        (0x27UL)  /* QEI2 input of phase B */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_Z                        (0x28UL)  /* QEI2 input of phase Z */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_H                        (0x29UL)  /* QEI2 input of phase H */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_PAUSE                    (0x2AUL)  /* QEI2 Pause input */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_SNAPI                    (0x2BUL)  /* QEI2 Snap input */
#define HPM_TRGM2_OUTPUT_SRC_HALL2_U                       (0x2CUL)  /* HALL2 input of phase U */
#define HPM_TRGM2_OUTPUT_SRC_HALL2_V                       (0x2DUL)  /* HALL2 input of phase V */
#define HPM_TRGM2_OUTPUT_SRC_HALL2_W                       (0x2EUL)  /* HALL2 input of phase W */
#define HPM_TRGM2_OUTPUT_SRC_HALL2_SNAPI                   (0x2FUL)  /* HALL2 Snap input */
#define HPM_TRGM2_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* The sequence conversion of ADC0 triggers input */
#define HPM_TRGM2_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* The sequence conversion of ADC1 triggers input */
#define HPM_TRGM2_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* The sequence conversion of ADC2 triggers input */
#define HPM_TRGM2_OUTPUT_SRC_ADCX_PTRGI2A                  (0x34UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 2A */
#define HPM_TRGM2_OUTPUT_SRC_ADCX_PTRGI2B                  (0x35UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 2B */
#define HPM_TRGM2_OUTPUT_SRC_ADCX_PTRGI2C                  (0x36UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 2C */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR2_SYNCI                  (0x37UL)  /* GPTMR2 counter synchronous input */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR2_IN2                    (0x38UL)  /* GPTMR2 channel 2 input */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR2_IN3                    (0x39UL)  /* GPTMR2 channel 3 input */
#define HPM_TRGM2_OUTPUT_SRC_DAC0_BUF_TRG                  (0x3AUL)  /* DAC0 buffer mode starts to trigger */
#define HPM_TRGM2_OUTPUT_SRC_DAC0_STP_TRG                  (0x3BUL)  /* DAC0 step mode starts to trigger */
#define HPM_TRGM2_OUTPUT_SRC_DAC1_STP_TRG                  (0x3CUL)  /* DAC1 step mode starts to trigger */
#define HPM_TRGM2_OUTPUT_SRC_ACMP2_WIN                     (0x3DUL)  /* Comparator 2 window mode input */
#define HPM_TRGM2_OUTPUT_SRC_PTPC_CAP0                     (0x3EUL)  /* PTPC input capture 0 */
#define HPM_TRGM2_OUTPUT_SRC_PTPC_CAP1                     (0x3FUL)  /* PTPC input capture 1 */
#define HPM_TRGM2_OUTPUT_SRC_SDM_TRG8                      (0x40UL)  /* SDM triggers input 8 */
#define HPM_TRGM2_OUTPUT_SRC_SDM_TRG9                      (0x41UL)  /* SDM triggers input 9 */
#define HPM_TRGM2_OUTPUT_SRC_SDM_TRG10                     (0x42UL)  /* SDM triggers input 10 */
#define HPM_TRGM2_OUTPUT_SRC_SDM_TRG11                     (0x43UL)  /* SDM triggers input 11 */

/* trgm3_output mux definitions */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P0                      (0x0UL)   /* TRGM3 Output 0 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P1                      (0x1UL)   /* TRGM3 Output 1 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P2                      (0x2UL)   /* TRGM3 Output 2 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P3                      (0x3UL)   /* TRGM3 Output 3 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P4                      (0x4UL)   /* TRGM3 Output 4 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P5                      (0x5UL)   /* TRGM3 Output 5 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P6                      (0x6UL)   /* TRGM3 Output 6 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P7                      (0x7UL)   /* TRGM3 Output 7 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P8                      (0x8UL)   /* TRGM3 Output 8 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P9                      (0x9UL)   /* TRGM3 Output 9 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P10                     (0xAUL)   /* TRGM3 Output 10 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P11                     (0xBUL)   /* TRGM3 Output 11 (to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_OUTX0                   (0xCUL)   /* TRGM3 Output X0 (to another TRGM) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_OUTX1                   (0xDUL)   /* TRGM3 Output X1 (to another TRGM) */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_SYNCI                    (0xEUL)   /* PWM timer 3 counter synchronously triggers input */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FRCI                     (0xFUL)   /* the input value for PWM timer 3 forces control */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FRCSYNCI                 (0x10UL)  /* the synchronous input for PWM timer 3 forces control */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_SHRLDSYNCI               (0x11UL)  /* PWM timer 3 Shadow register to activate trigger input */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FAULTI0                  (0x12UL)  /* PWM timer 3 Fault protection input 0 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FAULTI1                  (0x13UL)  /* PWM timer 3 Fault protection input 1 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FAULTI2                  (0x14UL)  /* PWM timer 3 Fault protection input 2 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FAULTI3                  (0x15UL)  /* PWM timer 3 Fault protection input 3 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN8                      (0x16UL)  /* PWM timer 3 capture input 8 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN9                      (0x17UL)  /* PWM timer 3 capture input 9 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN10                     (0x18UL)  /* PWM timer 3 capture input 10 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN11                     (0x19UL)  /* PWM timer 3 capture input 11 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN12                     (0x1AUL)  /* PWM timer 3 capture input 12 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN13                     (0x1BUL)  /* PWM timer 3 capture input 13 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN14                     (0x1CUL)  /* PWM timer 3 capture input 14 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN15                     (0x1DUL)  /* PWM timer 3 capture input 15 */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_A                        (0x26UL)  /* QEI3 input of phase A */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_B                        (0x27UL)  /* QEI3 input of phase B */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_Z                        (0x28UL)  /* QEI3 input of phase Z */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_H                        (0x29UL)  /* QEI3 input of phase H */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_PAUSE                    (0x2AUL)  /* QEI3 Pause input */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_SNAPI                    (0x2BUL)  /* QEI3 Snap input */
#define HPM_TRGM3_OUTPUT_SRC_HALL3_U                       (0x2CUL)  /* HALL3 input of phase U */
#define HPM_TRGM3_OUTPUT_SRC_HALL3_V                       (0x2DUL)  /* HALL3 input of phase V */
#define HPM_TRGM3_OUTPUT_SRC_HALL3_W                       (0x2EUL)  /* HALL3 input of phase W */
#define HPM_TRGM3_OUTPUT_SRC_HALL3_SNAPI                   (0x2FUL)  /* HALL3 Snap input */
#define HPM_TRGM3_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* The sequence conversion of ADC0 triggers input */
#define HPM_TRGM3_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* The sequence conversion of ADC1 triggers input */
#define HPM_TRGM3_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* The sequence conversion of ADC2 triggers input */
#define HPM_TRGM3_OUTPUT_SRC_ADCX_PTRGI3A                  (0x34UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 3A */
#define HPM_TRGM3_OUTPUT_SRC_ADCX_PTRGI3B                  (0x35UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 3B */
#define HPM_TRGM3_OUTPUT_SRC_ADCX_PTRGI3C                  (0x36UL)  /* The preemption conversion of ADC0, 1 , 2 triggers input 3C */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR3_SYNCI                  (0x37UL)  /* GPTMR3 counter synchronous input */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR3_IN2                    (0x38UL)  /* GPTMR3 channel 2 input */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR3_IN3                    (0x39UL)  /* GPTMR3 channel 3 input */
#define HPM_TRGM3_OUTPUT_SRC_DAC1_BUF_TRG                  (0x3AUL)  /* DAC1 buffer mode starts to trigger */
#define HPM_TRGM3_OUTPUT_SRC_DAC0_STP_TRG                  (0x3BUL)  /* DAC0 step mode starts to trigger */
#define HPM_TRGM3_OUTPUT_SRC_DAC1_STP_TRG                  (0x3CUL)  /* DAC1 step mode starts to trigger */
#define HPM_TRGM3_OUTPUT_SRC_ACMP3_WIN                     (0x3DUL)  /* Comparator 3 window mode input */
#define HPM_TRGM3_OUTPUT_SRC_PTPC_CAP0                     (0x3EUL)  /* PTPC input capture 0 */
#define HPM_TRGM3_OUTPUT_SRC_PTPC_CAP1                     (0x3FUL)  /* PTPC input capture 1 */
#define HPM_TRGM3_OUTPUT_SRC_SDFM_TRG12                    (0x40UL)  /* SDM triggers input 12 */
#define HPM_TRGM3_OUTPUT_SRC_SDFM_TRG13                    (0x41UL)  /* SDM triggers input 13 */
#define HPM_TRGM3_OUTPUT_SRC_SDFM_TRG14                    (0x42UL)  /* SDM triggers input 14 */
#define HPM_TRGM3_OUTPUT_SRC_SDFM_TRG15                    (0x43UL)  /* SDM triggers input 15 */

/* trgm0_filter mux definitions */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN0                      (0x0UL)   /* PWM timer 0 capture Input 0 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN1                      (0x1UL)   /* PWM timer 0 capture Input 1 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN2                      (0x2UL)   /* PWM timer 0 capture Input 2 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN3                      (0x3UL)   /* PWM timer 0 capture Input 3 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN4                      (0x4UL)   /* PWM timer 0 capture Input 4 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN5                      (0x5UL)   /* PWM timer 0 capture Input 5 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN6                      (0x6UL)   /* PWM timer 0 capture Input 6 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN7                      (0x7UL)   /* PWM timer 0 capture Input 7 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN0                     (0x8UL)   /* TRGM0 iutput 0 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN1                     (0x9UL)   /* TRGM0 iutput 1 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN2                     (0xAUL)   /* TRGM0 iutput 2 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN3                     (0xBUL)   /* TRGM0 iutput 3 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN4                     (0xCUL)   /* TRGM0 iutput 4 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN5                     (0xDUL)   /* TRGM0 iutput 5 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN6                     (0xEUL)   /* TRGM0 iutput 6 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN7                     (0xFUL)   /* TRGM0 iutput 7 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN8                     (0x10UL)  /* TRGM0 iutput 8 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN9                     (0x11UL)  /* TRGM0 iutput 9 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN10                    (0x12UL)  /* TRGM0 iutput 10 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN11                    (0x13UL)  /* TRGM0 iutput 11 */

/* trgm1_filter mux definitions */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN0                      (0x0UL)   /* PWM timer 1 capture Input 0 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN1                      (0x1UL)   /* PWM timer 1 capture Input 1 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN2                      (0x2UL)   /* PWM timer 1 capture Input 2 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN3                      (0x3UL)   /* PWM timer 1 capture Input 3 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN4                      (0x4UL)   /* PWM timer 1 capture Input 4 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN5                      (0x5UL)   /* PWM timer 1 capture Input 5 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN6                      (0x6UL)   /* PWM timer 1 capture Input 6 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN7                      (0x7UL)   /* PWM timer 1 capture Input 7 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN0                     (0x8UL)   /* TRGM1 iutput 0 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN1                     (0x9UL)   /* TRGM1 iutput 1 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN2                     (0xAUL)   /* TRGM1 iutput 2 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN3                     (0xBUL)   /* TRGM1 iutput 3 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN4                     (0xCUL)   /* TRGM1 iutput 4 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN5                     (0xDUL)   /* TRGM1 iutput 5 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN6                     (0xEUL)   /* TRGM1 iutput 6 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN7                     (0xFUL)   /* TRGM1 iutput 7 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN8                     (0x10UL)  /* TRGM1 iutput 8 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN9                     (0x11UL)  /* TRGM1 iutput 9 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN10                    (0x12UL)  /* TRGM1 iutput 10 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN11                    (0x13UL)  /* TRGM1 iutput 11 */

/* trgm2_filter mux definitions */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN0                      (0x0UL)   /* PWM timer 2 capture Input 0 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN1                      (0x1UL)   /* PWM timer 2 capture Input 1 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN2                      (0x2UL)   /* PWM timer 2 capture Input 2 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN3                      (0x3UL)   /* PWM timer 2 capture Input 3 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN4                      (0x4UL)   /* PWM timer 2 capture Input 4 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN5                      (0x5UL)   /* PWM timer 2 capture Input 5 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN6                      (0x6UL)   /* PWM timer 2 capture Input 6 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN7                      (0x7UL)   /* PWM timer 2 capture Input 7 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN0                     (0x8UL)   /* TRGM2 iutput 0 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN1                     (0x9UL)   /* TRGM2 iutput 1 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN2                     (0xAUL)   /* TRGM2 iutput 2 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN3                     (0xBUL)   /* TRGM2 iutput 3 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN4                     (0xCUL)   /* TRGM2 iutput 4 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN5                     (0xDUL)   /* TRGM2 iutput 5 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN6                     (0xEUL)   /* TRGM2 iutput 6 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN7                     (0xFUL)   /* TRGM2 iutput 7 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN8                     (0x10UL)  /* TRGM2 iutput 8 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN9                     (0x11UL)  /* TRGM2 iutput 9 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN10                    (0x12UL)  /* TRGM2 iutput 10 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN11                    (0x13UL)  /* TRGM2 iutput 11 */

/* trgm3_filter mux definitions */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN0                      (0x0UL)   /* PWM timer 3 capture Input 0 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN1                      (0x1UL)   /* PWM timer 3 capture Input 1 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN2                      (0x2UL)   /* PWM timer 3 capture Input 2 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN3                      (0x3UL)   /* PWM timer 3 capture Input 3 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN4                      (0x4UL)   /* PWM timer 3 capture Input 4 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN5                      (0x5UL)   /* PWM timer 3 capture Input 5 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN6                      (0x6UL)   /* PWM timer 3 capture Input 6 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN7                      (0x7UL)   /* PWM timer 3 capture Input 7 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN0                     (0x8UL)   /* TRGM3 iutput 0 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN1                     (0x9UL)   /* TRGM3 iutput 1 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN2                     (0xAUL)   /* TRGM3 iutput 2 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN3                     (0xBUL)   /* TRGM3 iutput 3 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN4                     (0xCUL)   /* TRGM3 iutput 4 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN5                     (0xDUL)   /* TRGM3 iutput 5 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN6                     (0xEUL)   /* TRGM3 iutput 6 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN7                     (0xFUL)   /* TRGM3 iutput 7 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN8                     (0x10UL)  /* TRGM3 iutput 8 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN9                     (0x11UL)  /* TRGM3 iutput 9 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN10                    (0x12UL)  /* TRGM3 iutput 10 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN11                    (0x13UL)  /* TRGM3 iutput 11 */

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
#define HPM_TRGM0_DMA_SRC_QEI0                             (0x1BUL)  /* DMA request for QEI0 */
#define HPM_TRGM0_DMA_SRC_HALL0                            (0x1CUL)  /* DMA request for HALL0 */

/* trgm1_dma mux definitions */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP0                        (0x0UL)   /* The capture input or matches output of PWM timer 1 comparator 0 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP1                        (0x1UL)   /* The capture input or matches output of PWM timer 1 comparator 1 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP2                        (0x2UL)   /* The capture input or matches output of PWM timer 1 comparator 2 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP3                        (0x3UL)   /* The capture input or matches output of PWM timer 1 comparator 3 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP4                        (0x4UL)   /* The capture input or matches output of PWM timer 1 comparator 4 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP5                        (0x5UL)   /* The capture input or matches output of PWM timer 1 comparator 5 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP6                        (0x6UL)   /* The capture input or matches output of PWM timer 1 comparator 6 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP7                        (0x7UL)   /* The capture input or matches output of PWM timer 1 comparator 7 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP8                        (0x8UL)   /* The capture input or matches output of PWM timer 1 comparator 8 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP9                        (0x9UL)   /* The capture input or matches output of PWM timer 1 comparator 9 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP10                       (0xAUL)   /* The capture input or matches output of PWM timer 1 comparator 10 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP11                       (0xBUL)   /* The capture input or matches output of PWM timer 1 comparator 11 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP12                       (0xCUL)   /* The capture input or matches output of PWM timer 1 comparator 12 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP13                       (0xDUL)   /* The capture input or matches output of PWM timer 1 comparator 13 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP14                       (0xEUL)   /* The capture input or matches output of PWM timer 1 comparator 14 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP15                       (0xFUL)   /* The capture input or matches output of PWM timer 1 comparator 15 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP16                       (0x10UL)  /* The capture input or matches output of PWM timer 1 comparator 16 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP17                       (0x11UL)  /* The capture input or matches output of PWM timer 1 comparator 17 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP18                       (0x12UL)  /* The capture input or matches output of PWM timer 1 comparator 18 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP19                       (0x13UL)  /* The capture input or matches output of PWM timer 1 comparator 19 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP20                       (0x14UL)  /* The capture input or matches output of PWM timer 1 comparator 20 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP21                       (0x15UL)  /* The capture input or matches output of PWM timer 1 comparator 21 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP22                       (0x16UL)  /* The capture input or matches output of PWM timer 1 comparator 22 */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP23                       (0x17UL)  /* The capture input or matches output of PWM timer 1 comparator 23 */
#define HPM_TRGM1_DMA_SRC_PWM1_RLD                         (0x18UL)  /* PWM timer 1 counter reload */
#define HPM_TRGM1_DMA_SRC_PWM1_HALFRLD                     (0x19UL)  /* PWM timer 1 half cycle reload */
#define HPM_TRGM1_DMA_SRC_PWM1_XRLD                        (0x1AUL)  /* PWM timer 1 extended counter reload */
#define HPM_TRGM1_DMA_SRC_QEI1                             (0x1BUL)  /* DMA request for QEI1 */
#define HPM_TRGM1_DMA_SRC_HALL1                            (0x1CUL)  /* DMA request for HALL1 */

/* trgm2_dma mux definitions */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP0                        (0x0UL)   /* The capture input or matches output of PWM timer 2 comparator 0 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP1                        (0x1UL)   /* The capture input or matches output of PWM timer 2 comparator 1 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP2                        (0x2UL)   /* The capture input or matches output of PWM timer 2 comparator 2 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP3                        (0x3UL)   /* The capture input or matches output of PWM timer 2 comparator 3 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP4                        (0x4UL)   /* The capture input or matches output of PWM timer 2 comparator 4 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP5                        (0x5UL)   /* The capture input or matches output of PWM timer 2 comparator 5 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP6                        (0x6UL)   /* The capture input or matches output of PWM timer 2 comparator 6 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP7                        (0x7UL)   /* The capture input or matches output of PWM timer 2 comparator 7 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP8                        (0x8UL)   /* The capture input or matches output of PWM timer 2 comparator 8 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP9                        (0x9UL)   /* The capture input or matches output of PWM timer 2 comparator 9 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP10                       (0xAUL)   /* The capture input or matches output of PWM timer 2 comparator 10 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP11                       (0xBUL)   /* The capture input or matches output of PWM timer 2 comparator 11 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP12                       (0xCUL)   /* The capture input or matches output of PWM timer 2 comparator 12 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP13                       (0xDUL)   /* The capture input or matches output of PWM timer 2 comparator 13 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP14                       (0xEUL)   /* The capture input or matches output of PWM timer 2 comparator 14 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP15                       (0xFUL)   /* The capture input or matches output of PWM timer 2 comparator 15 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP16                       (0x10UL)  /* The capture input or matches output of PWM timer 2 comparator 16 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP17                       (0x11UL)  /* The capture input or matches output of PWM timer 2 comparator 17 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP18                       (0x12UL)  /* The capture input or matches output of PWM timer 2 comparator 18 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP19                       (0x13UL)  /* The capture input or matches output of PWM timer 2 comparator 19 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP20                       (0x14UL)  /* The capture input or matches output of PWM timer 2 comparator 20 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP21                       (0x15UL)  /* The capture input or matches output of PWM timer 2 comparator 21 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP22                       (0x16UL)  /* The capture input or matches output of PWM timer 2 comparator 22 */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP23                       (0x17UL)  /* The capture input or matches output of PWM timer 2 comparator 23 */
#define HPM_TRGM2_DMA_SRC_PWM2_RLD                         (0x18UL)  /* PWM timer 2 counter reload */
#define HPM_TRGM2_DMA_SRC_PWM2_HALFRLD                     (0x19UL)  /* PWM timer 2 half cycle reload */
#define HPM_TRGM2_DMA_SRC_PWM2_XRLD                        (0x1AUL)  /* PWM timer 2 extended counter reload */
#define HPM_TRGM2_DMA_SRC_QEI2                             (0x1BUL)  /* DMA request for QEI2 */
#define HPM_TRGM2_DMA_SRC_HALL2                            (0x1CUL)  /* DMA request for HALL2 */

/* trgm3_dma mux definitions */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP0                        (0x0UL)   /* The capture input or matches output of PWM timer 3 comparator 0 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP1                        (0x1UL)   /* The capture input or matches output of PWM timer 3 comparator 1 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP2                        (0x2UL)   /* The capture input or matches output of PWM timer 3 comparator 2 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP3                        (0x3UL)   /* The capture input or matches output of PWM timer 3 comparator 3 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP4                        (0x4UL)   /* The capture input or matches output of PWM timer 3 comparator 4 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP5                        (0x5UL)   /* The capture input or matches output of PWM timer 3 comparator 5 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP6                        (0x6UL)   /* The capture input or matches output of PWM timer 3 comparator 6 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP7                        (0x7UL)   /* The capture input or matches output of PWM timer 3 comparator 7 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP8                        (0x8UL)   /* The capture input or matches output of PWM timer 3 comparator 8 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP9                        (0x9UL)   /* The capture input or matches output of PWM timer 3 comparator 9 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP10                       (0xAUL)   /* The capture input or matches output of PWM timer 3 comparator 10 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP11                       (0xBUL)   /* The capture input or matches output of PWM timer 3 comparator 11 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP12                       (0xCUL)   /* The capture input or matches output of PWM timer 3 comparator 12 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP13                       (0xDUL)   /* The capture input or matches output of PWM timer 3 comparator 13 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP14                       (0xEUL)   /* The capture input or matches output of PWM timer 3 comparator 14 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP15                       (0xFUL)   /* The capture input or matches output of PWM timer 3 comparator 15 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP16                       (0x10UL)  /* The capture input or matches output of PWM timer 3 comparator 16 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP17                       (0x11UL)  /* The capture input or matches output of PWM timer 3 comparator 17 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP18                       (0x12UL)  /* The capture input or matches output of PWM timer 3 comparator 18 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP19                       (0x13UL)  /* The capture input or matches output of PWM timer 3 comparator 19 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP20                       (0x14UL)  /* The capture input or matches output of PWM timer 3 comparator 20 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP21                       (0x15UL)  /* The capture input or matches output of PWM timer 3 comparator 21 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP22                       (0x16UL)  /* The capture input or matches output of PWM timer 3 comparator 22 */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP23                       (0x17UL)  /* The capture input or matches output of PWM timer 3 comparator 23 */
#define HPM_TRGM3_DMA_SRC_PWM3_RLD                         (0x18UL)  /* PWM timer 3 counter reload */
#define HPM_TRGM3_DMA_SRC_PWM3_HALFRLD                     (0x19UL)  /* PWM timer 3 half cycle reload */
#define HPM_TRGM3_DMA_SRC_PWM3_XRLD                        (0x1AUL)  /* PWM timer 3 extended counter reload */
#define HPM_TRGM3_DMA_SRC_QEI3                             (0x1BUL)  /* DMA request for QEI3 */
#define HPM_TRGM3_DMA_SRC_HALL3                            (0x1CUL)  /* DMA request for HALL3 */



#endif /* HPM_TRGMMUX_SRC_H */
