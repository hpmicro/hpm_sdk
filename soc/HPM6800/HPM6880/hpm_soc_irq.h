/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_SOC_IRQ_H
#define HPM_SOC_IRQ_H


/* List of external IRQs */
#define IRQn_GPIO0_A                   1       /* GPIO0_A IRQ */
#define IRQn_GPIO0_B                   2       /* GPIO0_B IRQ */
#define IRQn_GPIO0_C                   3       /* GPIO0_C IRQ */
#define IRQn_GPIO0_D                   4       /* GPIO0_D IRQ */
#define IRQn_GPIO0_E                   5       /* GPIO0_E IRQ */
#define IRQn_GPIO0_F                   6       /* GPIO0_F IRQ */
#define IRQn_GPIO0_X                   7       /* GPIO0_X IRQ */
#define IRQn_GPIO0_Y                   8       /* GPIO0_Y IRQ */
#define IRQn_GPIO0_Z                   9       /* GPIO0_Z IRQ */
#define IRQn_MCAN0                     10      /* MCAN0 IRQ */
#define IRQn_MCAN1                     11      /* MCAN1 IRQ */
#define IRQn_MCAN2                     12      /* MCAN2 IRQ */
#define IRQn_MCAN3                     13      /* MCAN3 IRQ */
#define IRQn_MCAN4                     14      /* MCAN4 IRQ */
#define IRQn_MCAN5                     15      /* MCAN5 IRQ */
#define IRQn_MCAN6                     16      /* MCAN6 IRQ */
#define IRQn_MCAN7                     17      /* MCAN7 IRQ */
#define IRQn_PTPC                      18      /* PTPC IRQ */
#define IRQn_UART0                     27      /* UART0 IRQ */
#define IRQn_UART1                     28      /* UART1 IRQ */
#define IRQn_UART2                     29      /* UART2 IRQ */
#define IRQn_UART3                     30      /* UART3 IRQ */
#define IRQn_UART4                     31      /* UART4 IRQ */
#define IRQn_UART5                     32      /* UART5 IRQ */
#define IRQn_UART6                     33      /* UART6 IRQ */
#define IRQn_UART7                     34      /* UART7 IRQ */
#define IRQn_I2C0                      35      /* I2C0 IRQ */
#define IRQn_I2C1                      36      /* I2C1 IRQ */
#define IRQn_I2C2                      37      /* I2C2 IRQ */
#define IRQn_I2C3                      38      /* I2C3 IRQ */
#define IRQn_SPI0                      39      /* SPI0 IRQ */
#define IRQn_SPI1                      40      /* SPI1 IRQ */
#define IRQn_SPI2                      41      /* SPI2 IRQ */
#define IRQn_SPI3                      42      /* SPI3 IRQ */
#define IRQn_GPTMR0                    43      /* GPTMR0 IRQ */
#define IRQn_GPTMR1                    44      /* GPTMR1 IRQ */
#define IRQn_GPTMR2                    45      /* GPTMR2 IRQ */
#define IRQn_GPTMR3                    46      /* GPTMR3 IRQ */
#define IRQn_GPTMR4                    47      /* GPTMR4 IRQ */
#define IRQn_GPTMR5                    48      /* GPTMR5 IRQ */
#define IRQn_GPTMR6                    49      /* GPTMR6 IRQ */
#define IRQn_GPTMR7                    50      /* GPTMR7 IRQ */
#define IRQn_EWDG0                     51      /* EWDG0 IRQ */
#define IRQn_EWDG1                     52      /* EWDG1 IRQ */
#define IRQn_MBX0A                     53      /* MBX0A IRQ */
#define IRQn_MBX0B                     54      /* MBX0B IRQ */
#define IRQn_MBX1A                     55      /* MBX1A IRQ */
#define IRQn_MBX1B                     56      /* MBX1B IRQ */
#define IRQn_RNG                       57      /* RNG IRQ */
#define IRQn_HDMA                      58      /* HDMA IRQ */
#define IRQn_ADC0                      59      /* ADC0 IRQ */
#define IRQn_ADC1                      60      /* ADC1 IRQ */
#define IRQn_SDM                       61      /* SDM IRQ */
#define IRQn_OPAMP                     62      /* OPAMP IRQ */
#define IRQn_I2S0                      63      /* I2S0 IRQ */
#define IRQn_I2S1                      64      /* I2S1 IRQ */
#define IRQn_I2S2                      65      /* I2S2 IRQ */
#define IRQn_I2S3                      66      /* I2S3 IRQ */
#define IRQn_DAO                       67      /* DAO IRQ */
#define IRQn_PDM                       68      /* PDM IRQ */
#define IRQn_SMIX_DMA                  69      /* SMIX_DMA IRQ */
#define IRQn_SMIX_ASRC                 70      /* SMIX_ASRC IRQ */
#define IRQn_CAM0                      71      /* CAM0 IRQ */
#define IRQn_CAM1                      72      /* CAM1 IRQ */
#define IRQn_LCDC                      73      /* LCDC IRQ */
#define IRQn_LCDC1                     74      /* LCDC1 IRQ */
#define IRQn_PDMA                      75      /* PDMA IRQ */
#define IRQn_JPEG                      76      /* JPEG IRQ */
#define IRQn_GWCK0_FUNC                77      /* GWCK0_FUNC IRQ */
#define IRQn_GWCK0_ERR                 78      /* GWCK0_ERR IRQ */
#define IRQn_GWCK1_FUNC                79      /* GWCK1_FUNC IRQ */
#define IRQn_GWCK1_ERR                 80      /* GWCK1_ERR IRQ */
#define IRQn_MIPI_DSI0                 81      /* MIPI_DSI0 IRQ */
#define IRQn_MIPI_DSI1                 82      /* MIPI_DSI1 IRQ */
#define IRQn_MIPI_CSI0                 83      /* MIPI_CSI0 IRQ */
#define IRQn_MIPI_CSI0_AP              84      /* MIPI_CSI0_AP IRQ */
#define IRQn_MIPI_CSI0_DIAG            85      /* MIPI_CSI0_DIAG IRQ */
#define IRQn_MIPI_CSI1_AP              86      /* MIPI_CSI1_AP IRQ */
#define IRQn_MIPI_CSI1_DIAG            87      /* MIPI_CSI1_DIAG IRQ */
#define IRQn_MIPI_CSI1                 88      /* MIPI_CSI1 IRQ */
#define IRQn_LCB0                      89      /* LCB0 IRQ */
#define IRQn_LCB1                      90      /* LCB1 IRQ */
#define IRQn_GPU                       91      /* GPU IRQ */
#define IRQn_ENET0                     92      /* ENET0 IRQ */
#define IRQn_NTMR0                     93      /* NTMR0 IRQ */
#define IRQn_USB0                      94      /* USB0 IRQ */
#define IRQn_SDXC0                     95      /* SDXC0 IRQ */
#define IRQn_SDXC1                     96      /* SDXC1 IRQ */
#define IRQn_SDP                       97      /* SDP IRQ */
#define IRQn_XPI0                      98      /* XPI0 IRQ */
#define IRQn_XDMA                      99      /* XDMA IRQ */
#define IRQn_DDR                       100     /* DDR IRQ */
#define IRQn_FFA                       101     /* FFA IRQ */
#define IRQn_PSEC                      102     /* PSEC IRQ */
#define IRQn_TSNS                      103     /* TSNS IRQ */
#define IRQn_VAD                       104     /* VAD IRQ */
#define IRQn_PGPIO                     105     /* PGPIO IRQ */
#define IRQn_PWDG                      106     /* PWDG IRQ */
#define IRQn_PTMR                      107     /* PTMR IRQ */
#define IRQn_PUART                     108     /* PUART IRQ */
#define IRQn_FUSE                      109     /* FUSE IRQ */
#define IRQn_SECMON                    110     /* SECMON IRQ */
#define IRQn_RTC                       111     /* RTC IRQ */
#define IRQn_BGPIO                     112     /* BGPIO IRQ */
#define IRQn_BVIO                      113     /* BVIO IRQ */
#define IRQn_BROWNOUT                  114     /* BROWNOUT IRQ */
#define IRQn_SYSCTL                    115     /* SYSCTL IRQ */
#define IRQn_DEBUG0                    116     /* DEBUG0 IRQ */
#define IRQn_DEBUG1                    117     /* DEBUG1 IRQ */

#endif /* HPM_SOC_IRQ_H */
