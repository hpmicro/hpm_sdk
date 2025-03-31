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
#define HPM_DMA_SRC_UART0_RX                               (0x8UL)   /* UART0 Receive */
#define HPM_DMA_SRC_UART0_TX                               (0x9UL)   /* UART0 Transmit */
#define HPM_DMA_SRC_UART1_RX                               (0xAUL)   /* UART1 Receive */
#define HPM_DMA_SRC_UART1_TX                               (0xBUL)   /* UART1 Transmit */
#define HPM_DMA_SRC_UART2_RX                               (0xCUL)   /* UART2 Receive */
#define HPM_DMA_SRC_UART2_TX                               (0xDUL)   /* UART2 Transmit */
#define HPM_DMA_SRC_UART3_RX                               (0xEUL)   /* UART3 Receive */
#define HPM_DMA_SRC_UART3_TX                               (0xFUL)   /* UART3 Transmit */
#define HPM_DMA_SRC_I2C0                                   (0x18UL)  /* I2C0 */
#define HPM_DMA_SRC_I2C1                                   (0x19UL)  /* I2C1 */
#define HPM_DMA_SRC_I2C2                                   (0x1AUL)  /* I2C2 */
#define HPM_DMA_SRC_MCAN0                                  (0x1CUL)  /* CAN0 */
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
#define HPM_DMA_SRC_DAC0                                   (0x50UL)  /* DAC0 */
#define HPM_DMA_SRC_XPI0_RX                                (0x52UL)  /* XPI0 Receive */
#define HPM_DMA_SRC_XPI0_TX                                (0x53UL)  /* XPI0 Transmit */



#endif /* HPM_DMAMUX_SRC_H */
