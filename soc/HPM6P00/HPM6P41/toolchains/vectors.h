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
    IRQ_DEFAULT_HANDLER 5 /* GPIO0_X IRQ handler */
    IRQ_DEFAULT_HANDLER 6 /* GPIO0_Y IRQ handler */
    IRQ_DEFAULT_HANDLER 7 /* GPIO1_A IRQ handler */
    IRQ_DEFAULT_HANDLER 8 /* GPIO1_B IRQ handler */
    IRQ_DEFAULT_HANDLER 9 /* GPIO1_C IRQ handler */
    IRQ_DEFAULT_HANDLER 10 /* GPIO1_D IRQ handler */
    IRQ_DEFAULT_HANDLER 11 /* GPIO1_X IRQ handler */
    IRQ_DEFAULT_HANDLER 12 /* GPIO1_Y IRQ handler */
    IRQ_DEFAULT_HANDLER 13 /* GPTMR0 IRQ handler */
    IRQ_DEFAULT_HANDLER 14 /* GPTMR1 IRQ handler */
    IRQ_DEFAULT_HANDLER 15 /* GPTMR2 IRQ handler */
    IRQ_DEFAULT_HANDLER 16 /* GPTMR3 IRQ handler */
    IRQ_DEFAULT_HANDLER 17 /* Reserved */
    IRQ_DEFAULT_HANDLER 18 /* Reserved */
    IRQ_DEFAULT_HANDLER 19 /* Reserved */
    IRQ_DEFAULT_HANDLER 20 /* Reserved */
    IRQ_DEFAULT_HANDLER 21 /* UART0 IRQ handler */
    IRQ_DEFAULT_HANDLER 22 /* UART1 IRQ handler */
    IRQ_DEFAULT_HANDLER 23 /* UART2 IRQ handler */
    IRQ_DEFAULT_HANDLER 24 /* UART3 IRQ handler */
    IRQ_DEFAULT_HANDLER 25 /* Reserved */
    IRQ_DEFAULT_HANDLER 26 /* Reserved */
    IRQ_DEFAULT_HANDLER 27 /* Reserved */
    IRQ_DEFAULT_HANDLER 28 /* Reserved */
    IRQ_DEFAULT_HANDLER 29 /* I2C0 IRQ handler */
    IRQ_DEFAULT_HANDLER 30 /* I2C1 IRQ handler */
    IRQ_DEFAULT_HANDLER 31 /* I2C2 IRQ handler */
    IRQ_DEFAULT_HANDLER 32 /* Reserved */
    IRQ_DEFAULT_HANDLER 33 /* SPI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 34 /* SPI1 IRQ handler */
    IRQ_DEFAULT_HANDLER 35 /* SPI2 IRQ handler */
    IRQ_DEFAULT_HANDLER 36 /* Reserved */
    IRQ_DEFAULT_HANDLER 37 /* TSNS IRQ handler */
    IRQ_DEFAULT_HANDLER 38 /* MBX0A IRQ handler */
    IRQ_DEFAULT_HANDLER 39 /* MBX0B IRQ handler */
    IRQ_DEFAULT_HANDLER 40 /* MBX1A IRQ handler */
    IRQ_DEFAULT_HANDLER 41 /* MBX1B IRQ handler */
    IRQ_DEFAULT_HANDLER 42 /* EWDG0 IRQ handler */
    IRQ_DEFAULT_HANDLER 43 /* EWDG1 IRQ handler */
    IRQ_DEFAULT_HANDLER 44 /* Reserved */
    IRQ_DEFAULT_HANDLER 45 /* Reserved */
    IRQ_DEFAULT_HANDLER 46 /* HDMA IRQ handler */
    IRQ_DEFAULT_HANDLER 47 /* LOBS IRQ handler */
    IRQ_DEFAULT_HANDLER 48 /* ADC0 IRQ handler */
    IRQ_DEFAULT_HANDLER 49 /* ADC1 IRQ handler */
    IRQ_DEFAULT_HANDLER 50 /* ADC2 IRQ handler */
    IRQ_DEFAULT_HANDLER 51 /* Reserved */
    IRQ_DEFAULT_HANDLER 52 /* DAC0 IRQ handler */
    IRQ_DEFAULT_HANDLER 53 /* DAC1 IRQ handler */
    IRQ_DEFAULT_HANDLER 54 /* ACMP0[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 55 /* ACMP0[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 56 /* ACMP1[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 57 /* ACMP1[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 58 /* ACMP2[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 59 /* ACMP2[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 60 /* ACMP3[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 61 /* ACMP3[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 62 /* Reserved */
    IRQ_DEFAULT_HANDLER 63 /* Reserved */
    IRQ_DEFAULT_HANDLER 64 /* Reserved */
    IRQ_DEFAULT_HANDLER 65 /* Reserved */
    IRQ_DEFAULT_HANDLER 66 /* MCAN0 IRQ handler */
    IRQ_DEFAULT_HANDLER 67 /* MCAN1 IRQ handler */
    IRQ_DEFAULT_HANDLER 68 /* Reserved */
    IRQ_DEFAULT_HANDLER 69 /* Reserved */
    IRQ_DEFAULT_HANDLER 70 /* PTPC IRQ handler */
    IRQ_DEFAULT_HANDLER 71 /* QEI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 72 /* QEI1 IRQ handler */
    IRQ_DEFAULT_HANDLER 73 /* PWM0 IRQ handler */
    IRQ_DEFAULT_HANDLER 74 /* PWM1 IRQ handler */
    IRQ_DEFAULT_HANDLER 75 /* PWM2 IRQ handler */
    IRQ_DEFAULT_HANDLER 76 /* Reserved */
    IRQ_DEFAULT_HANDLER 77 /* RDC0 IRQ handler */
    IRQ_DEFAULT_HANDLER 78 /* SDM0 IRQ handler */
    IRQ_DEFAULT_HANDLER 79 /* SEI0[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 80 /* SEI0[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 81 /* MTG0 IRQ handler */
    IRQ_DEFAULT_HANDLER 82 /* VSC0 IRQ handler */
    IRQ_DEFAULT_HANDLER 83 /* CLC0[0] IRQ handler */
    IRQ_DEFAULT_HANDLER 84 /* CLC0[1] IRQ handler */
    IRQ_DEFAULT_HANDLER 85 /* TRGMUX0 IRQ handler */
    IRQ_DEFAULT_HANDLER 86 /* TRGMUX1 IRQ handler */
    IRQ_DEFAULT_HANDLER 87 /* Reserved */
    IRQ_DEFAULT_HANDLER 88 /* NTMR0 IRQ handler */
    IRQ_DEFAULT_HANDLER 89 /* USB0 IRQ handler */
    IRQ_DEFAULT_HANDLER 90 /* XPI0 IRQ handler */
    IRQ_DEFAULT_HANDLER 91 /* Reserved */
    IRQ_DEFAULT_HANDLER 92 /* Reserved */
    IRQ_DEFAULT_HANDLER 93 /* XDMA IRQ handler */
    IRQ_DEFAULT_HANDLER 94 /* Reserved */
    IRQ_DEFAULT_HANDLER 95 /* SDP IRQ handler */
    IRQ_DEFAULT_HANDLER 96 /* RNG IRQ handler */
    IRQ_DEFAULT_HANDLER 97 /* PSEC IRQ handler */
    IRQ_DEFAULT_HANDLER 98 /* PGPIO IRQ handler */
    IRQ_DEFAULT_HANDLER 99 /* PEWDG IRQ handler */
    IRQ_DEFAULT_HANDLER 100 /* PTMR IRQ handler */
    IRQ_DEFAULT_HANDLER 101 /* PUART IRQ handler */
    IRQ_DEFAULT_HANDLER 102 /* FUSE IRQ handler */
    IRQ_DEFAULT_HANDLER 103 /* SECMON IRQ handler */
    IRQ_DEFAULT_HANDLER 104 /* PAD_WAKEUP IRQ handler */
    IRQ_DEFAULT_HANDLER 105 /* BROWNOUT IRQ handler */
    IRQ_DEFAULT_HANDLER 106 /* SYSCTL IRQ handler */
    IRQ_DEFAULT_HANDLER 107 /* CPU0 IRQ handler */
    IRQ_DEFAULT_HANDLER 108 /* CPU1 IRQ handler */
    IRQ_DEFAULT_HANDLER 109 /* DEBUG0 IRQ handler */
    IRQ_DEFAULT_HANDLER 110 /* DEBUG1 IRQ handler */

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
    IRQ_HANDLER 5 /* GPIO0_X IRQ handler */
    IRQ_HANDLER 6 /* GPIO0_Y IRQ handler */
    IRQ_HANDLER 7 /* GPIO1_A IRQ handler */
    IRQ_HANDLER 8 /* GPIO1_B IRQ handler */
    IRQ_HANDLER 9 /* GPIO1_C IRQ handler */
    IRQ_HANDLER 10 /* GPIO1_D IRQ handler */
    IRQ_HANDLER 11 /* GPIO1_X IRQ handler */
    IRQ_HANDLER 12 /* GPIO1_Y IRQ handler */
    IRQ_HANDLER 13 /* GPTMR0 IRQ handler */
    IRQ_HANDLER 14 /* GPTMR1 IRQ handler */
    IRQ_HANDLER 15 /* GPTMR2 IRQ handler */
    IRQ_HANDLER 16 /* GPTMR3 IRQ handler */
    IRQ_HANDLER 17 /* Reserved */
    IRQ_HANDLER 18 /* Reserved */
    IRQ_HANDLER 19 /* Reserved */
    IRQ_HANDLER 20 /* Reserved */
    IRQ_HANDLER 21 /* UART0 IRQ handler */
    IRQ_HANDLER 22 /* UART1 IRQ handler */
    IRQ_HANDLER 23 /* UART2 IRQ handler */
    IRQ_HANDLER 24 /* UART3 IRQ handler */
    IRQ_HANDLER 25 /* Reserved */
    IRQ_HANDLER 26 /* Reserved */
    IRQ_HANDLER 27 /* Reserved */
    IRQ_HANDLER 28 /* Reserved */
    IRQ_HANDLER 29 /* I2C0 IRQ handler */
    IRQ_HANDLER 30 /* I2C1 IRQ handler */
    IRQ_HANDLER 31 /* I2C2 IRQ handler */
    IRQ_HANDLER 32 /* Reserved */
    IRQ_HANDLER 33 /* SPI0 IRQ handler */
    IRQ_HANDLER 34 /* SPI1 IRQ handler */
    IRQ_HANDLER 35 /* SPI2 IRQ handler */
    IRQ_HANDLER 36 /* Reserved */
    IRQ_HANDLER 37 /* TSNS IRQ handler */
    IRQ_HANDLER 38 /* MBX0A IRQ handler */
    IRQ_HANDLER 39 /* MBX0B IRQ handler */
    IRQ_HANDLER 40 /* MBX1A IRQ handler */
    IRQ_HANDLER 41 /* MBX1B IRQ handler */
    IRQ_HANDLER 42 /* EWDG0 IRQ handler */
    IRQ_HANDLER 43 /* EWDG1 IRQ handler */
    IRQ_HANDLER 44 /* Reserved */
    IRQ_HANDLER 45 /* Reserved */
    IRQ_HANDLER 46 /* HDMA IRQ handler */
    IRQ_HANDLER 47 /* LOBS IRQ handler */
    IRQ_HANDLER 48 /* ADC0 IRQ handler */
    IRQ_HANDLER 49 /* ADC1 IRQ handler */
    IRQ_HANDLER 50 /* ADC2 IRQ handler */
    IRQ_HANDLER 51 /* Reserved */
    IRQ_HANDLER 52 /* DAC0 IRQ handler */
    IRQ_HANDLER 53 /* DAC1 IRQ handler */
    IRQ_HANDLER 54 /* ACMP0[0] IRQ handler */
    IRQ_HANDLER 55 /* ACMP0[1] IRQ handler */
    IRQ_HANDLER 56 /* ACMP1[0] IRQ handler */
    IRQ_HANDLER 57 /* ACMP1[1] IRQ handler */
    IRQ_HANDLER 58 /* ACMP2[0] IRQ handler */
    IRQ_HANDLER 59 /* ACMP2[1] IRQ handler */
    IRQ_HANDLER 60 /* ACMP3[0] IRQ handler */
    IRQ_HANDLER 61 /* ACMP3[1] IRQ handler */
    IRQ_HANDLER 62 /* Reserved */
    IRQ_HANDLER 63 /* Reserved */
    IRQ_HANDLER 64 /* Reserved */
    IRQ_HANDLER 65 /* Reserved */
    IRQ_HANDLER 66 /* MCAN0 IRQ handler */
    IRQ_HANDLER 67 /* MCAN1 IRQ handler */
    IRQ_HANDLER 68 /* Reserved */
    IRQ_HANDLER 69 /* Reserved */
    IRQ_HANDLER 70 /* PTPC IRQ handler */
    IRQ_HANDLER 71 /* QEI0 IRQ handler */
    IRQ_HANDLER 72 /* QEI1 IRQ handler */
    IRQ_HANDLER 73 /* PWM0 IRQ handler */
    IRQ_HANDLER 74 /* PWM1 IRQ handler */
    IRQ_HANDLER 75 /* PWM2 IRQ handler */
    IRQ_HANDLER 76 /* Reserved */
    IRQ_HANDLER 77 /* RDC0 IRQ handler */
    IRQ_HANDLER 78 /* SDM0 IRQ handler */
    IRQ_HANDLER 79 /* SEI0[0] IRQ handler */
    IRQ_HANDLER 80 /* SEI0[1] IRQ handler */
    IRQ_HANDLER 81 /* MTG0 IRQ handler */
    IRQ_HANDLER 82 /* VSC0 IRQ handler */
    IRQ_HANDLER 83 /* CLC0[0] IRQ handler */
    IRQ_HANDLER 84 /* CLC0[1] IRQ handler */
    IRQ_HANDLER 85 /* TRGMUX0 IRQ handler */
    IRQ_HANDLER 86 /* TRGMUX1 IRQ handler */
    IRQ_HANDLER 87 /* Reserved */
    IRQ_HANDLER 88 /* NTMR0 IRQ handler */
    IRQ_HANDLER 89 /* USB0 IRQ handler */
    IRQ_HANDLER 90 /* XPI0 IRQ handler */
    IRQ_HANDLER 91 /* Reserved */
    IRQ_HANDLER 92 /* Reserved */
    IRQ_HANDLER 93 /* XDMA IRQ handler */
    IRQ_HANDLER 94 /* Reserved */
    IRQ_HANDLER 95 /* SDP IRQ handler */
    IRQ_HANDLER 96 /* RNG IRQ handler */
    IRQ_HANDLER 97 /* PSEC IRQ handler */
    IRQ_HANDLER 98 /* PGPIO IRQ handler */
    IRQ_HANDLER 99 /* PEWDG IRQ handler */
    IRQ_HANDLER 100 /* PTMR IRQ handler */
    IRQ_HANDLER 101 /* PUART IRQ handler */
    IRQ_HANDLER 102 /* FUSE IRQ handler */
    IRQ_HANDLER 103 /* SECMON IRQ handler */
    IRQ_HANDLER 104 /* PAD_WAKEUP IRQ handler */
    IRQ_HANDLER 105 /* BROWNOUT IRQ handler */
    IRQ_HANDLER 106 /* SYSCTL IRQ handler */
    IRQ_HANDLER 107 /* CPU0 IRQ handler */
    IRQ_HANDLER 108 /* CPU1 IRQ handler */
    IRQ_HANDLER 109 /* DEBUG0 IRQ handler */
    IRQ_HANDLER 110 /* DEBUG1 IRQ handler */

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
    IRQ_HANDLER 5 /* GPIO0_X IRQ handler */
    IRQ_HANDLER 6 /* GPIO0_Y IRQ handler */
    IRQ_HANDLER 7 /* GPIO1_A IRQ handler */
    IRQ_HANDLER 8 /* GPIO1_B IRQ handler */
    IRQ_HANDLER 9 /* GPIO1_C IRQ handler */
    IRQ_HANDLER 10 /* GPIO1_D IRQ handler */
    IRQ_HANDLER 11 /* GPIO1_X IRQ handler */
    IRQ_HANDLER 12 /* GPIO1_Y IRQ handler */
    IRQ_HANDLER 13 /* GPTMR0 IRQ handler */
    IRQ_HANDLER 14 /* GPTMR1 IRQ handler */
    IRQ_HANDLER 15 /* GPTMR2 IRQ handler */
    IRQ_HANDLER 16 /* GPTMR3 IRQ handler */
    IRQ_HANDLER 17 /* Reserved */
    IRQ_HANDLER 18 /* Reserved */
    IRQ_HANDLER 19 /* Reserved */
    IRQ_HANDLER 20 /* Reserved */
    IRQ_HANDLER 21 /* UART0 IRQ handler */
    IRQ_HANDLER 22 /* UART1 IRQ handler */
    IRQ_HANDLER 23 /* UART2 IRQ handler */
    IRQ_HANDLER 24 /* UART3 IRQ handler */
    IRQ_HANDLER 25 /* Reserved */
    IRQ_HANDLER 26 /* Reserved */
    IRQ_HANDLER 27 /* Reserved */
    IRQ_HANDLER 28 /* Reserved */
    IRQ_HANDLER 29 /* I2C0 IRQ handler */
    IRQ_HANDLER 30 /* I2C1 IRQ handler */
    IRQ_HANDLER 31 /* I2C2 IRQ handler */
    IRQ_HANDLER 32 /* Reserved */
    IRQ_HANDLER 33 /* SPI0 IRQ handler */
    IRQ_HANDLER 34 /* SPI1 IRQ handler */
    IRQ_HANDLER 35 /* SPI2 IRQ handler */
    IRQ_HANDLER 36 /* Reserved */
    IRQ_HANDLER 37 /* TSNS IRQ handler */
    IRQ_HANDLER 38 /* MBX0A IRQ handler */
    IRQ_HANDLER 39 /* MBX0B IRQ handler */
    IRQ_HANDLER 40 /* MBX1A IRQ handler */
    IRQ_HANDLER 41 /* MBX1B IRQ handler */
    IRQ_HANDLER 42 /* EWDG0 IRQ handler */
    IRQ_HANDLER 43 /* EWDG1 IRQ handler */
    IRQ_HANDLER 44 /* Reserved */
    IRQ_HANDLER 45 /* Reserved */
    IRQ_HANDLER 46 /* HDMA IRQ handler */
    IRQ_HANDLER 47 /* LOBS IRQ handler */
    IRQ_HANDLER 48 /* ADC0 IRQ handler */
    IRQ_HANDLER 49 /* ADC1 IRQ handler */
    IRQ_HANDLER 50 /* ADC2 IRQ handler */
    IRQ_HANDLER 51 /* Reserved */
    IRQ_HANDLER 52 /* DAC0 IRQ handler */
    IRQ_HANDLER 53 /* DAC1 IRQ handler */
    IRQ_HANDLER 54 /* ACMP0[0] IRQ handler */
    IRQ_HANDLER 55 /* ACMP0[1] IRQ handler */
    IRQ_HANDLER 56 /* ACMP1[0] IRQ handler */
    IRQ_HANDLER 57 /* ACMP1[1] IRQ handler */
    IRQ_HANDLER 58 /* ACMP2[0] IRQ handler */
    IRQ_HANDLER 59 /* ACMP2[1] IRQ handler */
    IRQ_HANDLER 60 /* ACMP3[0] IRQ handler */
    IRQ_HANDLER 61 /* ACMP3[1] IRQ handler */
    IRQ_HANDLER 62 /* Reserved */
    IRQ_HANDLER 63 /* Reserved */
    IRQ_HANDLER 64 /* Reserved */
    IRQ_HANDLER 65 /* Reserved */
    IRQ_HANDLER 66 /* MCAN0 IRQ handler */
    IRQ_HANDLER 67 /* MCAN1 IRQ handler */
    IRQ_HANDLER 68 /* Reserved */
    IRQ_HANDLER 69 /* Reserved */
    IRQ_HANDLER 70 /* PTPC IRQ handler */
    IRQ_HANDLER 71 /* QEI0 IRQ handler */
    IRQ_HANDLER 72 /* QEI1 IRQ handler */
    IRQ_HANDLER 73 /* PWM0 IRQ handler */
    IRQ_HANDLER 74 /* PWM1 IRQ handler */
    IRQ_HANDLER 75 /* PWM2 IRQ handler */
    IRQ_HANDLER 76 /* Reserved */
    IRQ_HANDLER 77 /* RDC0 IRQ handler */
    IRQ_HANDLER 78 /* SDM0 IRQ handler */
    IRQ_HANDLER 79 /* SEI0[0] IRQ handler */
    IRQ_HANDLER 80 /* SEI0[1] IRQ handler */
    IRQ_HANDLER 81 /* MTG0 IRQ handler */
    IRQ_HANDLER 82 /* VSC0 IRQ handler */
    IRQ_HANDLER 83 /* CLC0[0] IRQ handler */
    IRQ_HANDLER 84 /* CLC0[1] IRQ handler */
    IRQ_HANDLER 85 /* TRGMUX0 IRQ handler */
    IRQ_HANDLER 86 /* TRGMUX1 IRQ handler */
    IRQ_HANDLER 87 /* Reserved */
    IRQ_HANDLER 88 /* NTMR0 IRQ handler */
    IRQ_HANDLER 89 /* USB0 IRQ handler */
    IRQ_HANDLER 90 /* XPI0 IRQ handler */
    IRQ_HANDLER 91 /* Reserved */
    IRQ_HANDLER 92 /* Reserved */
    IRQ_HANDLER 93 /* XDMA IRQ handler */
    IRQ_HANDLER 94 /* Reserved */
    IRQ_HANDLER 95 /* SDP IRQ handler */
    IRQ_HANDLER 96 /* RNG IRQ handler */
    IRQ_HANDLER 97 /* PSEC IRQ handler */
    IRQ_HANDLER 98 /* PGPIO IRQ handler */
    IRQ_HANDLER 99 /* PEWDG IRQ handler */
    IRQ_HANDLER 100 /* PTMR IRQ handler */
    IRQ_HANDLER 101 /* PUART IRQ handler */
    IRQ_HANDLER 102 /* FUSE IRQ handler */
    IRQ_HANDLER 103 /* SECMON IRQ handler */
    IRQ_HANDLER 104 /* PAD_WAKEUP IRQ handler */
    IRQ_HANDLER 105 /* BROWNOUT IRQ handler */
    IRQ_HANDLER 106 /* SYSCTL IRQ handler */
    IRQ_HANDLER 107 /* CPU0 IRQ handler */
    IRQ_HANDLER 108 /* CPU1 IRQ handler */
    IRQ_HANDLER 109 /* DEBUG0 IRQ handler */
    IRQ_HANDLER 110 /* DEBUG1 IRQ handler */

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
    IRQ_DEFAULT_S_HANDLER 5 /* GPIO0_X IRQ handler */
    IRQ_DEFAULT_S_HANDLER 6 /* GPIO0_Y IRQ handler */
    IRQ_DEFAULT_S_HANDLER 7 /* GPIO1_A IRQ handler */
    IRQ_DEFAULT_S_HANDLER 8 /* GPIO1_B IRQ handler */
    IRQ_DEFAULT_S_HANDLER 9 /* GPIO1_C IRQ handler */
    IRQ_DEFAULT_S_HANDLER 10 /* GPIO1_D IRQ handler */
    IRQ_DEFAULT_S_HANDLER 11 /* GPIO1_X IRQ handler */
    IRQ_DEFAULT_S_HANDLER 12 /* GPIO1_Y IRQ handler */
    IRQ_DEFAULT_S_HANDLER 13 /* GPTMR0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 14 /* GPTMR1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 15 /* GPTMR2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 16 /* GPTMR3 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 17 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 18 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 19 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 20 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 21 /* UART0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 22 /* UART1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 23 /* UART2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 24 /* UART3 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 25 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 26 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 27 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 28 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 29 /* I2C0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 30 /* I2C1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 31 /* I2C2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 32 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 33 /* SPI0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 34 /* SPI1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 35 /* SPI2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 36 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 37 /* TSNS IRQ handler */
    IRQ_DEFAULT_S_HANDLER 38 /* MBX0A IRQ handler */
    IRQ_DEFAULT_S_HANDLER 39 /* MBX0B IRQ handler */
    IRQ_DEFAULT_S_HANDLER 40 /* MBX1A IRQ handler */
    IRQ_DEFAULT_S_HANDLER 41 /* MBX1B IRQ handler */
    IRQ_DEFAULT_S_HANDLER 42 /* EWDG0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 43 /* EWDG1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 44 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 45 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 46 /* HDMA IRQ handler */
    IRQ_DEFAULT_S_HANDLER 47 /* LOBS IRQ handler */
    IRQ_DEFAULT_S_HANDLER 48 /* ADC0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 49 /* ADC1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 50 /* ADC2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 51 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 52 /* DAC0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 53 /* DAC1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 54 /* ACMP0[0] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 55 /* ACMP0[1] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 56 /* ACMP1[0] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 57 /* ACMP1[1] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 58 /* ACMP2[0] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 59 /* ACMP2[1] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 60 /* ACMP3[0] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 61 /* ACMP3[1] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 62 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 63 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 64 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 65 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 66 /* MCAN0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 67 /* MCAN1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 68 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 69 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 70 /* PTPC IRQ handler */
    IRQ_DEFAULT_S_HANDLER 71 /* QEI0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 72 /* QEI1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 73 /* PWM0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 74 /* PWM1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 75 /* PWM2 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 76 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 77 /* RDC0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 78 /* SDM0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 79 /* SEI0[0] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 80 /* SEI0[1] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 81 /* MTG0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 82 /* VSC0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 83 /* CLC0[0] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 84 /* CLC0[1] IRQ handler */
    IRQ_DEFAULT_S_HANDLER 85 /* TRGMUX0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 86 /* TRGMUX1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 87 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 88 /* NTMR0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 89 /* USB0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 90 /* XPI0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 91 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 92 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 93 /* XDMA IRQ handler */
    IRQ_DEFAULT_S_HANDLER 94 /* Reserved */
    IRQ_DEFAULT_S_HANDLER 95 /* SDP IRQ handler */
    IRQ_DEFAULT_S_HANDLER 96 /* RNG IRQ handler */
    IRQ_DEFAULT_S_HANDLER 97 /* PSEC IRQ handler */
    IRQ_DEFAULT_S_HANDLER 98 /* PGPIO IRQ handler */
    IRQ_DEFAULT_S_HANDLER 99 /* PEWDG IRQ handler */
    IRQ_DEFAULT_S_HANDLER 100 /* PTMR IRQ handler */
    IRQ_DEFAULT_S_HANDLER 101 /* PUART IRQ handler */
    IRQ_DEFAULT_S_HANDLER 102 /* FUSE IRQ handler */
    IRQ_DEFAULT_S_HANDLER 103 /* SECMON IRQ handler */
    IRQ_DEFAULT_S_HANDLER 104 /* PAD_WAKEUP IRQ handler */
    IRQ_DEFAULT_S_HANDLER 105 /* BROWNOUT IRQ handler */
    IRQ_DEFAULT_S_HANDLER 106 /* SYSCTL IRQ handler */
    IRQ_DEFAULT_S_HANDLER 107 /* CPU0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 108 /* CPU1 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 109 /* DEBUG0 IRQ handler */
    IRQ_DEFAULT_S_HANDLER 110 /* DEBUG1 IRQ handler */

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
    IRQ_S_HANDLER 5 /* GPIO0_X IRQ handler */
    IRQ_S_HANDLER 6 /* GPIO0_Y IRQ handler */
    IRQ_S_HANDLER 7 /* GPIO1_A IRQ handler */
    IRQ_S_HANDLER 8 /* GPIO1_B IRQ handler */
    IRQ_S_HANDLER 9 /* GPIO1_C IRQ handler */
    IRQ_S_HANDLER 10 /* GPIO1_D IRQ handler */
    IRQ_S_HANDLER 11 /* GPIO1_X IRQ handler */
    IRQ_S_HANDLER 12 /* GPIO1_Y IRQ handler */
    IRQ_S_HANDLER 13 /* GPTMR0 IRQ handler */
    IRQ_S_HANDLER 14 /* GPTMR1 IRQ handler */
    IRQ_S_HANDLER 15 /* GPTMR2 IRQ handler */
    IRQ_S_HANDLER 16 /* GPTMR3 IRQ handler */
    IRQ_S_HANDLER 17 /* Reserved */
    IRQ_S_HANDLER 18 /* Reserved */
    IRQ_S_HANDLER 19 /* Reserved */
    IRQ_S_HANDLER 20 /* Reserved */
    IRQ_S_HANDLER 21 /* UART0 IRQ handler */
    IRQ_S_HANDLER 22 /* UART1 IRQ handler */
    IRQ_S_HANDLER 23 /* UART2 IRQ handler */
    IRQ_S_HANDLER 24 /* UART3 IRQ handler */
    IRQ_S_HANDLER 25 /* Reserved */
    IRQ_S_HANDLER 26 /* Reserved */
    IRQ_S_HANDLER 27 /* Reserved */
    IRQ_S_HANDLER 28 /* Reserved */
    IRQ_S_HANDLER 29 /* I2C0 IRQ handler */
    IRQ_S_HANDLER 30 /* I2C1 IRQ handler */
    IRQ_S_HANDLER 31 /* I2C2 IRQ handler */
    IRQ_S_HANDLER 32 /* Reserved */
    IRQ_S_HANDLER 33 /* SPI0 IRQ handler */
    IRQ_S_HANDLER 34 /* SPI1 IRQ handler */
    IRQ_S_HANDLER 35 /* SPI2 IRQ handler */
    IRQ_S_HANDLER 36 /* Reserved */
    IRQ_S_HANDLER 37 /* TSNS IRQ handler */
    IRQ_S_HANDLER 38 /* MBX0A IRQ handler */
    IRQ_S_HANDLER 39 /* MBX0B IRQ handler */
    IRQ_S_HANDLER 40 /* MBX1A IRQ handler */
    IRQ_S_HANDLER 41 /* MBX1B IRQ handler */
    IRQ_S_HANDLER 42 /* EWDG0 IRQ handler */
    IRQ_S_HANDLER 43 /* EWDG1 IRQ handler */
    IRQ_S_HANDLER 44 /* Reserved */
    IRQ_S_HANDLER 45 /* Reserved */
    IRQ_S_HANDLER 46 /* HDMA IRQ handler */
    IRQ_S_HANDLER 47 /* LOBS IRQ handler */
    IRQ_S_HANDLER 48 /* ADC0 IRQ handler */
    IRQ_S_HANDLER 49 /* ADC1 IRQ handler */
    IRQ_S_HANDLER 50 /* ADC2 IRQ handler */
    IRQ_S_HANDLER 51 /* Reserved */
    IRQ_S_HANDLER 52 /* DAC0 IRQ handler */
    IRQ_S_HANDLER 53 /* DAC1 IRQ handler */
    IRQ_S_HANDLER 54 /* ACMP0[0] IRQ handler */
    IRQ_S_HANDLER 55 /* ACMP0[1] IRQ handler */
    IRQ_S_HANDLER 56 /* ACMP1[0] IRQ handler */
    IRQ_S_HANDLER 57 /* ACMP1[1] IRQ handler */
    IRQ_S_HANDLER 58 /* ACMP2[0] IRQ handler */
    IRQ_S_HANDLER 59 /* ACMP2[1] IRQ handler */
    IRQ_S_HANDLER 60 /* ACMP3[0] IRQ handler */
    IRQ_S_HANDLER 61 /* ACMP3[1] IRQ handler */
    IRQ_S_HANDLER 62 /* Reserved */
    IRQ_S_HANDLER 63 /* Reserved */
    IRQ_S_HANDLER 64 /* Reserved */
    IRQ_S_HANDLER 65 /* Reserved */
    IRQ_S_HANDLER 66 /* MCAN0 IRQ handler */
    IRQ_S_HANDLER 67 /* MCAN1 IRQ handler */
    IRQ_S_HANDLER 68 /* Reserved */
    IRQ_S_HANDLER 69 /* Reserved */
    IRQ_S_HANDLER 70 /* PTPC IRQ handler */
    IRQ_S_HANDLER 71 /* QEI0 IRQ handler */
    IRQ_S_HANDLER 72 /* QEI1 IRQ handler */
    IRQ_S_HANDLER 73 /* PWM0 IRQ handler */
    IRQ_S_HANDLER 74 /* PWM1 IRQ handler */
    IRQ_S_HANDLER 75 /* PWM2 IRQ handler */
    IRQ_S_HANDLER 76 /* Reserved */
    IRQ_S_HANDLER 77 /* RDC0 IRQ handler */
    IRQ_S_HANDLER 78 /* SDM0 IRQ handler */
    IRQ_S_HANDLER 79 /* SEI0[0] IRQ handler */
    IRQ_S_HANDLER 80 /* SEI0[1] IRQ handler */
    IRQ_S_HANDLER 81 /* MTG0 IRQ handler */
    IRQ_S_HANDLER 82 /* VSC0 IRQ handler */
    IRQ_S_HANDLER 83 /* CLC0[0] IRQ handler */
    IRQ_S_HANDLER 84 /* CLC0[1] IRQ handler */
    IRQ_S_HANDLER 85 /* TRGMUX0 IRQ handler */
    IRQ_S_HANDLER 86 /* TRGMUX1 IRQ handler */
    IRQ_S_HANDLER 87 /* Reserved */
    IRQ_S_HANDLER 88 /* NTMR0 IRQ handler */
    IRQ_S_HANDLER 89 /* USB0 IRQ handler */
    IRQ_S_HANDLER 90 /* XPI0 IRQ handler */
    IRQ_S_HANDLER 91 /* Reserved */
    IRQ_S_HANDLER 92 /* Reserved */
    IRQ_S_HANDLER 93 /* XDMA IRQ handler */
    IRQ_S_HANDLER 94 /* Reserved */
    IRQ_S_HANDLER 95 /* SDP IRQ handler */
    IRQ_S_HANDLER 96 /* RNG IRQ handler */
    IRQ_S_HANDLER 97 /* PSEC IRQ handler */
    IRQ_S_HANDLER 98 /* PGPIO IRQ handler */
    IRQ_S_HANDLER 99 /* PEWDG IRQ handler */
    IRQ_S_HANDLER 100 /* PTMR IRQ handler */
    IRQ_S_HANDLER 101 /* PUART IRQ handler */
    IRQ_S_HANDLER 102 /* FUSE IRQ handler */
    IRQ_S_HANDLER 103 /* SECMON IRQ handler */
    IRQ_S_HANDLER 104 /* PAD_WAKEUP IRQ handler */
    IRQ_S_HANDLER 105 /* BROWNOUT IRQ handler */
    IRQ_S_HANDLER 106 /* SYSCTL IRQ handler */
    IRQ_S_HANDLER 107 /* CPU0 IRQ handler */
    IRQ_S_HANDLER 108 /* CPU1 IRQ handler */
    IRQ_S_HANDLER 109 /* DEBUG0 IRQ handler */
    IRQ_S_HANDLER 110 /* DEBUG1 IRQ handler */

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
    IRQ_S_HANDLER 5 /* GPIO0_X IRQ handler */
    IRQ_S_HANDLER 6 /* GPIO0_Y IRQ handler */
    IRQ_S_HANDLER 7 /* GPIO1_A IRQ handler */
    IRQ_S_HANDLER 8 /* GPIO1_B IRQ handler */
    IRQ_S_HANDLER 9 /* GPIO1_C IRQ handler */
    IRQ_S_HANDLER 10 /* GPIO1_D IRQ handler */
    IRQ_S_HANDLER 11 /* GPIO1_X IRQ handler */
    IRQ_S_HANDLER 12 /* GPIO1_Y IRQ handler */
    IRQ_S_HANDLER 13 /* GPTMR0 IRQ handler */
    IRQ_S_HANDLER 14 /* GPTMR1 IRQ handler */
    IRQ_S_HANDLER 15 /* GPTMR2 IRQ handler */
    IRQ_S_HANDLER 16 /* GPTMR3 IRQ handler */
    IRQ_S_HANDLER 17 /* Reserved */
    IRQ_S_HANDLER 18 /* Reserved */
    IRQ_S_HANDLER 19 /* Reserved */
    IRQ_S_HANDLER 20 /* Reserved */
    IRQ_S_HANDLER 21 /* UART0 IRQ handler */
    IRQ_S_HANDLER 22 /* UART1 IRQ handler */
    IRQ_S_HANDLER 23 /* UART2 IRQ handler */
    IRQ_S_HANDLER 24 /* UART3 IRQ handler */
    IRQ_S_HANDLER 25 /* Reserved */
    IRQ_S_HANDLER 26 /* Reserved */
    IRQ_S_HANDLER 27 /* Reserved */
    IRQ_S_HANDLER 28 /* Reserved */
    IRQ_S_HANDLER 29 /* I2C0 IRQ handler */
    IRQ_S_HANDLER 30 /* I2C1 IRQ handler */
    IRQ_S_HANDLER 31 /* I2C2 IRQ handler */
    IRQ_S_HANDLER 32 /* Reserved */
    IRQ_S_HANDLER 33 /* SPI0 IRQ handler */
    IRQ_S_HANDLER 34 /* SPI1 IRQ handler */
    IRQ_S_HANDLER 35 /* SPI2 IRQ handler */
    IRQ_S_HANDLER 36 /* Reserved */
    IRQ_S_HANDLER 37 /* TSNS IRQ handler */
    IRQ_S_HANDLER 38 /* MBX0A IRQ handler */
    IRQ_S_HANDLER 39 /* MBX0B IRQ handler */
    IRQ_S_HANDLER 40 /* MBX1A IRQ handler */
    IRQ_S_HANDLER 41 /* MBX1B IRQ handler */
    IRQ_S_HANDLER 42 /* EWDG0 IRQ handler */
    IRQ_S_HANDLER 43 /* EWDG1 IRQ handler */
    IRQ_S_HANDLER 44 /* Reserved */
    IRQ_S_HANDLER 45 /* Reserved */
    IRQ_S_HANDLER 46 /* HDMA IRQ handler */
    IRQ_S_HANDLER 47 /* LOBS IRQ handler */
    IRQ_S_HANDLER 48 /* ADC0 IRQ handler */
    IRQ_S_HANDLER 49 /* ADC1 IRQ handler */
    IRQ_S_HANDLER 50 /* ADC2 IRQ handler */
    IRQ_S_HANDLER 51 /* Reserved */
    IRQ_S_HANDLER 52 /* DAC0 IRQ handler */
    IRQ_S_HANDLER 53 /* DAC1 IRQ handler */
    IRQ_S_HANDLER 54 /* ACMP0[0] IRQ handler */
    IRQ_S_HANDLER 55 /* ACMP0[1] IRQ handler */
    IRQ_S_HANDLER 56 /* ACMP1[0] IRQ handler */
    IRQ_S_HANDLER 57 /* ACMP1[1] IRQ handler */
    IRQ_S_HANDLER 58 /* ACMP2[0] IRQ handler */
    IRQ_S_HANDLER 59 /* ACMP2[1] IRQ handler */
    IRQ_S_HANDLER 60 /* ACMP3[0] IRQ handler */
    IRQ_S_HANDLER 61 /* ACMP3[1] IRQ handler */
    IRQ_S_HANDLER 62 /* Reserved */
    IRQ_S_HANDLER 63 /* Reserved */
    IRQ_S_HANDLER 64 /* Reserved */
    IRQ_S_HANDLER 65 /* Reserved */
    IRQ_S_HANDLER 66 /* MCAN0 IRQ handler */
    IRQ_S_HANDLER 67 /* MCAN1 IRQ handler */
    IRQ_S_HANDLER 68 /* Reserved */
    IRQ_S_HANDLER 69 /* Reserved */
    IRQ_S_HANDLER 70 /* PTPC IRQ handler */
    IRQ_S_HANDLER 71 /* QEI0 IRQ handler */
    IRQ_S_HANDLER 72 /* QEI1 IRQ handler */
    IRQ_S_HANDLER 73 /* PWM0 IRQ handler */
    IRQ_S_HANDLER 74 /* PWM1 IRQ handler */
    IRQ_S_HANDLER 75 /* PWM2 IRQ handler */
    IRQ_S_HANDLER 76 /* Reserved */
    IRQ_S_HANDLER 77 /* RDC0 IRQ handler */
    IRQ_S_HANDLER 78 /* SDM0 IRQ handler */
    IRQ_S_HANDLER 79 /* SEI0[0] IRQ handler */
    IRQ_S_HANDLER 80 /* SEI0[1] IRQ handler */
    IRQ_S_HANDLER 81 /* MTG0 IRQ handler */
    IRQ_S_HANDLER 82 /* VSC0 IRQ handler */
    IRQ_S_HANDLER 83 /* CLC0[0] IRQ handler */
    IRQ_S_HANDLER 84 /* CLC0[1] IRQ handler */
    IRQ_S_HANDLER 85 /* TRGMUX0 IRQ handler */
    IRQ_S_HANDLER 86 /* TRGMUX1 IRQ handler */
    IRQ_S_HANDLER 87 /* Reserved */
    IRQ_S_HANDLER 88 /* NTMR0 IRQ handler */
    IRQ_S_HANDLER 89 /* USB0 IRQ handler */
    IRQ_S_HANDLER 90 /* XPI0 IRQ handler */
    IRQ_S_HANDLER 91 /* Reserved */
    IRQ_S_HANDLER 92 /* Reserved */
    IRQ_S_HANDLER 93 /* XDMA IRQ handler */
    IRQ_S_HANDLER 94 /* Reserved */
    IRQ_S_HANDLER 95 /* SDP IRQ handler */
    IRQ_S_HANDLER 96 /* RNG IRQ handler */
    IRQ_S_HANDLER 97 /* PSEC IRQ handler */
    IRQ_S_HANDLER 98 /* PGPIO IRQ handler */
    IRQ_S_HANDLER 99 /* PEWDG IRQ handler */
    IRQ_S_HANDLER 100 /* PTMR IRQ handler */
    IRQ_S_HANDLER 101 /* PUART IRQ handler */
    IRQ_S_HANDLER 102 /* FUSE IRQ handler */
    IRQ_S_HANDLER 103 /* SECMON IRQ handler */
    IRQ_S_HANDLER 104 /* PAD_WAKEUP IRQ handler */
    IRQ_S_HANDLER 105 /* BROWNOUT IRQ handler */
    IRQ_S_HANDLER 106 /* SYSCTL IRQ handler */
    IRQ_S_HANDLER 107 /* CPU0 IRQ handler */
    IRQ_S_HANDLER 108 /* CPU1 IRQ handler */
    IRQ_S_HANDLER 109 /* DEBUG0 IRQ handler */
    IRQ_S_HANDLER 110 /* DEBUG1 IRQ handler */

#endif
