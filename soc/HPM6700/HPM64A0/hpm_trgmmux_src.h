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
#define HPM_TRGM0_INPUT_SRC_TRGM0_P0                       (0x2UL)   /* TRGM0 input data0(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P1                       (0x3UL)   /* TRGM0 input data1(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P2                       (0x4UL)   /* TRGM0 input data2(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P3                       (0x5UL)   /* TRGM0 input data3(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P4                       (0x6UL)   /* TRGM0 input data4(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P5                       (0x7UL)   /* TRGM0 input data5(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P6                       (0x8UL)   /* TRGM0 input data6(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P7                       (0x9UL)   /* TRGM0 input data7(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P8                       (0xAUL)   /* TRGM0 input data8(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P9                       (0xBUL)   /* TRGM0 input data9(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P10                      (0xCUL)   /* TRGM0 input data10(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM0_P11                      (0xDUL)   /* TRGM0 input data11(from IO) */
#define HPM_TRGM0_INPUT_SRC_TRGM3_OUTX0                    (0xEUL)   /* TRGM3 output0(from other TRGM to TRGM0) */
#define HPM_TRGM0_INPUT_SRC_TRGM3_OUTX1                    (0xFUL)   /* TRGM3 output1(from other TRGM to TRGM0) */
#define HPM_TRGM0_INPUT_SRC_TRGM2_OUTX0                    (0x10UL)  /* TRGM2 output0(from other TRGM to TRGM0) */
#define HPM_TRGM0_INPUT_SRC_TRGM2_OUTX1                    (0x11UL)  /* TRGM2 output1(from other TRGM to TRGM0) */
#define HPM_TRGM0_INPUT_SRC_TRGM1_OUTX0                    (0x12UL)  /* TRGM1 output0(from other TRGM to TRGM0) */
#define HPM_TRGM0_INPUT_SRC_TRGM1_OUTX1                    (0x13UL)  /* TRGM1 output1(from other TRGM to TRGM0) */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH8REF                    (0x14UL)  /* PWM0 channel8 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH9REF                    (0x15UL)  /* PWM0 channel9 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH10REF                   (0x16UL)  /* PWM0 channel10 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH11REF                   (0x17UL)  /* PWM0 channel11 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH12REF                   (0x18UL)  /* PWM0 channel12 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH13REF                   (0x19UL)  /* PWM0 channel13 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH14REF                   (0x1AUL)  /* PWM0 channel14 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH15REF                   (0x1BUL)  /* PWM0 channel15 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH16REF                   (0x1CUL)  /* PWM0 channel16 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH17REF                   (0x1DUL)  /* PWM0 channel17 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH18REF                   (0x1EUL)  /* PWM0 channel18 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH19REF                   (0x1FUL)  /* PWM0 channel19 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH20REF                   (0x20UL)  /* PWM0 channel20 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH21REF                   (0x21UL)  /* PWM0 channel21 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH22REF                   (0x22UL)  /* PWM0 channel22 trigger out */
#define HPM_TRGM0_INPUT_SRC_PWM0_CH23REF                   (0x23UL)  /* PWM0 channel23 trigger out */
#define HPM_TRGM0_INPUT_SRC_QEI0_TRGO                      (0x24UL)  /* QEI0 trigger out */
#define HPM_TRGM0_INPUT_SRC_HALL0_TRGO                     (0x25UL)  /* HALL0 trigger out */
#define HPM_TRGM0_INPUT_SRC_USB0_SOF                       (0x26UL)  /* USB0 start of frame marker */
#define HPM_TRGM0_INPUT_SRC_USB1_SOF                       (0x27UL)  /* USB1 start of frame marker */
#define HPM_TRGM0_INPUT_SRC_ENET0_PTP_OUT3                 (0x28UL)  /* ENET0 PTP output bit3 */
#define HPM_TRGM0_INPUT_SRC_ENET1_PTP_OUT3                 (0x29UL)  /* ENET1 PTP output bit3 */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP0                      (0x2AUL)  /* PTPC compare output0 */
#define HPM_TRGM0_INPUT_SRC_PTPC_CMP1                      (0x2BUL)  /* PTPC compare output1 */
#define HPM_TRGM0_INPUT_SRC_SYNT0_CH0                      (0x2CUL)  /* SYNT channel0 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT0_CH1                      (0x2DUL)  /* SYNT channel1 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT0_CH2                      (0x2EUL)  /* SYNT channel2 pulse output */
#define HPM_TRGM0_INPUT_SRC_SYNT0_CH3                      (0x2FUL)  /* SYNT channel3 pulse output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT2                    (0x30UL)  /* GPTMR0 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR0_OUT3                    (0x31UL)  /* GPTMR0 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT2                    (0x32UL)  /* GPTMR1 channel2 compare output */
#define HPM_TRGM0_INPUT_SRC_GPTMR1_OUT3                    (0x33UL)  /* GPTMR1 channel3 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP0_OUT                      (0x34UL)  /* CMP0 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP1_OUT                      (0x35UL)  /* CMP1 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP2_OUT                      (0x36UL)  /* CMP2 compare output */
#define HPM_TRGM0_INPUT_SRC_ACMP3_OUT                      (0x37UL)  /* CMP3 compare output */
#define HPM_TRGM0_INPUT_SRC_DEBUG_FLAG                     (0x38UL)  /* debug mode flag */

