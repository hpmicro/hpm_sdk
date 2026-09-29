/*
 * Copyright (c) 2026 HPMicro
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

    SECTION `.isr_vector`:CODE:ROOT(8)
    PUBWEAK default_irq_handler
default_irq_handler
    j default_irq_handler
    IRQ_DEFAULT_HANDLER 1 /* GPIO0_A IRQ handler */
    IRQ_DEFAULT_HANDLER 2 /* GPIO0_B IRQ handler */
    IRQ_DEFAULT_HANDLER 3 /* GPIO0_C IRQ handler */
    IRQ_DEFAULT_HANDLER 4 /* GPIO0_X IRQ handler */
    IRQ_DEFAULT_HANDLER 5 /* GPIO0_Y IRQ handler */
    IRQ_DEFAULT_HANDLER 6 /* GPTMR0 IRQ handler */
    IRQ_DEFAULT_HANDLER 7 /* GPTMR1 IRQ handler */
    IRQ_DEFAULT_HANDLER 8 /* UART0 IRQ handler */
    IRQ_DEFAULT_HANDLER 9 /* UART1 IRQ handler */
    IRQ_DEFAULT_HANDLER 10 /* UART2 IRQ handler */
    IRQ_DEFAULT_HANDLER 11 /* UART3 IRQ handler */
    IRQ_DEFAULT_HANDLER 12 /* I2C0 IRQ handler */
    IRQ_DEFAULT_HANDLER 13 /* I2C1 IRQ handler */
    IRQ_DEFAULT_HANDLER 14 /* I2C2 IRQ handler */
    IRQ_DEFAULT_HANDLER 15 /* SPI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 16 /* SPI1 IRQ handler */
    IRQ_DEFAULT_HANDLER 17 /* SPI2 IRQ handler */
    IRQ_DEFAULT_HANDLER 18 /* SPI3 IRQ handler */
    IRQ_DEFAULT_HANDLER 19 /* TSNS IRQ handler */
    IRQ_DEFAULT_HANDLER 20 /* MBX0A IRQ handler */
    IRQ_DEFAULT_HANDLER 21 /* MBX0B IRQ handler */
    IRQ_DEFAULT_HANDLER 22 /* EWDG0 IRQ handler */
    IRQ_DEFAULT_HANDLER 23 /* EWDG1 IRQ handler */
    IRQ_DEFAULT_HANDLER 24 /* HDMA0 IRQ handler */
    IRQ_DEFAULT_HANDLER 25 /* ADC0 IRQ handler */
    IRQ_DEFAULT_HANDLER 26 /* ADC1 IRQ handler */
    IRQ_DEFAULT_HANDLER 27 /* ADC2 IRQ handler */
    IRQ_DEFAULT_HANDLER 28 /* ADC3 IRQ handler */
    IRQ_DEFAULT_HANDLER 29 /* ACMP0[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 30 /* ACMP0[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 31 /* ACMP1[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 32 /* ACMP1[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 33 /* I2S0 IRQ handler */
    IRQ_DEFAULT_HANDLER 34 /* I2S1 IRQ handler */
    IRQ_DEFAULT_HANDLER 35 /* MCAN0 IRQ handler */
    IRQ_DEFAULT_HANDLER 36 /* PTPC IRQ handler */
    IRQ_DEFAULT_HANDLER 37 /* PWM0 IRQ handler */
    IRQ_DEFAULT_HANDLER 38 /* PWM1 IRQ handler */
    IRQ_DEFAULT_HANDLER 39 /* PWM2 IRQ handler */
    IRQ_DEFAULT_HANDLER 40 /* PWM3 IRQ handler */
    IRQ_DEFAULT_HANDLER 41 /* TRGM[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 42 /* TRGM[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 43 /* USB0 IRQ handler */
    IRQ_DEFAULT_HANDLER 44 /* XPI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 45 /* Reserved */
    IRQ_DEFAULT_HANDLER 46 /* Reserved */
    IRQ_DEFAULT_HANDLER 47 /* HDMA1 IRQ handler */
    IRQ_DEFAULT_HANDLER 48 /* PGPIO IRQ handler */
    IRQ_DEFAULT_HANDLER 49 /* PEWDG IRQ handler */
    IRQ_DEFAULT_HANDLER 50 /* PTMR IRQ handler */
    IRQ_DEFAULT_HANDLER 51 /* PUART IRQ handler */
    IRQ_DEFAULT_HANDLER 52 /* FUSE IRQ handler */
    IRQ_DEFAULT_HANDLER 53 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_DEFAULT_HANDLER 54 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_DEFAULT_HANDLER 55 /* BROWNOUT IRQ handler */
    IRQ_DEFAULT_HANDLER 56 /* SYSCTL IRQ handler */
    IRQ_DEFAULT_HANDLER 57 /* DEBUG0 IRQ handler */
    IRQ_DEFAULT_HANDLER 58 /* DEBUG1 IRQ handler */

    EXTERN irq_handler_trap
    SECTION `.vector_table`:CODE:ROOT(8)
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
    IRQ_HANDLER 4 /* GPIO0_X IRQ handler */
    IRQ_HANDLER 5 /* GPIO0_Y IRQ handler */
    IRQ_HANDLER 6 /* GPTMR0 IRQ handler */
    IRQ_HANDLER 7 /* GPTMR1 IRQ handler */
    IRQ_HANDLER 8 /* UART0 IRQ handler */
    IRQ_HANDLER 9 /* UART1 IRQ handler */
    IRQ_HANDLER 10 /* UART2 IRQ handler */
    IRQ_HANDLER 11 /* UART3 IRQ handler */
    IRQ_HANDLER 12 /* I2C0 IRQ handler */
    IRQ_HANDLER 13 /* I2C1 IRQ handler */
    IRQ_HANDLER 14 /* I2C2 IRQ handler */
    IRQ_HANDLER 15 /* SPI0 IRQ handler */
    IRQ_HANDLER 16 /* SPI1 IRQ handler */
    IRQ_HANDLER 17 /* SPI2 IRQ handler */
    IRQ_HANDLER 18 /* SPI3 IRQ handler */
    IRQ_HANDLER 19 /* TSNS IRQ handler */
    IRQ_HANDLER 20 /* MBX0A IRQ handler */
    IRQ_HANDLER 21 /* MBX0B IRQ handler */
    IRQ_HANDLER 22 /* EWDG0 IRQ handler */
    IRQ_HANDLER 23 /* EWDG1 IRQ handler */
    IRQ_HANDLER 24 /* HDMA0 IRQ handler */
    IRQ_HANDLER 25 /* ADC0 IRQ handler */
    IRQ_HANDLER 26 /* ADC1 IRQ handler */
    IRQ_HANDLER 27 /* ADC2 IRQ handler */
    IRQ_HANDLER 28 /* ADC3 IRQ handler */
    IRQ_HANDLER 29 /* ACMP0[0] IRQ handler */
    IRQ_HANDLER 30 /* ACMP0[1] IRQ handler */
    IRQ_HANDLER 31 /* ACMP1[0] IRQ handler */
    IRQ_HANDLER 32 /* ACMP1[1] IRQ handler */
    IRQ_HANDLER 33 /* I2S0 IRQ handler */
    IRQ_HANDLER 34 /* I2S1 IRQ handler */
    IRQ_HANDLER 35 /* MCAN0 IRQ handler */
    IRQ_HANDLER 36 /* PTPC IRQ handler */
    IRQ_HANDLER 37 /* PWM0 IRQ handler */
    IRQ_HANDLER 38 /* PWM1 IRQ handler */
    IRQ_HANDLER 39 /* PWM2 IRQ handler */
    IRQ_HANDLER 40 /* PWM3 IRQ handler */
    IRQ_HANDLER 41 /* TRGM[0] IRQ handler */
    IRQ_HANDLER 42 /* TRGM[1] IRQ handler */
    IRQ_HANDLER 43 /* USB0 IRQ handler */
    IRQ_HANDLER 44 /* XPI0 IRQ handler */
    IRQ_HANDLER 45 /* Reserved */
    IRQ_HANDLER 46 /* Reserved */
    IRQ_HANDLER 47 /* HDMA1 IRQ handler */
    IRQ_HANDLER 48 /* PGPIO IRQ handler */
    IRQ_HANDLER 49 /* PEWDG IRQ handler */
    IRQ_HANDLER 50 /* PTMR IRQ handler */
    IRQ_HANDLER 51 /* PUART IRQ handler */
    IRQ_HANDLER 52 /* FUSE IRQ handler */
    IRQ_HANDLER 53 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_HANDLER 54 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_HANDLER 55 /* BROWNOUT IRQ handler */
    IRQ_HANDLER 56 /* SYSCTL IRQ handler */
    IRQ_HANDLER 57 /* DEBUG0 IRQ handler */
    IRQ_HANDLER 58 /* DEBUG1 IRQ handler */

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
.align 8

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
    IRQ_HANDLER 4 /* GPIO0_X IRQ handler */
    IRQ_HANDLER 5 /* GPIO0_Y IRQ handler */
    IRQ_HANDLER 6 /* GPTMR0 IRQ handler */
    IRQ_HANDLER 7 /* GPTMR1 IRQ handler */
    IRQ_HANDLER 8 /* UART0 IRQ handler */
    IRQ_HANDLER 9 /* UART1 IRQ handler */
    IRQ_HANDLER 10 /* UART2 IRQ handler */
    IRQ_HANDLER 11 /* UART3 IRQ handler */
    IRQ_HANDLER 12 /* I2C0 IRQ handler */
    IRQ_HANDLER 13 /* I2C1 IRQ handler */
    IRQ_HANDLER 14 /* I2C2 IRQ handler */
    IRQ_HANDLER 15 /* SPI0 IRQ handler */
    IRQ_HANDLER 16 /* SPI1 IRQ handler */
    IRQ_HANDLER 17 /* SPI2 IRQ handler */
    IRQ_HANDLER 18 /* SPI3 IRQ handler */
    IRQ_HANDLER 19 /* TSNS IRQ handler */
    IRQ_HANDLER 20 /* MBX0A IRQ handler */
    IRQ_HANDLER 21 /* MBX0B IRQ handler */
    IRQ_HANDLER 22 /* EWDG0 IRQ handler */
    IRQ_HANDLER 23 /* EWDG1 IRQ handler */
    IRQ_HANDLER 24 /* HDMA0 IRQ handler */
    IRQ_HANDLER 25 /* ADC0 IRQ handler */
    IRQ_HANDLER 26 /* ADC1 IRQ handler */
    IRQ_HANDLER 27 /* ADC2 IRQ handler */
    IRQ_HANDLER 28 /* ADC3 IRQ handler */
    IRQ_HANDLER 29 /* ACMP0[0] IRQ handler */
    IRQ_HANDLER 30 /* ACMP0[1] IRQ handler */
    IRQ_HANDLER 31 /* ACMP1[0] IRQ handler */
    IRQ_HANDLER 32 /* ACMP1[1] IRQ handler */
    IRQ_HANDLER 33 /* I2S0 IRQ handler */
    IRQ_HANDLER 34 /* I2S1 IRQ handler */
    IRQ_HANDLER 35 /* MCAN0 IRQ handler */
    IRQ_HANDLER 36 /* PTPC IRQ handler */
    IRQ_HANDLER 37 /* PWM0 IRQ handler */
    IRQ_HANDLER 38 /* PWM1 IRQ handler */
    IRQ_HANDLER 39 /* PWM2 IRQ handler */
    IRQ_HANDLER 40 /* PWM3 IRQ handler */
    IRQ_HANDLER 41 /* TRGM[0] IRQ handler */
    IRQ_HANDLER 42 /* TRGM[1] IRQ handler */
    IRQ_HANDLER 43 /* USB0 IRQ handler */
    IRQ_HANDLER 44 /* XPI0 IRQ handler */
    IRQ_HANDLER 45 /* Reserved */
    IRQ_HANDLER 46 /* Reserved */
    IRQ_HANDLER 47 /* HDMA1 IRQ handler */
    IRQ_HANDLER 48 /* PGPIO IRQ handler */
    IRQ_HANDLER 49 /* PEWDG IRQ handler */
    IRQ_HANDLER 50 /* PTMR IRQ handler */
    IRQ_HANDLER 51 /* PUART IRQ handler */
    IRQ_HANDLER 52 /* FUSE IRQ handler */
    IRQ_HANDLER 53 /* DGO_PAD_WAKEUP IRQ handler */
    IRQ_HANDLER 54 /* DGO_CNT_WAKEUP IRQ handler */
    IRQ_HANDLER 55 /* BROWNOUT IRQ handler */
    IRQ_HANDLER 56 /* SYSCTL IRQ handler */
    IRQ_HANDLER 57 /* DEBUG0 IRQ handler */
    IRQ_HANDLER 58 /* DEBUG1 IRQ handler */

#endif
