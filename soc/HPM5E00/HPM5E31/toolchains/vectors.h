/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifdef __IAR_SYSTEMS_ASM__

IRQ_HANDLER macro
    dc32 default_isr_\1
    endm

IRQ_DEFAULT_HANDLER macro
    PUBWEAK default_isr_\1
default_isr_\1
    j default_irq_handler
    endm

    SECTION `.isr_vector`:CODE:ROOT(9)
    PUBWEAK default_irq_handler
default_irq_handler
    j default_irq_handler
    IRQ_DEFAULT_HANDLER 1 /* GPIO0_A IRQ handler */
    IRQ_DEFAULT_HANDLER 2 /* GPIO0_B IRQ handler */
    IRQ_DEFAULT_HANDLER 3 /* GPIO0_C IRQ handler */
    IRQ_DEFAULT_HANDLER 4 /* GPIO0_D IRQ handler */
    IRQ_DEFAULT_HANDLER 5 /* GPIO0_E IRQ handler */
    IRQ_DEFAULT_HANDLER 6 /* GPIO0_F IRQ handler */
    IRQ_DEFAULT_HANDLER 7 /* GPIO0_V IRQ handler */
    IRQ_DEFAULT_HANDLER 8 /* GPIO0_W IRQ handler */
    IRQ_DEFAULT_HANDLER 9 /* GPIO0_X IRQ handler */
    IRQ_DEFAULT_HANDLER 10 /* GPIO0_Y IRQ handler */
    IRQ_DEFAULT_HANDLER 11 /* GPTMR0 IRQ handler */
    IRQ_DEFAULT_HANDLER 12 /* GPTMR1 IRQ handler */
    IRQ_DEFAULT_HANDLER 13 /* GPTMR2 IRQ handler */
    IRQ_DEFAULT_HANDLER 14 /* GPTMR3 IRQ handler */
    IRQ_DEFAULT_HANDLER 15 /* OWR0 IRQ handler */
    IRQ_DEFAULT_HANDLER 16 /* OWR1 IRQ handler */
    IRQ_DEFAULT_HANDLER 17 /* EUI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 18 /* EUI1 IRQ handler */
    IRQ_DEFAULT_HANDLER 19 /* UART0 IRQ handler */
    IRQ_DEFAULT_HANDLER 20 /* UART1 IRQ handler */
    IRQ_DEFAULT_HANDLER 21 /* UART2 IRQ handler */
    IRQ_DEFAULT_HANDLER 22 /* UART3 IRQ handler */
    IRQ_DEFAULT_HANDLER 23 /* UART4 IRQ handler */
    IRQ_DEFAULT_HANDLER 24 /* UART5 IRQ handler */
    IRQ_DEFAULT_HANDLER 25 /* UART6 IRQ handler */
    IRQ_DEFAULT_HANDLER 26 /* UART7 IRQ handler */
    IRQ_DEFAULT_HANDLER 27 /* I2C0 IRQ handler */
    IRQ_DEFAULT_HANDLER 28 /* I2C1 IRQ handler */
    IRQ_DEFAULT_HANDLER 29 /* I2C2 IRQ handler */
    IRQ_DEFAULT_HANDLER 30 /* I2C3 IRQ handler */
    IRQ_DEFAULT_HANDLER 31 /* SPI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 32 /* SPI1 IRQ handler */
    IRQ_DEFAULT_HANDLER 33 /* SPI2 IRQ handler */
    IRQ_DEFAULT_HANDLER 34 /* SPI3 IRQ handler */
    IRQ_DEFAULT_HANDLER 35 /* TSNS IRQ handler */
    IRQ_DEFAULT_HANDLER 36 /* MBX0A IRQ handler */
    IRQ_DEFAULT_HANDLER 37 /* MBX0B IRQ handler */
    IRQ_DEFAULT_HANDLER 38 /* EWDG0 IRQ handler */
    IRQ_DEFAULT_HANDLER 39 /* EWDG1 IRQ handler */
    IRQ_DEFAULT_HANDLER 40 /* HDMA IRQ handler */
    IRQ_DEFAULT_HANDLER 41 /* LOBS IRQ handler */
    IRQ_DEFAULT_HANDLER 42 /* ADC0 IRQ handler */
    IRQ_DEFAULT_HANDLER 43 /* ADC1 IRQ handler */
    IRQ_DEFAULT_HANDLER 44 /* ACMP0[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 45 /* ACMP0[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 46 /* MCAN0 IRQ handler */
    IRQ_DEFAULT_HANDLER 47 /* MCAN1 IRQ handler */
    IRQ_DEFAULT_HANDLER 48 /* MCAN2 IRQ handler */
    IRQ_DEFAULT_HANDLER 49 /* MCAN3 IRQ handler */
    IRQ_DEFAULT_HANDLER 50 /* PTPC IRQ handler */
    IRQ_DEFAULT_HANDLER 51 /* QEI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 52 /* QEI1 IRQ handler */
    IRQ_DEFAULT_HANDLER 53 /* PWM0 IRQ handler */
    IRQ_DEFAULT_HANDLER 54 /* PWM1 IRQ handler */
    IRQ_DEFAULT_HANDLER 55 /* SDM0 IRQ handler */
    IRQ_DEFAULT_HANDLER 56 /* TRGM[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 57 /* TRGM[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 58 /* ENET0 IRQ handler */
    IRQ_DEFAULT_HANDLER 59 /* NTMR0 IRQ handler */
    IRQ_DEFAULT_HANDLER 60 /* USB0 IRQ handler */
    IRQ_DEFAULT_HANDLER 61 /* ESC IRQ handler */
    IRQ_DEFAULT_HANDLER 62 /* ESC_SYNC0 IRQ handler */
    IRQ_DEFAULT_HANDLER 63 /* ESC_SYNC1 IRQ handler */
    IRQ_DEFAULT_HANDLER 64 /* ESC_RESET IRQ handler */
    IRQ_DEFAULT_HANDLER 65 /* XPI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 66 /* PPI IRQ handler */
    IRQ_DEFAULT_HANDLER 67 /* XDMA IRQ handler */
    IRQ_DEFAULT_HANDLER 68 /* PGPIO IRQ handler */
    IRQ_DEFAULT_HANDLER 69 /* PEWDG IRQ handler */
    IRQ_DEFAULT_HANDLER 70 /* PTMR IRQ handler */
    IRQ_DEFAULT_HANDLER 71 /* PUART IRQ handler */
    IRQ_DEFAULT_HANDLER 72 /* FUSE IRQ handler */
    IRQ_DEFAULT_HANDLER 73 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_DEFAULT_HANDLER 74 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_DEFAULT_HANDLER 75 /* BROWNOUT IRQ handler */
    IRQ_DEFAULT_HANDLER 76 /* SYSCTL IRQ handler */
    IRQ_DEFAULT_HANDLER 77 /* CPU0 IRQ handler */
    IRQ_DEFAULT_HANDLER 78 /* DEBUG0 IRQ handler */
    IRQ_DEFAULT_HANDLER 79 /* DEBUG1 IRQ handler */

    EXTERN irq_handler_trap
    SECTION `.vector_table`:CODE:ROOT(9)
    PUBLIC __vector_table
    DATA

