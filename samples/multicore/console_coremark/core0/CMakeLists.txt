# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(SES_TOOLCHAIN_VARIANT "Andes")

set(CUSTOM_LINKER_FILE $ENV{HPM_SDK_BASE}/samples/multicore/console_coremark/linker/gcc/core0.ld)

set(CONFIG_COREMARK 1)
set(CMAKE_BUILD_TYPE debug)
set(INCLUDE_BOOTHEADER 1)


find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dualcore_console_coremark)

sdk_compile_definitions("-DFLASH_XIP=1")
sdk_compile_definitions("-DINIT_EXT_RAM_FOR_DATA=1")
sdk_compile_definitions("-DBOARD_SHOW_CLOCK=0")

if(DEFINED SEC_CORE_IMG_START)
    sdk_compile_definitions(-DSEC_CORE_IMG_START=${SEC_CORE_IMG_START})
else()
    sdk_compile_definitions("-DSEC_CORE_IMG_START=0x01180000")
endif()

set(opt_string "-O3 -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting")
separate_arguments(opt_list UNIX_COMMAND ${opt_string})

foreach(opt ${opt_list})
    sdk_compile_options(${opt})
endforeach()
sdk_get_compile_options(all_opts)
sdk_compile_definitions(COMPILER_FLAGS="${all_opts}")

sdk_inc(src/port)
sdk_inc(src)
sdk_app_src(src/demo_dualcore_coremark.c
        src/sec_core_img.c
        src/port/core_portme.c
        )

sdk_ses_compile_options(-mabi=ilp32d)
sdk_ses_compile_options(-march=rv32gc)
generate_ses_project()
