/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_TRGMMUX_SRC_H
#define HPM_TRGMMUX_SRC_H

/* trgm0_input mux definitions */
#define HPM_TRGM0_INPUT_SRC_VSS                            (0x0UL)   /* low level voltage */
#define HPM_TRGM0_INPUT_SRC_VDD                            (0x1UL)   /* high level voltage */
#define HPM_TRGM0_INPUT_SRC_USB0_SOF                       (0x2UL)   /* USB0 start of frame marker */
#define HPM_TRGM0_INPUT_SRC_ENET0_PTP3                     (0x3UL)   /* ENET0 PTP output bit3 */
#define HPM_TRGM0_INPUT_SRC_ESC_SYNC0                      (0x4UL)   /* ethercat sync0 event */
#define HPM_TRGM0_INPUT_SRC_TSN_PTP3                       (0x5UL)   /* TSW PTP output bit 3 */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP0                      (0x6UL)   /* PTPC compare output0 */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP1                      (0x7UL)   /* PTPC compare output1 */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN0                    (0x8UL)   /* PWM0 input bit0(from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CAPIN1                    (0x9UL)   /* PWM0 input bit1(from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN0                    (0xAUL)   /* PWM1 input bit0(from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM1_CAPIN1                    (0xBUL)   /* PWM1 input bit1(from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM2_CAPIN0                    (0xCUL)   /* PWM2 input bit0(from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM2_CAPIN1                    (0xDUL)   /* PWM2 input bit1(from IO) */
#define HPM_TRGM0_INPUT_SRC_PWM3_CAPIN0                    (0xEUL)   /* PWM3 input bit0(from IO) */
#define HPM_TRGM0_INPUT_SRC_ESC_SYNC1                      (0xFUL)   /* ethercat sync1 event */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH00                      (0x10UL)  /* SYNT channel0 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH01                      (0x11UL)  /* SYNT channel1 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH02                      (0x12UL)  /* SYNT channel2 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH03                      (0x13UL)  /* SYNT channel3 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH04                      (0x14UL)  /* SYNT channel4 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH05                      (0x15UL)  /* SYNT channel5 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH06                      (0x16UL)  /* SYNT channel6 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH07                      (0x17UL)  /* SYNT channel7 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH08                      (0x18UL)  /* SYNT channel8 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH09                      (0x19UL)  /* SYNT channel9 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH10                      (0x1AUL)  /* SYNT channel10 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH11                      (0x1BUL)  /* SYNT channel11 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH12                      (0x1CUL)  /* SYNT channel12 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH13                      (0x1DUL)  /* SYNT channel13 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH14                      (0x1EUL)  /* SYNT channel14 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT_CH15                      (0x1FUL)  /* SYNT channel15 pulse output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT2                    (0x20UL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT3                    (0x21UL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT2                    (0x22UL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT3                    (0x23UL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR2_OUT2                    (0x24UL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR2_OUT3                    (0x25UL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR3_OUT2                    (0x26UL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR3_OUT3                    (0x27UL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR4_OUT2                    (0x28UL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR4_OUT3                    (0x29UL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR5_OUT2                    (0x2AUL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR5_OUT3                    (0x2BUL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR6_OUT2                    (0x2CUL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR6_OUT3                    (0x2DUL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR7_OUT2                    (0x2EUL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR7_OUT3                    (0x2FUL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP0_CH0_OUT                  (0x30UL)  /* CMP0 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP0_CH1_OUT                  (0x31UL)  /* CMP1 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_CH0_OUT                  (0x32UL)  /* CMP2 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_CH1_OUT                  (0x33UL)  /* CMP3 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP2_CH0_OUT                  (0x34UL)  /* CMP4 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP2_CH1_OUT                  (0x35UL)  /* CMP5 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP3_CH0_OUT                  (0x36UL)  /* CMP6 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP3_CH1_OUT                  (0x37UL)  /* CMP7 compare output */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_0                     (0x38UL)  /* SEI trigger out0 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_1                     (0x39UL)  /* SEI trigger out1 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_2                     (0x3AUL)  /* SEI trigger out2 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_3                     (0x3BUL)  /* SEI trigger out3 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_4                     (0x3CUL)  /* SEI trigger out4 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_5                     (0x3DUL)  /* SEI trigger out5 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_6                     (0x3EUL)  /* SEI trigger out6 */
#define HPM_TRGM0_INPUT_SRC_SEI_TRGO_7                     (0x3FUL)  /* SEI trigger out7 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_0                    (0x40UL)  /* PWM0 trigger out0 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_1                    (0x41UL)  /* PWM0 trigger out1 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_2                    (0x42UL)  /* PWM0 trigger out2 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_3                    (0x43UL)  /* PWM0 trigger out3 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_4                    (0x44UL)  /* PWM0 trigger out4 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_5                    (0x45UL)  /* PWM0 trigger out5 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_6                    (0x46UL)  /* PWM0 trigger out6 */
#define HPM_TRGM0_INPUT_SRC_PWM0_TRGO_7                    (0x47UL)  /* PWM0 trigger out7 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_0                    (0x48UL)  /* PWM1 trigger out0 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_1                    (0x49UL)  /* PWM1 trigger out1 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_2                    (0x4AUL)  /* PWM1 trigger out2 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_3                    (0x4BUL)  /* PWM1 trigger out3 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_4                    (0x4CUL)  /* PWM1 trigger out4 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_5                    (0x4DUL)  /* PWM1 trigger out5 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_6                    (0x4EUL)  /* PWM1 trigger out6 */
#define HPM_TRGM0_INPUT_SRC_PWM1_TRGO_7                    (0x46UL)  /* PWM1 trigger out7 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_0                    (0x50UL)  /* PWM2 trigger out0 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_1                    (0x51UL)  /* PWM2 trigger out1 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_2                    (0x52UL)  /* PWM2 trigger out2 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_3                    (0x53UL)  /* PWM2 trigger out3 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_4                    (0x54UL)  /* PWM2 trigger out4 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_5                    (0x55UL)  /* PWM2 trigger out5 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_6                    (0x56UL)  /* PWM2 trigger out6 */
#define HPM_TRGM0_INPUT_SRC_PWM2_TRGO_7                    (0x57UL)  /* PWM2 trigger out7 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRGO_0                    (0x58UL)  /* PWM3 trigger out0 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRGO_1                    (0x59UL)  /* PWM3 trigger out1 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRGO_2                    (0x5AUL)  /* PWM3 trigger out2 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRGO_3                    (0x5BUL)  /* PWM3 trigger out3 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRGO_4                    (0x5CUL)  /* PWM3 trigger out4 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRGO_5                    (0x5DUL)  /* PWM3 trigger out5 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRGO_6                    (0x5EUL)  /* PWM3 trigger out6 */
#define HPM_TRGM0_INPUT_SRC_PWM3_TRGO_7                    (0x5FUL)  /* PWM3 trigger out7 */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P00                      (0x60UL)  /* TRGM input data0(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P01                      (0x61UL)  /* TRGM input data1(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P02                      (0x62UL)  /* TRGM input data2(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P03                      (0x63UL)  /* TRGM input data3(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P04                      (0x64UL)  /* TRGM input data4(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P05                      (0x65UL)  /* TRGM input data5(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P06                      (0x66UL)  /* TRGM input data6(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P07                      (0x67UL)  /* TRGM input data7(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P08                      (0x68UL)  /* TRGM input data8(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P09                      (0x69UL)  /* TRGM input data9(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P10                      (0x6AUL)  /* TRGM input data10(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P11                      (0x6BUL)  /* TRGM input data11(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P12                      (0x6CUL)  /* TRGM input data12(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P13                      (0x6DUL)  /* TRGM input data13(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P14                      (0x6EUL)  /* TRGM input data14(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P15                      (0x6FUL)  /* TRGM input data15(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P16                      (0x70UL)  /* TRGM input data16(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P17                      (0x71UL)  /* TRGM input data17(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P18                      (0x72UL)  /* TRGM input data18(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P19                      (0x73UL)  /* TRGM input data19(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P20                      (0x74UL)  /* TRGM input data20(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P21                      (0x75UL)  /* TRGM input data21(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P22                      (0x76UL)  /* TRGM input data22(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P23                      (0x77UL)  /* TRGM input data23(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P24                      (0x78UL)  /* TRGM input data24(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P25                      (0x79UL)  /* TRGM input data25(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P26                      (0x7AUL)  /* TRGM input data26(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P27                      (0x7BUL)  /* TRGM input data27(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P28                      (0x7CUL)  /* TRGM input data28(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P29                      (0x7DUL)  /* TRGM input data29(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P30                      (0x7EUL)  /* TRGM input data30(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P31                      (0x7FUL)  /* TRGM input data31(from IO) */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT00                      (0x80UL)  /* PLB trigger out0 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT01                      (0x81UL)  /* PLB trigger out1 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT02                      (0x82UL)  /* PLB trigger out2 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT03                      (0x83UL)  /* PLB trigger out3 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT04                      (0x84UL)  /* PLB trigger out4 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT05                      (0x85UL)  /* PLB trigger out5 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT06                      (0x86UL)  /* PLB trigger out6 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT07                      (0x87UL)  /* PLB trigger out7 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT08                      (0x88UL)  /* PLB trigger out8 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT09                      (0x89UL)  /* PLB trigger out9 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT10                      (0x8AUL)  /* PLB trigger out10 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT11                      (0x8BUL)  /* PLB trigger out11 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT12                      (0x8CUL)  /* PLB trigger out12 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT13                      (0x8DUL)  /* PLB trigger out13 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT14                      (0x8EUL)  /* PLB trigger out14 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT15                      (0x8FUL)  /* PLB trigger out15 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT16                      (0x90UL)  /* PLB trigger out16 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT17                      (0x91UL)  /* PLB trigger out17 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT18                      (0x92UL)  /* PLB trigger out18 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT19                      (0x93UL)  /* PLB trigger out19 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT20                      (0x94UL)  /* PLB trigger out20 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT21                      (0x95UL)  /* PLB trigger out21 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT22                      (0x96UL)  /* PLB trigger out22 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT23                      (0x97UL)  /* PLB trigger out23 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT24                      (0x98UL)  /* PLB trigger out24 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT25                      (0x99UL)  /* PLB trigger out25 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT26                      (0x9AUL)  /* PLB trigger out26 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT27                      (0x9BUL)  /* PLB trigger out27 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT28                      (0x9CUL)  /* PLB trigger out28 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT29                      (0x9DUL)  /* PLB trigger out29 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT30                      (0x9EUL)  /* PLB trigger out30 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT31                      (0x9FUL)  /* PLB trigger out31 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT32                      (0xA0UL)  /* PLB trigger out32 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT33                      (0xA1UL)  /* PLB trigger out33 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT34                      (0xA2UL)  /* PLB trigger out34 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT35                      (0xA3UL)  /* PLB trigger out35 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT36                      (0xA4UL)  /* PLB trigger out36 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT37                      (0xA5UL)  /* PLB trigger out37 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT38                      (0xA6UL)  /* PLB trigger out38 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT39                      (0xA7UL)  /* PLB trigger out39 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT40                      (0xA8UL)  /* PLB trigger out40 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT41                      (0xA9UL)  /* PLB trigger out41 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT42                      (0xAAUL)  /* PLB trigger out42 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT43                      (0xABUL)  /* PLB trigger out43 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT44                      (0xACUL)  /* PLB trigger out44 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT45                      (0xADUL)  /* PLB trigger out45 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT46                      (0xAEUL)  /* PLB trigger out46 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT47                      (0xAFUL)  /* PLB trigger out47 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT48                      (0xB0UL)  /* PLB trigger out48 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT49                      (0xB1UL)  /* PLB trigger out49 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT50                      (0xB2UL)  /* PLB trigger out50 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT51                      (0xAAUL)  /* PLB trigger out51 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT52                      (0xB4UL)  /* PLB trigger out52 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT53                      (0xB5UL)  /* PLB trigger out53 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT54                      (0xB6UL)  /* PLB trigger out54 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT55                      (0xB7UL)  /* PLB trigger out55 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT56                      (0xB8UL)  /* PLB trigger out56 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT57                      (0xB9UL)  /* PLB trigger out57 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT58                      (0xBAUL)  /* PLB trigger out58 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT59                      (0xBBUL)  /* PLB trigger out59 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT60                      (0xBCUL)  /* PLB trigger out60 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT61                      (0xBDUL)  /* PLB trigger out61 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT62                      (0xBEUL)  /* PLB trigger out62 */
#define HPM_TRGM0_INPUT_SRC_PLB_OUT63                      (0xBFUL)  /* PLB trigger out63 */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPHZ0                   (0xC0UL)  /* inform of sdm1 channel0 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPHZ1                   (0xC1UL)  /* inform of sdm1 channel1 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPHZ2                   (0xC2UL)  /* inform of sdm1 channel2 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPHZ3                   (0xC3UL)  /* inform of sdm1 channel3 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPHA0                   (0xC4UL)  /* inform of sdm1 channel0 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPHA1                   (0xC5UL)  /* inform of sdm1 channel1 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPHA2                   (0xC6UL)  /* inform of sdm1 channel2 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPHA3                   (0xC7UL)  /* inform of sdm1 channel3 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPL0                    (0xC8UL)  /* inform of sdm1 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPL1                    (0xC9UL)  /* inform of sdm1 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPL2                    (0xCAUL)  /* inform of sdm1 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM1_COMPL3                    (0xCBUL)  /* inform of sdm1 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ0                   (0xCCUL)  /* inform of sdm0 channel0 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ1                   (0xCDUL)  /* inform of sdm0 channel1 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ2                   (0xCEUL)  /* inform of sdm0 channel2 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHZ3                   (0xCFUL)  /* inform of sdm0 channel3 value through zero */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL0                    (0xD0UL)  /* inform of sdm0 channel0 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL1                    (0xD1UL)  /* inform of sdm0 channel1 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL2                    (0xD2UL)  /* inform of sdm0 channel2 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPL3                    (0xD3UL)  /* inform of sdm0 channel3 value over upper limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA0                   (0xD4UL)  /* inform of sdm0 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA1                   (0xD5UL)  /* inform of sdm0 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA2                   (0xD6UL)  /* inform of sdm0 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_SDM0_COMPHA3                   (0xD7UL)  /* inform of sdm0 channel value beyond lower limit */
#define HPM_TRGM0_INPUT_SRC_ADC0_TRGO                      (0xD8UL)  /* ADC0 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC1_TRGO                      (0xD9UL)  /* ADC1 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC2_TRGO                      (0xDAUL)  /* ADC2 trigger out */
#define HPM_TRGM0_INPUT_SRC_ADC3_TRGO                      (0xDBUL)  /* ADC3 trigger out */
#define HPM_TRGM0_INPUT_SRC_RDC0_TRGO_0                    (0xDCUL)  /* RDC0 trigger out0 */
#define HPM_TRGM0_INPUT_SRC_RDC0_TRGO_1                    (0xDDUL)  /* RDC0 trigger out1 */
#define HPM_TRGM0_INPUT_SRC_RDC1_TRGO_0                    (0xDEUL)  /* RDC1 trigger out0 */
#define HPM_TRGM0_INPUT_SRC_RDC1_TRGO_1                    (0xDFUL)  /* RDC1 trigger out1 */
#define HPM_TRGM0_INPUT_SRC_DEBUG_FLAG                     (0xE0UL)  /* debug mode flag */
#define HPM_TRGM0_INPUT_SRC_QEI0_TRGO                      (0xE1UL)  /* QEI0 trigger out */
#define HPM_TRGM0_INPUT_SRC_QEI1_TRGO                      (0xE2UL)  /* QEI1 trigger out */
#define HPM_TRGM0_INPUT_SRC_QEI2_TRGO                      (0xE3UL)  /* QEI2 trigger out */
#define HPM_TRGM0_INPUT_SRC_QEI3_TRGO                      (0xE4UL)  /* QEI3 trigger out */