/* trgm1_input mux definitions */
#define HPM_TRGM1_INPUT_SRC_VSS                            (0x0UL)   /* low level voltage */
#define HPM_TRGM1_INPUT_SRC_VDD                            (0x1UL)   /* high level voltage */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P0                       (0x2UL)   /* TRGM1 input data0(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P1                       (0x3UL)   /* TRGM1 input data1(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P2                       (0x4UL)   /* TRGM1 input data2(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P3                       (0x5UL)   /* TRGM1 input data3(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P4                       (0x6UL)   /* TRGM1 input data4(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P5                       (0x7UL)   /* TRGM1 input data5(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P6                       (0x8UL)   /* TRGM1 input data6(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P7                       (0x9UL)   /* TRGM1 input data7(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P8                       (0xAUL)   /* TRGM1 input data8(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P9                       (0xBUL)   /* TRGM1 input data9(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P10                      (0xCUL)   /* TRGM1 input data10(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM1_P11                      (0xDUL)   /* TRGM1 input data11(from IO) */
#define HPM_TRGM1_INPUT_SRC_TRGM3_OUTX0                    (0xEUL)   /* TRGM3 output0(from other TRGM to TRGM1) */
#define HPM_TRGM1_INPUT_SRC_TRGM3_OUTX1                    (0xFUL)   /* TRGM3 output1(from other TRGM to TRGM1) */
#define HPM_TRGM1_INPUT_SRC_TRGM2_OUTX0                    (0x10UL)  /* TRGM2 output0(from other TRGM to TRGM1) */
#define HPM_TRGM1_INPUT_SRC_TRGM2_OUTX1                    (0x11UL)  /* TRGM2 output1(from other TRGM to TRGM1) */
#define HPM_TRGM1_INPUT_SRC_TRGM0_OUTX0                    (0x12UL)  /* TRGM0 output0(from other TRGM to TRGM1) */
#define HPM_TRGM1_INPUT_SRC_TRGM0_OUTX1                    (0x13UL)  /* TRGM0 output1(from other TRGM to TRGM1) */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH8REF                    (0x14UL)  /* PWM1 channel8 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH9REF                    (0x15UL)  /* PWM1 channel9 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH10REF                   (0x16UL)  /* PWM1 channel10 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH11REF                   (0x17UL)  /* PWM1 channel11 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH12REF                   (0x18UL)  /* PWM1 channel12 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH13REF                   (0x19UL)  /* PWM1 channel13 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH14REF                   (0x1AUL)  /* PWM1 channel14 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH15REF                   (0x1BUL)  /* PWM1 channel15 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH16REF                   (0x1CUL)  /* PWM1 channel16 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH17REF                   (0x1DUL)  /* PWM1 channel17 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH18REF                   (0x1EUL)  /* PWM1 channel18 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH19REF                   (0x1FUL)  /* PWM1 channel19 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH20REF                   (0x20UL)  /* PWM1 channel20 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH21REF                   (0x21UL)  /* PWM1 channel21 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH22REF                   (0x22UL)  /* PWM1 channel22 trigger out */
#define HPM_TRGM1_INPUT_SRC_PWM1_CH23REF                   (0x23UL)  /* PWM1 channel23 trigger out */
#define HPM_TRGM1_INPUT_SRC_QEI1_TRGO                      (0x24UL)  /* QEI1 trigger out */
#define HPM_TRGM1_INPUT_SRC_HALL1_TRGO                     (0x25UL)  /* HALL1 trigger out */
#define HPM_TRGM1_INPUT_SRC_USB0_SOF                       (0x26UL)  /* USB0 start of frame marker */
#define HPM_TRGM1_INPUT_SRC_USB1_SOF                       (0x27UL)  /* USB1 start of frame marker */
#define HPM_TRGM1_INPUT_SRC_ENET0_PTP_OUT3                 (0x28UL)  /* ENET0 PTP output bit3 */
#define HPM_TRGM1_INPUT_SRC_ENET1_PTP_OUT3                 (0x29UL)  /* ENET1 PTP output bit3 */
#define HPM_TRGM1_INPUT_SRC_PTPC_CMP0                      (0x2AUL)  /* PTPC compare output0 */
#define HPM_TRGM1_INPUT_SRC_PTPC_CMP1                      (0x2BUL)  /* PTPC compare output1 */
#define HPM_TRGM1_INPUT_SRC_SYNT_CH0                       (0x2CUL)  /* SYNT channel0 pulse output */
#define HPM_TRGM1_INPUT_SRC_SYNT_CH1                       (0x2DUL)  /* SYNT channel1 pulse output */
#define HPM_TRGM1_INPUT_SRC_SYNT_CH2                       (0x2EUL)  /* SYNT channel2 pulse output */
#define HPM_TRGM1_INPUT_SRC_SYNT_CH3                       (0x2FUL)  /* SYNT channel3 pulse output */
#define HPM_TRGM1_INPUT_SRC_GPTMR2_OUT2                    (0x30UL)  /* GPTMR2 channel2 compare output */
#define HPM_TRGM1_INPUT_SRC_GPTMR2_OUT3                    (0x31UL)  /* GPTMR2 channel3 compare output */
#define HPM_TRGM1_INPUT_SRC_GPTMR3_OUT2                    (0x32UL)  /* GPTMR3 channel2 compare output */
#define HPM_TRGM1_INPUT_SRC_GPTMR3_OUT3                    (0x33UL)  /* GPTMR3 channel3 compare output */
#define HPM_TRGM1_INPUT_SRC_ACMP0_OUT                      (0x34UL)  /* CMP0 compare output */
#define HPM_TRGM1_INPUT_SRC_ACMP1_OUT                      (0x35UL)  /* CMP1 compare output */
#define HPM_TRGM1_INPUT_SRC_ACMP2_OUT                      (0x36UL)  /* CMP2 compare output */
#define HPM_TRGM1_INPUT_SRC_ACMP3_OUT                      (0x37UL)  /* CMP3 compare output */
#define HPM_TRGM1_INPUT_SRC_DEBUG_FLAG                     (0x38UL)  /* debug mode flag */

/* trgm2_input mux definitions */
#define HPM_TRGM2_INPUT_SRC_VSS                            (0x0UL)   /* low level voltage */
#define HPM_TRGM2_INPUT_SRC_VDD                            (0x1UL)   /* high level voltage */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P0                       (0x2UL)   /* TRGM2 input data0(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P1                       (0x3UL)   /* TRGM2 input data1(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P2                       (0x4UL)   /* TRGM2 input data2(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P3                       (0x5UL)   /* TRGM2 input data3(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P4                       (0x6UL)   /* TRGM2 input data4(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P5                       (0x7UL)   /* TRGM2 input data5(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P6                       (0x8UL)   /* TRGM2 input data6(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P7                       (0x9UL)   /* TRGM2 input data7(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P8                       (0xAUL)   /* TRGM0 input data8(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P9                       (0xBUL)   /* TRGM0 input data9(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P10                      (0xCUL)   /* TRGM0 input data10(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM2_P11                      (0xDUL)   /* TRGM0 input data11(from IO) */
#define HPM_TRGM2_INPUT_SRC_TRGM3_OUTX0                    (0xEUL)   /* TRGM3 output0(from other TRGM to TRGM2) */
#define HPM_TRGM2_INPUT_SRC_TRGM3_OUTX1                    (0xFUL)   /* TRGM3 output1(from other TRGM to TRGM2) */
#define HPM_TRGM2_INPUT_SRC_TRGM1_OUTX0                    (0x10UL)  /* TRGM1 output0(from other TRGM to TRGM2) */
#define HPM_TRGM2_INPUT_SRC_TRGM1_OUTX1                    (0x11UL)  /* TRGM1 output1(from other TRGM to TRGM2) */
#define HPM_TRGM2_INPUT_SRC_TRGM0_OUTX0                    (0x12UL)  /* TRGM0 output0(from other TRGM to TRGM2) */
#define HPM_TRGM2_INPUT_SRC_TRGM0_OUTX1                    (0x13UL)  /* TRGM0 output1(from other TRGM to TRGM2) */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH8REF                    (0x14UL)  /* PWM2 channel8 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH9REF                    (0x15UL)  /* PWM2 channel9 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH10REF                   (0x16UL)  /* PWM2 channel10 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH11REF                   (0x17UL)  /* PWM2 channel11 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH12REF                   (0x18UL)  /* PWM2 channel12 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH13REF                   (0x19UL)  /* PWM2 channel13 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH14REF                   (0x1AUL)  /* PWM2 channel14 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH15REF                   (0x1BUL)  /* PWM2 channel15 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH16REF                   (0x1CUL)  /* PWM2 channel16 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH17REF                   (0x1DUL)  /* PWM2 channel17 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH18REF                   (0x1EUL)  /* PWM2 channel18 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH19REF                   (0x1FUL)  /* PWM2 channel19 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH20REF                   (0x20UL)  /* PWM2 channel20 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH21REF                   (0x21UL)  /* PWM2 channel21 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH22REF                   (0x22UL)  /* PWM2 channel22 trigger out */
#define HPM_TRGM2_INPUT_SRC_PWM2_CH23REF                   (0x23UL)  /* PWM2 channel23 trigger out */
#define HPM_TRGM2_INPUT_SRC_QEI2_TRGO                      (0x24UL)  /* QEI2 trigger out */
#define HPM_TRGM2_INPUT_SRC_HALL2_TRGO                     (0x25UL)  /* HALL2 trigger out */
#define HPM_TRGM2_INPUT_SRC_USB0_SOF                       (0x26UL)  /* USB0 start of frame marker */
#define HPM_TRGM2_INPUT_SRC_USB1_SOF                       (0x27UL)  /* USB1 start of frame marker */
#define HPM_TRGM2_INPUT_SRC_ENET0_PTP_OUT3                 (0x28UL)  /* ENET0 PTP output bit3 */
#define HPM_TRGM2_INPUT_SRC_ENET1_PTP_OUT3                 (0x29UL)  /* ENET1 PTP output bit3 */
#define HPM_TRGM2_INPUT_SRC_PTPC_CMP0                      (0x2AUL)  /* PTPC compare output0 */
#define HPM_TRGM2_INPUT_SRC_PTPC_CMP1                      (0x2BUL)  /* PTPC compare output1 */
#define HPM_TRGM2_INPUT_SRC_SYNT_CH0                       (0x2CUL)  /* SYNT channel0 pulse output */
#define HPM_TRGM2_INPUT_SRC_SYNT_CH1                       (0x2DUL)  /* SYNT channel1 pulse output */
#define HPM_TRGM2_INPUT_SRC_SYNT_CH2                       (0x2EUL)  /* SYNT channel2 pulse output */
#define HPM_TRGM2_INPUT_SRC_SYNT_CH3                       (0x2FUL)  /* SYNT channel3 pulse output */
#define HPM_TRGM2_INPUT_SRC_GPTMR4_OUT2                    (0x30UL)  /* GPTMR4 channel2 compare output */
#define HPM_TRGM2_INPUT_SRC_GPTMR4_OUT3                    (0x31UL)  /* GPTMR4 channel3 compare output */
#define HPM_TRGM2_INPUT_SRC_GPTMR5_OUT2                    (0x32UL)  /* GPTMR5 channel2 compare output */
#define HPM_TRGM2_INPUT_SRC_GPTMR5_OUT3                    (0x33UL)  /* GPTMR5 channel3 compare output */
#define HPM_TRGM2_INPUT_SRC_ACMP0_OUT                      (0x34UL)  /* CMP0 compare output */
#define HPM_TRGM2_INPUT_SRC_ACMP1_OUT                      (0x35UL)  /* CMP1 compare output */
#define HPM_TRGM2_INPUT_SRC_ACMP2_OUT                      (0x36UL)  /* CMP2 compare output */
#define HPM_TRGM2_INPUT_SRC_ACMP3_OUT                      (0x37UL)  /* CMP3 compare output */
#define HPM_TRGM2_INPUT_SRC_DEBUG_FLAG                     (0x38UL)  /* debug mode flag */

