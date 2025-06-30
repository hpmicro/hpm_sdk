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
#define HPM_DMA_SRC_GPTMR0_0                               (0x20UL)  /* GPTMR0 channel 0 */
#define HPM_DMA_SRC_GPTMR0_1                               (0x21UL)  /* GPTMR0 channel 1 */
#define HPM_DMA_SRC_GPTMR0_2                               (0x22UL)  /* GPTMR0 channel 2 */
#define HPM_DMA_SRC_GPTMR0_3                               (0x23UL)  /* GPTMR0 channel 3 */
#define HPM_DMA_SRC_GPTMR1_0                               (0x24UL)  /* GPTMR1 channel 0 */
#define HPM_DMA_SRC_GPTMR1_1                               (0x25UL)  /* GPTMR1 channel 1 */
#define HPM_DMA_SRC_GPTMR1_2                               (0x26UL)  /* GPTMR1 channel 2 */
#define HPM_DMA_SRC_GPTMR1_3                               (0x27UL)  /* GPTMR1 channel 3 */
#define HPM_DMA_SRC_GPTMR2_0                               (0x28UL)  /* GPTMR2 channel 0 */
#define HPM_DMA_SRC_GPTMR2_1                               (0x29UL)  /* GPTMR2 channel 1 */
#define HPM_DMA_SRC_GPTMR2_2                               (0x2AUL)  /* GPTMR2 channel 2 */
#define HPM_DMA_SRC_GPTMR2_3                               (0x2BUL)  /* GPTMR2 channel 3 */
#define HPM_DMA_SRC_GPTMR3_0                               (0x2CUL)  /* GPTMR3 channel 0 */
#define HPM_DMA_SRC_GPTMR3_1                               (0x2DUL)  /* GPTMR3 channel 1 */
#define HPM_DMA_SRC_GPTMR3_2                               (0x2EUL)  /* GPTMR3 channel 2 */
#define HPM_DMA_SRC_GPTMR3_3                               (0x2FUL)  /* GPTMR3 channel 3 */
#define HPM_DMA_SRC_OWR0_RX                                (0x30UL)  /* Owire0 Receive */
#define HPM_DMA_SRC_OWR0_TX                                (0x31UL)  /* Owire0 Transmit */
#define HPM_DMA_SRC_OWR1_RX                                (0x32UL)  /* Owire1 Receive */
#define HPM_DMA_SRC_OWR1_TX                                (0x33UL)  /* Owire1 Transmit */
#define HPM_DMA_SRC_MOT_0                                  (0x34UL)  /* TRGM DMA0(include pwm0~1,qei0~1,trgm0~1) */
#define HPM_DMA_SRC_MOT_1                                  (0x35UL)  /* TRGM DMA1(include pwm0~1,qei0~1,trgm0~1) */
#define HPM_DMA_SRC_MOT_2                                  (0x36UL)  /* TRGM DMA2(include pwm0~1,qei0~1,trgm0~1) */
#define HPM_DMA_SRC_MOT_3                                  (0x37UL)  /* TRGM DMA3(include pwm0~1,qei0~1,trgm0~1) */
#define HPM_DMA_SRC_MOT_4                                  (0x38UL)  /* TRGM DMA4(include pwm0~1,qei0~1,trgm0~1) */
#define HPM_DMA_SRC_MOT_5                                  (0x39UL)  /* TRGM DMA5(include pwm0~1,qei0~1,trgm0~1) */
#define HPM_DMA_SRC_MOT_6                                  (0x3AUL)  /* TRGM DMA6(include pwm0~1,qei0~1,trgm0~1) */
#define HPM_DMA_SRC_MOT_7                                  (0x3BUL)  /* TRGM DMA7(include pwm0~1,qei0~1,trgm0~1) */
#define HPM_DMA_SRC_ACMP0_0                                (0x3CUL)  /* ACMP0 */
#define HPM_DMA_SRC_ACMP0_1                                (0x3DUL)  /* ACMP1 */
#define HPM_DMA_SRC_XPI0_RX                                (0x3EUL)  /* XPI0 Receive */
#define HPM_DMA_SRC_XPI0_TX                                (0x3FUL)  /* XPI0 Transmit */
#define HPM_DMA_SRC_ESC_SYNC0                              (0x40UL)  /* Ethercat sync0 */
#define HPM_DMA_SRC_ESC_SYNC1                              (0x41UL)  /* Ethercat sync1 */



#endif /* HPM_DMAMUX_SRC_H */