__vector_table
#if (!defined(USE_NONVECTOR_MODE) || (USE_NONVECTOR_MODE == 0)) && defined(CONFIG_FREERTOS)
    dc32 freertos_risc_v_trap_handler
#else
    dc32 irq_handler_trap
#endif
    IRQ_HANDLER 1 /* GPIO0_A IRQ handler */
    IRQ_HANDLER 2 /* GPIO0_B IRQ handler */
    IRQ_HANDLER 3 /* GPIO0_C IRQ handler */
    IRQ_HANDLER 4 /* GPIO0_D IRQ handler */
    IRQ_HANDLER 5 /* GPIO0_E IRQ handler */
    IRQ_HANDLER 6 /* GPIO0_F IRQ handler */
    IRQ_HANDLER 7 /* GPIO0_V IRQ handler */
    IRQ_HANDLER 8 /* GPIO0_W IRQ handler */
    IRQ_HANDLER 9 /* GPIO0_X IRQ handler */
    IRQ_HANDLER 10 /* GPIO0_Y IRQ handler */
    IRQ_HANDLER 11 /* GPTMR0 IRQ handler */
    IRQ_HANDLER 12 /* GPTMR1 IRQ handler */
    IRQ_HANDLER 13 /* GPTMR2 IRQ handler */
    IRQ_HANDLER 14 /* GPTMR3 IRQ handler */
    IRQ_HANDLER 15 /* OWR0 IRQ handler */
    IRQ_HANDLER 16 /* OWR1 IRQ handler */
    IRQ_HANDLER 17 /* EUI0 IRQ handler */
    IRQ_HANDLER 18 /* EUI1 IRQ handler */
    IRQ_HANDLER 19 /* UART0 IRQ handler */
    IRQ_HANDLER 20 /* UART1 IRQ handler */
    IRQ_HANDLER 21 /* UART2 IRQ handler */
    IRQ_HANDLER 22 /* UART3 IRQ handler */
    IRQ_HANDLER 23 /* UART4 IRQ handler */
    IRQ_HANDLER 24 /* UART5 IRQ handler */
    IRQ_HANDLER 25 /* UART6 IRQ handler */
    IRQ_HANDLER 26 /* UART7 IRQ handler */
    IRQ_HANDLER 27 /* I2C0 IRQ handler */
    IRQ_HANDLER 28 /* I2C1 IRQ handler */
    IRQ_HANDLER 29 /* I2C2 IRQ handler */
    IRQ_HANDLER 30 /* I2C3 IRQ handler */
    IRQ_HANDLER 31 /* SPI0 IRQ handler */
    IRQ_HANDLER 32 /* SPI1 IRQ handler */
    IRQ_HANDLER 33 /* SPI2 IRQ handler */
    IRQ_HANDLER 34 /* SPI3 IRQ handler */
    IRQ_HANDLER 35 /* TSNS IRQ handler */
    IRQ_HANDLER 36 /* MBX0A IRQ handler */
    IRQ_HANDLER 37 /* MBX0B IRQ handler */
    IRQ_HANDLER 38 /* EWDG0 IRQ handler */
    IRQ_HANDLER 39 /* EWDG1 IRQ handler */
    IRQ_HANDLER 40 /* HDMA IRQ handler */
    IRQ_HANDLER 41 /* LOBS IRQ handler */
    IRQ_HANDLER 42 /* ADC0 IRQ handler */
    IRQ_HANDLER 43 /* ADC1 IRQ handler */
    IRQ_HANDLER 44 /* ACMP0[0] IRQ handler */
    IRQ_HANDLER 45 /* ACMP0[1] IRQ handler */
    IRQ_HANDLER 46 /* MCAN0 IRQ handler */
    IRQ_HANDLER 47 /* MCAN1 IRQ handler */
    IRQ_HANDLER 48 /* MCAN2 IRQ handler */
    IRQ_HANDLER 49 /* MCAN3 IRQ handler */
    IRQ_HANDLER 50 /* PTPC IRQ handler */
    IRQ_HANDLER 51 /* QEI0 IRQ handler */
    IRQ_HANDLER 52 /* QEI1 IRQ handler */
    IRQ_HANDLER 53 /* PWM0 IRQ handler */
    IRQ_HANDLER 54 /* PWM1 IRQ handler */
    IRQ_HANDLER 55 /* SDM0 IRQ handler */
    IRQ_HANDLER 56 /* TRGM[0] IRQ handler */
    IRQ_HANDLER 57 /* TRGM[1] IRQ handler */
    IRQ_HANDLER 58 /* ENET0 IRQ handler */
    IRQ_HANDLER 59 /* NTMR0 IRQ handler */
    IRQ_HANDLER 60 /* USB0 IRQ handler */
    IRQ_HANDLER 61 /* ESC IRQ handler */
    IRQ_HANDLER 62 /* ESC_SYNC0 IRQ handler */
    IRQ_HANDLER 63 /* ESC_SYNC1 IRQ handler */
    IRQ_HANDLER 64 /* ESC_RESET IRQ handler */
    IRQ_HANDLER 65 /* XPI0 IRQ handler */
    IRQ_HANDLER 66 /* PPI IRQ handler */
    IRQ_HANDLER 67 /* XDMA IRQ handler */
    IRQ_HANDLER 68 /* PGPIO IRQ handler */
    IRQ_HANDLER 69 /* PEWDG IRQ handler */
    IRQ_HANDLER 70 /* PTMR IRQ handler */
    IRQ_HANDLER 71 /* PUART IRQ handler */
    IRQ_HANDLER 72 /* FUSE IRQ handler */
    IRQ_HANDLER 73 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_HANDLER 74 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_HANDLER 75 /* BROWNOUT IRQ handler */
    IRQ_HANDLER 76 /* SYSCTL IRQ handler */
    IRQ_HANDLER 77 /* CPU0 IRQ handler */
    IRQ_HANDLER 78 /* DEBUG0 IRQ handler */
    IRQ_HANDLER 79 /* DEBUG1 IRQ handler */

