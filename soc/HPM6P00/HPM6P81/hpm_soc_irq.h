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
#define IRQn_GPIO0_X                   5       /* GPIO0_X IRQ */
#define IRQn_GPIO0_Y                   6       /* GPIO0_Y IRQ */
#define IRQn_GPIO1_A                   7       /* GPIO1_A IRQ */
#define IRQn_GPIO1_B                   8       /* GPIO1_B IRQ */
#define IRQn_GPIO1_C                   9       /* GPIO1_C IRQ */
#define IRQn_GPIO1_D                   10      /* GPIO1_D IRQ */
#define IRQn_GPIO1_X                   11      /* GPIO1_X IRQ */
#define IRQn_GPIO1_Y                   12      /* GPIO1_Y IRQ */
#define IRQn_GPTMR0                    13      /* GPTMR0 IRQ */
#define IRQn_GPTMR1                    14      /* GPTMR1 IRQ */
#define IRQn_GPTMR2                    15      /* GPTMR2 IRQ */
#define IRQn_GPTMR3                    16      /* GPTMR3 IRQ */
#define IRQn_UART0                     21      /* UART0 IRQ */
#define IRQn_UART1                     22      /* UART1 IRQ */
#define IRQn_UART2                     23      /* UART2 IRQ */
#define IRQn_UART3                     24      /* UART3 IRQ */
#define IRQn_UART4                     25      /* UART4 IRQ */
#define IRQn_UART5                     26      /* UART5 IRQ */
#define IRQn_UART6                     27      /* UART6 IRQ */
#define IRQn_UART7                     28      /* UART7 IRQ */
#define IRQn_I2C0                      29      /* I2C0 IRQ */
#define IRQn_I2C1                      30      /* I2C1 IRQ */
#define IRQn_I2C2                      31      /* I2C2 IRQ */
#define IRQn_I2C3                      32      /* I2C3 IRQ */
#define IRQn_SPI0                      33      /* SPI0 IRQ */
#define IRQn_SPI1                      34      /* SPI1 IRQ */
#define IRQn_SPI2                      35      /* SPI2 IRQ */
#define IRQn_SPI3                      36      /* SPI3 IRQ */
#define IRQn_TSNS                      37      /* TSNS IRQ */
#define IRQn_MBX0A                     38      /* MBX0A IRQ */
#define IRQn_MBX0B                     39      /* MBX0B IRQ */
#define IRQn_MBX1A                     40      /* MBX1A IRQ */
#define IRQn_MBX1B                     41      /* MBX1B IRQ */
#define IRQn_EWDG0                     42      /* EWDG0 IRQ */
#define IRQn_EWDG1                     43      /* EWDG1 IRQ */
#define IRQn_EWDG2                     44      /* EWDG2 IRQ */
#define IRQn_EWDG3                     45      /* EWDG3 IRQ */
#define IRQn_HDMA                      46      /* HDMA IRQ */
#define IRQn_LOBS                      47      /* LOBS IRQ */
#define IRQn_ADC0                      48      /* ADC0 IRQ */
#define IRQn_ADC1                      49      /* ADC1 IRQ */
#define IRQn_ADC2                      50      /* ADC2 IRQ */
#define IRQn_ADC3                      51      /* ADC3 IRQ */
#define IRQn_DAC0                      52      /* DAC0 IRQ */
#define IRQn_DAC1                      53      /* DAC1 IRQ */
#define IRQn_ACMP0_0                   54      /* ACMP0[0] IRQ */
#define IRQn_ACMP0_1                   55      /* ACMP0[1] IRQ */
#define IRQn_ACMP1_0                   56      /* ACMP1[0] IRQ */
#define IRQn_ACMP1_1                   57      /* ACMP1[1] IRQ */
#define IRQn_ACMP2_0                   58      /* ACMP2[0] IRQ */
#define IRQn_ACMP2_1                   59      /* ACMP2[1] IRQ */
#define IRQn_ACMP3_0                   60      /* ACMP3[0] IRQ */
#define IRQn_ACMP3_1                   61      /* ACMP3[1] IRQ */
#define IRQn_I2S0                      62      /* I2S0 IRQ */
#define IRQn_I2S1                      63      /* I2S1 IRQ */
#define IRQn_DAO                       64      /* DAO IRQ */
#define IRQn_PDM                       65      /* PDM IRQ */
#define IRQn_MCAN0                     66      /* MCAN0 IRQ */
#define IRQn_MCAN1                     67      /* MCAN1 IRQ */
#define IRQn_MCAN2                     68      /* MCAN2 IRQ */
#define IRQn_MCAN3                     69      /* MCAN3 IRQ */
#define IRQn_PTPC                      70      /* PTPC IRQ */
#define IRQn_QEI0                      71      /* QEI0 IRQ */
#define IRQn_QEI1                      72      /* QEI1 IRQ */
#define IRQn_PWM0                      73      /* PWM0 IRQ */
#define IRQn_PWM1                      74      /* PWM1 IRQ */
#define IRQn_PWM2                      75      /* PWM2 IRQ */
#define IRQn_PWM3                      76      /* PWM3 IRQ */
#define IRQn_RDC0                      77      /* RDC0 IRQ */
#define IRQn_SDM0                      78      /* SDM0 IRQ */
#define IRQn_SEI0_0                    79      /* SEI0[0] IRQ */
#define IRQn_SEI0_1                    80      /* SEI0[1] IRQ */
#define IRQn_MTG0                      81      /* MTG0 IRQ */
#define IRQn_VSC0                      82      /* VSC0 IRQ */
#define IRQn_CLC0_0                    83      /* CLC0[0] IRQ */
#define IRQn_CLC0_1                    84      /* CLC0[1] IRQ */
#define IRQn_TRGMUX0                   85      /* TRGMUX0 IRQ */
#define IRQn_TRGMUX1                   86      /* TRGMUX1 IRQ */
#define IRQn_ENET0                     87      /* ENET0 IRQ */
#define IRQn_NTMR0                     88      /* NTMR0 IRQ */
#define IRQn_USB0                      89      /* USB0 IRQ */
#define IRQn_XPI0                      90      /* XPI0 IRQ */
#define IRQn_FEMC                      91      /* FEMC IRQ */
#define IRQn_PPI                       92      /* PPI IRQ */
#define IRQn_XDMA                      93      /* XDMA IRQ */
#define IRQn_FFA                       94      /* FFA IRQ */
#define IRQn_SDP                       95      /* SDP IRQ */
#define IRQn_RNG                       96      /* RNG IRQ */
#define IRQn_PSEC                      97      /* PSEC IRQ */
#define IRQn_PGPIO                     98      /* PGPIO IRQ */
#define IRQn_PEWDG                     99      /* PEWDG IRQ */
#define IRQn_PTMR                      100     /* PTMR IRQ */
#define IRQn_PUART                     101     /* PUART IRQ */
#define IRQn_FUSE                      102     /* FUSE IRQ */
#define IRQn_SECMON                    103     /* SECMON IRQ */
#define IRQn_PAD_WAKEUP                104     /* PAD_WAKEUP IRQ */
#define IRQn_BROWNOUT                  105     /* BROWNOUT IRQ */
#define IRQn_SYSCTL                    106     /* SYSCTL IRQ */
#define IRQn_CPU0                      107     /* CPU0 IRQ */
#define IRQn_CPU1                      108     /* CPU1 IRQ */
#define IRQn_DEBUG0                    109     /* DEBUG0 IRQ */
#define IRQn_DEBUG1                    110     /* DEBUG1 IRQ */

#endif /* HPM_SOC_IRQ_H */
