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
#define HPM_TRGM0_INPUT_SRC_USB0_SOF                       (0x2UL)   /* USB0 frame start */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP0                      (0x4UL)   /* PTPC output comparison 0 */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP1                      (0x5UL)   /* PTPC output comparison 1 */
#define HPM_TRGM0_INPUT_SRC_QEI0_TRGO                      (0x6UL)   /* QEI1 triggers output */
#define HPM_TRGM0_INPUT_SRC_QEI1_TRGO                      (0x7UL)   /* QEI0 triggers output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT2                    (0x8UL)   /* GPTMR0 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT3                    (0x9UL)   /* GPTMR0 channel 3 */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT2                    (0xAUL)   /* GPTMR1 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT3                    (0xBUL)   /* GPTMR1 channel 3 */
#define HPM_TRGM0_INPUT_SRC_GPTMR2_OUT2                    (0xCUL)   /* GPTMR2 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR2_OUT3                    (0xDUL)   /* GPTMR2 channel 3 */
#define HPM_TRGM0_INPUT_SRC_GPTMR3_OUT2                    (0xEUL)   /* GPTMR2 channel 2 */
#define HPM_TRGM0_INPUT_SRC_GPTMR3_OUT3                    (0xFUL)   /* GPTMR3 channel 3 */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN0                    (0x10UL)  /* PWM0 Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN1                    (0x11UL)  /* PWM0 Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN0                    (0x12UL)  /* PWM1 Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN1                    (0x13UL)  /* PWM1 Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM2_CAPIN0                    (0x14UL)  /* PWM2 Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM2_CAPIN1                    (0x15UL)  /* PWM2 Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH00                      (0x18UL)  /* SYNT0 Channel 0 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH01                      (0x19UL)  /* SYNT0 Channel 1 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH02                      (0x1AUL)  /* SYNT0 Channel 2 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH03                      (0x1BUL)  /* SYNT0 Channel 3 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH04                      (0x1CUL)  /* SYNT0 Channel 4 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH05                      (0x1DUL)  /* SYNT0 Channel 5 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH06                      (0x1EUL)  /* SYNT0 Channel 6 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH07                      (0x1FUL)  /* SYNT0 Channel 7 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH08                      (0x20UL)  /* SYNT0 Channel 8 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH09                      (0x21UL)  /* SYNT0 Channel 9 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH10                      (0x22UL)  /* SYNT0 Channel 10 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH11                      (0x23UL)  /* SYNT0 Channel 11 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH12                      (0x24UL)  /* SYNT0 Channel 12 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH13                      (0x25UL)  /* SYNT0 Channel 13 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH14                      (0x26UL)  /* SYNT0 Channel 14 */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH15                      (0x27UL)  /* SYNT0 Channel 15 */
#define HPM_TRGM0_INPUT_SRC_ACMP0_CH0_OUT                  (0x28UL)  /* Comparator 0 output */
#define HPM_TRGM0_INPUT_SRC_ACMP0_CH1_OUT                  (0x29UL)  /* Comparator 1 output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_CH0_OUT                  (0x2AUL)  /* Comparator 2 output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_CH1_OUT                  (0x2BUL)  /* Comparator 3 output */
#define HPM_TRGM0_INPUT_SRC_ACMP2_CH0_OUT                  (0x2CUL)  /* Comparator 4 output */
#define HPM_TRGM0_INPUT_SRC_ACMP2_CH1_OUT                  (0x2DUL)  /* Comparator 5 output */
#define HPM_TRGM0_INPUT_SRC_ACMP3_CH0_OUT                  (0x2EUL)  /* Comparator 6 output */
#define HPM_TRGM0_INPUT_SRC_ACMP3_CH1_OUT                  (0x2FUL)  /* Comparator 7 output */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_0                     (0x30UL)  /* SEI triggers output 0 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_1                     (0x31UL)  /* SEI triggers output 1 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_2                     (0x32UL)  /* SEI triggers output 2 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_3                     (0x33UL)  /* SEI triggers output 3 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_4                     (0x34UL)  /* SEI triggers output 4 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_5                     (0x35UL)  /* SEI triggers output 5 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_6                     (0x36UL)  /* SEI triggers output 6 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_7                     (0x37UL)  /* SEI triggers output 7 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ0                   (0x38UL)  /* SDM0 amplitude zero-crossing threshold0 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ1                   (0x39UL)  /* SDM0 amplitude zero-crossing threshold1 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ2                   (0x3AUL)  /* SDM0 amplitude zero-crossing threshold2 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ3                   (0x3BUL)  /* SDM0 amplitude zero-crossing threshold3 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL0                    (0x3CUL)  /* sdm0 amplitude lower threshold0 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL1                    (0x3DUL)  /* sdm0 amplitude lower threshold1 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL2                    (0x3EUL)  /* sdm0 amplitude lower threshold2 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL3                    (0x3FUL)  /* sdm0 amplitude lower threshold3 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA0                   (0x40UL)  /* sdm0 amplitude upper threshold0 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA1                   (0x41UL)  /* sdm0 amplitude upper threshold1 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA2                   (0x42UL)  /* sdm0 amplitude upper threshold2 */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA3                   (0x43UL)  /* sdm0 amplitude upper threshold3 */
#define HPM_TRGM0_INPUT_SRC_ADC0_TRGO                      (0x44UL)  /* ADC0 triggers output */
#define HPM_TRGM0_INPUT_SRC_ADC1_TRGO                      (0x45UL)  /* ADC1 triggers output */
#define HPM_TRGM0_INPUT_SRC_ADC2_TRGO                      (0x46UL)  /* ADC2 triggers output */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_0                    (0x48UL)  /* PWM0 triggers output0 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_1                    (0x49UL)  /* PWM0 triggers output1 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_2                    (0x4AUL)  /* PWM0 triggers output2 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_3                    (0x4BUL)  /* PWM0 triggers output3 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_4                    (0x4CUL)  /* PWM0 triggers output4 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_5                    (0x4DUL)  /* PWM0 triggers output5 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_6                    (0x4EUL)  /* PWM0 triggers output6 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_7                    (0x46UL)  /* PWM0 triggers output7 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_0                    (0x50UL)  /* PWM1 triggers output0 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_1                    (0x51UL)  /* PWM1 triggers output1 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_2                    (0x52UL)  /* PWM1 triggers output2 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_3                    (0x53UL)  /* PWM1 triggers output3 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_4                    (0x54UL)  /* PWM1 triggers output4 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_5                    (0x55UL)  /* PWM1 triggers output5 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_6                    (0x56UL)  /* PWM1 triggers output6 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_7                    (0x57UL)  /* PWM1 triggers output7 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_0                    (0x58UL)  /* PWM2 triggers output0 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_1                    (0x59UL)  /* PWM2 triggers output1 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_2                    (0x5AUL)  /* PWM2 triggers output2 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_3                    (0x5BUL)  /* PWM2 triggers output3 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_4                    (0x5CUL)  /* PWM2 triggers output4 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_5                    (0x5DUL)  /* PWM2 triggers output5 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_6                    (0x5EUL)  /* PWM2 triggers output6 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_7                    (0x5FUL)  /* PWM2 triggers output7 */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P00                      (0x68UL)  /* TRGM0 Input 0 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P01                      (0x69UL)  /* TRGM0 Input 1 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P02                      (0x6AUL)  /* TRGM0 Input 2 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P03                      (0x6BUL)  /* TRGM0 Input 3 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P04                      (0x6CUL)  /* TRGM0 Input 4 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P05                      (0x6DUL)  /* TRGM0 Input 5 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P06                      (0x6EUL)  /* TRGM0 Input 6 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P07                      (0x6FUL)  /* TRGM0 Input 7 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P08                      (0x70UL)  /* TRGM0 Input 8 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P09                      (0x71UL)  /* TRGM0 Input 9 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P10                      (0x72UL)  /* TRGM0 Input 10 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P11                      (0x73UL)  /* TRGM0 Input 11 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P12                      (0x74UL)  /* TRGM0 Input 12 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P13                      (0x75UL)  /* TRGM0 Input 13 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P14                      (0x76UL)  /* TRGM0 Input 14 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P15                      (0x77UL)  /* TRGM0 Input 15 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P16                      (0x78UL)  /* TRGM0 Input 16 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P17                      (0x79UL)  /* TRGM0 Input 17 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P18                      (0x7AUL)  /* TRGM0 Input 18 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P19                      (0x7BUL)  /* TRGM0 Input 19 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P20                      (0x7CUL)  /* TRGM0 Input 20 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P21                      (0x7DUL)  /* TRGM0 Input 21 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P22                      (0x7EUL)  /* TRGM0 Input 22 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P23                      (0x7FUL)  /* TRGM0 Input 23 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P24                      (0x80UL)  /* TRGM0 Input 24 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P25                      (0x81UL)  /* TRGM0 Input 25 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P26                      (0x82UL)  /* TRGM0 Input 26 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P27                      (0x83UL)  /* TRGM0 Input 27 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P28                      (0x84UL)  /* TRGM0 Input 28 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P29                      (0x85UL)  /* TRGM0 Input 29 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P30                      (0x86UL)  /* TRGM0 Input 30 (from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P31                      (0x87UL)  /* TRGM0 Input 31 (from IO) */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT00                      (0x88UL)  /* PLB module output 0 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT01                      (0x89UL)  /* PLB module output 1 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT02                      (0x8AUL)  /* PLB module output 2 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT03                      (0x8BUL)  /* PLB module output 3 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT04                      (0x8CUL)  /* PLB module output 4 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT05                      (0x8DUL)  /* PLB module output 5 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT06                      (0x8EUL)  /* PLB module output 6 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT07                      (0x8FUL)  /* PLB module output 7 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT08                      (0x90UL)  /* PLB module output 8 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT09                      (0x91UL)  /* PLB module output 9 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT10                      (0x92UL)  /* PLB module output 10 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT11                      (0x93UL)  /* PLB module output 11 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT12                      (0x94UL)  /* PLB module output 12 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT13                      (0x95UL)  /* PLB module output 13 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT14                      (0x96UL)  /* PLB module output 14 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT15                      (0x97UL)  /* PLB module output 15 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT16                      (0x98UL)  /* PLB module output 16 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT17                      (0x99UL)  /* PLB module output 17 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT18                      (0x9AUL)  /* PLB module output 18 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT19                      (0x9BUL)  /* PLB module output 19 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT20                      (0x9CUL)  /* PLB module output 20 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT21                      (0x9DUL)  /* PLB module output 21 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT22                      (0x9EUL)  /* PLB module output 22 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT23                      (0x9FUL)  /* PLB module output 23 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT24                      (0xA0UL)  /* PLB module output 24 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT25                      (0xA1UL)  /* PLB module output 25 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT26                      (0xA2UL)  /* PLB module output 26 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT27                      (0xA3UL)  /* PLB module output 27 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT28                      (0xA4UL)  /* PLB module output 28 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT29                      (0xA5UL)  /* PLB module output 29 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT30                      (0xA6UL)  /* PLB module output 30 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT31                      (0xA7UL)  /* PLB module output 31 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT32                      (0xA8UL)  /* PLB module output 32 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT33                      (0xA9UL)  /* PLB module output 33 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT34                      (0xAAUL)  /* PLB module output 34 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT35                      (0xABUL)  /* PLB module output 35 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT36                      (0xACUL)  /* PLB module output 36 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT37                      (0xADUL)  /* PLB module output 37 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT38                      (0xAEUL)  /* PLB module output 38 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT39                      (0xAFUL)  /* PLB module output 39 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT40                      (0xB0UL)  /* PLB module output 40 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT41                      (0xB1UL)  /* PLB module output 41 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT42                      (0xB2UL)  /* PLB module output 42 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT43                      (0xAAUL)  /* PLB module output 43 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT44                      (0xB4UL)  /* PLB module output 44 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT45                      (0xB5UL)  /* PLB module output 45 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT46                      (0xB6UL)  /* PLB module output 46 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT47                      (0xB7UL)  /* PLB module output 47 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT48                      (0xB8UL)  /* PLB module output 48 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT49                      (0xB9UL)  /* PLB module output 49 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT50                      (0xBAUL)  /* PLB module output 50 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT51                      (0xBBUL)  /* PLB module output 51 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT52                      (0xBCUL)  /* PLB module output 52 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT53                      (0xBDUL)  /* PLB module output 53 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT54                      (0xBEUL)  /* PLB module output 54 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT55                      (0xBFUL)  /* PLB module output 55 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT56                      (0xC0UL)  /* PLB module output 56 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT57                      (0xC1UL)  /* PLB module output 57 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT58                      (0xC2UL)  /* PLB module output 58 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT59                      (0xC3UL)  /* PLB module output 59 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT60                      (0xC4UL)  /* PLB module output 60 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT61                      (0xC5UL)  /* PLB module output 61 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT62                      (0xC6UL)  /* PLB module output 62 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT63                      (0xC7UL)  /* PLB module output 63 */
#define HPM_TRGM0_INPUT_SRC_RDC0_TRGO_0                    (0xC8UL)  /* RDC triggers output 0 */
#define HPM_TRGM0_INPUT_SRC_RDC0_TRGO_1                    (0xC9UL)  /* RDC triggers output 1 */
#define HPM_TRGM0_INPUT_SRC_DEBUG_FLAG                     (0xCAUL)  /* the flag bit of debug mode enters */

