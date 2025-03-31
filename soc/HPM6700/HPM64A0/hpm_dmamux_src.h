/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_DMAMUX_SRC_H
#define HPM_DMAMUX_SRC_H

/* dma mux definitions */
#define HPM_DMA_SRC_SPI0_RX                                (0x0UL)   /* SPI0 Receive */
#define HPM_DMA_SRC_SPI0_TX                                (0x1UL)   /* SPI0 Transmit */
#define HPM_DMA_SRC_SPI1_RX                                (0x2UL)   /* SPI1 Receive */
#define HPM_DMA_SRC_SPI1_TX                                (0x3UL)   /* SPI1 Transmit */
#define HPM_DMA_SRC_SPI2_RX                                (0x4UL)   /* SPI2 Receive */
#define HPM_DMA_SRC_SPI2_TX                                (0x5UL)   /* SPI2 Transmit */
#define HPM_DMA_SRC_SPI3_RX                                (0x6UL)   /* SPI3 Receive */
#define HPM_DMA_SRC_SPI3_TX                                (0x7UL)   /* SPI3 Transmit */
#define HPM_DMA_SRC_UART0_RX                               (0x8UL)   /* UART0 Receive */
#define HPM_DMA_SRC_UART0_TX                               (0x9UL)   /* UART0 Transmit */
#define HPM_DMA_SRC_UART1_RX                               (0xAUL)   /* UART1 Receive */
#define HPM_DMA_SRC_UART1_TX                               (0xBUL)   /* UART1 Transmit */
#define HPM_DMA_SRC_UART2_RX                               (0xCUL)   /* UART2 Receive */
#define HPM_DMA_SRC_UART2_TX                               (0xDUL)   /* UART2 Transmit */
#define HPM_DMA_SRC_UART3_RX                               (0xEUL)   /* UART3 Receive */
#define HPM_DMA_SRC_UART3_TX                               (0xFUL)   /* UART3 Transmit */
#define HPM_DMA_SRC_UART4_RX                               (0x10UL)  /* UART4 Receive */
#define HPM_DMA_SRC_UART4_TX                               (0x11UL)  /* UART4 Transmit */
#define HPM_DMA_SRC_UART5_RX                               (0x12UL)  /* UART5 Receive */
#define HPM_DMA_SRC_UART5_TX                               (0x13UL)  /* UART5 Transmit */
#define HPM_DMA_SRC_UART6_RX                               (0x14UL)  /* UART6 Receive */
#define HPM_DMA_SRC_UART6_TX                               (0x15UL)  /* UART6 Transmit */
#define HPM_DMA_SRC_UART7_RX                               (0x16UL)  /* UART7 Receive */
#define HPM_DMA_SRC_UART7_TX                               (0x17UL)  /* UART7 Transmit */
#define HPM_DMA_SRC_UART8_RX                               (0x18UL)  /* UART8 Receive */
#define HPM_DMA_SRC_UART8_TX                               (0x19UL)  /* UART8 Transmit */
#define HPM_DMA_SRC_UART9_RX                               (0x1AUL)  /* UART9 Receive */
#define HPM_DMA_SRC_UART9_TX                               (0x1BUL)  /* UART9 Transmit */
#define HPM_DMA_SRC_UART10_RX                              (0x1CUL)  /* UART10 Receive */
#define HPM_DMA_SRC_UART10_TX                              (0x1DUL)  /* UART10 Transmit */
#define HPM_DMA_SRC_UART11_RX                              (0x1EUL)  /* UART11 Receive */
#define HPM_DMA_SRC_UART11_TX                              (0x1FUL)  /* UART11 Transmit */
#define HPM_DMA_SRC_UART12_RX                              (0x20UL)  /* UART12 Receive */
#define HPM_DMA_SRC_UART12_TX                              (0x21UL)  /* UART12 Transmit */
#define HPM_DMA_SRC_UART13_RX                              (0x22UL)  /* UART13 Receive */
#define HPM_DMA_SRC_UART13_TX                              (0x23UL)  /* UART13 Transmit */
#define HPM_DMA_SRC_UART14_RX                              (0x24UL)  /* UART14 Receive */
#define HPM_DMA_SRC_UART14_TX                              (0x25UL)  /* UART14 Transmit */
#define HPM_DMA_SRC_UART15_RX                              (0x26UL)  /* UART15 Receive */
#define HPM_DMA_SRC_UART15_TX                              (0x27UL)  /* UART15 Transmit */
#define HPM_DMA_SRC_I2S0_RX                                (0x28UL)  /* I2S0 Receive */
#define HPM_DMA_SRC_I2S0_TX                                (0x29UL)  /* I2S0 Transmit */
#define HPM_DMA_SRC_I2S1_RX                                (0x2AUL)  /* I2S1 Receive */
#define HPM_DMA_SRC_I2S1_TX                                (0x2BUL)  /* I2S1 Transmit */
#define HPM_DMA_SRC_I2S2_RX                                (0x2CUL)  /* I2S2 Receive */
#define HPM_DMA_SRC_I2S2_TX                                (0x2DUL)  /* I2S2 Transmit */
#define HPM_DMA_SRC_I2S3_RX                                (0x2EUL)  /* I2S3 Receive */
#define HPM_DMA_SRC_I2S3_TX                                (0x2FUL)  /* I2S3 Transmit */
#define HPM_DMA_SRC_MOT0_0                                 (0x30UL)  /* TRGM0 DMA0(include PWM0, QEI0, HALL0) */
#define HPM_DMA_SRC_MOT0_1                                 (0x31UL)  /* TRGM0 DMA1(include PWM0, QEI0, HALL0) */
#define HPM_DMA_SRC_MOT0_2                                 (0x32UL)  /* TRGM0 DMA2(include PWM0, QEI0, HALL0) */
#define HPM_DMA_SRC_MOT0_3                                 (0x33UL)  /* TRGM0 DMA3(include PWM0, QEI0, HALL0) */
#define HPM_DMA_SRC_MOT1_0                                 (0x34UL)  /* TRGM1 DMA0(include PWM1, QEI1, HALL1) */
#define HPM_DMA_SRC_MOT1_1                                 (0x35UL)  /* TRGM1 DMA1(include PWM1, QEI1, HALL1) */
#define HPM_DMA_SRC_MOT1_2                                 (0x36UL)  /* TRGM1 DMA2(include PWM1, QEI1, HALL1) */
#define HPM_DMA_SRC_MOT1_3                                 (0x37UL)  /* TRGM1 DMA3(include PWM1, QEI1, HALL1) */
#define HPM_DMA_SRC_MOT2_0                                 (0x38UL)  /* TRGM2 DMA0(include PWM2, QEI2, HALL2) */
#define HPM_DMA_SRC_MOT2_1                                 (0x39UL)  /* TRGM2 DMA1(include PWM2, QEI2, HALL2) */
#define HPM_DMA_SRC_MOT2_2                                 (0x3AUL)  /* TRGM2 DMA2(include PWM2, QEI2, HALL2) */
#define HPM_DMA_SRC_MOT2_3                                 (0x3BUL)  /* TRGM2 DMA3(include PWM2, QEI2, HALL2) */
#define HPM_DMA_SRC_MOT3_0                                 (0x3CUL)  /* TRGM3 DMA0(include PWM3, QEI3, HALL3) */
#define HPM_DMA_SRC_MOT3_1                                 (0x3DUL)  /* TRGM3 DMA1(include PWM3, QEI3, HALL3) */
#define HPM_DMA_SRC_MOT3_2                                 (0x3EUL)  /* TRGM3 DMA2(include PWM3, QEI3, HALL3) */
#define HPM_DMA_SRC_MOT3_3                                 (0x3FUL)  /* TRGM3 DMA3(include PWM3, QEI3, HALL3) */
#define HPM_DMA_SRC_NTMR0_0                                (0x40UL)  /* NTMR0 channel0 */
#define HPM_DMA_SRC_NTMR0_1                                (0x41UL)  /* NTMR0 channel1 */
#define HPM_DMA_SRC_NTMR0_2                                (0x42UL)  /* NTMR0 channel2 */
#define HPM_DMA_SRC_NTMR0_3                                (0x43UL)  /* NTMR0 channel3 */
#define HPM_DMA_SRC_NTMR1_0                                (0x44UL)  /* NTMR1 channel0 */
#define HPM_DMA_SRC_NTMR1_1                                (0x45UL)  /* NTMR1 channel1 */
#define HPM_DMA_SRC_NTMR1_2                                (0x46UL)  /* NTMR1 channel2 */
#define HPM_DMA_SRC_NTMR1_3                                (0x47UL)  /* NTMR1 channel3 */
#define HPM_DMA_SRC_GPTMR0_0                               (0x48UL)  /* GPTMR0 channel0 */
#define HPM_DMA_SRC_GPTMR0_1                               (0x49UL)  /* GPTMR0 channel1 */
#define HPM_DMA_SRC_GPTMR0_2                               (0x4AUL)  /* GPTMR0 channel2 */
#define HPM_DMA_SRC_GPTMR0_3                               (0x4BUL)  /* GPTMR0 channel3 */
#define HPM_DMA_SRC_GPTMR1_0                               (0x4CUL)  /* GPTMR1 channel0 */
#define HPM_DMA_SRC_GPTMR1_1                               (0x4DUL)  /* GPTMR1 channel1 */
#define HPM_DMA_SRC_GPTMR1_2                               (0x4EUL)  /* GPTMR1 channel2 */
#define HPM_DMA_SRC_GPTMR1_3                               (0x4FUL)  /* GPTMR1 channel3 */
#define HPM_DMA_SRC_GPTMR2_0                               (0x50UL)  /* GPTMR2 channel0 */
#define HPM_DMA_SRC_GPTMR2_1                               (0x51UL)  /* GPTMR2 channel1 */
#define HPM_DMA_SRC_GPTMR2_2                               (0x52UL)  /* GPTMR2 channel2 */
#define HPM_DMA_SRC_GPTMR2_3                               (0x53UL)  /* GPTMR2 channel3 */
#define HPM_DMA_SRC_GPTMR3_0                               (0x54UL)  /* GPTMR3 channel0 */
#define HPM_DMA_SRC_GPTMR3_1                               (0x55UL)  /* GPTMR3 channel1 */
#define HPM_DMA_SRC_GPTMR3_2                               (0x56UL)  /* GPTMR3 channel2 */
#define HPM_DMA_SRC_GPTMR3_3                               (0x57UL)  /* GPTMR3 channel3 */
#define HPM_DMA_SRC_GPTMR4_0                               (0x58UL)  /* GPTMR4 channel0 */
#define HPM_DMA_SRC_GPTMR4_1                               (0x59UL)  /* GPTMR4 channel1 */
#define HPM_DMA_SRC_GPTMR4_2                               (0x5AUL)  /* GPTMR4 channel2 */
#define HPM_DMA_SRC_GPTMR4_3                               (0x5BUL)  /* GPTMR4 channel3 */
#define HPM_DMA_SRC_GPTMR5_0                               (0x5CUL)  /* GPTMR5 channel0 */
#define HPM_DMA_SRC_GPTMR5_1                               (0x5DUL)  /* GPTMR5 channel1 */
#define HPM_DMA_SRC_GPTMR5_2                               (0x5EUL)  /* GPTMR5 channel2 */
#define HPM_DMA_SRC_GPTMR5_3                               (0x5FUL)  /* GPTMR5 channel3 */
#define HPM_DMA_SRC_GPTMR6_0                               (0x60UL)  /* GPTMR6 channel0 */
#define HPM_DMA_SRC_GPTMR6_1                               (0x61UL)  /* GPTMR6 channel1 */
#define HPM_DMA_SRC_GPTMR6_2                               (0x62UL)  /* GPTMR6 channel2 */
#define HPM_DMA_SRC_GPTMR6_3                               (0x63UL)  /* GPTMR6 channel3 */
#define HPM_DMA_SRC_GPTMR7_0                               (0x64UL)  /* GPTMR7 channel0 */
#define HPM_DMA_SRC_GPTMR7_1                               (0x65UL)  /* GPTMR7 channel1 */
#define HPM_DMA_SRC_GPTMR7_2                               (0x66UL)  /* GPTMR7 channel2 */
#define HPM_DMA_SRC_GPTMR7_3                               (0x67UL)  /* GPTMR7 channel3 */
#define HPM_DMA_SRC_I2C0                                   (0x68UL)  /* I2C0 */
#define HPM_DMA_SRC_I2C1                                   (0x69UL)  /* I2C1 */
#define HPM_DMA_SRC_I2C2                                   (0x6AUL)  /* I2C2 */
#define HPM_DMA_SRC_I2C3                                   (0x6BUL)  /* I2C3 */
#define HPM_DMA_SRC_XPI0_RX                                (0x6CUL)  /* XPI0 Receive */
#define HPM_DMA_SRC_XPI0_TX                                (0x6DUL)  /* XPI0 Transmit */
#define HPM_DMA_SRC_XPI1_RX                                (0x6EUL)  /* XPI1 Receive */
#define HPM_DMA_SRC_XPI1_TX                                (0x6FUL)  /* XPI1 Transmit */
#define HPM_DMA_SRC_ACMP_0                                 (0x70UL)  /* ACMP0 */
#define HPM_DMA_SRC_ACMP_1                                 (0x71UL)  /* ACMP1 */
#define HPM_DMA_SRC_ACMP_2                                 (0x72UL)  /* ACMP2 */
#define HPM_DMA_SRC_ACMP_3                                 (0x73UL)  /* ACMP3 */



#endif /* HPM_DMAMUX_SRC_H */