/* trgm3_input mux definitions */
#define HPM_TRGM3_INPUT_SRC_VSS                            (0x0UL)   /* low level voltage */
#define HPM_TRGM3_INPUT_SRC_VDD                            (0x1UL)   /* high level voltage */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P0                       (0x2UL)   /* TRGM3 input data0(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P1                       (0x3UL)   /* TRGM3 input data1(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P2                       (0x4UL)   /* TRGM3 input data2(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P3                       (0x5UL)   /* TRGM3 input data3(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P4                       (0x6UL)   /* TRGM3 input data4(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P5                       (0x7UL)   /* TRGM3 input data5(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P6                       (0x8UL)   /* TRGM3 input data6(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P7                       (0x9UL)   /* TRGM3 input data7(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P8                       (0xAUL)   /* TRGM3 input data8(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P9                       (0xBUL)   /* TRGM3 input data9(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P10                      (0xCUL)   /* TGM3 input data10(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM3_P11                      (0xDUL)   /* TGM3 input data11(from IO) */
#define HPM_TRGM3_INPUT_SRC_TRGM2_OUTX0                    (0xEUL)   /* TRGM2 output0(from other TRGM to TRGM3) */
#define HPM_TRGM3_INPUT_SRC_TRGM2_OUTX1                    (0xFUL)   /* TRGM2 output1(from other TRGM to TRGM3) */
#define HPM_TRGM3_INPUT_SRC_TRGM1_OUTX0                    (0x10UL)  /* TRGM1 output0(from other TRGM to TRGM3) */
#define HPM_TRGM3_INPUT_SRC_TRGM1_OUTX1                    (0x11UL)  /* TRGM1 output1(from other TRGM to TRGM3) */
#define HPM_TRGM3_INPUT_SRC_TRGM0_OUTX0                    (0x12UL)  /* TRGM0 output0(from other TRGM to TRGM3) */
#define HPM_TRGM3_INPUT_SRC_TRGM0_OUTX1                    (0x13UL)  /* TRGM0 output1(from other TRGM to TRGM3) */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH8REF                    (0x14UL)  /* PWM3 channel8 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH9REF                    (0x15UL)  /* PWM3 channel9 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH10REF                   (0x16UL)  /* PWM3 channel10 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH11REF                   (0x17UL)  /* PWM3 channel11 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH12REF                   (0x18UL)  /* PWM3 channel12 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH13REF                   (0x19UL)  /* PWM3 channel13 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH14REF                   (0x1AUL)  /* PWM3 channel14 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH15REF                   (0x1BUL)  /* PWM3 channel15 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH16REF                   (0x1CUL)  /* PWM3 channel16 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH17REF                   (0x1DUL)  /* PWM3 channel17 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH18REF                   (0x1EUL)  /* PWM3 channel18 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH19REF                   (0x1FUL)  /* PWM3 channel19 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH20REF                   (0x20UL)  /* PWM3 channel20 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH21REF                   (0x21UL)  /* PWM3 channel21 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH22REF                   (0x22UL)  /* PWM3 channel22 trigger out */
#define HPM_TRGM3_INPUT_SRC_PWM3_CH23REF                   (0x23UL)  /* PWM3 channel23 trigger out */
#define HPM_TRGM3_INPUT_SRC_QEI3_TRGO                      (0x24UL)  /* QEI3 trigger out */
#define HPM_TRGM3_INPUT_SRC_HALL3_TRGO                     (0x25UL)  /* HALL3 trigger out */
#define HPM_TRGM3_INPUT_SRC_USB0_SOF                       (0x26UL)  /* USB0 start of frame */
#define HPM_TRGM3_INPUT_SRC_USB1_SOF                       (0x27UL)  /* USB1 start of frame */
#define HPM_TRGM3_INPUT_SRC_ENET0_PTP_OUT3                 (0x28UL)  /* ENET0 PTP output bit3 */
#define HPM_TRGM3_INPUT_SRC_ENET1_PTP_OUT3                 (0x29UL)  /* ENET1 PTP output bit3 */
#define HPM_TRGM3_INPUT_SRC_PTPC_CMP0                      (0x2AUL)  /* PTPC compare output0 */
#define HPM_TRGM3_INPUT_SRC_PTPC_CMP1                      (0x2BUL)  /* PTPC compare output1 */
#define HPM_TRGM3_INPUT_SRC_SYNT_CH0                       (0x2CUL)  /* SYNT channel0 pulse output */
#define HPM_TRGM3_INPUT_SRC_SYNT_CH1                       (0x2DUL)  /* SYNT channel1 pulse output */
#define HPM_TRGM3_INPUT_SRC_SYNT_CH2                       (0x2EUL)  /* SYNT channel2 pulse output */
#define HPM_TRGM3_INPUT_SRC_SYNT_CH3                       (0x2FUL)  /* SYNT channel3 pulse output */
#define HPM_TRGM3_INPUT_SRC_GPTMR6_OUT2                    (0x30UL)  /* GPTMR6 channel2 compare output */
#define HPM_TRGM3_INPUT_SRC_GPTMR6_OUT3                    (0x31UL)  /* GPTMR6 channel3 compare output */
#define HPM_TRGM3_INPUT_SRC_GPTMR7_OUT2                    (0x32UL)  /* GPTMR7 channel2 compare output */
#define HPM_TRGM3_INPUT_SRC_GPTMR7_OUT3                    (0x33UL)  /* GPTMR7 channel3 compare output */
#define HPM_TRGM3_INPUT_SRC_ACMP0_OUT                      (0x34UL)  /* CMP0 compare output */
#define HPM_TRGM3_INPUT_SRC_ACMP1_OUT                      (0x35UL)  /* CMP1 compare output */
#define HPM_TRGM3_INPUT_SRC_ACMP2_OUT                      (0x36UL)  /* CMP2 compare output */
#define HPM_TRGM3_INPUT_SRC_ACMP3_OUT                      (0x37UL)  /* CMP3 compare output */
#define HPM_TRGM3_INPUT_SRC_DEBUG_FLAG                     (0x38UL)  /* debug mode flag */

