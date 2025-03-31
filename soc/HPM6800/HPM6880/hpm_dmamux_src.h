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
#define HPM_DMA_SRC_MCAN0                                  (0x18UL)  /* CAN0 */
#define HPM_DMA_SRC_MCAN1                                  (0x19UL)  /* CAN1 */
#define HPM_DMA_SRC_MCAN2                                  (0x1AUL)  /* CAN2 */
#define HPM_DMA_SRC_MCAN3                                  (0x1BUL)  /* CAN3 */
#define HPM_DMA_SRC_MCAN4                                  (0x1CUL)  /* CAN4 */
#define HPM_DMA_SRC_MCAN5                                  (0x1DUL)  /* CAN5 */
#define HPM_DMA_SRC_MCAN6                                  (0x1EUL)  /* CAN6 */
#define HPM_DMA_SRC_MCAN7                                  (0x1FUL)  /* CAN7 */
#define HPM_DMA_SRC_I2S0_RX                                (0x20UL)  /* I2S0 Receive */
#define HPM_DMA_SRC_I2S0_TX                                (0x21UL)  /* I2S0 Transmit */
#define HPM_DMA_SRC_I2S1_RX                                (0x22UL)  /* I2S1 Receive */
#define HPM_DMA_SRC_I2S1_TX                                (0x23UL)  /* I2S1 Transmit */
#define HPM_DMA_SRC_I2S2_RX                                (0x24UL)  /* I2S2 Receive */
#define HPM_DMA_SRC_I2S2_TX                                (0x25UL)  /* I2S2 Transmit */
#define HPM_DMA_SRC_I2S3_RX                                (0x26UL)  /* I2S3 Receive */
#define HPM_DMA_SRC_I2S3_TX                                (0x27UL)  /* I2S3 Transmit */
#define HPM_DMA_SRC_GPTMR0_0                               (0x28UL)  /* GPTMR0 channel0 */
#define HPM_DMA_SRC_GPTMR0_1                               (0x29UL)  /* GPTMR0 channel1 */
#define HPM_DMA_SRC_GPTMR0_2                               (0x2AUL)  /* GPTMR0 channel2 */
#define HPM_DMA_SRC_GPTMR0_3                               (0x2BUL)  /* GPTMR0 channel3 */
#define HPM_DMA_SRC_GPTMR1_0                               (0x2CUL)  /* GPTMR1 channel0 */
#define HPM_DMA_SRC_GPTMR1_1                               (0x2DUL)  /* GPTMR1 channel1 */
#define HPM_DMA_SRC_GPTMR1_2                               (0x2EUL)  /* GPTMR1 channel2 */
#define HPM_DMA_SRC_GPTMR1_3                               (0x2FUL)  /* GPTMR1 channel3 */
#define HPM_DMA_SRC_GPTMR2_0                               (0x30UL)  /* GPTMR2 channel0 */
#define HPM_DMA_SRC_GPTMR2_1                               (0x31UL)  /* GPTMR2 channel1 */
#define HPM_DMA_SRC_GPTMR2_2                               (0x32UL)  /* GPTMR2 channel2 */
#define HPM_DMA_SRC_GPTMR2_3                               (0x33UL)  /* GPTMR2 channel3 */
#define HPM_DMA_SRC_GPTMR3_0                               (0x34UL)  /* GPTMR3 channel0 */
#define HPM_DMA_SRC_GPTMR3_1                               (0x35UL)  /* GPTMR3 channel1 */
#define HPM_DMA_SRC_GPTMR3_2                               (0x36UL)  /* GPTMR3 channel2 */
#define HPM_DMA_SRC_GPTMR3_3                               (0x37UL)  /* GPTMR3 channel3 */
#define HPM_DMA_SRC_GPTMR4_0                               (0x38UL)  /* GPTMR4 channel0 */
#define HPM_DMA_SRC_GPTMR4_1                               (0x39UL)  /* GPTMR4 channel1 */
#define HPM_DMA_SRC_GPTMR4_2                               (0x3AUL)  /* GPTMR4 channel2 */
#define HPM_DMA_SRC_GPTMR4_3                               (0x3BUL)  /* GPTMR4 channel3 */
#define HPM_DMA_SRC_GPTMR5_0                               (0x3CUL)  /* GPTMR5 channel0 */
#define HPM_DMA_SRC_GPTMR5_1                               (0x3DUL)  /* GPTMR5 channel1 */
#define HPM_DMA_SRC_GPTMR5_2                               (0x3EUL)  /* GPTMR5 channel2 */
#define HPM_DMA_SRC_GPTMR5_3                               (0x3FUL)  /* GPTMR5 channel3 */
#define HPM_DMA_SRC_GPTMR6_0                               (0x40UL)  /* GPTMR6 channel0 */
#define HPM_DMA_SRC_GPTMR6_1                               (0x41UL)  /* GPTMR6 channel1 */
#define HPM_DMA_SRC_GPTMR6_2                               (0x42UL)  /* GPTMR6 channel2 */
#define HPM_DMA_SRC_GPTMR6_3                               (0x43UL)  /* GPTMR6 channel3 */
#define HPM_DMA_SRC_GPTMR7_0                               (0x44UL)  /* GPTMR7 channel0 */
#define HPM_DMA_SRC_GPTMR7_1                               (0x45UL)  /* GPTMR7 channel1 */
#define HPM_DMA_SRC_GPTMR7_2                               (0x46UL)  /* GPTMR7 channel2 */
#define HPM_DMA_SRC_GPTMR7_3                               (0x47UL)  /* GPTMR7 channel3 */
#define HPM_DMA_SRC_I2C0                                   (0x48UL)  /* I2C0 */
#define HPM_DMA_SRC_I2C1                                   (0x49UL)  /* I2C1 */
#define HPM_DMA_SRC_I2C2                                   (0x4AUL)  /* I2C2 */
#define HPM_DMA_SRC_I2C3                                   (0x4BUL)  /* I2C3 */
#define HPM_DMA_SRC_XPI0_RX                                (0x4CUL)  /* XPI0 Receive */
#define HPM_DMA_SRC_XPI0_TX                                (0x4DUL)  /* XPI0 Transmit */



#endif /* HPM_DMAMUX_SRC_H */
