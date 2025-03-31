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
#define IRQn_GPIO0_Z                   7       /* GPIO0_Z IRQ */
#define IRQn_GPIO1_A                   8       /* GPIO1_A IRQ */
#define IRQn_GPIO1_B                   9       /* GPIO1_B IRQ */
#define IRQn_GPIO1_C                   10      /* GPIO1_C IRQ */
#define IRQn_GPIO1_D                   11      /* GPIO1_D IRQ */
#define IRQn_GPIO1_X                   12      /* GPIO1_X IRQ */
#define IRQn_GPIO1_Y                   13      /* GPIO1_Y IRQ */
#define IRQn_GPIO1_Z                   14      /* GPIO1_Z IRQ */
#define IRQn_ADC0                      15      /* ADC0 IRQ */
#define IRQn_ADC1                      16      /* ADC1 IRQ */
#define IRQn_ADC2                      17      /* ADC2 IRQ */
#define IRQn_SDFM                      18      /* SDFM IRQ */
#define IRQn_DAC0                      19      /* DAC0 IRQ */
#define IRQn_DAC1                      20      /* DAC1 IRQ */
#define IRQn_ACMP_0                    21      /* ACMP[0] IRQ */
#define IRQn_ACMP_1                    22      /* ACMP[1] IRQ */
#define IRQn_ACMP_2                    23      /* ACMP[2] IRQ */
#define IRQn_ACMP_3                    24      /* ACMP[3] IRQ */
#define IRQn_SPI0                      25      /* SPI0 IRQ */
#define IRQn_SPI1                      26      /* SPI1 IRQ */
#define IRQn_SPI2                      27      /* SPI2 IRQ */
#define IRQn_SPI3                      28      /* SPI3 IRQ */
#define IRQn_UART0                     29      /* UART0 IRQ */
#define IRQn_UART1                     30      /* UART1 IRQ */
#define IRQn_UART2                     31      /* UART2 IRQ */
#define IRQn_UART3                     32      /* UART3 IRQ */
#define IRQn_UART4                     33      /* UART4 IRQ */
#define IRQn_UART5                     34      /* UART5 IRQ */
#define IRQn_UART6                     35      /* UART6 IRQ */
#define IRQn_UART7                     36      /* UART7 IRQ */
#define IRQn_MCAN0                     37      /* MCAN0 IRQ */
#define IRQn_MCAN1                     38      /* MCAN1 IRQ */
#define IRQn_MCAN2                     39      /* MCAN2 IRQ */
#define IRQn_MCAN3                     40      /* MCAN3 IRQ */
#define IRQn_PTPC                      41      /* PTPC IRQ */
#define IRQn_WDG0                      42      /* WDG0 IRQ */
#define IRQn_WDG1                      43      /* WDG1 IRQ */
#define IRQn_TSNS                      44      /* TSNS IRQ */
#define IRQn_MBX0A                     45      /* MBX0A IRQ */
#define IRQn_MBX0B                     46      /* MBX0B IRQ */
#define IRQn_MBX1A                     47      /* MBX1A IRQ */
#define IRQn_MBX1B                     48      /* MBX1B IRQ */
#define IRQn_GPTMR0                    49      /* GPTMR0 IRQ */
#define IRQn_GPTMR1                    50      /* GPTMR1 IRQ */
#define IRQn_GPTMR2                    51      /* GPTMR2 IRQ */
#define IRQn_GPTMR3                    52      /* GPTMR3 IRQ */
#define IRQn_I2C0                      53      /* I2C0 IRQ */
#define IRQn_I2C1                      54      /* I2C1 IRQ */
#define IRQn_I2C2                      55      /* I2C2 IRQ */
#define IRQn_I2C3                      56      /* I2C3 IRQ */
#define IRQn_PWM0                      57      /* PWM0 IRQ */
#define IRQn_HALL0                     58      /* HALL0 IRQ */
#define IRQn_QEI0                      59      /* QEI0 IRQ */
#define IRQn_PWM1                      60      /* PWM1 IRQ */
#define IRQn_HALL1                     61      /* HALL1 IRQ */
#define IRQn_QEI1                      62      /* QEI1 IRQ */
#define IRQn_PWM2                      63      /* PWM2 IRQ */
#define IRQn_HALL2                     64      /* HALL2 IRQ */
#define IRQn_QEI2                      65      /* QEI2 IRQ */
#define IRQn_PWM3                      66      /* PWM3 IRQ */
#define IRQn_HALL3                     67      /* HALL3 IRQ */
#define IRQn_QEI3                      68      /* QEI3 IRQ */
#define IRQn_SDP                       69      /* SDP IRQ */
#define IRQn_XPI0                      70      /* XPI0 IRQ */
#define IRQn_XDMA                      71      /* XDMA IRQ */
#define IRQn_HDMA                      72      /* HDMA IRQ */
#define IRQn_RNG                       73      /* RNG IRQ */
#define IRQn_USB0                      74      /* USB0 IRQ */
#define IRQn_PSEC                      75      /* PSEC IRQ */
#define IRQn_PGPIO                     76      /* PGPIO IRQ */
#define IRQn_PWDG                      77      /* PWDG IRQ */
#define IRQn_PTMR                      78      /* PTMR IRQ */
#define IRQn_PUART                     79      /* PUART IRQ */
#define IRQn_FUSE                      80      /* FUSE IRQ */
#define IRQn_SECMON                    81      /* SECMON IRQ */
#define IRQn_RTC                       82      /* RTC IRQ */
#define IRQn_BUTN                      83      /* BUTN IRQ */
#define IRQn_BGPIO                     84      /* BGPIO IRQ */
#define IRQn_BVIO                      85      /* BVIO IRQ */
#define IRQn_BROWNOUT                  86      /* BROWNOUT IRQ */
#define IRQn_SYSCTL                    87      /* SYSCTL IRQ */
#define IRQn_DEBUG_0                   88      /* DEBUG[0] IRQ */
#define IRQn_DEBUG_1                   89      /* DEBUG[1] IRQ */
#define IRQn_LIN0                      90      /* LIN0 IRQ */
#define IRQn_LIN1                      91      /* LIN1 IRQ */
#define IRQn_LIN2                      92      /* LIN2 IRQ */
#define IRQn_LIN3                      93      /* LIN3 IRQ */

#endif /* HPM_SOC_IRQ_H */
