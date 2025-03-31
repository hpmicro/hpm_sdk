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
#define HPM_DMA_SRC_I2C0                                   (0x18UL)  /* I2C0 */
#define HPM_DMA_SRC_I2C1                                   (0x19UL)  /* I2C1 */
#define HPM_DMA_SRC_I2C2                                   (0x1AUL)  /* I2C2 */
#define HPM_DMA_SRC_I2C3                                   (0x1BUL)  /* I2C3 */
#define HPM_DMA_SRC_MCAN0                                  (0x1CUL)  /* CAN0 */
#define HPM_DMA_SRC_MCAN1                                  (0x1DUL)  /* CAN1 */
#define HPM_DMA_SRC_MCAN2                                  (0x1EUL)  /* CAN2 */
#define HPM_DMA_SRC_MCAN3                                  (0x1FUL)  /* CAN3 */
#define HPM_DMA_SRC_I2S0_RX_0                              (0x20UL)  /* I2S0 Receive Channel 0 */
#define HPM_DMA_SRC_I2S0_RX_1                              (0x21UL)  /* I2S0 Receive Channel 1 */
#define HPM_DMA_SRC_I2S0_RX_2                              (0x22UL)  /* I2S0 Receive Channel 2 */
#define HPM_DMA_SRC_I2S0_RX_3                              (0x23UL)  /* I2S0 Receive Channel 3 */
#define HPM_DMA_SRC_I2S0_TX_0                              (0x24UL)  /* I2S0 Transmit Channel 0 */
#define HPM_DMA_SRC_I2S0_TX_1                              (0x25UL)  /* I2S0 Transmit Channel 1 */
#define HPM_DMA_SRC_I2S0_TX_2                              (0x26UL)  /* I2S0 Transmit Channel 2 */
#define HPM_DMA_SRC_I2S0_TX_3                              (0x27UL)  /* I2S0 Transmit Channel 3 */
#define HPM_DMA_SRC_I2S1_RX_0                              (0x28UL)  /* I2S1 Receive Channel 0 */
#define HPM_DMA_SRC_I2S1_RX_1                              (0x29UL)  /* I2S1 Receive Channel 1 */
#define HPM_DMA_SRC_I2S1_RX_2                              (0x2AUL)  /* I2S1 Receive Channel 2 */
#define HPM_DMA_SRC_I2S1_RX_3                              (0x2BUL)  /* I2S1 Receive Channel 3 */
#define HPM_DMA_SRC_I2S1_TX_0                              (0x2CUL)  /* I2S1 Transmit Channel 0 */
#define HPM_DMA_SRC_I2S1_TX_1                              (0x2DUL)  /* I2S1 Transmit Channel 1 */
#define HPM_DMA_SRC_I2S1_TX_2                              (0x2EUL)  /* I2S1 Transmit Channel 2 */
#define HPM_DMA_SRC_I2S1_TX_3                              (0x2FUL)  /* I2S1 Transmit Channel 3 */
#define HPM_DMA_SRC_GPTMR0_0                               (0x30UL)  /* GPTMR0 Channel 0 */
#define HPM_DMA_SRC_GPTMR0_1                               (0x31UL)  /* GPTMR0 Channel 1 */
#define HPM_DMA_SRC_GPTMR0_2                               (0x32UL)  /* GPTMR0 Channel 2 */
#define HPM_DMA_SRC_GPTMR0_3                               (0x33UL)  /* GPTMR0 Channel 3 */
#define HPM_DMA_SRC_GPTMR1_0                               (0x34UL)  /* GPTMR1 Channel 0 */
#define HPM_DMA_SRC_GPTMR1_1                               (0x35UL)  /* GPTMR1 Channel 1 */
#define HPM_DMA_SRC_GPTMR1_2                               (0x36UL)  /* GPTMR1 Channel 2 */
#define HPM_DMA_SRC_GPTMR1_3                               (0x37UL)  /* GPTMR1 Channel 3 */
#define HPM_DMA_SRC_GPTMR2_0                               (0x38UL)  /* GPTMR2 channel 0 */
#define HPM_DMA_SRC_GPTMR2_1                               (0x39UL)  /* GPTMR2 Channel 1 */
#define HPM_DMA_SRC_GPTMR2_2                               (0x3AUL)  /* GPTMR2 Channel 2 */
#define HPM_DMA_SRC_GPTMR2_3                               (0x3BUL)  /* GPTMR2 Channel 3 */
#define HPM_DMA_SRC_GPTMR3_0                               (0x3CUL)  /* GPTMR3 channel 0 */
#define HPM_DMA_SRC_GPTMR3_1                               (0x3DUL)  /* GPTMR3 Channel 1 */
#define HPM_DMA_SRC_GPTMR3_2                               (0x3EUL)  /* GPTMR3 Channel 2 */
#define HPM_DMA_SRC_GPTMR3_3                               (0x3FUL)  /* GPTMR3 Channel 3 */
#define HPM_DMA_SRC_MOT_0                                  (0x40UL)  /* TRGM0 DMA request 0 (contains DMA requests for PWM0, PWM1, PWM2, PWM3, QEI0, QEI1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_1                                  (0x41UL)  /* TRGM0 DMA request 1 (contains DMA requests for PWM0, PWM1, PWM2, PWM3, QEI0, QEI1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_2                                  (0x42UL)  /* TRGM0 DMA request 2 (contains DMA requests for PWM0, PWM1, PWM2, PWM3, QEI0, QEI1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_3                                  (0x43UL)  /* TRGM0 DMA request 3 (contains DMA requests for PWM0, PWM1, PWM2, PWM3, QEI0, QEI1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_4                                  (0x44UL)  /* TRGM0 DMA request 4 (contains DMA requests for PWM0, PWM1, PWM2, PWM3, QEI0, QEI1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_5                                  (0x45UL)  /* TRGM0 DMA request 5 (contains DMA requests for PWM0, PWM1, PWM2, PWM3, QEI0, QEI1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_6                                  (0x46UL)  /* TRGM0 DMA request 6 (contains DMA requests for PWM0, PWM1, PWM2, PWM3, QEI0, QEI1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_7                                  (0x47UL)  /* TRGM0 DMA request 7 (contains DMA requests for PWM0, PWM1, PWM2, PWM3, QEI0, QEI1, SEI0, SEI1) */
#define HPM_DMA_SRC_ACMP0_0                                (0x48UL)  /* ACMP0 Channel 0 */
#define HPM_DMA_SRC_ACMP0_1                                (0x49UL)  /* ACMP0 Channel 1 */
#define HPM_DMA_SRC_ACMP1_0                                (0x4AUL)  /* ACMP1 Channel 0 */
#define HPM_DMA_SRC_ACMP1_1                                (0x4BUL)  /* ACMP1 Channel 1 */
#define HPM_DMA_SRC_ACMP2_0                                (0x4CUL)  /* ACMP2 Channel 0 */
#define HPM_DMA_SRC_ACMP2_1                                (0x4DUL)  /* ACMP2 Channel 1 */
#define HPM_DMA_SRC_ACMP3_0                                (0x4EUL)  /* ACMP3 Channel 0 */
#define HPM_DMA_SRC_ACMP3_1                                (0x4FUL)  /* ACMP3 Channel 1 */
#define HPM_DMA_SRC_DAC0                                   (0x50UL)  /* DAC0 */
#define HPM_DMA_SRC_DAC1                                   (0x51UL)  /* DAC1 */
#define HPM_DMA_SRC_XPI0_RX                                (0x52UL)  /* XPI0 Receive */
#define HPM_DMA_SRC_XPI0_TX                                (0x53UL)  /* XPI0 Transmit */



#endif /* HPM_DMAMUX_SRC_H */