/* trgm0_output mux definitions */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P0                      (0x0UL)   /* TRGM0 output data0(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P1                      (0x1UL)   /* TRGM0 output data1(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P2                      (0x2UL)   /* TRGM0 output data2(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P3                      (0x3UL)   /* TRGM0 output data3(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P4                      (0x4UL)   /* TRGM0 output data4(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P5                      (0x5UL)   /* TRGM0 output data5(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P6                      (0x6UL)   /* TRGM0 output data6(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P7                      (0x7UL)   /* TRGM0 output data7(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P8                      (0x8UL)   /* TRGM0 output data8(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P9                      (0x9UL)   /* TRGM0 output data9(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P10                     (0xAUL)   /* TRGM0 output data10(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_P11                     (0xBUL)   /* TRGM0 output data11(to IO) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_OUTX0                   (0xCUL)   /* TRGM0 output data0(to other TRGM) */
#define HPM_TRGM0_OUTPUT_SRC_TRGM0_OUTX1                   (0xDUL)   /* TRGM0 output data1(to other TRGM) */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_SYNCI                    (0xEUL)   /* PWM0 counter sync input */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FRCI                     (0xFUL)   /* PWM0 force output control */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FRCSYNCI                 (0x10UL)  /* PWM0 force output sync control */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_SHRLDSYNCI               (0x11UL)  /* PWM0 shadow register valid signal */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI0                  (0x12UL)  /* PWM0 fault0 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI1                  (0x13UL)  /* PWM0 fault1 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI2                  (0x14UL)  /* PWM0 fault2 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_FAULTI3                  (0x15UL)  /* PWM0 fault3 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN8                      (0x16UL)  /* PWM0 capture in8 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN9                      (0x17UL)  /* PWM0 capture in9 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN10                     (0x18UL)  /* PWM0 capture in10 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN11                     (0x19UL)  /* PWM0 capture in11 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN12                     (0x1AUL)  /* PWM0 capture in12 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN13                     (0x1BUL)  /* PWM0 capture in13 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN14                     (0x1CUL)  /* PWM0 capture in14 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN15                     (0x1DUL)  /* PWM0 capture in15 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN16                     (0x1EUL)  /* PWM0 capture in16 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN17                     (0x1FUL)  /* PWM0 capture in17 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN18                     (0x20UL)  /* PWM0 capture in18 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN19                     (0x21UL)  /* PWM0 capture in19 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN20                     (0x22UL)  /* PWM0 capture in20 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN21                     (0x23UL)  /* PWM0 capture in21 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN22                     (0x24UL)  /* PWM0 capture in22 */
#define HPM_TRGM0_OUTPUT_SRC_PWM0_IN23                     (0x25UL)  /* PWM0 capture in23 */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_A                        (0x26UL)  /* QEI0 phase A input */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_B                        (0x27UL)  /* QEI0 phase B input */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_Z                        (0x28UL)  /* QEI0 phase Z input */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_H                        (0x29UL)  /* QEI0 phase H input */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_PAUSE                    (0x2AUL)  /* QEI0 counter pause */
#define HPM_TRGM0_OUTPUT_SRC_QEI0_SNAPI                    (0x2BUL)  /* QEI0 snap pulse input */
#define HPM_TRGM0_OUTPUT_SRC_HALL0_U                       (0x2CUL)  /* HALL0 phase U input */
#define HPM_TRGM0_OUTPUT_SRC_HALL0_V                       (0x2DUL)  /* HALL0 phase V input */
#define HPM_TRGM0_OUTPUT_SRC_HALL0_W                       (0x2EUL)  /* HALL0 phase W input */
#define HPM_TRGM0_OUTPUT_SRC_HALL0_SNAPI                   (0x2FUL)  /* HALL0 snap pulse input */
#define HPM_TRGM0_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* ADC0 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* ADC1 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* ADC2 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADC3_STRGI                    (0x33UL)  /* ADC3 sequence queue trigger */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0A                  (0x34UL)  /* "ADC0, 1, 2, 3" preemption trigger0A */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0B                  (0x35UL)  /* "ADC0, 1, 2, 3" preemption trigger0B */
#define HPM_TRGM0_OUTPUT_SRC_ADCX_PTRGI0C                  (0x36UL)  /* "ADC0, 1, 2, 3" preemption trigger0C */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_SYNCI                  (0x37UL)  /* GPTMR0 counter sync input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN2                    (0x38UL)  /* GPTMR0 channel2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR0_IN3                    (0x39UL)  /* GPTMR0 channel3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_SYNCI                  (0x3AUL)  /* GPTMR1 counter sync input */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN2                    (0x3BUL)  /* GPTMR1 channel2 capture in */
#define HPM_TRGM0_OUTPUT_SRC_GPTMR1_IN3                    (0x3CUL)  /* GPTMR1 channel3 capture in */
#define HPM_TRGM0_OUTPUT_SRC_ACMP0_WIN                     (0x3DUL)  /* CMP0 window */
#define HPM_TRGM0_OUTPUT_SRC_PTPC_CAP0                     (0x3EUL)  /* PTPC capture in0 */
#define HPM_TRGM0_OUTPUT_SRC_PTPC_CAP1                     (0x3FUL)  /* PTPC capture in1 */