#else

.weak default_irq_handler
.align 2
default_irq_handler:
1:    j 1b

.macro IRQ_HANDLER irq
  .weak default_isr_\irq
  .set default_isr_\irq, default_irq_handler
  .long default_isr_\irq
.endm

.section .vector_table, "a"
.global __vector_table
.align 9

#if (!defined(USE_NONVECTOR_MODE) || (USE_NONVECTOR_MODE == 0)) && defined(CONFIG_FREERTOS)
    .set default_isr_trap, freertos_risc_v_trap_handler
#else
    .set default_isr_trap, irq_handler_trap
#endif

__vector_table:
    .weak default_isr_trap
    .long default_isr_trap
    IRQ_HANDLER 1 /* GPIO0_A IRQ handler */
    IRQ_HANDLER 2 /* GPIO0_B IRQ handler */
    IRQ_HANDLER 3 /* GPIO0_C IRQ handler */
    IRQ_HANDLER 4 /* GPIO0_D IRQ handler */
    IRQ_HANDLER 5 /* GPIO0_E IRQ handler */
    IRQ_HANDLER 6 /* GPIO0_F IRQ handler */
    IRQ_HANDLER 7 /* GPIO0_V IRQ handler */
    IRQ_HANDLER 8 /* GPIO0_W IRQ handler */
    IRQ_HANDLER 9 /* GPIO0_X IRQ handler */
    IRQ_HANDLER 10 /* GPIO0_Y IRQ handler */
    IRQ_HANDLER 11 /* GPTMR0 IRQ handler */
    IRQ_HANDLER 12 /* GPTMR1 IRQ handler */
    IRQ_HANDLER 13 /* GPTMR2 IRQ handler */
    IRQ_HANDLER 14 /* GPTMR3 IRQ handler */
    IRQ_HANDLER 15 /* OWR0 IRQ handler */
    IRQ_HANDLER 16 /* OWR1 IRQ handler */
    IRQ_HANDLER 17 /* EUI0 IRQ handler */
    IRQ_HANDLER 18 /* EUI1 IRQ handler */
    IRQ_HANDLER 19 /* UART0 IRQ handler */
    IRQ_HANDLER 20 /* UART1 IRQ handler */
    IRQ_HANDLER 21 /* UART2 IRQ handler */
    IRQ_HANDLER 22 /* UART3 IRQ handler */
    IRQ_HANDLER 23 /* UART4 IRQ handler */
    IRQ_HANDLER 24 /* UART5 IRQ handler */
    IRQ_HANDLER 25 /* UART6 IRQ handler */
    IRQ_HANDLER 26 /* UART7 IRQ handler */
    IRQ_HANDLER 27 /* I2C0 IRQ handler */
    IRQ_HANDLER 28 /* I2C1 IRQ handler */
    IRQ_HANDLER 29 /* I2C2 IRQ handler */
    IRQ_HANDLER 30 /* I2C3 IRQ handler */
    IRQ_HANDLER 31 /* SPI0 IRQ handler */
    IRQ_HANDLER 32 /* SPI1 IRQ handler */
    IRQ_HANDLER 33 /* SPI2 IRQ handler */
    IRQ_HANDLER 34 /* SPI3 IRQ handler */
    IRQ_HANDLER 35 /* TSNS IRQ handler */
    IRQ_HANDLER 36 /* MBX0A IRQ handler */
    IRQ_HANDLER 37 /* MBX0B IRQ handler */
    IRQ_HANDLER 38 /* EWDG0 IRQ handler */
    IRQ_HANDLER 39 /* EWDG1 IRQ handler */
    IRQ_HANDLER 40 /* HDMA IRQ handler */
    IRQ_HANDLER 41 /* LOBS IRQ handler */
    IRQ_HANDLER 42 /* ADC0 IRQ handler */
    IRQ_HANDLER 43 /* ADC1 IRQ handler */
    IRQ_HANDLER 44 /* ACMP0[0] IRQ handler */
    IRQ_HANDLER 45 /* ACMP0[1] IRQ handler */
    IRQ_HANDLER 46 /* MCAN0 IRQ handler */
    IRQ_HANDLER 47 /* MCAN1 IRQ handler */
    IRQ_HANDLER 48 /* MCAN2 IRQ handler */
    IRQ_HANDLER 49 /* MCAN3 IRQ handler */
    IRQ_HANDLER 50 /* PTPC IRQ handler */
    IRQ_HANDLER 51 /* QEI0 IRQ handler */
    IRQ_HANDLER 52 /* QEI1 IRQ handler */
    IRQ_HANDLER 53 /* PWM0 IRQ handler */
    IRQ_HANDLER 54 /* PWM1 IRQ handler */
    IRQ_HANDLER 55 /* SDM0 IRQ handler */
    IRQ_HANDLER 56 /* TRGM[0] IRQ handler */
    IRQ_HANDLER 57 /* TRGM[1] IRQ handler */
    IRQ_HANDLER 58 /* ENET0 IRQ handler */
    IRQ_HANDLER 59 /* NTMR0 IRQ handler */
    IRQ_HANDLER 60 /* USB0 IRQ handler */
    IRQ_HANDLER 61 /* ESC IRQ handler */
    IRQ_HANDLER 62 /* ESC_SYNC0 IRQ handler */
    IRQ_HANDLER 63 /* ESC_SYNC1 IRQ handler */
    IRQ_HANDLER 64 /* ESC_RESET IRQ handler */
    IRQ_HANDLER 65 /* XPI0 IRQ handler */
    IRQ_HANDLER 66 /* PPI IRQ handler */
    IRQ_HANDLER 67 /* XDMA IRQ handler */
    IRQ_HANDLER 68 /* PGPIO IRQ handler */
    IRQ_HANDLER 69 /* PEWDG IRQ handler */
    IRQ_HANDLER 70 /* PTMR IRQ handler */
    IRQ_HANDLER 71 /* PUART IRQ handler */
    IRQ_HANDLER 72 /* FUSE IRQ handler */
    IRQ_HANDLER 73 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_HANDLER 74 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_HANDLER 75 /* BROWNOUT IRQ handler */
    IRQ_HANDLER 76 /* SYSCTL IRQ handler */
    IRQ_HANDLER 77 /* CPU0 IRQ handler */
    IRQ_HANDLER 78 /* DEBUG0 IRQ handler */
    IRQ_HANDLER 79 /* DEBUG1 IRQ handler */