/* trgm0_output mux definitions */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P00                     (0x0UL)   /* TRGM Output 0 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P01                     (0x1UL)   /* TRGM Output 1 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P02                     (0x2UL)   /* TRGM Output 2 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P03                     (0x3UL)   /* TRGM Output 3 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P04                     (0x4UL)   /* TRGM Output 4 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P05                     (0x5UL)   /* TRGM Output 5 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P06                     (0x6UL)   /* TRGM Output 6 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P07                     (0x7UL)   /* TRGM Output 7 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P08                     (0x8UL)   /* TRGM Output 8 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P09                     (0x9UL)   /* TRGM Output 9 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P10                     (0xAUL)   /* TRGM Output 10 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P11                     (0xBUL)   /* TRGM Output 11 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P12                     (0xCUL)   /* TRGM Output 12 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P13                     (0xDUL)   /* TRGM Output 13 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P14                     (0xEUL)   /* TRGM Output 14 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P15                     (0xFUL)   /* TRGM Output 15 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P16                     (0x10UL)  /* TRGM Output 16 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P17                     (0x11UL)  /* TRGM Output 17 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P18                     (0x12UL)  /* TRGM Output 18 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P19                     (0x13UL)  /* TRGM Output 19 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P20                     (0x14UL)  /* TRGM Output 20 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P21                     (0x15UL)  /* TRGM Output 21 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P22                     (0x16UL)  /* TRGM Output 22 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P23                     (0x17UL)  /* TRGM Output 23 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P24                     (0x18UL)  /* TRGM Output 24 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P25                     (0x19UL)  /* TRGM Output 25 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P26                     (0x1AUL)  /* TRGM Output 26 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P27                     (0x1BUL)  /* TRGM Output 27 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P28                     (0x1CUL)  /* TRGM Output 28 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P29                     (0x1DUL)  /* TRGM Output 29 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P30                     (0x1EUL)  /* TRGM Output 30 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P31                     (0x1FUL)  /* TRGM Output 31 (output to IO) */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC0                  (0x20UL)  /* SDM triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC1                  (0x21UL)  /* SDM triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC2                  (0x22UL)  /* SDM triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC3                  (0x23UL)  /* SDM triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC4                  (0x24UL)  /* SDM triggers input 4 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC5                  (0x25UL)  /* SDM triggers input 5 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC6                  (0x26UL)  /* SDM triggers input 6 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC7                  (0x27UL)  /* SDM triggers input 7 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC8                  (0x28UL)  /* SDM triggers input 8 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC9                  (0x29UL)  /* SDM triggers input 9 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC10                 (0x2AUL)  /* SDM triggers input 10 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC11                 (0x2BUL)  /* SDM triggers input 11 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC12                 (0x2CUL)  /* SDM triggers input 12 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC13                 (0x2DUL)  /* SDM triggers input 13 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC14                 (0x2EUL)  /* SDM triggers input 14 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC15                 (0x2FUL)  /* SDM triggers input 15 */
#define HPM_TRGM0_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* The sequence conversion of ADC0 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* The sequence conversion of ADC1 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* The sequence conversion of ADC2 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0A                  (0x34UL)  /* The preemption conversion of ADC0, 1 triggers input 0A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0B                  (0x35UL)  /* The preemption conversion of ADC0, 1 triggers input 0B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0C                  (0x36UL)  /* The preemption conversion of ADC0, 1 triggers input 0C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1A                  (0x37UL)  /* The preemption conversion of ADC0, 1 triggers input 1A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1B                  (0x38UL)  /* The preemption conversion of ADC0, 1 triggers input 1B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1C                  (0x39UL)  /* The preemption conversion of ADC0, 1 triggers input 1C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2A                  (0x3AUL)  /* The preemption conversion of ADC0, 1 triggers input 2A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2B                  (0x3BUL)  /* The preemption conversion of ADC0, 1 triggers input 2B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2C                  (0x3CUL)  /* The preemption conversion of ADC0, 1 triggers input 2C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3A                  (0x3DUL)  /* The preemption conversion of ADC0, 1 triggers input 3A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3B                  (0x3EUL)  /* The preemption conversion of ADC0, 1 triggers input 3B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3C                  (0x3FUL)  /* The preemption conversion of ADC0, 1 triggers input 3C */
#define HPM_TRGM0_OUTPUT_SRC_VSC0_TRIG_IN0                 (0x40UL)  /* VSC0 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_VSC0_TRIG_IN1                 (0x41UL)  /* VSC0 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_RDC0_TRIG_IN0                 (0x42UL)  /* RDC0 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_RDC0_TRIG_IN1                 (0x43UL)  /* RDC0 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_TRIG_IN                  (0x44UL)  /* QEI0 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_QEI1_TRIG_IN                  (0x45UL)  /* QEI1 triggers input */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_PAUSE                    (0x46UL)  /* QEI0 Pause input */
#define HPM_TRGM0_OUTPUT_SRC_QEI1_PAUSE                    (0x47UL)  /* QEI1 Pause input */
#define HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN0                 (0x48UL)  /* QEO0 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1                 (0x49UL)  /* QEO0 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_QEO1_TRIG_IN0                 (0x4AUL)  /* QEO1 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO1_TRIG_IN1                 (0x4BUL)  /* QEO1 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN0                  (0x4CUL)  /* SEI triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN1                  (0x4DUL)  /* SEI triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN2                  (0x4EUL)  /* SEI triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN3                  (0x4FUL)  /* SEI triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN4                  (0x50UL)  /* SEI triggers input 4 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN5                  (0x51UL)  /* SEI triggers input 5 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN6                  (0x52UL)  /* SEI triggers input 6 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN7                  (0x53UL)  /* SEI triggers input 7 */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_CH0_WIN                 (0x54UL)  /* Comparator 0 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_CH1_WIN                 (0x55UL)  /* Comparator 1 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP1_CH0_WIN                 (0x56UL)  /* Comparator 2 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP1_CH1_WIN                 (0x57UL)  /* Comparator 3 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP2_CH0_WIN                 (0x58UL)  /* Comparator 4 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP2_CH1_WIN                 (0x59UL)  /* Comparator 5 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP3_CH0_WIN                 (0x5AUL)  /* Comparator 6 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_ACMP3_CH1_WIN                 (0x5BUL)  /* Comparator 7 window mode input */
#define HPM_TRGM0_OUTPUT_SRC_DAC0_BUFTRG                   (0x5CUL)  /* DAC0 buffer mode starts to trigger; This bit is also activated as DAC0 ladder mode 0 */
#define HPM_TRGM0_OUTPUT_SRC_DAC1_BUFTRG                   (0x5DUL)  /* DAC1 buffer mode starts to trigger; This bit is also activated as DAC1 ladder mode 0 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN2                    (0x5EUL)  /* GPTMR0 channel 2 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN3                    (0x5FUL)  /* GPTMR0 channel 3 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_SYNCI                  (0x60UL)  /* GPTMR0 counter synchronous input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN2                    (0x61UL)  /* GPTMR1 channel 2 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN3                    (0x62UL)  /* GPTMR1 channel 3 input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_SYNCI                  (0x63UL)  /* GPTMR1 counter synchronous input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_IN2                    (0x64UL)  /* GPTMR2 channel 2 input; This bit is also activated as DAC0 ladder mode 1 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_IN3                    (0x65UL)  /* GPTMR2 channel 3 input; This bit is also activated as DAC0 ladder mode 2 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_SYNCI                  (0x66UL)  /* GPTMR2 counter synchronous input; This bit is also activated as DAC0 ladder mode 3 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_IN2                    (0x67UL)  /* GPTMR3 channel 2 input; This bit is also activated as DAC1 ladder mode 1 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_IN3                    (0x68UL)  /* GPTMR3 channel 3 input; This bit is also activated as DAC1 ladder mode 2 */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_SYNCI                  (0x69UL)  /* GPTMR3 counter synchronous input; This bit is also activated as DAC1 ladder mode 3 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_00                     (0x6AUL)  /* PLB module input 00 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_01                     (0x6BUL)  /* PLB module input 01 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_02                     (0x6CUL)  /* PLB module input 02 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_03                     (0x6DUL)  /* PLB module input 03 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_04                     (0x6EUL)  /* PLB module input 04 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_05                     (0x6FUL)  /* PLB module input 05 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_06                     (0x70UL)  /* PLB module input 06 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_07                     (0x71UL)  /* PLB module input 07 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_08                     (0x72UL)  /* PLB module input 08 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_09                     (0x73UL)  /* PLB module input 09 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_10                     (0x74UL)  /* PLB module input 10 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_11                     (0x75UL)  /* PLB module input 11 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_12                     (0x76UL)  /* PLB module input 12 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_13                     (0x77UL)  /* PLB module input 13 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_14                     (0x78UL)  /* PLB module input 14 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_15                     (0x79UL)  /* PLB module input 15 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_16                     (0x7AUL)  /* PLB module input 16 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_17                     (0x7BUL)  /* PLB module input 17 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_18                     (0x7CUL)  /* PLB module input 18 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_19                     (0x7DUL)  /* PLB module input 19 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_20                     (0x7EUL)  /* PLB module input 20 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_21                     (0x7FUL)  /* PLB module input 21 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_22                     (0x80UL)  /* PLB module input 22 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_23                     (0x81UL)  /* PLB module input 23 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_24                     (0x82UL)  /* PLB module input 24 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_25                     (0x83UL)  /* PLB module input 25 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_26                     (0x84UL)  /* PLB module input 26 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_27                     (0x85UL)  /* PLB module input 27 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_28                     (0x86UL)  /* PLB module input 28 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_29                     (0x87UL)  /* PLB module input 29 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_30                     (0x88UL)  /* PLB module input 30 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_31                     (0x89UL)  /* PLB module input 31 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_32                     (0x8AUL)  /* PLB module input 32 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_33                     (0x8BUL)  /* PLB module input 33 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_34                     (0x8CUL)  /* PLB module input 34 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_35                     (0x8DUL)  /* PLB module input 35 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_36                     (0x8EUL)  /* PLB module input 36 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_37                     (0x8FUL)  /* PLB module input 37 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_38                     (0x90UL)  /* PLB module input 38 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_39                     (0x91UL)  /* PLB module input 39 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_40                     (0x92UL)  /* PLB module input 40 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_41                     (0x93UL)  /* PLB module input 41 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_42                     (0x94UL)  /* PLB module input 42 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_43                     (0x95UL)  /* PLB module input 43 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_44                     (0x96UL)  /* PLB module input 44 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_45                     (0x97UL)  /* PLB module input 45 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_46                     (0x98UL)  /* PLB module input 46 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_47                     (0x99UL)  /* PLB module input 47 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_48                     (0x9AUL)  /* PLB module input 48 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_49                     (0x9BUL)  /* PLB module input 49 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_50                     (0x9CUL)  /* PLB module input 50 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_51                     (0x9DUL)  /* PLB module input 51 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_52                     (0x9EUL)  /* PLB module input 52 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_53                     (0x9FUL)  /* PLB module input 53 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_54                     (0xA0UL)  /* PLB module input 54 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_55                     (0xA1UL)  /* PLB module input 55 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_56                     (0xA2UL)  /* PLB module input 56 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_57                     (0xA3UL)  /* PLB module input 57 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_58                     (0xA4UL)  /* PLB module input 58 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_59                     (0xA5UL)  /* PLB module input 59 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_60                     (0xA6UL)  /* PLB module input 60 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_61                     (0xA7UL)  /* PLB module input 61 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_62                     (0xA8UL)  /* PLB module input 62 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_63                     (0xA9UL)  /* PLB module input 63 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN0                 (0xAAUL)  /* PWM0 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN1                 (0xABUL)  /* PWM0 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN2                 (0xACUL)  /* PWM0 triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN3                 (0xADUL)  /* PWM0 triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN4                 (0xAEUL)  /* PWM0 triggers input 4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN5                 (0xAFUL)  /* PWM0 triggers input 5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN6                 (0xB0UL)  /* PWM0 triggers input 6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN7                 (0xB1UL)  /* PWM0 triggers input 7 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN0                 (0xB2UL)  /* PWM1 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN1                 (0xB3UL)  /* PWM1 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN2                 (0xB4UL)  /* PWM1 triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN3                 (0xB5UL)  /* PWM1 triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN4                 (0xB6UL)  /* PWM1 triggers input 4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN5                 (0xB7UL)  /* PWM1 triggers input 5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN6                 (0xB8UL)  /* PWM1 triggers input 6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN7                 (0xB9UL)  /* PWM1 triggers input 7 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN0                 (0xBAUL)  /* PWM2 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN1                 (0xBBUL)  /* PWM2 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN2                 (0xBCUL)  /* PWM2 triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN3                 (0xBDUL)  /* PWM2 triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN4                 (0xBEUL)  /* PWM2 triggers input 4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN5                 (0xBFUL)  /* PWM2 triggers input 5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN6                 (0xC0UL)  /* PWM2 triggers input 6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN7                 (0xC1UL)  /* PWM2 triggers input 7 */
#define HPM_TRGM0_OUTPUT_SRC_MCAN_PTPC0_CAP                (0xCAUL)  /* PTPC input capture 0 */
#define HPM_TRGM0_OUTPUT_SRC_MCAN_PTPC1_CAP                (0xCBUL)  /* PTPC input capture 1 */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG0                    (0xCCUL)  /* UART0/1/2/3 Trigger signal */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG1                    (0xCDUL)  /* UART4/5/6/7 Trigger signal */
#define HPM_TRGM0_OUTPUT_SRC_SYNCTIMER_TRIG                (0xCEUL)  /* SYNT triggers input */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ0                     (0xCFUL)  /* TRGM interrupt signal 0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ1                     (0xD0UL)  /* TRGM interrupt signal 1 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA0                     (0xD1UL)  /* TRGM DMA request 0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA1                     (0xD2UL)  /* TRGM DMA request 1 */
#define HPM_TRGM0_OUTPUT_SRC_MTG0_TRIG_IN0                 (0xD3UL)  /* MTG0 triggers input 0 */
#define HPM_TRGM0_OUTPUT_SRC_MTG0_TRIG_IN1                 (0xD4UL)  /* MTG0 triggers input 1 */
#define HPM_TRGM0_OUTPUT_SRC_MTG0_TRIG_IN2                 (0xD5UL)  /* MTG0 triggers input 2 */
#define HPM_TRGM0_OUTPUT_SRC_MTG0_TRIG_IN3                 (0xD6UL)  /* MTG0 triggers input 3 */
#define HPM_TRGM0_OUTPUT_SRC_SYNT_TRIG_IN                  (0xD7UL)  /* SYNT triggers input */

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
#define HPM_TRGM0_FILTER_SRC_PWM2_IN0                      (0x10UL)  /* PWM timer 2 capture Input 0 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN1                      (0x11UL)  /* PWM timer 2 capture Input 1 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN2                      (0x12UL)  /* PWM timer 2 capture Input 2 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN3                      (0x13UL)  /* PWM timer 2 capture Input 3 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN4                      (0x14UL)  /* PWM timer 2 capture Input 4 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN5                      (0x15UL)  /* PWM timer 2 capture Input 5 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN6                      (0x16UL)  /* PWM timer 2 capture Input 6 */
#define HPM_TRGM0_FILTER_SRC_PWM2_IN7                      (0x17UL)  /* PWM timer 2 capture Input 7 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P00                     (0x20UL)  /* TRGM iutput 0 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P01                     (0x21UL)  /* TRGM iutput 1 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P02                     (0x22UL)  /* TRGM iutput 2 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P03                     (0x23UL)  /* TRGM iutput 3 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P04                     (0x24UL)  /* TRGM iutput 4 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P05                     (0x25UL)  /* TRGM iutput 5 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P06                     (0x26UL)  /* TRGM iutput 6 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P07                     (0x27UL)  /* TRGM iutput 7 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P08                     (0x28UL)  /* TRGM iutput 8 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P09                     (0x29UL)  /* TRGM iutput 9 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P10                     (0x2AUL)  /* TRGM iutput 10 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P11                     (0x2BUL)  /* TRGM iutput 11 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P12                     (0x2CUL)  /* TRGM iutput 12 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P13                     (0x2DUL)  /* TRGM iutput 13 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P14                     (0x2EUL)  /* TRGM iutput 14 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P15                     (0x2FUL)  /* TRGM iutput 15 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P16                     (0x30UL)  /* TRGM iutput 16 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P17                     (0x31UL)  /* TRGM iutput 17 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P18                     (0x32UL)  /* TRGM iutput 18 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P19                     (0x33UL)  /* TRGM iutput 19 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P20                     (0x34UL)  /* TRGM iutput 20 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P21                     (0x35UL)  /* TRGM iutput 21 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P22                     (0x36UL)  /* TRGM iutput 22 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P23                     (0x37UL)  /* TRGM iutput 23 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P24                     (0x38UL)  /* TRGM iutput 24 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P25                     (0x39UL)  /* TRGM iutput 25 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P26                     (0x3AUL)  /* TRGM iutput 26 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P27                     (0x3BUL)  /* TRGM iutput 27 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P28                     (0x3CUL)  /* TRGM iutput 28 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P29                     (0x3DUL)  /* TRGM iutput 29 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P30                     (0x3EUL)  /* TRGM iutput 30 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P31                     (0x3FUL)  /* TRGM iutput 31 */