/* trgm1_output mux definitions */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P0                      (0x0UL)   /* TRGM1 output data0(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P1                      (0x1UL)   /* TRGM1 output data1(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P2                      (0x2UL)   /* TRGM1 output data2(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P3                      (0x3UL)   /* TRGM1 output data3(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P4                      (0x4UL)   /* TRGM1 output data4(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P5                      (0x5UL)   /* TRGM1 output data5(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P6                      (0x6UL)   /* TRGM1 output data6(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P7                      (0x7UL)   /* TRGM1 output data7(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P8                      (0x8UL)   /* TRGM1 output data8(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P9                      (0x9UL)   /* TRGM1 output data9(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P10                     (0xAUL)   /* TRGM1 output data10(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_P11                     (0xBUL)   /* TRGM1 output data11(to IO) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_OUTX0                   (0xCUL)   /* TRGM1 output data0(to other TRGM) */
#define HPM_TRGM1_OUTPUT_SRC_TRGM1_OUTX1                   (0xDUL)   /* TRGM1 output data1(to other TRGM) */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_SYNCI                    (0xEUL)   /* PWM1 counter sync input */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FRCI                     (0xFUL)   /* PWM1 force output control */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FRCSYNCI                 (0x10UL)  /* PWM1 force output sync control */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_SHRLDSYNCI               (0x11UL)  /* PWM1 shadow register valid signal */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FAULTI0                  (0x12UL)  /* PWM1 fault0 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FAULTI1                  (0x13UL)  /* PWM1 fault1 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FAULTI2                  (0x14UL)  /* PWM1 fault2 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_FAULTI3                  (0x15UL)  /* PWM1 fault3 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN8                      (0x16UL)  /* PWM1 capture in8 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN9                      (0x17UL)  /* PWM1 capture in9 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN10                     (0x18UL)  /* PWM1 capture in10 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN11                     (0x19UL)  /* PWM1 capture in11 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN12                     (0x1AUL)  /* PWM1 capture in12 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN13                     (0x1BUL)  /* PWM1 capture in13 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN14                     (0x1CUL)  /* PWM1 capture in14 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN15                     (0x1DUL)  /* PWM1 capture in15 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN16                     (0x1EUL)  /* PWM1 capture in16 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN17                     (0x1FUL)  /* PWM1 capture in17 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN18                     (0x20UL)  /* PWM1 capture in18 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN19                     (0x21UL)  /* PWM1 capture in19 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN20                     (0x22UL)  /* PWM1 capture in20 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN21                     (0x23UL)  /* PWM1 capture in21 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN22                     (0x24UL)  /* PWM1 capture in22 */
#define HPM_TRGM1_OUTPUT_SRC_PWM1_IN23                     (0x25UL)  /* PWM1 capture in23 */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_A                        (0x26UL)  /* QEI1 phase A input */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_B                        (0x27UL)  /* QEI1 phase B input */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_Z                        (0x28UL)  /* QEI1 phase Z input */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_H                        (0x29UL)  /* QEI1 phase H input */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_PAUSE                    (0x2AUL)  /* QEI1 counter pause */
#define HPM_TRGM1_OUTPUT_SRC_QEI1_SNAPI                    (0x2BUL)  /* QEI1 snap pulse input */
#define HPM_TRGM1_OUTPUT_SRC_HALL1_U                       (0x2CUL)  /* HALL1 phase U input */
#define HPM_TRGM1_OUTPUT_SRC_HALL1_V                       (0x2DUL)  /* HALL1 phase V input */
#define HPM_TRGM1_OUTPUT_SRC_HALL1_W                       (0x2EUL)  /* HALL1 phase W input */
#define HPM_TRGM1_OUTPUT_SRC_HALL1_SNAPI                   (0x2FUL)  /* HALL1 snap pulse input */
#define HPM_TRGM1_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* ADC0 sequence queue trigger */
#define HPM_TRGM1_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* ADC1 sequence queue trigger */
#define HPM_TRGM1_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* ADC2 sequence queue trigger */
#define HPM_TRGM1_OUTPUT_SRC_ADC3_STRGI                    (0x33UL)  /* ADC3 sequence queue trigger */
#define HPM_TRGM1_OUTPUT_SRC_ADCX_PTRGI1A                  (0x34UL)  /* "ADC0, 1, 2, 3" preemption trigger1A */
#define HPM_TRGM1_OUTPUT_SRC_ADCX_PTRGI1B                  (0x35UL)  /* "ADC0, 1, 2, 3" preemption trigger1B */
#define HPM_TRGM1_OUTPUT_SRC_ADCX_PTRGI1C                  (0x36UL)  /* "ADC0, 1, 2, 3" preemption trigger1C */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR2_SYNCI                  (0x37UL)  /* GPTMR2 counter sync input */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR2_IN2                    (0x38UL)  /* GPTMR2 channel2 capture in */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR2_IN3                    (0x39UL)  /* GPTMR2 channel3 capture in */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR3_SYNCI                  (0x3AUL)  /* GPTMR3 counter sync input */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR3_IN2                    (0x3BUL)  /* GPTMR3 channel2 capture in */
#define HPM_TRGM1_OUTPUT_SRC_GPTMR3_IN3                    (0x3CUL)  /* GPTMR3 channel3 capture in */
#define HPM_TRGM1_OUTPUT_SRC_ACMP1_WIN                     (0x3DUL)  /* CMP1 window */
#define HPM_TRGM1_OUTPUT_SRC_PTPC_CAP0                     (0x3EUL)  /* PTPC capture in0 */
#define HPM_TRGM1_OUTPUT_SRC_PTPC_CAP1                     (0x3FUL)  /* PTPC capture in1 */

/* trgm2_output mux definitions */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P0                      (0x0UL)   /* TRGM2 output data0(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P1                      (0x1UL)   /* TRGM2 output data1(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P2                      (0x2UL)   /* TRGM2 output data2(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P3                      (0x3UL)   /* TRGM2 output data3(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P4                      (0x4UL)   /* TRGM2 output data4(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P5                      (0x5UL)   /* TRGM2 output data5(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P6                      (0x6UL)   /* TRGM2 output data6(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P7                      (0x7UL)   /* TRGM2 output data7(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P8                      (0x8UL)   /* TRGM2 output data8(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P9                      (0x9UL)   /* TRGM2 output data9(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P10                     (0xAUL)   /* TRGM2 output data10(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_P11                     (0xBUL)   /* TRGM2 output data11(to IO) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_OUTX0                   (0xCUL)   /* TRGM2 output data0(to other TRGM) */
#define HPM_TRGM2_OUTPUT_SRC_TRGM2_OUTX1                   (0xDUL)   /* TRGM2 output data1(to other TRGM) */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_SYNCI                    (0xEUL)   /* PWM2 counter sync input */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FRCI                     (0xFUL)   /* PWM2 force output control */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FRCSYNCI                 (0x10UL)  /* PWM2 force output sync control */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_SHRLDSYNCI               (0x11UL)  /* PWM2 shadow register valid signal */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FAULTI0                  (0x12UL)  /* PWM2 fault0 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FAULTI1                  (0x13UL)  /* PWM2 fault1 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FAULTI2                  (0x14UL)  /* PWM2 fault2 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_FAULTI3                  (0x15UL)  /* PWM2 fault3 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN8                      (0x16UL)  /* PWM2 capture in8 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN9                      (0x17UL)  /* PWM2 capture in9 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN10                     (0x18UL)  /* PWM2 capture in10 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN11                     (0x19UL)  /* PWM2 capture in11 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN12                     (0x1AUL)  /* PWM2 capture in12 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN13                     (0x1BUL)  /* PWM2 capture in13 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN14                     (0x1CUL)  /* PWM2 capture in14 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN15                     (0x1DUL)  /* PWM2 capture in15 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN16                     (0x1EUL)  /* PWM2 capture in16 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN17                     (0x1FUL)  /* PWM2 capture in17 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN18                     (0x20UL)  /* PWM2 capture in18 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN19                     (0x21UL)  /* PWM2 capture in19 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN20                     (0x22UL)  /* PWM2 capture in20 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN21                     (0x23UL)  /* PWM2 capture in21 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN22                     (0x24UL)  /* PWM2 capture in22 */
#define HPM_TRGM2_OUTPUT_SRC_PWM2_IN23                     (0x25UL)  /* PWM2 capture in23 */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_A                        (0x26UL)  /* QEI2 phase A input */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_B                        (0x27UL)  /* QEI2 phase B input */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_Z                        (0x28UL)  /* QEI2 phase Z input */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_H                        (0x29UL)  /* QEI2 phase H input */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_PAUSE                    (0x2AUL)  /* QEI2 counter pause */
#define HPM_TRGM2_OUTPUT_SRC_QEI2_SNAPI                    (0x2BUL)  /* QEI2 snap pulse input */
#define HPM_TRGM2_OUTPUT_SRC_HALL2_U                       (0x2CUL)  /* HALL2 phase U input */
#define HPM_TRGM2_OUTPUT_SRC_HALL2_V                       (0x2DUL)  /* HALL2 phase V input */
#define HPM_TRGM2_OUTPUT_SRC_HALL2_W                       (0x2EUL)  /* HALL2 phase W input */
#define HPM_TRGM2_OUTPUT_SRC_HALL2_SNAPI                   (0x2FUL)  /* HALL2 snap pulse input */
#define HPM_TRGM2_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* ADC0 sequence queue trigger */
#define HPM_TRGM2_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* ADC1 sequence queue trigger */
#define HPM_TRGM2_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* ADC2 sequence queue trigger */
#define HPM_TRGM2_OUTPUT_SRC_ADC3_STRGI                    (0x33UL)  /* ADC3 sequence queue trigger */
#define HPM_TRGM2_OUTPUT_SRC_ADCX_PTRGI2A                  (0x34UL)  /* "ADC0, 1, 2, 3" preemption triggerA */
#define HPM_TRGM2_OUTPUT_SRC_ADCX_PTRGI2B                  (0x35UL)  /* "ADC0, 1, 2, 3" preemption triggerB */
#define HPM_TRGM2_OUTPUT_SRC_ADCX_PTRGI2C                  (0x36UL)  /* "ADC0, 1, 2, 3" preemption triggerC */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR4_SYNCI                  (0x37UL)  /* GPTMR4 counter sync input */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR4_IN2                    (0x38UL)  /* GPTMR4 channel2 capture in */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR4_IN3                    (0x39UL)  /* GPTMR4 channel3 capture in */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR5_SYNCI                  (0x3AUL)  /* GPTMR5 counter sync input */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR5_IN2                    (0x3BUL)  /* GPTMR5 channel2 capture in */
#define HPM_TRGM2_OUTPUT_SRC_GPTMR5_IN3                    (0x3CUL)  /* GPTMR5 channel3 capture in */
#define HPM_TRGM2_OUTPUT_SRC_ACMP2_WIN                     (0x3DUL)  /* CMP2 window */
#define HPM_TRGM2_OUTPUT_SRC_PTPC_CAP0                     (0x3EUL)  /* PTPC capture in0 */
#define HPM_TRGM2_OUTPUT_SRC_PTPC_CAP1                     (0x3FUL)  /* PTPC capture in1 */