#endif

#ifdef __IAR_SYSTEMS_ASM__

IRQ_S_HANDLER macro
    dc32 default_isr_s_\1
    endm

IRQ_DEFAULT_S_HANDLER macro
    PUBWEAK default_isr_s_\1
default_isr_s_\1
    j default_irq_s_handler
    endm

    SECTION `.isr_s_vector`:CODE:ROOT(9)
    PUBWEAK default_irq_s_handler
default_irq_s_handler
    j default_irq_s_handler
    IRQ_DEFAULT_S_HANDLER 1 /* GPIO0_A IRQ handler */
    IRQ_DEFAULT_S_HANDLER 2 /* GPIO0_B IRQ handler */
    IRQ_DEFAULT_S_HANDLER 3 /* GPIO0_C IRQ handler */
    IRQ_DEFAULT_S_HANDLER 4 /* GPIO0_D IRQ handler */
    IRQ_DEFAULT_S_HANDLER 5 /* GPIO0_E IRQ handler */
    IRQ_DEFAULT_S_HANDLER 6 /* GPIO0_F IRQ handler */
    IRQ_DEFAULT_S_HANDLER 7 /* GPIO0_V IRQ handler */
    IRQ_DEFAULT_S_HANDLER 8 /* GPIO0_W IRQ handler */
    IRQ_DEFAULT_S_HANDLER 9 /* GPIO0_X IRQ handler */
    IRQ_DEFAULT_S_HANDLER 10 /* GPIO0_Y IRQ handler */
    IRQ_DEFAULT_S_HANDLER 11 /* GPTMR0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 12 /* GPTMR1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 13 /* GPTMR2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 14 /* GPTMR3 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 15 /* OWR0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 16 /* OWR1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 17 /* EUI0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 18 /* EUI1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 19 /* UART0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 20 /* UART1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 21 /* UART2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 22 /* UART3 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 23 /* UART4 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 24 /* UART5 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 25 /* UART6 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 26 /* UART7 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 27 /* I2C0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 28 /* I2C1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 29 /* I2C2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 30 /* I2C3 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 31 /* SPI0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 32 /* SPI1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 33 /* SPI2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 34 /* SPI3 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 35 /* TSNS IRQ handler */
    IRQ_DEFAULT_S_HANDLER 36 /* MBX0A IRQ handler */
    IRQ_DEFAULT_S_HANDLER 37 /* MBX0B IRQ handler */
    IRQ_DEFAULT_S_HANDLER 38 /* EWDG0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 39 /* EWDG1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 40 /* HDMA IRQ handler */
    IRQ_DEFAULT_S_HANDLER 41 /* LOBS IRQ handler */
    IRQ_DEFAULT_S_HANDLER 42 /* ADC0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 43 /* ADC1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 44 /* ACMP0[0] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 45 /* ACMP0[1] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 46 /* MCAN0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 47 /* MCAN1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 48 /* MCAN2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 49 /* MCAN3 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 50 /* PTPC IRQ handler */
    IRQ_DEFAULT_S_HANDLER 51 /* QEI0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 52 /* QEI1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 53 /* PWM0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 54 /* PWM1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 55 /* SDM0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 56 /* TRGM[0] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 57 /* TRGM[1] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 58 /* ENET0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 59 /* NTMR0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 60 /* USB0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 61 /* ESC IRQ handler */
    IRQ_DEFAULT_S_HANDLER 62 /* ESC_SYNC0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 63 /* ESC_SYNC1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 64 /* ESC_RESET IRQ handler */
    IRQ_DEFAULT_S_HANDLER 65 /* XPI0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 66 /* PPI IRQ handler */
    IRQ_DEFAULT_S_HANDLER 67 /* XDMA IRQ handler */
    IRQ_DEFAULT_S_HANDLER 68 /* PGPIO IRQ handler */
    IRQ_DEFAULT_S_HANDLER 69 /* PEWDG IRQ handler */
    IRQ_DEFAULT_S_HANDLER 70 /* PTMR IRQ handler */
    IRQ_DEFAULT_S_HANDLER 71 /* PUART IRQ handler */
    IRQ_DEFAULT_S_HANDLER 72 /* FUSE IRQ handler */
    IRQ_DEFAULT_S_HANDLER 73 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_DEFAULT_S_HANDLER 74 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_DEFAULT_S_HANDLER 75 /* BROWNOUT IRQ handler */
    IRQ_DEFAULT_S_HANDLER 76 /* SYSCTL IRQ handler */
    IRQ_DEFAULT_S_HANDLER 77 /* CPU0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 78 /* DEBUG0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 79 /* DEBUG1 IRQ handler */

    EXTERN irq_handler_s_trap
    SECTION `.vector_s_table`:CODE:ROOT(9)
    PUBLIC __vector_s_table
    DATA

