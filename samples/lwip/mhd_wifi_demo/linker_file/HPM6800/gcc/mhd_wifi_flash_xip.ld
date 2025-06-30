/*
 * Copyright (c) 2024-2025 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

ENTRY(_start)

STACK_SIZE = _stack_size;
HEAP_SIZE = _heap_size;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x80000000, LENGTH = _flash_size - 0x3000
    ILM (wx) : ORIGIN = 0x00000000, LENGTH = 256K
    DLM (w) : ORIGIN = 0x00080000, LENGTH = 224K
    AXI_SRAM (wx) : ORIGIN = 0x01200000, LENGTH = 512K
    DLM_NONCACHEABLE (wx) : ORIGIN = 0x000B8000, LENGTH = 32K
    SDRAM (wx) : ORIGIN = 0x40000000, LENGTH = _extram_size
    AHB_SRAM (w) : ORIGIN = 0xF0400000, LENGTH = 32k
    APB_SRAM (w): ORIGIN = 0xF4130000, LENGTH = 16k
}

__nor_cfg_option_load_addr__ = ORIGIN(FLASH) + 0x400;
__boot_header_load_addr__ = ORIGIN(FLASH) + 0x1000;
__app_load_addr__ = ORIGIN(FLASH) + 0x3000;
__boot_header_length__ = __boot_header_end__ - __boot_header_start__;
__app_offset__ = __app_load_addr__ - __boot_header_load_addr__;


SECTIONS
{
    .nor_cfg_option __nor_cfg_option_load_addr__ : {
        KEEP(*(.nor_cfg_option))
    } > FLASH

    .boot_header __boot_header_load_addr__ : {
        __boot_header_start__ = .;
        KEEP(*(.boot_header))
        KEEP(*(.fw_info_table))
        KEEP(*(.dc_info))
        __boot_header_end__ = .;
    } > FLASH

    .start __app_load_addr__ : {
        . = ALIGN(8);
        KEEP(*(.start))
        . = ALIGN(8);
    } > FLASH

    __vector_load_addr__ = ADDR(.start) + SIZEOF(.start);
    .vectors ORIGIN(ILM) : AT(__vector_load_addr__) {
        . = ALIGN(8);
        __vector_ram_start__ = .;
        KEEP(*(.vector_table))
        KEEP(*(.isr_vector))
        . = ALIGN(8);
        __vector_ram_end__ = .;
    } > ILM

     __fast_load_addr__ = etext + SIZEOF(.data);
        .fast : AT(__fast_load_addr__) {
            . = ALIGN(8);
            PROVIDE(__ramfunc_start__ = .);
            *(.fast)
            *(.fast.*)

            /* FreeRTOS */
            KEEP(*tasks.*o* (.text .text* .rodata .rodata*))
            KEEP(*timers.*o* (.text .text* .rodata .rodata*))
            KEEP(*queue.*o* (.text .text* .rodata .rodata*))
            KEEP(*list.*o* (.text .text* .rodata .rodata*))
            KEEP(*stream_buffer.*o* (.text .text* .rodata .rodata*))
            KEEP(*event_groups.*o* (.text .text* .rodata .rodata*))
            KEEP(*croutine.*o* (.text .text* .rodata .rodata*))
            KEEP(*low_power_inf.*o* (.text .text* .rodata .rodata*))
            KEEP(*heap_*.*o* (.text .text* .rodata .rodata*))
            KEEP(*port*.*o* (.text .text* .rodata .rodata*))
            KEEP(*hpm_port.*o* (.text .text* .rodata .rodata*))

            /* Drivers */
            KEEP(*hpm_sdxc_drv.*o* (.text .text* .rodata .rodata*))
            KEEP(*hpm_uart*.*o* (.text .text* .rodata .rodata*))

            KEEP(*hpm_gpio_drv*.*o* (.text .text* .rodata .rodata*))
            KEEP(*trap*.*o* (.text .text* .rodata .rodata*))

            /* middleware: hpm_sdmmc */
            KEEP(*hpm_sdmmc*.*o* (.text .text* .rodata .rodata*))

            /* middleware: cherry shell */
            KEEP(*chry*.*o* (.text .text* .rodata .rodata*))
            KEEP(*shell*.*o* (.text .text* .rodata .rodata*))

            /* middleware: lwip */
            KEEP(*api_lib.*o* (.text .text* .rodata .rodata*))
            KEEP(*api_msg.*o* (.text .text* .rodata .rodata*))
            KEEP(*if_api.*o* (.text .text* .rodata .rodata*))
            KEEP(*err.*o* (.text .text* .rodata .rodata*))
            KEEP(*netbuf.*o* (.text .text* .rodata .rodata*))
            KEEP(*netifapi.*o* (.text .text* .rodata .rodata*))
            KEEP(*sockets.*o* (.text .text* .rodata .rodata*))
            KEEP(*tcpip.*o* (.text .text* .rodata .rodata*))

            KEEP(*autoip.*o* (.text .text* .rodata .rodata*))
            KEEP(*dhcp.*o* (.text .text* .rodata .rodata*))
            KEEP(*etharp.*o* (.text .text* .rodata .rodata*))
            KEEP(*icmp.*o* (.text .text* .rodata .rodata*))
            KEEP(*igmp.*o* (.text .text* .rodata .rodata*))
            KEEP(*ip4*.*o* (.text .text* .rodata .rodata*))

            KEEP(*altcp.*o* (.text .text* .rodata .rodata*))
            KEEP(*altcp_alloc.*o* (.text .text* .rodata .rodata*))
            KEEP(*altcp_tcp.*o* (.text .text* .rodata .rodata*))
            KEEP(*def.*o* (.text .text* .rodata .rodata*))
            KEEP(*inet_chksum.*o* (.text .text* .rodata .rodata*))
            KEEP(*init.*o* (.text .text* .rodata .rodata*))
            KEEP(*ip.*o* (.text .text* .rodata .rodata*))
            KEEP(*mem.*o* (.text .text* .rodata .rodata*))
            KEEP(*memp.*o* (.text .text* .rodata .rodata*))
            KEEP(*netif.*o* (.text .text* .rodata .rodata*))
            KEEP(*pbuf.*o* (.text .text* .rodata .rodata*))
            KEEP(*raw.*o* (.text .text* .rodata .rodata*))
            KEEP(*stats.*o* (.text .text* .rodata .rodata*))
            KEEP(*sys.*o* (.text .text* .rodata .rodata*))
            KEEP(*tcp_in.*o* (.text .text* .rodata .rodata*))
            KEEP(*tcp_out.*o* (.text .text* .rodata .rodata*))
            KEEP(*timeouts.*o* (.text .text* .rodata .rodata*))
            KEEP(*udp.*o* (.text .text* .rodata .rodata*))

            /* MHD WiFi stack */
            KEEP(*host_network.*o* (.text .text* .rodata .rodata*))
            KEEP(*host_buffer.*o* (.text .text* .rodata .rodata*))
            KEEP(*host_sdio*.*o* (.text .text* .rodata .rodata*))
            KEEP(*host_gpio*.*o* (.text .text* .rodata .rodata*))
            KEEP(*mhd_thread*.*o* (.text .text* .rodata .rodata*))
            KEEP(*mhd_sdpcm*.*o* (.text .text* .rodata .rodata*))

            /* apps */
             KEEP(*iperf_cmd*.o* (.text .text* .rodata .rodata*))
             KEEP(*ping_cmd*.o* (.text .text* .rodata .rodata*))

            . = ALIGN(8);
            PROVIDE(__ramfunc_end__ = .);
        } > ILM

    .text (__vector_load_addr__ + SIZEOF(.vectors)) : {
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

        KEEP (*(.init))
        KEEP (*(.fini))

        /* section information for usbh class */
        . = ALIGN(8);
        __usbh_class_info_start__ = .;
        KEEP(*(.usbh_class_info))
        __usbh_class_info_end__ = .;

        /* RT-Thread related sections - Start */
        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        . = ALIGN(4);

        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        . = ALIGN(4);

        /* section information for modules */
        . = ALIGN(4);
        __rtmsymtab_start = .;
        KEEP(*(RTMSymTab))
        __rtmsymtab_end = .;

        /* RT-Thread related sections - end */
        . = ALIGN(8);
    } > FLASH

    .eh_frame :
    {
        __eh_frame_start = .;
        KEEP(*(.eh_frame))
        __eh_frame_end = .;
    }  > FLASH

    .eh_frame_hdr :
    {
        KEEP(*(.eh_frame_hdr))
    }  > FLASH
    __eh_frame_hdr_start = SIZEOF(.eh_frame_hdr) > 0 ? ADDR(.eh_frame_hdr) : 0;
    __eh_frame_hdr_end = SIZEOF(.eh_frame_hdr) > 0 ? . : 0;

    .rel : {
        KEEP(*(.rel*))
    } > FLASH

    . = ALIGN(8);
    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);

    __data_load_addr__ = etext;
    .data : AT(__data_load_addr__) {
        . = ALIGN(8);
        __data_start__ = .;
        __global_pointer$ = . + 0x800;
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)

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
    } > DLM



    __tdata_load_addr__ = etext + SIZEOF(.data) + SIZEOF(.fast);
    .tdata : AT(__tdata_load_addr__) {
        . = ALIGN(8);
        PROVIDE(__tdata_start__ = .);
        *(.tdata)
        *(.tdata.*)
        *(.gnu.linkonce.td.*)
        . = ALIGN(8);
        PROVIDE(__tdata_end__ = .);
    } > DLM

    .tbss (NOLOAD) : {
        . = ALIGN(8);
        PROVIDE(__tbss_start__ = .);
        __thread_pointer$ = .;
        *(.tbss)
        *(.tbss.*)
        *(.gnu.linkonce.tb.*)
        *(.tcommon)
        . = ALIGN(8);
        PROVIDE(__tbss_end__ = .);
    } > DLM

    __noncacheable_init_load_addr__ = etext + SIZEOF(.data) + SIZEOF(.fast) + SIZEOF(.tdata);
    .noncacheable.init : AT(__noncacheable_init_load_addr__) {
        . = ALIGN(8);
        __noncacheable_init_start__ = .;
        KEEP(*(.noncacheable.init))
        KEEP(*(.noncacheable.init.*))
        __noncacheable_init_end__ = .;
        . = ALIGN(8);
    } > DLM_NONCACHEABLE

    __fast_ram_init_load_addr__ = etext + SIZEOF(.data) + SIZEOF(.fast) + SIZEOF(.tdata) + SIZEOF(.noncacheable.init);
    .fast_ram.init : AT(__fast_ram_init_load_addr__) {
        . = ALIGN(8);
        __fast_ram_init_start__ = .;
        KEEP(*(.fast_ram.init))
        KEEP(*(.fast_ram.init.*))
        __fast_ram_init_end__ = .;
        . = ALIGN(8);
    } > DLM

    .bss (NOLOAD) : {
        . = ALIGN(8);
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(.sbss*)
        *(.scommon)
        *(.scommon*)
        *(.dynsbss*)
        *(COMMON)
        . = ALIGN(8);
        _end = .;
        __bss_end__ = .;
    } > DLM

    .framebuffer (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.framebuffer))
        KEEP(*(.framebuffer.*))
        . = ALIGN(8);
    } > AXI_SRAM

    .noncacheable (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.noncacheable))  /* Legacy sections, kept for backwards compatibility */
        KEEP(*(.noncacheable.non_init))
        KEEP(*(.noncacheable.non_init.*))
        __noncacheable_bss_start__ = .;
        KEEP(*(.noncacheable.bss))
        KEEP(*(.noncacheable.bss.*))
        __noncacheable_bss_end__ = .;
        . = ALIGN(8);
    } > DLM_NONCACHEABLE

    .ahb_sram (NOLOAD) : {
        KEEP(*(.ahb_sram))
        KEEP(*(.ahb_sram.*))
    } > AHB_SRAM

    .apb_sram (NOLOAD) : {
        KEEP(*(.backup_sram))
        KEEP(*(.backup_sram.*))
    } > APB_SRAM

    .fast_ram (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.fast_ram))  /* Legacy sections, kept for backwards compatibility */
        KEEP(*(.fast_ram.non_init))
        KEEP(*(.fast_ram.non_init.*))
        __fast_ram_bss_start__ = .;
        KEEP(*(.fast_ram.bss))
        KEEP(*(.fast_ram.bss.*))
        __fast_ram_bss_end__ = .;
        . = ALIGN(8);
    } > SDRAM

    .heap (NOLOAD) : {
        . = ALIGN(8);
        __heap_start__ = .;
        . += HEAP_SIZE;
        __heap_end__ = .;
    } > AXI_SRAM

    .stack (NOLOAD) : {
        . = ALIGN(16);
        __stack_base__ = .;
        . += STACK_SIZE;
        . = ALIGN(16);
        PROVIDE (_stack = .);
        PROVIDE (_stack_safe = .);
    } > DLM

    __noncacheable_start__ = ORIGIN(DLM_NONCACHEABLE);
    __noncacheable_end__ = ORIGIN(DLM_NONCACHEABLE) + LENGTH(DLM_NONCACHEABLE);

    __fw_size__ = SIZEOF(.start) + SIZEOF(.vectors) + SIZEOF(.rel) + SIZEOF(.text) + SIZEOF(.data) + SIZEOF(.fast) + SIZEOF(.tdata) + SIZEOF(.noncacheable.init) + SIZEOF(.fast_ram.init) + SIZEOF(.eh_frame) + SIZEOF(.eh_frame_hdr);
    __last_addr__ = __fast_ram_init_load_addr__ + SIZEOF(.fast_ram.init);
    ASSERT(((__fw_size__ <= LENGTH(FLASH)) && (__last_addr__ <= (ORIGIN(FLASH) + LENGTH(FLASH)))), "******  FAILED! FLASH does not have enough space!  ******")
}