/* trgm3_output mux definitions */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P0                      (0x0UL)   /* TRGM3 output data0(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P1                      (0x1UL)   /* TRGM3 output data1(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P2                      (0x2UL)   /* TRGM3 output data2(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P3                      (0x3UL)   /* TRGM3 output data3(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P4                      (0x4UL)   /* TRGM3 output data4(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P5                      (0x5UL)   /* TRGM3 output data5(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P6                      (0x6UL)   /* TRGM3 output data6(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P7                      (0x7UL)   /* TRGM3 output data7(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P8                      (0x8UL)   /* TRGM3 output data8(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P9                      (0x9UL)   /* TRGM3 output data9(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P10                     (0xAUL)   /* TRGM3 output data10(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_P11                     (0xBUL)   /* TRGM3 output data11(to IO) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_OUTX0                   (0xCUL)   /* TRGM3 output data0(to other TRGM) */
#define HPM_TRGM3_OUTPUT_SRC_TRGM3_OUTX1                   (0xDUL)   /* TRGM3 output data1(to other TRGM) */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_SYNCI                    (0xEUL)   /* PWM3 counter sync input */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FRCI                     (0xFUL)   /* PWM3 force output control */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FRCSYNCI                 (0x10UL)  /* PWM3 force output sync control */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_SHRLDSYNCI               (0x11UL)  /* PWM3 shadow register valid signal */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FAULTI0                  (0x12UL)  /* PWM3 fault0 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FAULTI1                  (0x13UL)  /* PWM3 fault1 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FAULTI2                  (0x14UL)  /* PWM3 fault2 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_FAULTI3                  (0x15UL)  /* PWM3 fault3 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN8                      (0x16UL)  /* PWM3 capture in8 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN9                      (0x17UL)  /* PWM3 capture in9 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN10                     (0x18UL)  /* PWM3 capture in10 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN11                     (0x19UL)  /* PWM3 capture in11 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN12                     (0x1AUL)  /* PWM3 capture in12 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN13                     (0x1BUL)  /* PWM3 capture in13 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN14                     (0x1CUL)  /* PWM3 capture in14 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN15                     (0x1DUL)  /* PWM3 capture in15 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN16                     (0x1EUL)  /* PWM3 capture in16 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN17                     (0x1FUL)  /* PWM3 capture in17 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN18                     (0x20UL)  /* PWM3 capture in18 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN19                     (0x21UL)  /* PWM3 capture in19 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN20                     (0x22UL)  /* PWM3 capture in20 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN21                     (0x23UL)  /* PWM3 capture in21 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN22                     (0x24UL)  /* PWM3 capture in22 */
#define HPM_TRGM3_OUTPUT_SRC_PWM3_IN23                     (0x25UL)  /* PWM3 capture in23 */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_A                        (0x26UL)  /* QEI3 phase A input */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_B                        (0x27UL)  /* QEI3 phase B input */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_Z                        (0x28UL)  /* QEI3 phase Z input */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_H                        (0x29UL)  /* QEI3 phase H input */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_PAUSE                    (0x2AUL)  /* QEI3 pause */
#define HPM_TRGM3_OUTPUT_SRC_QEI3_SNAPI                    (0x2BUL)  /* QEI3 snap pulse input */
#define HPM_TRGM3_OUTPUT_SRC_HALL3_U                       (0x2CUL)  /* HALL3 phase U input */
#define HPM_TRGM3_OUTPUT_SRC_HALL3_V                       (0x2DUL)  /* HALL3 phase V input */
#define HPM_TRGM3_OUTPUT_SRC_HALL3_W                       (0x2EUL)  /* HALL3 phase W input */
#define HPM_TRGM3_OUTPUT_SRC_HALL3_SNAPI                   (0x2FUL)  /* HALL3 snap pulse input */
#define HPM_TRGM3_OUTPUT_SRC_ADC0_STRGI                    (0x30UL)  /* ADC0 sequence queue trigger */
#define HPM_TRGM3_OUTPUT_SRC_ADC1_STRGI                    (0x31UL)  /* ADC1 sequence queue trigger */
#define HPM_TRGM3_OUTPUT_SRC_ADC2_STRGI                    (0x32UL)  /* ADC2 sequence queue trigger */
#define HPM_TRGM3_OUTPUT_SRC_ADC3_STRGI                    (0x33UL)  /* ADC3 sequence queue trigger */
#define HPM_TRGM3_OUTPUT_SRC_ADCX_PTRGI3A                  (0x34UL)  /* "ADC0, 1, 2, 3" preemption triggerA */
#define HPM_TRGM3_OUTPUT_SRC_ADCX_PTRGI3B                  (0x35UL)  /* "ADC0, 1, 2, 3" preemption triggerB */
#define HPM_TRGM3_OUTPUT_SRC_ADCX_PTRGI3C                  (0x36UL)  /* "ADC0, 1, 2, 3" preemption triggerC */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR6_SYNCI                  (0x37UL)  /* GPTMR6 counter sync input */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR6_IN2                    (0x38UL)  /* GPTMR6 channel2 capture in */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR6_IN3                    (0x39UL)  /* GPTMR6 channel3 capture in */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR7_SYNCI                  (0x3AUL)  /* GPTMR7 counter sync input */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR7_IN2                    (0x3BUL)  /* GPTMR7 channel2 capture in */
#define HPM_TRGM3_OUTPUT_SRC_GPTMR7_IN3                    (0x3CUL)  /* GPTMR7 channel3 capture in */
#define HPM_TRGM3_OUTPUT_SRC_ACMP3_WIN                     (0x3DUL)  /* CMP3 window */
#define HPM_TRGM3_OUTPUT_SRC_PTPC_CAP0                     (0x3EUL)  /* PTPC capture in0 */
#define HPM_TRGM3_OUTPUT_SRC_PTPC_CAP1                     (0x3FUL)  /* PTPC capture in1 */

/* trgm0_filter mux definitions */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN0                      (0x0UL)   /* PWM0 capture in0 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN1                      (0x1UL)   /* PWM0 capture in1 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN2                      (0x2UL)   /* PWM0 capture in2 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN3                      (0x3UL)   /* PWM0 capture in3 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN4                      (0x4UL)   /* PWM0 capture in4 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN5                      (0x5UL)   /* PWM0 capture in5 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN6                      (0x6UL)   /* PWM0 capture in6 */
#define HPM_TRGM0_FILTER_SRC_PWM0_IN7                      (0x7UL)   /* PWM0 capture in7 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN0                     (0x8UL)   /* TRGM0 input0 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN1                     (0x9UL)   /* TRGM0 input1 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN2                     (0xAUL)   /* TRGM0 input2 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN3                     (0xBUL)   /* TRGM0 input3 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN4                     (0xCUL)   /* TRGM0 input4 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN5                     (0xDUL)   /* TRGM0 input5 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN6                     (0xEUL)   /* TRGM0 input6 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN7                     (0xFUL)   /* TRGM0 input7 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN8                     (0x10UL)  /* TRGM0 input8 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN9                     (0x11UL)  /* TRGM0 input9 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN10                    (0x12UL)  /* TRGM0 input10 */
#define HPM_TRGM0_FILTER_SRC_TRGM0_IN11                    (0x13UL)  /* TRGM0 input11 */

