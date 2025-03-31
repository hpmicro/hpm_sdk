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
#define HPM_DMA_SRC_MOT0_0                                 (0x18UL)  /* TRGM0 DMA request 0  (contains DMA requests for PWMT0, QEI0, HALL0) */
#define HPM_DMA_SRC_MOT0_1                                 (0x19UL)  /* TRGM0 DMA request 1  (contains DMA requests for PWMT0, QEI0, HALL0) */
#define HPM_DMA_SRC_MOT0_2                                 (0x1AUL)  /* TRGM0 DMA request 2  (contains DMA requests for PWMT0, QEI0, HALL0) */
#define HPM_DMA_SRC_MOT0_3                                 (0x1BUL)  /* TRGM0 DMA request 3  (contains DMA requests for PWMT0, QEI0, HALL0) */
#define HPM_DMA_SRC_MOT1_0                                 (0x1CUL)  /* TRGM1 DMA request 0  (contains DMA requests for PWMT1, QEI1, HALL1) */
#define HPM_DMA_SRC_MOT1_1                                 (0x1DUL)  /* TRGM1 DMA request 1  (contains DMA requests for PWMT1, QEI1, HALL1) */
#define HPM_DMA_SRC_MOT1_2                                 (0x1EUL)  /* TRGM1 DMA request 2  (contains DMA requests for PWMT1, QEI1, HALL1) */
#define HPM_DMA_SRC_MOT1_3                                 (0x1FUL)  /* TRGM1 DMA request 3  (contains DMA requests for PWMT1, QEI1, HALL1) */
#define HPM_DMA_SRC_MOT2_0                                 (0x20UL)  /* TRGM2 DMA request 0  (contains DMA requests for PWMT2, QEI2, HALL2) */
#define HPM_DMA_SRC_MOT2_1                                 (0x21UL)  /* TRGM2 DMA request 1  (contains DMA requests for PWMT2, QEI2, HALL2) */
#define HPM_DMA_SRC_MOT2_2                                 (0x22UL)  /* TRGM2 DMA request 2  (contains DMA requests for PWMT2, QEI2, HALL2) */
#define HPM_DMA_SRC_MOT2_3                                 (0x23UL)  /* TRGM2 DMA request 3  (contains DMA requests for PWMT2, QEI2, HALL2) */
#define HPM_DMA_SRC_MOT3_0                                 (0x24UL)  /* TRGM3 DMA request 0  (contains DMA requests for PWMT3, QEI3, HALL3) */
#define HPM_DMA_SRC_MOT3_1                                 (0x25UL)  /* TRGM3 DMA request 1  (contains DMA requests for PWMT3, QEI3, HALL3) */
#define HPM_DMA_SRC_MOT3_2                                 (0x26UL)  /* TRGM3 DMA request 2  (contains DMA requests for PWMT3, QEI3, HALL3) */
#define HPM_DMA_SRC_MOT3_3                                 (0x27UL)  /* TRGM3 DMA request 3  (contains DMA requests for PWMT3, QEI3, HALL3) */
#define HPM_DMA_SRC_GPTMR0_0                               (0x28UL)  /* GPTMR0 Channel 0 */
#define HPM_DMA_SRC_GPTMR0_1                               (0x29UL)  /* GPTMR0 Channel 1 */
#define HPM_DMA_SRC_GPTMR0_2                               (0x2AUL)  /* GPTMR0 Channel 2 */
#define HPM_DMA_SRC_GPTMR0_3                               (0x2BUL)  /* GPTMR0 Channel 3 */
#define HPM_DMA_SRC_GPTMR1_0                               (0x2CUL)  /* GPTMR1 Channel 0 */
#define HPM_DMA_SRC_GPTMR1_1                               (0x2DUL)  /* GPTMR1 Channel 1 */
#define HPM_DMA_SRC_GPTMR1_2                               (0x2EUL)  /* GPTMR1 Channel 2 */
#define HPM_DMA_SRC_GPTMR1_3                               (0x2FUL)  /* GPTMR1 Channel 3 */
#define HPM_DMA_SRC_GPTMR2_0                               (0x30UL)  /* GPTMR2 channel 0 */
#define HPM_DMA_SRC_GPTMR2_1                               (0x31UL)  /* GPTMR2 Channel 1 */
#define HPM_DMA_SRC_GPTMR2_2                               (0x32UL)  /* GPTMR2 Channel 2 */
#define HPM_DMA_SRC_GPTMR2_3                               (0x33UL)  /* GPTMR2 Channel 3 */
#define HPM_DMA_SRC_GPTMR3_0                               (0x34UL)  /* GPTMR3 channel 0 */
#define HPM_DMA_SRC_GPTMR3_1                               (0x35UL)  /* GPTMR3 Channel 1 */
#define HPM_DMA_SRC_GPTMR3_2                               (0x36UL)  /* GPTMR3 Channel 2 */
#define HPM_DMA_SRC_GPTMR3_3                               (0x37UL)  /* GPTMR3 Channel 3 */
#define HPM_DMA_SRC_I2C0                                   (0x38UL)  /* I2C0 */
#define HPM_DMA_SRC_I2C1                                   (0x39UL)  /* I2C1 */
#define HPM_DMA_SRC_I2C2                                   (0x3AUL)  /* I2C2 */
#define HPM_DMA_SRC_I2C3                                   (0x3BUL)  /* I2C3 */
#define HPM_DMA_SRC_XPI0_RX                                (0x3CUL)  /* XPI0 Receive */
#define HPM_DMA_SRC_XPI0_TX                                (0x3DUL)  /* XPI0 Transmit */
#define HPM_DMA_SRC_DAC0                                   (0x3EUL)  /* DAC0 */
#define HPM_DMA_SRC_DAC1                                   (0x3FUL)  /* DAC1 */
#define HPM_DMA_SRC_ACMP_0                                 (0x40UL)  /* ACMP0 Channel 0 */
#define HPM_DMA_SRC_ACMP_1                                 (0x41UL)  /* ACMP0 Channel 1 */
#define HPM_DMA_SRC_ACMP_2                                 (0x42UL)  /* ACMP0 Channel 2 */
#define HPM_DMA_SRC_ACMP_3                                 (0x43UL)  /* ACMP0 Channel 3 */
#define HPM_DMA_SRC_MCAN0                                  (0x44UL)  /* CAN0 */
#define HPM_DMA_SRC_MCAN1                                  (0x45UL)  /* CAN1 */
#define HPM_DMA_SRC_MCAN2                                  (0x46UL)  /* CAN2 */
#define HPM_DMA_SRC_MCAN3                                  (0x47UL)  /* CAN3 */



#endif /* HPM_DMAMUX_SRC_H */
