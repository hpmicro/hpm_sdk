/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_DMAMUX_SRC_H
#define HPM_DMAMUX_SRC_H

/* dma mux definitions */
#define HPM_DMA_SRC_GPTMR0_0                               (0x0UL)   /* GPTMR0 Channel 0 */
#define HPM_DMA_SRC_GPTMR0_1                               (0x1UL)   /* GPTMR0 Channel 1 */
#define HPM_DMA_SRC_GPTMR0_2                               (0x2UL)   /* GPTMR0 Channel 2 */
#define HPM_DMA_SRC_GPTMR0_3                               (0x3UL)   /* GPTMR0 Channel 3 */
#define HPM_DMA_SRC_GPTMR1_0                               (0x4UL)   /* GPTMR1 Channel 0 */
#define HPM_DMA_SRC_GPTMR1_1                               (0x5UL)   /* GPTMR1 Channel 1 */
#define HPM_DMA_SRC_GPTMR1_2                               (0x6UL)   /* GPTMR1 Channel 2 */
#define HPM_DMA_SRC_GPTMR1_3                               (0x7UL)   /* GPTMR1 Channel 3 */
#define HPM_DMA_SRC_GPTMR2_0                               (0x8UL)   /* GPTMR2 channel 0 */
#define HPM_DMA_SRC_GPTMR2_1                               (0x9UL)   /* GPTMR2 Channel 1 */
#define HPM_DMA_SRC_GPTMR2_2                               (0xAUL)   /* GPTMR2 Channel 2 */
#define HPM_DMA_SRC_GPTMR2_3                               (0xBUL)   /* GPTMR2 Channel 3 */
#define HPM_DMA_SRC_GPTMR3_0                               (0xCUL)   /* GPTMR3 channel 0 */
#define HPM_DMA_SRC_GPTMR3_1                               (0xDUL)   /* GPTMR3 Channel 1 */
#define HPM_DMA_SRC_GPTMR3_2                               (0xEUL)   /* GPTMR3 Channel 2 */
#define HPM_DMA_SRC_GPTMR3_3                               (0xFUL)   /* GPTMR3 Channel 3 */
#define HPM_DMA_SRC_UART0_RX                               (0x14UL)  /* UART0 Receive */
#define HPM_DMA_SRC_UART0_TX                               (0x15UL)  /* UART0 Transmit */
#define HPM_DMA_SRC_UART1_RX                               (0x16UL)  /* UART1 Receive */
#define HPM_DMA_SRC_UART1_TX                               (0x17UL)  /* UART1 Transmit */
#define HPM_DMA_SRC_UART2_RX                               (0x18UL)  /* UART2 Receive */
#define HPM_DMA_SRC_UART2_TX                               (0x19UL)  /* UART2 Transmit */
#define HPM_DMA_SRC_UART3_RX                               (0x1AUL)  /* UART3 Receive */
#define HPM_DMA_SRC_UART3_TX                               (0x1BUL)  /* UART3 Transmit */
#define HPM_DMA_SRC_UART4_RX                               (0x1CUL)  /* UART4 Receive */
#define HPM_DMA_SRC_UART4_TX                               (0x1DUL)  /* UART4 Transmit */
#define HPM_DMA_SRC_UART5_RX                               (0x1EUL)  /* UART5 Receive */
#define HPM_DMA_SRC_UART5_TX                               (0x1FUL)  /* UART5 Transmit */
#define HPM_DMA_SRC_UART6_RX                               (0x20UL)  /* UART6 Receive */
#define HPM_DMA_SRC_UART6_TX                               (0x21UL)  /* UART6 Transmit */
#define HPM_DMA_SRC_UART7_RX                               (0x22UL)  /* UART7 Receive */
#define HPM_DMA_SRC_UART7_TX                               (0x23UL)  /* UART7 Transmit */
#define HPM_DMA_SRC_I2C0                                   (0x24UL)  /* I2C0 */
#define HPM_DMA_SRC_I2C1                                   (0x25UL)  /* I2C1 */
#define HPM_DMA_SRC_I2C2                                   (0x26UL)  /* I2C2 */
#define HPM_DMA_SRC_I2C3                                   (0x27UL)  /* I2C3 */
#define HPM_DMA_SRC_SPI0_RX                                (0x28UL)  /* SPI0 Receive */
#define HPM_DMA_SRC_SPI0_TX                                (0x29UL)  /* SPI0 Transmit */
#define HPM_DMA_SRC_SPI1_RX                                (0x2AUL)  /* SPI1 Receive */
#define HPM_DMA_SRC_SPI1_TX                                (0x2BUL)  /* SPI1 Transmit */
#define HPM_DMA_SRC_SPI2_RX                                (0x2CUL)  /* SPI2 Receive */
#define HPM_DMA_SRC_SPI2_TX                                (0x2DUL)  /* SPI2 Transmit */
#define HPM_DMA_SRC_SPI3_RX                                (0x2EUL)  /* SPI3 Receive */
#define HPM_DMA_SRC_SPI3_TX                                (0x2FUL)  /* SPI3 Transmit */
#define HPM_DMA_SRC_MCAN0                                  (0x30UL)  /* CAN0 */
#define HPM_DMA_SRC_MCAN1                                  (0x31UL)  /* CAN1 */
#define HPM_DMA_SRC_MCAN2                                  (0x32UL)  /* CAN2 */
#define HPM_DMA_SRC_MCAN3                                  (0x33UL)  /* CAN3 */
#define HPM_DMA_SRC_MOT_0                                  (0x34UL)  /* TRGM0 DMA request 0 (contains DMA requests for PWM0, PWM1, QEI0, QEI1, MMC0, MMC1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_1                                  (0x35UL)  /* TRGM0 DMA request 1 (contains DMA requests for PWM0, PWM1, QEI0, QEI1, MMC0, MMC1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_2                                  (0x36UL)  /* TRGM0 DMA request 2 (contains DMA requests for PWM0, PWM1, QEI0, QEI1, MMC0, MMC1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_3                                  (0x37UL)  /* TRGM0 DMA request 3 (contains DMA requests for PWM0, PWM1, QEI0, QEI1, MMC0, MMC1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_4                                  (0x38UL)  /* TRGM0 DMA request 4 (contains DMA requests for PWM0, PWM1, QEI0, QEI1, MMC0, MMC1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_5                                  (0x39UL)  /* TRGM0 DMA request 5 (contains DMA requests for PWM0, PWM1, QEI0, QEI1, MMC0, MMC1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_6                                  (0x3AUL)  /* TRGM0 DMA request 6 (contains DMA requests for PWM0, PWM1, QEI0, QEI1, MMC0, MMC1, SEI0, SEI1) */
#define HPM_DMA_SRC_MOT_7                                  (0x3BUL)  /* TRGM0 DMA request 7 (contains DMA requests for PWM0, PWM1, QEI0, QEI1, MMC0, MMC1, SEI0, SEI1) */
#define HPM_DMA_SRC_XPI0_RX                                (0x3CUL)  /* XPI0 Receive */
#define HPM_DMA_SRC_XPI0_TX                                (0x3DUL)  /* XPI0 Transmit */
#define HPM_DMA_SRC_DAC0                                   (0x3EUL)  /* DAC0 */
#define HPM_DMA_SRC_DAC1                                   (0x3FUL)  /* DAC1 */
#define HPM_DMA_SRC_ACMP0                                  (0x40UL)  /* ACMP0 */
#define HPM_DMA_SRC_ACMP1                                  (0x41UL)  /* ACMP1 */



#endif /* HPM_DMAMUX_SRC_H */