/* trgm1_filter mux definitions */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN0                      (0x0UL)   /* PWM1 capture in0 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN1                      (0x1UL)   /* PWM1 capture in1 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN2                      (0x2UL)   /* PWM1 capture in2 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN3                      (0x3UL)   /* PWM1 capture in3 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN4                      (0x4UL)   /* PWM1 capture in4 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN5                      (0x5UL)   /* PWM1 capture in5 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN6                      (0x6UL)   /* PWM1 capture in6 */
#define HPM_TRGM1_FILTER_SRC_PWM1_IN7                      (0x7UL)   /* PWM1 capture in7 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN0                     (0x8UL)   /* TRGM1 input0 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN1                     (0x9UL)   /* TRGM1 input1 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN2                     (0xAUL)   /* TRGM1 input2 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN3                     (0xBUL)   /* TRGM1 input3 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN4                     (0xCUL)   /* TRGM1 input4 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN5                     (0xDUL)   /* TRGM1 input5 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN6                     (0xEUL)   /* TRGM1 input6 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN7                     (0xFUL)   /* TRGM1 input7 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN8                     (0x10UL)  /* TRGM1 input8 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN9                     (0x11UL)  /* TRGM1 input9 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN10                    (0x12UL)  /* TRGM1 input10 */
#define HPM_TRGM1_FILTER_SRC_TRGM1_IN11                    (0x13UL)  /* TRGM1 input11 */

/* trgm2_filter mux definitions */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN0                      (0x0UL)   /* PWM0 capture in0 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN1                      (0x1UL)   /* PWM0 capture in1 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN2                      (0x2UL)   /* PWM0 capture in2 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN3                      (0x3UL)   /* PWM0 capture in3 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN4                      (0x4UL)   /* PWM0 capture in4 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN5                      (0x5UL)   /* PWM0 capture in5 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN6                      (0x6UL)   /* PWM0 capture in6 */
#define HPM_TRGM2_FILTER_SRC_PWM2_IN7                      (0x7UL)   /* PWM0 capture in7 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN0                     (0x8UL)   /* TRGM2 input0 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN1                     (0x9UL)   /* TRGM2 input1 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN2                     (0xAUL)   /* TRGM2 input2 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN3                     (0xBUL)   /* TRGM2 input3 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN4                     (0xCUL)   /* TRGM2 input4 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN5                     (0xDUL)   /* TRGM2 input5 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN6                     (0xEUL)   /* TRGM2 input6 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN7                     (0xFUL)   /* TRGM2 input7 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN8                     (0x10UL)  /* TRGM2 input8 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN9                     (0x11UL)  /* TRGM2 input9 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN10                    (0x12UL)  /* TRGM2 input10 */
#define HPM_TRGM2_FILTER_SRC_TRGM2_IN11                    (0x13UL)  /* TRGM2 input11 */

/* trgm3_filter mux definitions */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN0                      (0x0UL)   /* PWM3 capture in0 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN1                      (0x1UL)   /* PWM3 capture in1 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN2                      (0x2UL)   /* PWM3 capture in2 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN3                      (0x3UL)   /* PWM3 capture in3 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN4                      (0x4UL)   /* PWM3 capture in4 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN5                      (0x5UL)   /* PWM3 capture in5 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN6                      (0x6UL)   /* PWM3 capture in6 */
#define HPM_TRGM3_FILTER_SRC_PWM3_IN7                      (0x7UL)   /* PWM3 capture in7 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN0                     (0x8UL)   /* TRGM3 input0 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN1                     (0x9UL)   /* TRGM3 input1 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN2                     (0xAUL)   /* TRGM3 input2 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN3                     (0xBUL)   /* TRGM3 input3 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN4                     (0xCUL)   /* TRGM3 input4 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN5                     (0xDUL)   /* TRGM3 input5 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN6                     (0xEUL)   /* TRGM3 input6 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN7                     (0xFUL)   /* TRGM3 input7 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN8                     (0x10UL)  /* TRGM3 input8 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN9                     (0x11UL)  /* TRGM3 input9 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN10                    (0x12UL)  /* TRGM3 input10 */
#define HPM_TRGM3_FILTER_SRC_TRGM3_IN11                    (0x13UL)  /* TRGM3 input11 */

/* trgm0_dma mux definitions */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP0                        (0x0UL)   /* PWM0 CMP0 capture in or CMP0 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP1                        (0x1UL)   /* PWM0 CMP1 capture in or CMP1 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP2                        (0x2UL)   /* PWM0 CMP2 capture in or CMP2 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP3                        (0x3UL)   /* PWM0 CMP3 capture in or CMP3 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP4                        (0x4UL)   /* PWM0 CMP4 capture in or CMP4 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP5                        (0x5UL)   /* PWM0 CMP5 capture in or CMP5 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP6                        (0x6UL)   /* PWM0 CMP6 capture in or CMP6 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP7                        (0x7UL)   /* PWM0 CMP7 capture in or CMP7 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP8                        (0x8UL)   /* PWM0 CMP8 capture in or CMP8 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP9                        (0x9UL)   /* PWM0 CMP9 capture in or CMP9 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP10                       (0xAUL)   /* PWM0 CMP10 capture in or CMP10 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP11                       (0xBUL)   /* PWM0 CMP11 capture in or CMP11 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP12                       (0xCUL)   /* PWM0 CMP12 capture in or CMP12 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP13                       (0xDUL)   /* PWM0 CMP13 capture in or CMP13 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP14                       (0xEUL)   /* PWM0 CMP14 capture in or CMP14 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP15                       (0xFUL)   /* PWM0 CMP15 capture in or CMP15 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP16                       (0x10UL)  /* PWM0 CMP16 capture in or CMP16 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP17                       (0x11UL)  /* PWM0 CMP17 capture in or CMP17 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP18                       (0x12UL)  /* PWM0 CMP18 capture in or CMP18 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP19                       (0x13UL)  /* PWM0 CMP19 capture in or CMP19 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP20                       (0x14UL)  /* PWM0 CMP20 capture in or CMP20 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP21                       (0x15UL)  /* PWM0 CMP21 capture in or CMP21 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP22                       (0x16UL)  /* PWM0 CMP22 capture in or CMP22 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_CMP23                       (0x17UL)  /* PWM0 CMP23 capture in or CMP23 comparison result of compare value and timer value */
#define HPM_TRGM0_DMA_SRC_PWM0_RLD                         (0x18UL)  /* PWM0 main count reload */
#define HPM_TRGM0_DMA_SRC_PWM0_HALFRLD                     (0x19UL)  /* PWM0 count half reload */
#define HPM_TRGM0_DMA_SRC_PWM0_XRLD                        (0x1AUL)  /* PWM0 main count xreload */
#define HPM_TRGM0_DMA_SRC_QEI0                             (0x1BUL)  /* QEI0 dma request */
#define HPM_TRGM0_DMA_SRC_HALL0                            (0x1CUL)  /* HALL0 dma request */

