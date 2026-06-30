/*
 * Copyright (c) 2022-2023,2026 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"
#include "hpm_interrupt.h"


extern void system_init(void);

#ifndef MAIN_ENTRY
#define MAIN_ENTRY main
#endif
extern int MAIN_ENTRY(void);

__attribute__((weak)) void _clean_up(void)
{
    /* clean up plic, it will help while debugging */
    disable_irq_from_intc();
    intc_m_set_threshold(0);
    for (uint32_t irq = 0; irq < 128; irq++) {
        intc_m_complete_irq(irq);
    }
    /* clear any bits left in plic enable register */
    for (uint32_t i = 0; i < 4; i++) {
        *(volatile uint32_t *)(HPM_PLIC_BASE + HPM_PLIC_ENABLE_OFFSET + (i << 2)) = 0;
    }
}

/* Clear a memory section with word-aligned optimization */
static void section_clear(uint8_t *dst_start, uint8_t *dst_end)
{
    uint32_t i, size;
    uint32_t *dst_word;
    uint8_t *dst_byte;
    uint32_t word_count, byte_count;

    size = dst_end - dst_start;
    dst_byte = dst_start;
    if (((uintptr_t)dst_byte & 0x3) == 0) {
        /* Word-aligned, use word clear */
        word_count = size / 4;
        byte_count = size % 4;
        dst_word = (uint32_t *)dst_byte;
        for (i = 0; i < word_count; i++) {
            dst_word[i] = 0;
        }
        dst_byte = (uint8_t *)(dst_word + word_count);
        for (i = 0; i < byte_count; i++) {
            dst_byte[i] = 0;
        }
    } else {
        /* Not word-aligned, use byte clear */
        for (i = 0; i < size; i++) {
            dst_byte[i] = 0;
        }
    }
}

/* Copy a memory section with word-aligned optimization */
static void section_copy(uint8_t *dst_start, uint8_t *dst_end, uint8_t *src_start)
{
    uint32_t i, size;
    uint32_t *dst_word, *src_word;
    uint8_t *dst_byte, *src_byte;
    uint32_t word_count, byte_count;

    size = dst_end - dst_start;
    dst_byte = dst_start;
    src_byte = src_start;
    if ((((uintptr_t)dst_byte & 0x3) == 0) && (((uintptr_t)src_byte & 0x3) == 0)) {
        /* Both addresses are word-aligned, use word copy */
        word_count = size / 4;
        byte_count = size % 4;
        dst_word = (uint32_t *)dst_byte;
        src_word = (uint32_t *)src_byte;
        for (i = 0; i < word_count; i++) {
            dst_word[i] = src_word[i];
        }
        dst_byte = (uint8_t *)(dst_word + word_count);
        src_byte = (uint8_t *)(src_word + word_count);
        for (i = 0; i < byte_count; i++) {
            dst_byte[i] = src_byte[i];
        }
    } else {
        /* Not word-aligned, use byte copy */
        for (i = 0; i < size; i++) {
            dst_byte[i] = src_byte[i];
        }
    }
}

__attribute__((weak)) void c_startup(void)
{
    extern uint8_t __bss_start__[], __bss_end__[];
    extern uint8_t __tdata_start__[], __tdata_end__[];
    extern uint8_t __data_start__[], __data_end__[];
    extern uint8_t __ramfunc_start__[], __ramfunc_end__[];
    extern uint8_t __noncacheable_bss_start__[], __noncacheable_bss_end__[];
    extern uint8_t __noncacheable_init_start__[], __noncacheable_init_end__[];
    extern uint8_t __data_load_addr__[], __tdata_load_addr__[];
    extern uint8_t __fast_load_addr__[], __noncacheable_init_load_addr__[];
    extern uint8_t __fast_ram_bss_start__[], __fast_ram_bss_end__[];
    extern uint8_t __fast_ram_init_start__[], __fast_ram_init_end__[], __fast_ram_init_load_addr__[];

#if defined(FLASH_XIP) || defined(FLASH_UF2) || defined(FLASH_DFU)
    extern uint8_t __vector_ram_start__[], __vector_ram_end__[], __vector_load_addr__[];
    section_copy(__vector_ram_start__, __vector_ram_end__, __vector_load_addr__);
#endif

    /* bss section */
    section_clear(__bss_start__, __bss_end__);

    /* noncacheable bss section */
    section_clear(__noncacheable_bss_start__, __noncacheable_bss_end__);

    /* fast_ram bss section */
    section_clear(__fast_ram_bss_start__, __fast_ram_bss_end__);

    /* data section LMA: etext */
    section_copy(__data_start__, __data_end__, __data_load_addr__);

    /* ramfunc section LMA: etext + data length */
    section_copy(__ramfunc_start__, __ramfunc_end__, __fast_load_addr__);

    /* tdata section LMA: etext + data length + ramfunc length */
    section_copy(__tdata_start__, __tdata_end__, __tdata_load_addr__);

    /* noncacheable init section LMA: etext + data length + ramfunc length + tdata length*/
    section_copy(__noncacheable_init_start__, __noncacheable_init_end__, __noncacheable_init_load_addr__);

    /* fast_ram init section LMA: etext + data length + ramfunc length + tdata length*/
    section_copy(__fast_ram_init_start__, __fast_ram_init_end__, __fast_ram_init_load_addr__);
}

__attribute__((weak)) int main(void)
{
    while (1) {
        ;
    }
}

__attribute__((weak)) void reset_handler(void)
{
    fencei();

    /* Call platform specific hardware initialization */
    system_init();

    /* Entry function */
    MAIN_ENTRY();
}

/*
 * When compiling C++ code with static objects, the compiler inserts
 * a call to __cxa_atexit() with __dso_handle as one of the arguments.
 * The dummy versions of these symbols should be provided.
 */
__attribute__((weak)) void __cxa_atexit(void (*arg1)(void *), void *arg2, void *arg3)
{
    (void) arg1;
    (void) arg2;
    (void) arg3;
}

#if (!defined(__SEGGER_RTL_VERSION) || defined(__riscv_xandes)) && !defined(__ICCRISCV__)
__attribute__((weak)) void *__dso_handle = (void *) &__dso_handle;
#endif

__attribute__((weak)) void _init(void)
{
}


#ifdef __ICCRISCV__
int __low_level_init(void)
{
#ifdef IAR_MANUAL_COPY /* Enable this code snippet if the .isr_vector and .vector_table need to be copied to RAM manually */
#pragma section = ".isr_vector"
#pragma section = ".isr_vector_init"
#pragma section = ".vector_table"
#pragma section = ".vector_table_init"
    /* Initialize section .isr_vector, section .vector_table */
    uint8_t *__isr_vector_ram_start = __section_begin(".isr_vector");
    uint32_t __isr_vector_ram_size = __section_size(".isr_vector");
    uint8_t *__isr_vector_rom_start = __section_begin(".isr_vector_init");

    for (uint32_t i = 0; i < __isr_vector_ram_size; i++) {
        __isr_vector_ram_start[i] = __isr_vector_rom_start[i];
    }

    uint8_t *__vector_table_ram_start = __section_begin(".vector_table");
    uint32_t __vector_table_ram_size = __section_size(".vector_table");
    uint8_t *__vector_rom_start = __section_begin(".vector_table_init");

    for (uint32_t i = 0; i < __vector_table_ram_size; i++) {
        __vector_table_ram_start[i] = __vector_rom_start[i];
    }
#endif

    return 1;
}
#endif