/* trgm0_output mux definitions */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO0                     (0x0UL)   /* TRGM output data0(to IO) (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P00                     (0x0UL)   /* TRGM output data0(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO1                     (0x1UL)   /* TRGM output data1(to IO) (dropped macro  please use macro TRGM0_P01) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P01                     (0x1UL)   /* TRGM output data1(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO2                     (0x2UL)   /* TRGM output data2(to IO) (dropped macro  please use macro TRGM0_P02) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P02                     (0x2UL)   /* TRGM output data2(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO3                     (0x3UL)   /* TRGM output data3(to IO) (dropped macro  please use macro TRGM0_P03) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P03                     (0x3UL)   /* TRGM output data3(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO4                     (0x4UL)   /* TRGM output data4(to IO) (dropped macro  please use macro TRGM0_P04) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P04                     (0x4UL)   /* TRGM output data4(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO5                     (0x5UL)   /* TRGM output data5(to IO) (dropped macro  please use macro TRGM0_P05) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P05                     (0x5UL)   /* TRGM output data5(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO6                     (0x6UL)   /* TRGM output data6(to IO) (dropped macro  please use macro TRGM0_P06) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P06                     (0x6UL)   /* TRGM output data6(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO7                     (0x7UL)   /* TRGM output data7(to IO) (dropped macro  please use macro TRGM0_P07) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P07                     (0x7UL)   /* TRGM output data7(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO8                     (0x8UL)   /* TRGM output data8(to IO) (dropped macro  please use macro TRGM0_P08) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P08                     (0x8UL)   /* TRGM output data8(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO9                     (0x9UL)   /* TRGM output data9(to IO) (dropped macro  please use macro TRGM0_P09) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P09                     (0x9UL)   /* TRGM output data9(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO10                    (0xAUL)   /* TRGM output data10(to IO) (dropped macro  please use macro TRGM0_P10) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P10                     (0xAUL)   /* TRGM output data10(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO11                    (0xBUL)   /* TRGM output data11(to IO) (dropped macro  please use macro TRGM0_P11) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P11                     (0xBUL)   /* TRGM output data11(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO12                    (0xCUL)   /* TRGM output data12(to IO) (dropped macro  please use macro TRGM0_P12) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P12                     (0xCUL)   /* TRGM output data12(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO13                    (0xDUL)   /* TRGM output data13(to IO) (dropped macro  please use macro TRGM0_P13) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P13                     (0xDUL)   /* TRGM output data13(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO14                    (0xEUL)   /* TRGM output data14(to IO) (dropped macro  please use macro TRGM0_P14) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P14                     (0xEUL)   /* TRGM output data14(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO15                    (0xFUL)   /* TRGM output data15(to IO) (dropped macro  please use macro TRGM0_P15) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P15                     (0xFUL)   /* TRGM output data15(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO16                    (0x10UL)  /* TRGM output data16(to IO) (dropped macro  please use macro TRGM0_P16) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P16                     (0x10UL)  /* TRGM output data16(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO17                    (0x11UL)  /* TRGM output data17(to IO) (dropped macro  please use macro TRGM0_P17) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P17                     (0x11UL)  /* TRGM output data17(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO18                    (0x12UL)  /* TRGM output data18(to IO) (dropped macro  please use macro TRGM0_P18) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P18                     (0x12UL)  /* TRGM output data18(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO19                    (0x13UL)  /* TRGM output data19(to IO) (dropped macro  please use macro TRGM0_P19) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P19                     (0x13UL)  /* TRGM output data19(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO20                    (0x14UL)  /* TRGM output data20(to IO) (dropped macro  please use macro TRGM0_P20) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P20                     (0x14UL)  /* TRGM output data20(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO21                    (0x15UL)  /* TRGM output data21(to IO) (dropped macro  please use macro TRGM0_P21) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P21                     (0x15UL)  /* TRGM output data21(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO22                    (0x16UL)  /* TRGM output data22(to IO) (dropped macro  please use macro TRGM0_P22) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P22                     (0x16UL)  /* TRGM output data22(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO23                    (0x17UL)  /* TRGM output data23(to IO) (dropped macro  please use macro TRGM0_P23) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P23                     (0x17UL)  /* TRGM output data23(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO24                    (0x18UL)  /* TRGM output data24(to IO) (dropped macro  please use macro TRGM0_P24) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P24                     (0x18UL)  /* TRGM output data24(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO25                    (0x19UL)  /* TRGM output data25(to IO) (dropped macro  please use macro TRGM0_P25) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P25                     (0x19UL)  /* TRGM output data25(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO26                    (0x1AUL)  /* TRGM output data26(to IO) (dropped macro  please use macro TRGM0_P26) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P26                     (0x1AUL)  /* TRGM output data26(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO27                    (0x1BUL)  /* TRGM output data27(to IO) (dropped macro  please use macro TRGM0_P27) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P27                     (0x1BUL)  /* TRGM output data27(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO28                    (0x1CUL)  /* TRGM output data28(to IO) (dropped macro  please use macro TRGM0_P28) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P28                     (0x1CUL)  /* TRGM output data28(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO29                    (0x1DUL)  /* TRGM output data29(to IO) (dropped macro  please use macro TRGM0_P29) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P29                     (0x1DUL)  /* TRGM output data29(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO30                    (0x1EUL)  /* TRGM output data30(to IO) (dropped macro  please use macro TRGM0_P30) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P30                     (0x1EUL)  /* TRGM output data30(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_MOT_GPIO31                    (0x1FUL)  /* TRGM output data31(to IO) (dropped macro  please use macro TRGM0_P31) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P31                     (0x1FUL)  /* TRGM output data31(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC0                  (0x20UL)  /* SDM sync event input0 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC1                  (0x21UL)  /* SDM sync event input1 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC2                  (0x22UL)  /* SDM sync event input2 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC3                  (0x23UL)  /* SDM sync event input3 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC4                  (0x24UL)  /* SDM sync event input4 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC5                  (0x25UL)  /* SDM sync event input5 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC6                  (0x26UL)  /* SDM sync event input6 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC7                  (0x27UL)  /* SDM sync event input7 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC8                  (0x28UL)  /* SDM sync event input8 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC9                  (0x29UL)  /* SDM sync event input9 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC10                 (0x2AUL)  /* SDM sync event input10 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC11                 (0x2BUL)  /* SDM sync event input11 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC12                 (0x2CUL)  /* SDM sync event input12 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC13                 (0x2DUL)  /* SDM sync event input13 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC14                 (0x2EUL)  /* SDM sync event input14 */
#define HPM_TRGM0_OUTPUT_SRC_SDM_PWM_SOC15                 (0x2FUL)  /* SDM sync event input15 */
#define HPM_TRGM0_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* ADC0 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* ADC1 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* ADC2 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC3_STRGI                    (0x33UL)  /* ADC3 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0A                  (0x34UL)  /* ADC preemption trigger0A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0B                  (0x35UL)  /* ADC preemption trigger0B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0C                  (0x36UL)  /* ADC preemption trigger0C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1A                  (0x37UL)  /* ADC preemption trigger1A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1B                  (0x38UL)  /* ADC preemption trigger1B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI1C                  (0x39UL)  /* ADC preemption trigger1C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2A                  (0x3AUL)  /* ADC preemption trigger2A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2B                  (0x3BUL)  /* ADC preemption trigger2B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI2C                  (0x3CUL)  /* ADC preemption trigger2C */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3A                  (0x3DUL)  /* ADC preemption trigger3A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3B                  (0x3EUL)  /* ADC preemption trigger3B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI3C                  (0x3FUL)  /* ADC preemption trigger3C */
#define HPM_TRGM0_OUTPUT_SRC_VSC0_TRIG_IN0                 (0x40UL)  /* VSC0 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_VSC0_TRIG_IN1                 (0x41UL)  /* VSC0 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_VSC1_TRIG_IN0                 (0x42UL)  /* VSC1 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_VSC1_TRIG_IN1                 (0x43UL)  /* VSC1 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_RDC0_TRIG_IN0                 (0x44UL)  /* RDC0 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_RDC0_TRIG_IN1                 (0x45UL)  /* RDC0 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_RDC1_TRIG_IN0                 (0x46UL)  /* RDC1 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_RDC1_TRIG_IN1                 (0x47UL)  /* RDC1 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_TRIG_IN                  (0x48UL)  /* QEI0 trigger in */
#define HPM_TRGM0_OUTPUT_SRC_QEI1_TRIG_IN                  (0x49UL)  /* QEI1 trigger in */
#define HPM_TRGM0_OUTPUT_SRC_QEI2_TRIG_IN                  (0x4AUL)  /* QEI2 trigger in */
#define HPM_TRGM0_OUTPUT_SRC_QEI3_TRIG_IN                  (0x4BUL)  /* QEI3 trigger in */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_PAUSE                    (0x4CUL)  /* QEI0 pause */
#define HPM_TRGM0_OUTPUT_SRC_QEI1_PAUSE                    (0x4DUL)  /* QEI1 pause */
#define HPM_TRGM0_OUTPUT_SRC_QEI2_PAUSE                    (0x4EUL)  /* QEI2 pause */
#define HPM_TRGM0_OUTPUT_SRC_QEI3_PAUSE                    (0x4FUL)  /* QEI3 pause */
#define HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN0                 (0x50UL)  /* QEO0 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO0_TRIG_IN1                 (0x51UL)  /* QEO0 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_QEO1_TRIG_IN0                 (0x52UL)  /* QEO1 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO1_TRIG_IN1                 (0x53UL)  /* QEO1 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_QEO2_TRIG_IN0                 (0x54UL)  /* QEO2 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO2_TRIG_IN1                 (0x55UL)  /* QEO2 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_QEO3_TRIG_IN0                 (0x56UL)  /* QEO3 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_QEO3_TRIG_IN1                 (0x57UL)  /* QEO3 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN0                  (0x58UL)  /* SEI trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN1                  (0x59UL)  /* SEI trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN2                  (0x5AUL)  /* SEI trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN3                  (0x5BUL)  /* SEI trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN4                  (0x5CUL)  /* SEI trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN5                  (0x5DUL)  /* SEI trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN6                  (0x5EUL)  /* SEI trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_SEI_TRIG_IN7                  (0x5FUL)  /* SEI trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_CH0_WIN                 (0x60UL)  /* CMP0 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_CH1_WIN                 (0x61UL)  /* CMP1 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP1_CH0_WIN                 (0x62UL)  /* CMP2 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP1_CH1_WIN                 (0x63UL)  /* CMP3 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP2_CH0_WIN                 (0x64UL)  /* CMP4 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP2_CH1_WIN                 (0x65UL)  /* CMP5 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP3_CH0_WIN                 (0x66UL)  /* CMP6 window */
#define HPM_TRGM0_OUTPUT_SRC_ACMP3_CH1_WIN                 (0x67UL)  /* CMP7 window */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN2                    (0x68UL)  /* GPTMR0 channe2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN3                    (0x69UL)  /* GPTMR0 channe3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_SYNCI                  (0x6AUL)  /* GPTMR0 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN2                    (0x6BUL)  /* GPTMR1 channe2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN3                    (0x6CUL)  /* GPTMR1 channe3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_SYNCI                  (0x6DUL)  /* GPTMR1 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_IN2                    (0x6EUL)  /* GPTMR2 channe2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_IN3                    (0x6FUL)  /* GPTMR2 channe3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR2_SYNCI                  (0x70UL)  /* GPTMR2 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_IN2                    (0x71UL)  /* GPTMR3 channe2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_IN3                    (0x72UL)  /* GPTMR3 channe3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR3_SYNCI                  (0x73UL)  /* GPTMR3 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR4_IN2                    (0x74UL)  /* GPTMR4 channe2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR4_IN3                    (0x75UL)  /* GPTMR4 channe3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR4_SYNCI                  (0x76UL)  /* GPTMR4 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR5_IN2                    (0x77UL)  /* GPTMR5 channe2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR5_IN3                    (0x78UL)  /* GPTMR5 channe3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR5_SYNCI                  (0x79UL)  /* GPTMR5 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR6_IN2                    (0x7AUL)  /* GPTMR6 channe2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR6_IN3                    (0x7BUL)  /* GPTMR6 channe3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR6_SYNCI                  (0x7CUL)  /* GPTMR6 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR7_IN2                    (0x7DUL)  /* GPTMR7 channe2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR7_IN3                    (0x7EUL)  /* GPTMR7 channe3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR7_SYNCI                  (0x7FUL)  /* GPTMR7 hardware sync in */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_00                     (0x80UL)  /* PLB trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_01                     (0x81UL)  /* PLB trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_02                     (0x82UL)  /* PLB trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_03                     (0x83UL)  /* PLB trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_04                     (0x84UL)  /* PLB trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_05                     (0x85UL)  /* PLB trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_06                     (0x86UL)  /* PLB trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_07                     (0x87UL)  /* PLB trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_08                     (0x88UL)  /* PLB trigger in8 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_09                     (0x89UL)  /* PLB trigger in9 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_10                     (0x8AUL)  /* PLB trigger in10 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_11                     (0x8BUL)  /* PLB trigger in11 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_12                     (0x8CUL)  /* PLB trigger in12 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_13                     (0x8DUL)  /* PLB trigger in13 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_14                     (0x8EUL)  /* PLB trigger in14 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_15                     (0x8FUL)  /* PLB trigger in15 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_16                     (0x90UL)  /* PLB trigger in16 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_17                     (0x91UL)  /* PLB trigger in17 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_18                     (0x92UL)  /* PLB trigger in18 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_19                     (0x93UL)  /* PLB trigger in19 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_20                     (0x94UL)  /* PLB trigger in20 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_21                     (0x95UL)  /* PLB trigger in21 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_22                     (0x96UL)  /* PLB trigger in22 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_23                     (0x97UL)  /* PLB trigger in23 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_24                     (0x98UL)  /* PLB trigger in24 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_25                     (0x99UL)  /* PLB trigger in25 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_26                     (0x9AUL)  /* PLB trigger in26 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_27                     (0x9BUL)  /* PLB trigger in27 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_28                     (0x9CUL)  /* PLB trigger in28 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_29                     (0x9DUL)  /* PLB trigger in29 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_30                     (0x9EUL)  /* PLB trigger in30 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_31                     (0x9FUL)  /* PLB trigger in31 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_32                     (0xA0UL)  /* PLB trigger in32 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_33                     (0xA1UL)  /* PLB trigger in33 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_34                     (0xA2UL)  /* PLB trigger in34 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_35                     (0xA3UL)  /* PLB trigger in35 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_36                     (0xA4UL)  /* PLB trigger in36 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_37                     (0xA5UL)  /* PLB trigger in37 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_38                     (0xA6UL)  /* PLB trigger in38 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_39                     (0xA7UL)  /* PLB trigger in39 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_40                     (0xA8UL)  /* PLB trigger in40 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_41                     (0xA9UL)  /* PLB trigger in41 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_42                     (0xAAUL)  /* PLB trigger in42 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_43                     (0xABUL)  /* PLB trigger in43 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_44                     (0xACUL)  /* PLB trigger in44 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_45                     (0xADUL)  /* PLB trigger in45 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_46                     (0xAEUL)  /* PLB trigger in46 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_47                     (0xAFUL)  /* PLB trigger in47 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_48                     (0xB0UL)  /* PLB trigger in48 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_49                     (0xB1UL)  /* PLB trigger in49 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_50                     (0xB2UL)  /* PLB trigger in50 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_51                     (0xB3UL)  /* PLB trigger in51 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_52                     (0xB4UL)  /* PLB trigger in52 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_53                     (0xB5UL)  /* PLB trigger in53 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_54                     (0xB6UL)  /* PLB trigger in54 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_55                     (0xB7UL)  /* PLB trigger in55 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_56                     (0xB8UL)  /* PLB trigger in56 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_57                     (0xB9UL)  /* PLB trigger in57 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_58                     (0xBAUL)  /* PLB trigger in58 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_59                     (0xBBUL)  /* PLB trigger in59 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_60                     (0xBCUL)  /* PLB trigger in60 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_61                     (0xBDUL)  /* PLB trigger in61 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_62                     (0xBEUL)  /* PLB trigger in62 */
#define HPM_TRGM0_OUTPUT_SRC_PLB_IN_63                     (0xBFUL)  /* PLB trigger in63 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN0                 (0xC0UL)  /* PWM0 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN1                 (0xC1UL)  /* PWM0 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN2                 (0xC2UL)  /* PWM0 trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN3                 (0xC3UL)  /* PWM0 trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN4                 (0xC4UL)  /* PWM0 trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN5                 (0xC5UL)  /* PWM0 trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN6                 (0xC6UL)  /* PWM0 trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_TRIG_IN7                 (0xC7UL)  /* PWM0 trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN0                 (0xC8UL)  /* PWM1 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN1                 (0xC9UL)  /* PWM1 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN2                 (0xCAUL)  /* PWM1 trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN3                 (0xCBUL)  /* PWM1 trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN4                 (0xCCUL)  /* PWM1 trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN5                 (0xCDUL)  /* PWM1 trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN6                 (0xCEUL)  /* PWM1 trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM1_TRIG_IN7                 (0xCFUL)  /* PWM1 trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN0                 (0xD0UL)  /* PWM2 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN1                 (0xD1UL)  /* PWM2 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN2                 (0xD2UL)  /* PWM2 trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN3                 (0xD3UL)  /* PWM2 trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN4                 (0xD4UL)  /* PWM2 trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN5                 (0xD5UL)  /* PWM2 trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN6                 (0xD6UL)  /* PWM2 trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM2_TRIG_IN7                 (0xD7UL)  /* PWM2 trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_TRIG_IN0                 (0xD8UL)  /* PWM3 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_TRIG_IN1                 (0xD9UL)  /* PWM3 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_TRIG_IN2                 (0xDAUL)  /* PWM3 trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_TRIG_IN3                 (0xDBUL)  /* PWM3 trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_TRIG_IN4                 (0xDCUL)  /* PWM3 trigger in4 */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_TRIG_IN5                 (0xDDUL)  /* PWM3 trigger in5 */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_TRIG_IN6                 (0xDEUL)  /* PWM3 trigger in6 */
#define HPM_TRGM0_OUTPUT_SRC_PWM3_TRIG_IN7                 (0xDFUL)  /* PWM3 trigger in7 */
#define HPM_TRGM0_OUTPUT_SRC_MCAN_PTPC0_CAP                (0xE0UL)  /* PTPC capture in0 */
#define HPM_TRGM0_OUTPUT_SRC_MCAN_PTPC1_CAP                (0xE1UL)  /* PTPC capture in1 */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG0                    (0xE2UL)  /* UART0 ~ UART3 trigger event */
#define HPM_TRGM0_OUTPUT_SRC_UART_TRIG1                    (0xE3UL)  /* UART4 ~ UART7 trigger event */
#define HPM_TRGM0_OUTPUT_SRC_SYNCTIMER_TRIG                (0xE4UL)  /* SYNT sync trigger */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ0                     (0xE5UL)  /* TRGM interrupt0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_IRQ1                     (0xE6UL)  /* TRGM interrupt1 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA0                     (0xE7UL)  /* TRGM DMA request0 */
#define HPM_TRGM0_OUTPUT_SRC_TRGM_DMA1                     (0xE8UL)  /* TRGM DMA request1 */
#define HPM_TRGM0_OUTPUT_SRC_MTG0_TRIG_IN0                 (0xE9UL)  /* MTG0 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_MTG0_TRIG_IN1                 (0xEAUL)  /* MTG0 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_MTG0_TRIG_IN2                 (0xEBUL)  /* MTG0 trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_MTG0_TRIG_IN3                 (0xECUL)  /* MTG0 trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_MTG1_TRIG_IN0                 (0xEDUL)  /* MTG1 trigger in0 */
#define HPM_TRGM0_OUTPUT_SRC_MTG1_TRIG_IN1                 (0xEEUL)  /* MTG1 trigger in1 */
#define HPM_TRGM0_OUTPUT_SRC_MTG1_TRIG_IN2                 (0xEFUL)  /* MTG1 trigger in2 */
#define HPM_TRGM0_OUTPUT_SRC_MTG1_TRIG_IN3                 (0xF0UL)  /* MTG1 trigger in3 */
#define HPM_TRGM0_OUTPUT_SRC_ESC_TRIG_IN                   (0xF1UL)  /* ESC latch0 */

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
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN0                 (0x20UL)  /* TRGM input0 (dropped macro  please use macro TRGM0_P00) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P00                     (0x20UL)  /* TRGM input0 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN1                 (0x21UL)  /* TRGM input1 (dropped macro  please use macro TRGM0_P01) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P01                     (0x21UL)  /* TRGM input1 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN2                 (0x22UL)  /* TRGM input2 (dropped macro  please use macro TRGM0_P02) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P02                     (0x22UL)  /* TRGM input2 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN3                 (0x23UL)  /* TRGM input3 (dropped macro  please use macro TRGM0_P03) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P03                     (0x23UL)  /* TRGM input3 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN4                 (0x24UL)  /* TRGM input4 (dropped macro  please use macro TRGM0_P04) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P04                     (0x24UL)  /* TRGM input4 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN5                 (0x25UL)  /* TRGM input5 (dropped macro  please use macro TRGM0_P05) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P05                     (0x25UL)  /* TRGM input5 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN6                 (0x26UL)  /* TRGM input6 (dropped macro  please use macro TRGM0_P06) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P06                     (0x26UL)  /* TRGM input6 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN7                 (0x27UL)  /* TRGM input7 (dropped macro  please use macro TRGM0_P07) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P07                     (0x27UL)  /* TRGM input7 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN8                 (0x28UL)  /* TRGM input8 (dropped macro  please use macro TRGM0_P08) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P08                     (0x28UL)  /* TRGM input8 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN9                 (0x29UL)  /* TRGM input9 (dropped macro  please use macro TRGM0_P09) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P09                     (0x29UL)  /* TRGM input9 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN10                (0x2AUL)  /* TRGM input10 (dropped macro  please use macro TRGM0_P10) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P10                     (0x2AUL)  /* TRGM input10 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN11                (0x2BUL)  /* TRGM input11 (dropped macro  please use macro TRGM0_P11) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P11                     (0x2BUL)  /* TRGM input11 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN12                (0x2CUL)  /* TRGM input12 (dropped macro  please use macro TRGM0_P12) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P12                     (0x2CUL)  /* TRGM input12 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN13                (0x2DUL)  /* TRGM input13 (dropped macro  please use macro TRGM0_P13) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P13                     (0x2DUL)  /* TRGM input13 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN14                (0x2EUL)  /* TRGM input14 (dropped macro  please use macro TRGM0_P14) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P14                     (0x2EUL)  /* TRGM input14 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN15                (0x2FUL)  /* TRGM input15 (dropped macro  please use macro TRGM0_P15) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P15                     (0x2FUL)  /* TRGM input15 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN16                (0x30UL)  /* TRGM input16 (dropped macro  please use macro TRGM0_P16) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P16                     (0x30UL)  /* TRGM input16 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN17                (0x31UL)  /* TRGM input17 (dropped macro  please use macro TRGM0_P17) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P17                     (0x31UL)  /* TRGM input17 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN18                (0x32UL)  /* TRGM input18 (dropped macro  please use macro TRGM0_P18) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P18                     (0x32UL)  /* TRGM input18 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN19                (0x33UL)  /* TRGM input19 (dropped macro  please use macro TRGM0_P19) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P19                     (0x33UL)  /* TRGM input19 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN20                (0x34UL)  /* TRGM input20 (dropped macro  please use macro TRGM0_P20) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P20                     (0x34UL)  /* TRGM input20 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN21                (0x35UL)  /* TRGM input21 (dropped macro  please use macro TRGM0_P21) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P21                     (0x35UL)  /* TRGM input21 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN22                (0x36UL)  /* TRGM input22 (dropped macro  please use macro TRGM0_P22) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P22                     (0x36UL)  /* TRGM input22 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN23                (0x37UL)  /* TRGM input23 (dropped macro  please use macro TRGM0_P23) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P23                     (0x37UL)  /* TRGM input23 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN24                (0x38UL)  /* TRGM input24 (dropped macro  please use macro TRGM0_P24) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P24                     (0x38UL)  /* TRGM input24 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN25                (0x39UL)  /* TRGM input25 (dropped macro  please use macro TRGM0_P25) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P25                     (0x39UL)  /* TRGM input25 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN26                (0x3AUL)  /* TRGM input26 (dropped macro  please use macro TRGM0_P26) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P26                     (0x3AUL)  /* TRGM input26 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN27                (0x3BUL)  /* TRGM input27 (dropped macro  please use macro TRGM0_P27) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P27                     (0x3BUL)  /* TRGM input27 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN28                (0x3CUL)  /* TRGM input28 (dropped macro  please use macro TRGM0_P28) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P28                     (0x3CUL)  /* TRGM input28 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN29                (0x3DUL)  /* TRGM input29 (dropped macro  please use macro TRGM0_P29) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P29                     (0x3DUL)  /* TRGM input29 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN30                (0x3EUL)  /* TRGM input30 (dropped macro  please use macro TRGM0_P30) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P30                     (0x3EUL)  /* TRGM input30 */
#define HPM_TRGM0_FILTER_SRC_MOTO_GPIO_IN31                (0x3FUL)  /* TRGM input31 (dropped macro  please use macro TRGM0_P31) */
#define HPM_TRGM0_FILTER_SRC_TRGM0_P31                     (0x3FUL)  /* TRGM input31 */

