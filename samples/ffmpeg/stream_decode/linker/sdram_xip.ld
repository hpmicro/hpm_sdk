/*
 * Copyright (c) 2021 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

ENTRY(_start)

STACK_SIZE = DEFINED(_stack_size) ? _stack_size : 0x8000;
HEAP_SIZE = DEFINED(_heap_size) ? _heap_size : 0x8000;

MEMORY
{
    XPI0 (rx) : ORIGIN = 0x80000000, LENGTH = _flash_size
    SDRAM (wx) : ORIGIN = 0x40000000, LENGTH = (_extram_size - 4M)
    SDRAM_NONCACHEABLE (wx) : ORIGIN = 0x40000000 + (_extram_size - 4M), LENGTH = 4M
    AXI_SRAM (wx) : ORIGIN = 0x1040000, LENGTH = 2M - 256k
    ILM0(wx) : ORIGIN = 0x0000000, LENGTH = 256k
}

__nor_cfg_option_load_addr__ = ORIGIN(XPI0) + 0x400;
__boot_header_load_addr__ = ORIGIN(XPI0) + 0x1000;
__app_load_addr__ = ORIGIN(XPI0) + 0x3000;
__boot_header_length__ = __boot_header_end__ - __boot_header_start__;
__app_offset__ = __app_load_addr__ - __boot_header_load_addr__;

SECTIONS
{
    .nor_cfg_option __nor_cfg_option_load_addr__ : {
        KEEP(*(.nor_cfg_option))
    } > XPI0

    .boot_header __boot_header_load_addr__ : {
        __boot_header_start__ = .;
        KEEP(*(.boot_header))
        KEEP(*(.fw_info_table))
        KEEP(*(.dc_info))
        __boot_header_end__ = .;
    } > XPI0

    .start __app_load_addr__ : {
        . = ALIGN(8);
        KEEP(*(.start))
        KEEP(*(.text._init_ext_ram))
        KEEP(*(.text.board_init*))
        KEEP(*(.text.clock*))
        KEEP(*(.text.femc_*))
        KEEP(*(.text.sysctl*))
        KEEP(*(.text.init_sdram_pins*))
        KEEP(*(.text.get_frequency*))
        KEEP(*(.text.l1c*))
        KEEP(*(.text.pll*))
        KEEP(*(.text.ns2cycle*))
        KEEP(*(.text.c_startup*))

        *(.rodata)
        *(.rodata*)
        *(.srodata)
        *(.srodata*)


        *(.text.pred*)
        *(.ffmpeg_ro*)
        /* *(.text)
        *(.text*) */
        *(.fast)
        *(.hash)
        *(.dyn*)
        *(.gnu*)
        *(.pl*)
        KEEP(*(.eh_frame))
        *(.eh_frame*)

        KEEP (*(.init))
        KEEP (*(.fini))

    } > XPI0

    __vector_load_addr__ = ADDR(.start) + SIZEOF(.start);
    .vectors ORIGIN(AXI_SRAM) : AT(__vector_load_addr__) {
        . = ALIGN(8);
        __vector_ram_start__ = .;
        KEEP(*(.vector_table))
        KEEP(*(.isr_vector))
        . = ALIGN(8);
        __vector_ram_end__ = .;
    } > AXI_SRAM

    __ilm0_load_addr__ = __vector_load_addr__ + SIZEOF(.vectors);
    .ilm0 ORIGIN(ILM0) : AT(__ilm0_load_addr__) {
        . = ALIGN(8);
        __ilm_start__ = .;
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
        /* *(.text.pred*)
        *(.ffmpeg_ro*) */
        __ilm_end__ = .;
        . = ALIGN(8);
    } > ILM0

    .stack : {
        . = ALIGN(8);
        __stack_base__ = .;
        . += STACK_SIZE;
        . = ALIGN(8);
        PROVIDE (_stack = .);
        PROVIDE (_stack_safe = .);
    } > AXI_SRAM

    __text_load_addr__ = __ilm0_load_addr__ + SIZEOF(.ilm0);
    .text : AT(__text_load_addr__) {
        . = ALIGN(8);
        __text_start__ = .;
        *(.text)
        *(.text*)
        *(.hash)
        *(.dyn*)
        *(.gnu*)
        *(.pl*)
        KEEP(*(.eh_frame))
        *(.eh_frame*)

        KEEP (*(.init))
        KEEP (*(.fini))
        . = ALIGN(8);
        __text_end__ = .;
    } > AXI_SRAM

    .rel : {
        KEEP(*(.rel*))
    } > XPI0

    /* section information for usbh class */
    .usbh_class_info : {
        . = ALIGN(4);
        __usbh_class_info_start__ = .;
        KEEP(*(.usbh_class_info))
        __usbh_class_info_end__ = .;
        . = ALIGN(8);
    } > XPI0


    __etext = __text_load_addr__ + SIZEOF(.text);
    _etext = __text_load_addr__ + SIZEOF(.text);
    etext = __text_load_addr__ + SIZEOF(.text);


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
        __ramfunc_start__ = .;
        /* *(.fast) */
        . = ALIGN(8);
        __ramfunc_end__ = .;
    } > AXI_SRAM

    __fw_size__ = __ramfunc_end__ - __ramfunc_start__ + __data_end__ - __data_start__ + etext - __app_load_addr__;
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
    } > SDRAM

    .heap : {
        . = ALIGN(8);
        __heap_start__ = .;
        . += HEAP_SIZE;
        __heap_end__ = .;
        . = ALIGN(8);

    } > SDRAM

    .framebuffer (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.framebuffer))
        . = ALIGN(8);
    } > SDRAM


    .heap_5 (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.heap_5))
        . = ALIGN(8);
    } > AXI_SRAM

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
    } > SDRAM_NONCACHEABLE


    __noncacheable_start__ = ORIGIN(SDRAM_NONCACHEABLE);
    __noncacheable_end__ = ORIGIN(SDRAM_NONCACHEABLE) + LENGTH(SDRAM_NONCACHEABLE);



}