/* trgm0_dma mux definitions */
#define HPM_TRGM0_DMA_SRC_PWM0_REQ0                        (0x0UL)   /* DMA request 0 for PWM0 */
#define HPM_TRGM0_DMA_SRC_PWM0_REQ1                        (0x1UL)   /* DMA request 1 for PWM0 */
#define HPM_TRGM0_DMA_SRC_PWM0_REQ2                        (0x2UL)   /* DMA request 2 for PWM0 */
#define HPM_TRGM0_DMA_SRC_PWM0_REQ3                        (0x3UL)   /* DMA request 3 for PWM0 */
#define HPM_TRGM0_DMA_SRC_PWM1_REQ0                        (0x4UL)   /* DMA request 0 for PWM1 */
#define HPM_TRGM0_DMA_SRC_PWM1_REQ1                        (0x5UL)   /* DMA request 1 for PWM1 */
#define HPM_TRGM0_DMA_SRC_PWM1_REQ2                        (0x6UL)   /* DMA request 2 for PWM1 */
#define HPM_TRGM0_DMA_SRC_PWM1_REQ3                        (0x7UL)   /* DMA request 3 for PWM1 */
#define HPM_TRGM0_DMA_SRC_PWM2_REQ0                        (0x8UL)   /* DMA request 0 for PWM2 */
#define HPM_TRGM0_DMA_SRC_PWM2_REQ1                        (0x9UL)   /* DMA request 1 for PWM2 */
#define HPM_TRGM0_DMA_SRC_PWM2_REQ2                        (0xAUL)   /* DMA request 2 for PWM2 */
#define HPM_TRGM0_DMA_SRC_PWM2_REQ3                        (0xBUL)   /* DMA request 3 for PWM2 */
#define HPM_TRGM0_DMA_SRC_QEI0_REQ                         (0x10UL)  /* DMA request for QEI0 */
#define HPM_TRGM0_DMA_SRC_QEI1_REQ                         (0x11UL)  /* DMA request for QEI1 */
#define HPM_TRGM0_DMA_SRC_SEI_REQ0                         (0x12UL)  /* DMA request 0 for SEI0 */
#define HPM_TRGM0_DMA_SRC_SEI_REQ1                         (0x13UL)  /* DMA request 0 for SEI1 */
#define HPM_TRGM0_DMA_SRC_TRGM0                            (0x14UL)  /* DMA request 0 for TRGM (from TRGM output 209) */
#define HPM_TRGM0_DMA_SRC_TRGM1                            (0x15UL)  /* DMA request 1 for TRGM (from TRGM output 210) */



#endif /* HPM_TRGMMUX_SRC_H */