__vector_s_table
#if (!defined(USE_NONVECTOR_MODE) || (USE_NONVECTOR_MODE == 0)) && defined(CONFIG_FREERTOS)
    dc32 freertos_risc_v_trap_handler
#else
    dc32 irq_handler_s_trap
#endif
    IRQ_S_HANDLER 1 /* GPIO0_A IRQ handler */
    IRQ_S_HANDLER 2 /* GPIO0_B IRQ handler */
    IRQ_S_HANDLER 3 /* GPIO0_C IRQ handler */
    IRQ_S_HANDLER 4 /* GPIO0_D IRQ handler */
    IRQ_S_HANDLER 5 /* GPIO0_E IRQ handler */
    IRQ_S_HANDLER 6 /* GPIO0_F IRQ handler */
    IRQ_S_HANDLER 7 /* GPIO0_V IRQ handler */
    IRQ_S_HANDLER 8 /* GPIO0_W IRQ handler */
    IRQ_S_HANDLER 9 /* GPIO0_X IRQ handler */
    IRQ_S_HANDLER 10 /* GPIO0_Y IRQ handler */
    IRQ_S_HANDLER 11 /* GPTMR0 IRQ handler */
    IRQ_S_HANDLER 12 /* GPTMR1 IRQ handler */
    IRQ_S_HANDLER 13 /* GPTMR2 IRQ handler */
    IRQ_S_HANDLER 14 /* GPTMR3 IRQ handler */
    IRQ_S_HANDLER 15 /* OWR0 IRQ handler */
    IRQ_S_HANDLER 16 /* OWR1 IRQ handler */
    IRQ_S_HANDLER 17 /* EUI0 IRQ handler */
    IRQ_S_HANDLER 18 /* EUI1 IRQ handler */
    IRQ_S_HANDLER 19 /* UART0 IRQ handler */
    IRQ_S_HANDLER 20 /* UART1 IRQ handler */
    IRQ_S_HANDLER 21 /* UART2 IRQ handler */
    IRQ_S_HANDLER 22 /* UART3 IRQ handler */
    IRQ_S_HANDLER 23 /* UART4 IRQ handler */
    IRQ_S_HANDLER 24 /* UART5 IRQ handler */
    IRQ_S_HANDLER 25 /* UART6 IRQ handler */
    IRQ_S_HANDLER 26 /* UART7 IRQ handler */
    IRQ_S_HANDLER 27 /* I2C0 IRQ handler */
    IRQ_S_HANDLER 28 /* I2C1 IRQ handler */
    IRQ_S_HANDLER 29 /* I2C2 IRQ handler */
    IRQ_S_HANDLER 30 /* I2C3 IRQ handler */
    IRQ_S_HANDLER 31 /* SPI0 IRQ handler */
    IRQ_S_HANDLER 32 /* SPI1 IRQ handler */
    IRQ_S_HANDLER 33 /* SPI2 IRQ handler */
    IRQ_S_HANDLER 34 /* SPI3 IRQ handler */
    IRQ_S_HANDLER 35 /* TSNS IRQ handler */
    IRQ_S_HANDLER 36 /* MBX0A IRQ handler */
    IRQ_S_HANDLER 37 /* MBX0B IRQ handler */
    IRQ_S_HANDLER 38 /* EWDG0 IRQ handler */
    IRQ_S_HANDLER 39 /* EWDG1 IRQ handler */
    IRQ_S_HANDLER 40 /* HDMA IRQ handler */
    IRQ_S_HANDLER 41 /* LOBS IRQ handler */
    IRQ_S_HANDLER 42 /* ADC0 IRQ handler */
    IRQ_S_HANDLER 43 /* ADC1 IRQ handler */
    IRQ_S_HANDLER 44 /* ACMP0[0] IRQ handler */
    IRQ_S_HANDLER 45 /* ACMP0[1] IRQ handler */
    IRQ_S_HANDLER 46 /* MCAN0 IRQ handler */
    IRQ_S_HANDLER 47 /* MCAN1 IRQ handler */
    IRQ_S_HANDLER 48 /* MCAN2 IRQ handler */
    IRQ_S_HANDLER 49 /* MCAN3 IRQ handler */
    IRQ_S_HANDLER 50 /* PTPC IRQ handler */
    IRQ_S_HANDLER 51 /* QEI0 IRQ handler */
    IRQ_S_HANDLER 52 /* QEI1 IRQ handler */
    IRQ_S_HANDLER 53 /* PWM0 IRQ handler */
    IRQ_S_HANDLER 54 /* PWM1 IRQ handler */
    IRQ_S_HANDLER 55 /* SDM0 IRQ handler */
    IRQ_S_HANDLER 56 /* TRGM[0] IRQ handler */
    IRQ_S_HANDLER 57 /* TRGM[1] IRQ handler */
    IRQ_S_HANDLER 58 /* ENET0 IRQ handler */
    IRQ_S_HANDLER 59 /* NTMR0 IRQ handler */
    IRQ_S_HANDLER 60 /* USB0 IRQ handler */
    IRQ_S_HANDLER 61 /* ESC IRQ handler */
    IRQ_S_HANDLER 62 /* ESC_SYNC0 IRQ handler */
    IRQ_S_HANDLER 63 /* ESC_SYNC1 IRQ handler */
    IRQ_S_HANDLER 64 /* ESC_RESET IRQ handler */
    IRQ_S_HANDLER 65 /* XPI0 IRQ handler */
    IRQ_S_HANDLER 66 /* PPI IRQ handler */
    IRQ_S_HANDLER 67 /* XDMA IRQ handler */
    IRQ_S_HANDLER 68 /* PGPIO IRQ handler */
    IRQ_S_HANDLER 69 /* PEWDG IRQ handler */
    IRQ_S_HANDLER 70 /* PTMR IRQ handler */
    IRQ_S_HANDLER 71 /* PUART IRQ handler */
    IRQ_S_HANDLER 72 /* FUSE IRQ handler */
    IRQ_S_HANDLER 73 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_S_HANDLER 74 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_S_HANDLER 75 /* BROWNOUT IRQ handler */
    IRQ_S_HANDLER 76 /* SYSCTL IRQ handler */
    IRQ_S_HANDLER 77 /* CPU0 IRQ handler */
    IRQ_S_HANDLER 78 /* DEBUG0 IRQ handler */
    IRQ_S_HANDLER 79 /* DEBUG1 IRQ handler */