/* trgm1_dma mux definitions */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP0                        (0x0UL)   /* PWM1 CMP0 capture in or CMP0 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP1                        (0x1UL)   /* PWM1 CMP1 capture in or CMP1 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP2                        (0x2UL)   /* PWM1 CMP2 capture in or CMP2 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP3                        (0x3UL)   /* PWM1 CMP3 capture in or CMP3 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP4                        (0x4UL)   /* PWM1 CMP4 capture in or CMP4 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP5                        (0x5UL)   /* PWM1 CMP5 capture in or CMP5 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP6                        (0x6UL)   /* PWM1 CMP6 capture in or CMP6 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP7                        (0x7UL)   /* PWM1 CMP7 capture in or CMP7 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP8                        (0x8UL)   /* PWM1 CMP8 capture in or CMP8 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP9                        (0x9UL)   /* PWM1 CMP9 capture in or CMP9 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP10                       (0xAUL)   /* PWM1 CMP10 capture in or CMP10 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP11                       (0xBUL)   /* PWM1 CMP11 capture in or CMP11 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP12                       (0xCUL)   /* PWM1 CMP12 capture in or CMP12 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP13                       (0xDUL)   /* PWM1 CMP13 capture in or CMP13 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP14                       (0xEUL)   /* PWM1 CMP14 capture in or CMP14 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP15                       (0xFUL)   /* PWM1 CMP15 capture in or CMP15 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP16                       (0x10UL)  /* PWM1 CMP16 capture in or CMP16 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP17                       (0x11UL)  /* PWM1 CMP17 capture in or CMP17 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP18                       (0x12UL)  /* PWM1 CMP18 capture in or CMP18 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP19                       (0x13UL)  /* PWM1 CMP19 capture in or CMP19 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP20                       (0x14UL)  /* PWM1 CMP20 capture in or CMP20 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP21                       (0x15UL)  /* PWM1 CMP21 capture in or CMP21 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP22                       (0x16UL)  /* PWM1 CMP22 capture in or CMP22 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_CMP23                       (0x17UL)  /* PWM1 CMP23 capture in or CMP23 comparison result of compare value and timer value */
#define HPM_TRGM1_DMA_SRC_PWM1_RLD                         (0x18UL)  /* PWM1 main count reload */
#define HPM_TRGM1_DMA_SRC_PWM1_HALFRLD                     (0x19UL)  /* PWM1 count half reload */
#define HPM_TRGM1_DMA_SRC_PWM1_XRLD                        (0x1AUL)  /* PWM1 main count xreload */
#define HPM_TRGM1_DMA_SRC_QEI1                             (0x1BUL)  /* QEI1 dma request */
#define HPM_TRGM1_DMA_SRC_HALL1                            (0x1CUL)  /* HALL1 dma request */

/* trgm2_dma mux definitions */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP0                        (0x0UL)   /* PWM2 CMP0 capture in or CMP0 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP1                        (0x1UL)   /* PWM2 CMP1 capture in or CMP1 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP2                        (0x2UL)   /* PWM2 CMP2 capture in or CMP2 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP3                        (0x3UL)   /* PWM2 CMP3 capture in or CMP3 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP4                        (0x4UL)   /* PWM2 CMP4 capture in or CMP4 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP5                        (0x5UL)   /* PWM2 CMP5 capture in or CMP5 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP6                        (0x6UL)   /* PWM2 CMP6 capture in or CMP6 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP7                        (0x7UL)   /* PWM2 CMP7 capture in or CMP7 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP8                        (0x8UL)   /* PWM2 CMP8 capture in or CMP8 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP9                        (0x9UL)   /* PWM2 CMP9 capture in or CMP9 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP10                       (0xAUL)   /* PWM2 CMP10 capture in or CMP10 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP11                       (0xBUL)   /* PWM2 CMP11 capture in or CMP11 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP12                       (0xCUL)   /* PWM2 CMP12 capture in or CMP12 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP13                       (0xDUL)   /* PWM2 CMP13 capture in or CMP13 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP14                       (0xEUL)   /* PWM2 CMP14 capture in or CMP14 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP15                       (0xFUL)   /* PWM2 CMP15 capture in or CMP15 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP16                       (0x10UL)  /* PWM2 CMP16 capture in or CMP16 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP17                       (0x11UL)  /* PWM2 CMP17 capture in or CMP17 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP18                       (0x12UL)  /* PWM2 CMP18 capture in or CMP18 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP19                       (0x13UL)  /* PWM2 CMP19 capture in or CMP19 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP20                       (0x14UL)  /* PWM2 CMP20 capture in or CMP20 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP21                       (0x15UL)  /* PWM2 CMP21 capture in or CMP21 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP22                       (0x16UL)  /* PWM2 CMP22 capture in or CMP22 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_CMP23                       (0x17UL)  /* PWM2 CMP23 capture in or CMP23 comparison result of compare value and timer value */
#define HPM_TRGM2_DMA_SRC_PWM2_RLD                         (0x18UL)  /* PWM2 main count reload */
#define HPM_TRGM2_DMA_SRC_PWM2_HALFRLD                     (0x19UL)  /* PWM2 count half reload */
#define HPM_TRGM2_DMA_SRC_PWM2_XRLD                        (0x1AUL)  /* PWM2 main count xreload */
#define HPM_TRGM2_DMA_SRC_QEI2                             (0x1BUL)  /* QEI2 dma request */
#define HPM_TRGM2_DMA_SRC_HALL2                            (0x1CUL)  /* HALL2 dma request */

/* trgm3_dma mux definitions */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP0                        (0x0UL)   /* PWM3 CMP0 capture in or CMP0 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP1                        (0x1UL)   /* PWM3 CMP1 capture in or CMP1 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP2                        (0x2UL)   /* PWM3 CMP2 capture in or CMP2 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP3                        (0x3UL)   /* PWM3 CMP3 capture in or CMP3 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP4                        (0x4UL)   /* PWM3 CMP4 capture in or CMP4 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP5                        (0x5UL)   /* PWM3 CMP5 capture in or CMP5 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP6                        (0x6UL)   /* PWM3 CMP6 capture in or CMP6 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP7                        (0x7UL)   /* PWM3 CMP7 capture in or CMP7 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP8                        (0x8UL)   /* PWM3 CMP8 capture in or CMP8 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP9                        (0x9UL)   /* PWM3 CMP9 capture in or CMP9 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP10                       (0xAUL)   /* PWM3 CMP10 capture in or CMP10 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP11                       (0xBUL)   /* PWM3 CMP11 capture in or CMP11 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP12                       (0xCUL)   /* PWM3 CMP12 capture in or CMP12 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP13                       (0xDUL)   /* PWM3 CMP13 capture in or CMP13 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP14                       (0xEUL)   /* PWM3 CMP14 capture in or CMP14 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP15                       (0xFUL)   /* PWM3 CMP15 capture in or CMP15 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP16                       (0x10UL)  /* PWM3 CMP16 capture in or CMP16 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP17                       (0x11UL)  /* PWM3 CMP17 capture in or CMP17 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP18                       (0x12UL)  /* PWM3 CMP18 capture in or CMP18 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP19                       (0x13UL)  /* PWM3 CMP19 capture in or CMP19 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP20                       (0x14UL)  /* PWM3 CMP20 capture in or CMP20 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP21                       (0x15UL)  /* PWM3 CMP21 capture in or CMP21 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP22                       (0x16UL)  /* PWM3 CMP22 capture in or CMP22 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_CMP23                       (0x17UL)  /* PWM3 CMP23 capture in or CMP23 comparison result of compare value and timer value */
#define HPM_TRGM3_DMA_SRC_PWM3_RLD                         (0x18UL)  /* PWM3 main count reload */
#define HPM_TRGM3_DMA_SRC_PWM3_HALFRLD                     (0x19UL)  /* PWM3 count half reload */
#define HPM_TRGM3_DMA_SRC_PWM3_XRLD                        (0x1AUL)  /* PWM3 main count xreload */
#define HPM_TRGM3_DMA_SRC_QEI3                             (0x1BUL)  /* QEI3 dma request */
#define HPM_TRGM3_DMA_SRC_HALL3                            (0x1CUL)  /* HALL3 dma request */



#endif /* HPM_TRGMMUX_SRC_H */
