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
#define IRQn_GPIO1_A                   10      /* GPIO1_A IRQ */
#define IRQn_GPIO1_B                   11      /* GPIO1_B IRQ */
#define IRQn_GPIO1_C                   12      /* GPIO1_C IRQ */
#define IRQn_GPIO1_D                   13      /* GPIO1_D IRQ */
#define IRQn_GPIO1_E                   14      /* GPIO1_E IRQ */
#define IRQn_GPIO1_F                   15      /* GPIO1_F IRQ */
#define IRQn_GPIO1_X                   16      /* GPIO1_X IRQ */
#define IRQn_GPIO1_Y                   17      /* GPIO1_Y IRQ */
#define IRQn_GPIO1_Z                   18      /* GPIO1_Z IRQ */
#define IRQn_ADC0                      19      /* ADC0 IRQ */
#define IRQn_ADC1                      20      /* ADC1 IRQ */
#define IRQn_ADC2                      21      /* ADC2 IRQ */
#define IRQn_ADC3                      22      /* ADC3 IRQ */
#define IRQn_ACMP_0                    23      /* ACMP[0] IRQ */
#define IRQn_ACMP_1                    24      /* ACMP[1] IRQ */
#define IRQn_ACMP_2                    25      /* ACMP[2] IRQ */
#define IRQn_ACMP_3                    26      /* ACMP[3] IRQ */
#define IRQn_SPI0                      27      /* SPI0 IRQ */
#define IRQn_SPI1                      28      /* SPI1 IRQ */
#define IRQn_SPI2                      29      /* SPI2 IRQ */
#define IRQn_SPI3                      30      /* SPI3 IRQ */
#define IRQn_UART0                     31      /* UART0 IRQ */
#define IRQn_UART1                     32      /* UART1 IRQ */
#define IRQn_UART2                     33      /* UART2 IRQ */
#define IRQn_UART3                     34      /* UART3 IRQ */
#define IRQn_UART4                     35      /* UART4 IRQ */
#define IRQn_UART5                     36      /* UART5 IRQ */
#define IRQn_UART6                     37      /* UART6 IRQ */
#define IRQn_UART7                     38      /* UART7 IRQ */
#define IRQn_UART8                     39      /* UART8 IRQ */
#define IRQn_UART9                     40      /* UART9 IRQ */
#define IRQn_UART10                    41      /* UART10 IRQ */
#define IRQn_UART11                    42      /* UART11 IRQ */
#define IRQn_UART12                    43      /* UART12 IRQ */
#define IRQn_UART13                    44      /* UART13 IRQ */
#define IRQn_UART14                    45      /* UART14 IRQ */
#define IRQn_UART15                    46      /* UART15 IRQ */
#define IRQn_CAN0                      47      /* CAN0 IRQ */
#define IRQn_CAN1                      48      /* CAN1 IRQ */
#define IRQn_CAN2                      49      /* CAN2 IRQ */
#define IRQn_CAN3                      50      /* CAN3 IRQ */
#define IRQn_PTPC                      51      /* PTPC IRQ */
#define IRQn_WDG0                      52      /* WDG0 IRQ */
#define IRQn_WDG1                      53      /* WDG1 IRQ */
#define IRQn_WDG2                      54      /* WDG2 IRQ */
#define IRQn_WDG3                      55      /* WDG3 IRQ */
#define IRQn_MBX0A                     56      /* MBX0A IRQ */
#define IRQn_MBX0B                     57      /* MBX0B IRQ */
#define IRQn_MBX1A                     58      /* MBX1A IRQ */
#define IRQn_MBX1B                     59      /* MBX1B IRQ */
#define IRQn_GPTMR0                    60      /* GPTMR0 IRQ */
#define IRQn_GPTMR1                    61      /* GPTMR1 IRQ */
#define IRQn_GPTMR2                    62      /* GPTMR2 IRQ */
#define IRQn_GPTMR3                    63      /* GPTMR3 IRQ */
#define IRQn_GPTMR4                    64      /* GPTMR4 IRQ */
#define IRQn_GPTMR5                    65      /* GPTMR5 IRQ */
#define IRQn_GPTMR6                    66      /* GPTMR6 IRQ */
#define IRQn_GPTMR7                    67      /* GPTMR7 IRQ */
#define IRQn_I2C0                      68      /* I2C0 IRQ */
#define IRQn_I2C1                      69      /* I2C1 IRQ */
#define IRQn_I2C2                      70      /* I2C2 IRQ */
#define IRQn_I2C3                      71      /* I2C3 IRQ */
#define IRQn_PWM0                      72      /* PWM0 IRQ */
#define IRQn_HALL0                     73      /* HALL0 IRQ */
#define IRQn_QEI0                      74      /* QEI0 IRQ */
#define IRQn_PWM1                      75      /* PWM1 IRQ */
#define IRQn_HALL1                     76      /* HALL1 IRQ */
#define IRQn_QEI1                      77      /* QEI1 IRQ */
#define IRQn_PWM2                      78      /* PWM2 IRQ */
#define IRQn_HALL2                     79      /* HALL2 IRQ */
#define IRQn_QEI2                      80      /* QEI2 IRQ */
#define IRQn_PWM3                      81      /* PWM3 IRQ */
#define IRQn_HALL3                     82      /* HALL3 IRQ */
#define IRQn_QEI3                      83      /* QEI3 IRQ */
#define IRQn_SDP                       84      /* SDP IRQ */
#define IRQn_XPI0                      85      /* XPI0 IRQ */
#define IRQn_XPI1                      86      /* XPI1 IRQ */
#define IRQn_XDMA                      87      /* XDMA IRQ */
#define IRQn_HDMA                      88      /* HDMA IRQ */
#define IRQn_FEMC                      89      /* FEMC IRQ */
#define IRQn_RNG                       90      /* RNG IRQ */
#define IRQn_I2S0                      91      /* I2S0 IRQ */
#define IRQn_I2S1                      92      /* I2S1 IRQ */
#define IRQn_I2S2                      93      /* I2S2 IRQ */
#define IRQn_I2S3                      94      /* I2S3 IRQ */
#define IRQn_DAO                       95      /* DAO IRQ */
#define IRQn_PDM                       96      /* PDM IRQ */
#define IRQn_CAM0                      97      /* CAM0 IRQ */
#define IRQn_CAM1                      98      /* CAM1 IRQ */
#define IRQn_LCDC_D0                   99      /* LCDC_D0 IRQ */
#define IRQn_LCDC_D1                   100     /* LCDC_D1 IRQ */
#define IRQn_PDMA_D0                   101     /* PDMA_D0 IRQ */
#define IRQn_PDMA_D1                   102     /* PDMA_D1 IRQ */
#define IRQn_JPEG                      103     /* JPEG IRQ */
#define IRQn_NTMR0                     104     /* NTMR0 IRQ */
#define IRQn_NTMR1                     105     /* NTMR1 IRQ */
#define IRQn_USB0                      106     /* USB0 IRQ */
#define IRQn_USB1                      107     /* USB1 IRQ */
#define IRQn_ENET0                     108     /* ENET0 IRQ */
#define IRQn_ENET1                     109     /* ENET1 IRQ */
#define IRQn_SDXC0                     110     /* SDXC0 IRQ */
#define IRQn_SDXC1                     111     /* SDXC1 IRQ */
#define IRQn_PSEC                      112     /* PSEC IRQ */
#define IRQn_PGPIO                     113     /* PGPIO IRQ */
#define IRQn_PWDG                      114     /* PWDG IRQ */
#define IRQn_PTMR                      115     /* PTMR IRQ */
#define IRQn_PUART                     116     /* PUART IRQ */
#define IRQn_VAD                       117     /* VAD IRQ */
#define IRQn_FUSE                      118     /* FUSE IRQ */
#define IRQn_SECMON                    119     /* SECMON IRQ */
#define IRQn_RTC                       120     /* RTC IRQ */
#define IRQn_BUTN                      121     /* BUTN IRQ */
#define IRQn_BGPIO                     122     /* BGPIO IRQ */
#define IRQn_BVIO                      123     /* BVIO IRQ */
#define IRQn_BROWNOUT                  124     /* BROWNOUT IRQ */
#define IRQn_SYSCTL                    125     /* SYSCTL IRQ */
#define IRQn_DEBUG_0                   126     /* DEBUG[0] IRQ */
#define IRQn_DEBUG_1                   127     /* DEBUG[1] IRQ */

#endif /* HPM_SOC_IRQ_H */
