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
#define HPM_DMA_SRC_SPI4_RX                                (0x1CUL)  /* SPI4 Receive */
#define HPM_DMA_SRC_SPI4_TX                                (0x1DUL)  /* SPI4 Transmit */
#define HPM_DMA_SRC_SPI5_RX                                (0x1EUL)  /* SPI5 Receive */
#define HPM_DMA_SRC_SPI5_TX                                (0x1FUL)  /* SPI5 Transmit */
#define HPM_DMA_SRC_SPI6_RX                                (0x20UL)  /* SPI6 Receive */
#define HPM_DMA_SRC_SPI6_TX                                (0x21UL)  /* SPI6 Transmit */
#define HPM_DMA_SRC_SPI7_RX                                (0x22UL)  /* SPI7 Receive */
#define HPM_DMA_SRC_SPI7_TX                                (0x23UL)  /* SPI7 Transmit */
#define HPM_DMA_SRC_UART8_RX                               (0x24UL)  /* UART8 Receive */
#define HPM_DMA_SRC_UART8_TX                               (0x25UL)  /* UART8 Transmit */
#define HPM_DMA_SRC_UART9_RX                               (0x26UL)  /* UART9 Receive */
#define HPM_DMA_SRC_UART9_TX                               (0x27UL)  /* UART9 Transmit */
#define HPM_DMA_SRC_UART10_RX                              (0x28UL)  /* UART10 Receive */
#define HPM_DMA_SRC_UART10_TX                              (0x29UL)  /* UART10 Transmit */
#define HPM_DMA_SRC_UART11_RX                              (0x2AUL)  /* UART11 Receive */
#define HPM_DMA_SRC_UART11_TX                              (0x2BUL)  /* UART11 Transmit */
#define HPM_DMA_SRC_UART12_RX                              (0x2CUL)  /* UART12 Receive */
#define HPM_DMA_SRC_UART12_TX                              (0x2DUL)  /* UART12 Transmit */
#define HPM_DMA_SRC_UART13_RX                              (0x2EUL)  /* UART13 Receive */
#define HPM_DMA_SRC_UART13_TX                              (0x2FUL)  /* UART13 Transmit */
#define HPM_DMA_SRC_UART14_RX                              (0x30UL)  /* UART14 Receive */
#define HPM_DMA_SRC_UART14_TX                              (0x31UL)  /* UART14 Transmit */
#define HPM_DMA_SRC_UART15_RX                              (0x32UL)  /* UART15 Receive */
#define HPM_DMA_SRC_UART15_TX                              (0x33UL)  /* UART15 Transmit */
#define HPM_DMA_SRC_I2C4                                   (0x34UL)  /* I2C4 */
#define HPM_DMA_SRC_I2C5                                   (0x35UL)  /* I2C5 */
#define HPM_DMA_SRC_I2C6                                   (0x36UL)  /* I2C6 */
#define HPM_DMA_SRC_I2C7                                   (0x37UL)  /* I2C7 */
#define HPM_DMA_SRC_MCAN0                                  (0x38UL)  /* CAN0 */
#define HPM_DMA_SRC_MCAN1                                  (0x39UL)  /* CAN1 */
#define HPM_DMA_SRC_MCAN2                                  (0x3AUL)  /* CAN2 */
#define HPM_DMA_SRC_MCAN3                                  (0x3BUL)  /* CAN3 */
#define HPM_DMA_SRC_MCAN4                                  (0x3CUL)  /* CAN4 */
#define HPM_DMA_SRC_MCAN5                                  (0x3DUL)  /* CAN5 */
#define HPM_DMA_SRC_MCAN6                                  (0x3EUL)  /* CAN6 */
#define HPM_DMA_SRC_MCAN7                                  (0x3FUL)  /* CAN7 */
#define HPM_DMA_SRC_I2S0_RX                                (0x40UL)  /* I2S0 Receive */
#define HPM_DMA_SRC_I2S0_TX                                (0x41UL)  /* I2S0 Transmit */
#define HPM_DMA_SRC_I2S1_RX                                (0x42UL)  /* I2S1 Receive */
#define HPM_DMA_SRC_I2S1_TX                                (0x43UL)  /* I2S1 Transmit */
#define HPM_DMA_SRC_GPTMR0_0                               (0x44UL)  /* GPTMR0 channel 0 */
#define HPM_DMA_SRC_GPTMR0_1                               (0x45UL)  /* GPTMR0 channel 1 */
#define HPM_DMA_SRC_GPTMR0_2                               (0x46UL)  /* GPTMR0 channel 2 */
#define HPM_DMA_SRC_GPTMR0_3                               (0x47UL)  /* GPTMR0 channel 3 */
#define HPM_DMA_SRC_GPTMR1_0                               (0x48UL)  /* GPTMR1 channel 0 */
#define HPM_DMA_SRC_GPTMR1_1                               (0x49UL)  /* GPTMR1 channel 1 */
#define HPM_DMA_SRC_GPTMR1_2                               (0x4AUL)  /* GPTMR1 channel 2 */
#define HPM_DMA_SRC_GPTMR1_3                               (0x4BUL)  /* GPTMR1 channel 3 */
#define HPM_DMA_SRC_GPTMR2_0                               (0x4CUL)  /* GPTMR2 channel 0 */
#define HPM_DMA_SRC_GPTMR2_1                               (0x4DUL)  /* GPTMR2 channel 1 */
#define HPM_DMA_SRC_GPTMR2_2                               (0x4EUL)  /* GPTMR2 channel 2 */
#define HPM_DMA_SRC_GPTMR2_3                               (0x4FUL)  /* GPTMR2 channel 3 */
#define HPM_DMA_SRC_GPTMR3_0                               (0x50UL)  /* GPTMR3 channel 0 */
#define HPM_DMA_SRC_GPTMR3_1                               (0x51UL)  /* GPTMR3 channel 1 */
#define HPM_DMA_SRC_GPTMR3_2                               (0x52UL)  /* GPTMR3 channel 2 */
#define HPM_DMA_SRC_GPTMR3_3                               (0x53UL)  /* GPTMR3 channel 3 */
#define HPM_DMA_SRC_GPTMR4_0                               (0x54UL)  /* GPTMR4 channel 0 */
#define HPM_DMA_SRC_GPTMR4_1                               (0x55UL)  /* GPTMR4 channel 1 */
#define HPM_DMA_SRC_GPTMR4_2                               (0x56UL)  /* GPTMR4 channel 2 */
#define HPM_DMA_SRC_GPTMR4_3                               (0x57UL)  /* GPTMR4 channel 3 */
#define HPM_DMA_SRC_GPTMR5_0                               (0x58UL)  /* GPTMR5 channel 0 */
#define HPM_DMA_SRC_GPTMR5_1                               (0x59UL)  /* GPTMR5 channel 1 */
#define HPM_DMA_SRC_GPTMR5_2                               (0x5AUL)  /* GPTMR5 channel 2 */
#define HPM_DMA_SRC_GPTMR5_3                               (0x5BUL)  /* GPTMR5 channel 3 */
#define HPM_DMA_SRC_GPTMR6_0                               (0x5CUL)  /* GPTMR6 channel 0 */
#define HPM_DMA_SRC_GPTMR6_1                               (0x5DUL)  /* GPTMR6 channel 1 */
#define HPM_DMA_SRC_GPTMR6_2                               (0x5EUL)  /* GPTMR6 channel 2 */
#define HPM_DMA_SRC_GPTMR6_3                               (0x5FUL)  /* GPTMR6 channel 3 */
#define HPM_DMA_SRC_GPTMR7_0                               (0x60UL)  /* GPTMR7 channel 0 */
#define HPM_DMA_SRC_GPTMR7_1                               (0x61UL)  /* GPTMR7 channel 1 */
#define HPM_DMA_SRC_GPTMR7_2                               (0x62UL)  /* GPTMR7 channel 2 */
#define HPM_DMA_SRC_GPTMR7_3                               (0x63UL)  /* GPTMR7 channel 3 */
#define HPM_DMA_SRC_MOT_0                                  (0x64UL)  /* TRGM DMA0(include pwm0~3,qei0~3,sei0~3,trgmux0~1) */
#define HPM_DMA_SRC_MOT_1                                  (0x65UL)  /* TRGM DMA1(include pwm0~3,qei0~3,sei0~3,trgmux0~1) */
#define HPM_DMA_SRC_MOT_2                                  (0x66UL)  /* TRGM DMA2(include pwm0~3,qei0~3,sei0~3,trgmux0~1) */
#define HPM_DMA_SRC_MOT_3                                  (0x67UL)  /* TRGM DMA3(include pwm0~3,qei0~3,sei0~3,trgmux0~1) */
#define HPM_DMA_SRC_MOT_4                                  (0x68UL)  /* TRGM DMA4(include pwm0~3,qei0~3,sei0~3,trgmux0~1) */
#define HPM_DMA_SRC_MOT_5                                  (0x69UL)  /* TRGM DMA5(include pwm0~3,qei0~3,sei0~3,trgmux0~1) */
#define HPM_DMA_SRC_MOT_6                                  (0x6AUL)  /* TRGM DMA6(include pwm0~3,qei0~3,sei0~3,trgmux0~1) */
#define HPM_DMA_SRC_MOT_7                                  (0x6BUL)  /* TRGM DMA7(include pwm0~3,qei0~3,sei0~3,trgmux0~1) */
#define HPM_DMA_SRC_ACMP0_0                                (0x6CUL)  /* ACMP0 channel0 */
#define HPM_DMA_SRC_ACMP0_1                                (0x6DUL)  /* ACMP0 channel1 */
#define HPM_DMA_SRC_ACMP1_0                                (0x6EUL)  /* ACMP1 channel0 */
#define HPM_DMA_SRC_ACMP1_1                                (0x6FUL)  /* ACMP1 channel1 */
#define HPM_DMA_SRC_ACMP2_0                                (0x70UL)  /* ACMP2 channel0 */
#define HPM_DMA_SRC_ACMP2_1                                (0x71UL)  /* ACMP2 channel1 */
#define HPM_DMA_SRC_ACMP3_0                                (0x72UL)  /* ACMP3 channel0 */
#define HPM_DMA_SRC_ACMP3_1                                (0x73UL)  /* ACMP3 channel1 */
#define HPM_DMA_SRC_XPI0_RX                                (0x74UL)  /* XPI0 Receive */
#define HPM_DMA_SRC_XPI0_TX                                (0x75UL)  /* XPI0 Transmit */
#define HPM_DMA_SRC_ESC_SYNC0                              (0x76UL)  /* Ethercat sync0 */
#define HPM_DMA_SRC_ESC_SYNC1                              (0x77UL)  /* Ethercat sync1 */



#endif /* HPM_DMAMUX_SRC_H */
