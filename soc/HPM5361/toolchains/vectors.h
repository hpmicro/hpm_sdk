/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
.global default_irq_handler
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

__vector_table:
    .weak default_isr_trap
    .set default_isr_trap, irq_handler_trap
    .long default_isr_trap
    IRQ_HANDLER 1 /* GPIO0_A IRQ handler */
    IRQ_HANDLER 2 /* GPIO0_B IRQ handler */
    IRQ_HANDLER 3 /* GPIO0_X IRQ handler */
    IRQ_HANDLER 4 /* GPIO0_Y IRQ handler */
    IRQ_HANDLER 5 /* GPTMR0 IRQ handler */
    IRQ_HANDLER 6 /* GPTMR1 IRQ handler */
    IRQ_HANDLER 7 /* GPTMR2 IRQ handler */
    IRQ_HANDLER 8 /* GPTMR3 IRQ handler */
    IRQ_HANDLER 9 /* LIN0 IRQ handler */
    IRQ_HANDLER 10 /* LIN1 IRQ handler */
    IRQ_HANDLER 11 /* LIN2 IRQ handler */
    IRQ_HANDLER 12 /* LIN3 IRQ handler */
    IRQ_HANDLER 13 /* UART0 IRQ handler */
    IRQ_HANDLER 14 /* UART1 IRQ handler */
    IRQ_HANDLER 15 /* UART2 IRQ handler */
    IRQ_HANDLER 16 /* UART3 IRQ handler */
    IRQ_HANDLER 17 /* UART4 IRQ handler */
    IRQ_HANDLER 18 /* UART5 IRQ handler */
    IRQ_HANDLER 19 /* UART6 IRQ handler */
    IRQ_HANDLER 20 /* UART7 IRQ handler */
    IRQ_HANDLER 21 /* I2C0 IRQ handler */
    IRQ_HANDLER 22 /* I2C1 IRQ handler */
    IRQ_HANDLER 23 /* I2C2 IRQ handler */
    IRQ_HANDLER 24 /* I2C3 IRQ handler */
    IRQ_HANDLER 25 /* SPI0 IRQ handler */
    IRQ_HANDLER 26 /* SPI1 IRQ handler */
    IRQ_HANDLER 27 /* SPI2 IRQ handler */
    IRQ_HANDLER 28 /* SPI3 IRQ handler */
    IRQ_HANDLER 29 /* TSNS IRQ handler */
    IRQ_HANDLER 30 /* MBX0A IRQ handler */
    IRQ_HANDLER 31 /* MBX0B IRQ handler */
    IRQ_HANDLER 32 /* WDG0 IRQ handler */
    IRQ_HANDLER 33 /* WDG1 IRQ handler */
    IRQ_HANDLER 34 /* HDMA IRQ handler */
    IRQ_HANDLER 35 /* CAN0 IRQ handler */
    IRQ_HANDLER 36 /* CAN1 IRQ handler */
    IRQ_HANDLER 37 /* CAN2 IRQ handler */
    IRQ_HANDLER 38 /* CAN3 IRQ handler */
    IRQ_HANDLER 39 /* PTPC IRQ handler */
    IRQ_HANDLER 40 /* PWM0 IRQ handler */
    IRQ_HANDLER 41 /* QEI0 IRQ handler */
    IRQ_HANDLER 42 /* SEI0 IRQ handler */
    IRQ_HANDLER 43 /* MMC0 IRQ handler */
    IRQ_HANDLER 44 /* TRGMUX0 IRQ handler */
    IRQ_HANDLER 45 /* PWM1 IRQ handler */
    IRQ_HANDLER 46 /* QEI1 IRQ handler */
    IRQ_HANDLER 47 /* SEI1 IRQ handler */
    IRQ_HANDLER 48 /* MMC1 IRQ handler */
    IRQ_HANDLER 49 /* TRGMUX1 IRQ handler */
    IRQ_HANDLER 50 /* RDC IRQ handler */
    IRQ_HANDLER 51 /* USB0 IRQ handler */
    IRQ_HANDLER 52 /* XPI0 IRQ handler */
    IRQ_HANDLER 53 /* SDP IRQ handler */
    IRQ_HANDLER 54 /* PSEC IRQ handler */
    IRQ_HANDLER 55 /* SECMON IRQ handler */
    IRQ_HANDLER 56 /* RNG IRQ handler */
    IRQ_HANDLER 57 /* FUSE IRQ handler */
    IRQ_HANDLER 58 /* ADC0 IRQ handler */
    IRQ_HANDLER 59 /* ADC1 IRQ handler */
    IRQ_HANDLER 60 /* DAC0 IRQ handler */
    IRQ_HANDLER 61 /* DAC1 IRQ handler */
    IRQ_HANDLER 62 /* ACMP_0 IRQ handler */
    IRQ_HANDLER 63 /* ACMP_1 IRQ handler */
    IRQ_HANDLER 64 /* SYSCTL IRQ handler */
    IRQ_HANDLER 65 /* PGPIO IRQ handler */
    IRQ_HANDLER 66 /* PTMR IRQ handler */
    IRQ_HANDLER 67 /* PUART IRQ handler */
    IRQ_HANDLER 68 /* PWDG IRQ handler */
    IRQ_HANDLER 69 /* BROWNOUT IRQ handler */
    IRQ_HANDLER 70 /* PAD_WAKEUP IRQ handler */
    IRQ_HANDLER 71 /* DEBUG0 IRQ handler */
    IRQ_HANDLER 72 /* DEBUG1 IRQ handler */