#else

.weak default_irq_s_handler
.align 2
default_irq_s_handler:
1:    j 1b

.macro IRQ_S_HANDLER irq
  .weak default_isr_s_\irq
  .set default_isr_s_\irq, default_irq_s_handler
  .long default_isr_s_\irq
.endm

.section .vector_s_table, "a"
.global __vector_s_table
.align 9

#if (!defined(USE_NONVECTOR_MODE) || (USE_NONVECTOR_MODE == 0)) && defined(CONFIG_FREERTOS)
    .set default_isr_s_trap, freertos_risc_v_trap_handler
#else
    .set default_isr_s_trap, irq_handler_s_trap
#endif

__vector_s_table:
    .weak default_isr_s_trap
    .long default_isr_s_trap
    IRQ_S_HANDLER 1 /* GPIO0_A IRQ handler */
    IRQ_S_HANDLER 2 /* GPIO0_B IRQ handler */
    IRQ_S_HANDLER 3 /* GPIO0_C IRQ handler */
    IRQ_S_HANDLER 4 /* GPIO0_D IRQ handler */
    IRQ_S_HANDLER 5 /* GPIO0_E IRQ handler */
    IRQ_S_HANDLER 6 /* GPIO0_F IRQ handler */
    IRQ_S_HANDLER 7 /* GPIO0_V IRQ handler */
    IRQ_S_HANDLER 8 /* GPIO0_W IRQ handler */
    IRQ_S_HANDLER 9 /* GPIO0_X IRQ handler */
    IRQ_S_HANDLER 10 /* GPIO0_Y IRQ handler */
    IRQ_S_HANDLER 11 /* GPTMR0 IRQ handler */
    IRQ_S_HANDLER 12 /* GPTMR1 IRQ handler */
    IRQ_S_HANDLER 13 /* GPTMR2 IRQ handler */
    IRQ_S_HANDLER 14 /* GPTMR3 IRQ handler */
    IRQ_S_HANDLER 15 /* OWR0 IRQ handler */
    IRQ_S_HANDLER 16 /* OWR1 IRQ handler */
    IRQ_S_HANDLER 17 /* EUI0 IRQ handler */
    IRQ_S_HANDLER 18 /* EUI1 IRQ handler */
    IRQ_S_HANDLER 19 /* UART0 IRQ handler */
    IRQ_S_HANDLER 20 /* UART1 IRQ handler */
    IRQ_S_HANDLER 21 /* UART2 IRQ handler */
    IRQ_S_HANDLER 22 /* UART3 IRQ handler */
    IRQ_S_HANDLER 23 /* UART4 IRQ handler */
    IRQ_S_HANDLER 24 /* UART5 IRQ handler */
    IRQ_S_HANDLER 25 /* UART6 IRQ handler */
    IRQ_S_HANDLER 26 /* UART7 IRQ handler */
    IRQ_S_HANDLER 27 /* I2C0 IRQ handler */
    IRQ_S_HANDLER 28 /* I2C1 IRQ handler */
    IRQ_S_HANDLER 29 /* I2C2 IRQ handler */
    IRQ_S_HANDLER 30 /* I2C3 IRQ handler */
    IRQ_S_HANDLER 31 /* SPI0 IRQ handler */
    IRQ_S_HANDLER 32 /* SPI1 IRQ handler */
    IRQ_S_HANDLER 33 /* SPI2 IRQ handler */
    IRQ_S_HANDLER 34 /* SPI3 IRQ handler */
    IRQ_S_HANDLER 35 /* TSNS IRQ handler */
    IRQ_S_HANDLER 36 /* MBX0A IRQ handler */
    IRQ_S_HANDLER 37 /* MBX0B IRQ handler */
    IRQ_S_HANDLER 38 /* EWDG0 IRQ handler */
    IRQ_S_HANDLER 39 /* EWDG1 IRQ handler */
    IRQ_S_HANDLER 40 /* HDMA IRQ handler */
    IRQ_S_HANDLER 41 /* LOBS IRQ handler */
    IRQ_S_HANDLER 42 /* ADC0 IRQ handler */
    IRQ_S_HANDLER 43 /* ADC1 IRQ handler */
    IRQ_S_HANDLER 44 /* ACMP0[0] IRQ handler */
    IRQ_S_HANDLER 45 /* ACMP0[1] IRQ handler */
    IRQ_S_HANDLER 46 /* MCAN0 IRQ handler */
    IRQ_S_HANDLER 47 /* MCAN1 IRQ handler */
    IRQ_S_HANDLER 48 /* MCAN2 IRQ handler */
    IRQ_S_HANDLER 49 /* MCAN3 IRQ handler */
    IRQ_S_HANDLER 50 /* PTPC IRQ handler */
    IRQ_S_HANDLER 51 /* QEI0 IRQ handler */
    IRQ_S_HANDLER 52 /* QEI1 IRQ handler */
    IRQ_S_HANDLER 53 /* PWM0 IRQ handler */
    IRQ_S_HANDLER 54 /* PWM1 IRQ handler */
    IRQ_S_HANDLER 55 /* SDM0 IRQ handler */
    IRQ_S_HANDLER 56 /* TRGM[0] IRQ handler */
    IRQ_S_HANDLER 57 /* TRGM[1] IRQ handler */
    IRQ_S_HANDLER 58 /* ENET0 IRQ handler */
    IRQ_S_HANDLER 59 /* NTMR0 IRQ handler */
    IRQ_S_HANDLER 60 /* USB0 IRQ handler */
    IRQ_S_HANDLER 61 /* ESC IRQ handler */
    IRQ_S_HANDLER 62 /* ESC_SYNC0 IRQ handler */
    IRQ_S_HANDLER 63 /* ESC_SYNC1 IRQ handler */
    IRQ_S_HANDLER 64 /* ESC_RESET IRQ handler */
    IRQ_S_HANDLER 65 /* XPI0 IRQ handler */
    IRQ_S_HANDLER 66 /* PPI IRQ handler */
    IRQ_S_HANDLER 67 /* XDMA IRQ handler */
    IRQ_S_HANDLER 68 /* PGPIO IRQ handler */
    IRQ_S_HANDLER 69 /* PEWDG IRQ handler */
    IRQ_S_HANDLER 70 /* PTMR IRQ handler */
    IRQ_S_HANDLER 71 /* PUART IRQ handler */
    IRQ_S_HANDLER 72 /* FUSE IRQ handler */
    IRQ_S_HANDLER 73 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_S_HANDLER 74 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_S_HANDLER 75 /* BROWNOUT IRQ handler */
    IRQ_S_HANDLER 76 /* SYSCTL IRQ handler */
    IRQ_S_HANDLER 77 /* CPU0 IRQ handler */
    IRQ_S_HANDLER 78 /* DEBUG0 IRQ handler */
    IRQ_S_HANDLER 79 /* DEBUG1 IRQ handler */

#endif