/* trgm0_dma mux definitions */
#define HPM_TRGM0_DMA_SRC_PWM0_REQ0                        (0x0UL)   /* PWM0 DMA request0 */
#define HPM_TRGM0_DMA_SRC_PWM0_REQ1                        (0x1UL)   /* PWM0 DMA request1 */
#define HPM_TRGM0_DMA_SRC_PWM0_REQ2                        (0x2UL)   /* PWM0 DMA request2 */
#define HPM_TRGM0_DMA_SRC_PWM0_REQ3                        (0x3UL)   /* PWM0 DMA request3 */
#define HPM_TRGM0_DMA_SRC_PWM1_REQ0                        (0x4UL)   /* PWM1 DMA request0 */
#define HPM_TRGM0_DMA_SRC_PWM1_REQ1                        (0x5UL)   /* PWM1 DMA request1 */
#define HPM_TRGM0_DMA_SRC_PWM1_REQ2                        (0x6UL)   /* PWM1 DMA request2 */
#define HPM_TRGM0_DMA_SRC_PWM1_REQ3                        (0x7UL)   /* PWM1 DMA request3 */
#define HPM_TRGM0_DMA_SRC_PWM2_REQ0                        (0x8UL)   /* PWM2 DMA request0 */
#define HPM_TRGM0_DMA_SRC_PWM2_REQ1                        (0x9UL)   /* PWM2 DMA request1 */
#define HPM_TRGM0_DMA_SRC_PWM2_REQ2                        (0xAUL)   /* PWM2 DMA request2 */
#define HPM_TRGM0_DMA_SRC_PWM2_REQ3                        (0xBUL)   /* PWM2 DMA request3 */
#define HPM_TRGM0_DMA_SRC_PWM3_REQ0                        (0xCUL)   /* PWM3 DMA request0 */
#define HPM_TRGM0_DMA_SRC_PWM3_REQ1                        (0xDUL)   /* PWM3 DMA request1 */
#define HPM_TRGM0_DMA_SRC_PWM3_REQ2                        (0xEUL)   /* PWM3 DMA request2 */
#define HPM_TRGM0_DMA_SRC_PWM3_REQ3                        (0xFUL)   /* PWM3 DMA request3 */
#define HPM_TRGM0_DMA_SRC_QEI0_REQ                         (0x10UL)  /* QEI0 DMA request */
#define HPM_TRGM0_DMA_SRC_QEI1_REQ                         (0x11UL)  /* QEI1 DMA request */
#define HPM_TRGM0_DMA_SRC_QEI2_REQ                         (0x12UL)  /* QEI2 DMA request */
#define HPM_TRGM0_DMA_SRC_QEI3_REQ                         (0x13UL)  /* QEI3 DMA request */
#define HPM_TRGM0_DMA_SRC_SEI_REQ0                         (0x14UL)  /* SEI DMA request0 */
#define HPM_TRGM0_DMA_SRC_SEI_REQ1                         (0x15UL)  /* SEI DMA request1 */
#define HPM_TRGM0_DMA_SRC_SEI_REQ2                         (0x16UL)  /* SEI DMA request2 */
#define HPM_TRGM0_DMA_SRC_SEI_REQ3                         (0x17UL)  /* SEI DMA request3 */
#define HPM_TRGM0_DMA_SRC_TRGM0                            (0x18UL)  /* TRGM DMA request0 from TRGM output 231 */
#define HPM_TRGM0_DMA_SRC_TRGM1                            (0x19UL)  /* TRGM DMA request1 from TRGM output 232 */



#endif /* HPM_TRGMMUX_SRC_H */
