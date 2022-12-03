/*
 * Copyright (c) 2021 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

ENTRY(_start)

STACK_SIZE = DEFINED(_stack_size) ? _stack_size : 0x4000;
HEAP_SIZE = DEFINED(_heap_size) ? _heap_size : 0x4000;

MEMORY
{
    ILM (wx) : ORIGIN = 0, LENGTH = 256K
    SDRAM (wx) : ORIGIN = 0x40000000, LENGTH = 32M

    AXI_SRAM (wx) : ORIGIN = 0x1040000, LENGTH = 2M - 16k - 256k
    AXI_SRAM_NONCACHEABLE (wx) : ORIGIN = 0x011FC000, LENGTH = 16K
}

SECTIONS
{
    .start : {
        . = ALIGN(8);
        KEEP(*(.start))
    } > AXI_SRAM

    .vectors : {
        . = ALIGN(8);
        KEEP(*(.isr_vector))
        KEEP(*(.vector_table))
        *h264.c.obj (.text*)
        *h264_mb.c.obj (.text*)
        *.h264_cabac.c.obj (.text*)
        *(.text.fill*)
        *(.text.malloc*)
        *(.text.mem*)
        *(.text.realloc*)
        *(.text.rectangle*)
        *(.text.mb_tem*)
        *(.text.ff_h264*)
        *(.text.mov*)
        /* *(.text.parse_*)  */
        /* *(.text.get*) */
        *(.text.pred*)
        . = ALIGN(8);
    } > ILM

    .rel : {
        KEEP(*(.rel*))
    } > AXI_SRAM

    .text : {
        . = ALIGN(8);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.srodata)
        *(.srodata*)

        *(.hash)
        *(.dyn*)
        *(.gnu*)
        *(.pl*)

        KEEP(*(.eh_frame))
        *(.eh_frame*)

        KEEP (*(.init))
        KEEP (*(.fini))

        /* section information for usbh class */
        . = ALIGN(4);
        __usbh_class_info_start__ = .;
        KEEP(*(.usbh_class_info))
        __usbh_class_info_end__ = .;

        . = ALIGN(8);

        PROVIDE (__etext = .);
        PROVIDE (_etext = .);
        PROVIDE (etext = .);
    } > AXI_SRAM

    .data : AT(etext) {
        . = ALIGN(8);
        __data_start__ = .;
        __global_pointer$ = . + 0x800;
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)
        *(.tdata)
        *(.tdata*)

        KEEP(*(.jcr))
        KEEP(*(.dynamic))
        KEEP(*(.got*))
        KEEP(*(.got))
        KEEP(*(.gcc_except_table))
        KEEP(*(.gcc_except_table.*))

        . = ALIGN(8);
        PROVIDE(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE(__preinit_array_end = .);

        . = ALIGN(8);
        PROVIDE(__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE(__init_array_end = .);

        . = ALIGN(8);
        PROVIDE(__finit_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.finit_array.*)))
        KEEP(*(.finit_array))
        PROVIDE(__finit_array_end = .);

        . = ALIGN(8);
        KEEP(*crtbegin*.o(.ctors))
        KEEP(*(EXCLUDE_FILE (*crtend*.o) .ctors))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))

        . = ALIGN(8);
        KEEP(*crtbegin*.o(.dtors))
        KEEP(*(EXCLUDE_FILE (*crtend*.o) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))

        . = ALIGN(8);
        __data_end__ = .;
        PROVIDE (__edata = .);
        PROVIDE (_edata = .);
        PROVIDE (edata = .);
    } > AXI_SRAM

    .fast : AT(etext + __data_end__ - __data_start__) {
        . = ALIGN(8);
        PROVIDE(__ramfunc_start__ = .);
        *(.fast)
        *(.fast*)
        . = ALIGN(8);
        PROVIDE(__ramfunc_end__ = .);
    } > AXI_SRAM

    .bss : {
        . = ALIGN(8);
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(.tbss*)
        *(.sbss*)
        *(.scommon)
        *(.scommon*)
        *(.tcommon*)
        *(.dynsbss*)
        *(COMMON)
        . = ALIGN(8);
        _end = .;
        __bss_end__ = .;
    } > AXI_SRAM

    .framebuffer (NOLOAD) : {
        KEEP(*(.framebuffer))
    } > SDRAM

    .dual_core_share (NOLOAD) : {
        KEEP(*(.dual_core_share))
    } > AXI_SRAM

    .heap_5 (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.heap_5))
        . = ALIGN(8);
    } > AXI_SRAM

    .heap : {
        . = ALIGN(8);
        __heap_start__ = .;
        . += HEAP_SIZE;
        __heap_end__ = .;

    } > SDRAM

    .noncacheable : AT(etext + __data_end__ - __data_start__ + __ramfunc_end__ - __ramfunc_start__) {
        . = ALIGN(8);
        __noncacheable_init_start__ = .;
        KEEP(*(.noncacheable.init))
        __noncacheable_init_end__ = .;
        KEEP(*(.noncacheable))
        __noncacheable_bss_start__ = .;
        KEEP(*(.noncacheable.bss))
        __noncacheable_bss_end__ = .;
        . = ALIGN(8);
    } > AXI_SRAM_NONCACHEABLE

    .stack : {
        . = ALIGN(8);
        __stack_base__ = .;
        . += STACK_SIZE;
        PROVIDE (_stack = .);
        PROVIDE (_stack_safe = .);
    } > AXI_SRAM

    __noncacheable_start__ = ORIGIN(AXI_SRAM_NONCACHEABLE);
    __noncacheable_end__ = ORIGIN(AXI_SRAM_NONCACHEABLE) + LENGTH(AXI_SRAM_NONCACHEABLE);

}
