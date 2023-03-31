# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../linker/gcc/core0.ld)

set(CMAKE_BUILD_TYPE "")
set(CONFIG_LVGL 1)
if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt911")
endif()
set(CONFIG_HPM_TOUCH 1)
set(CONFIG_LV_INDEV 1)
set(CONFIG_LV_DEMO lv_demo_widgets)
set(CONFIG_COREMARK 1)
set(INCLUDE_BOOTHEADER 1)


find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lv_demo_dualcore_coremark)

sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")
sdk_compile_definitions(-DCONFIG_LV_GPU_HPM_SOC_INCLUDE="hpm_soc.h")
sdk_compile_definitions("-DFLASH_XIP=1")
sdk_compile_definitions("-DINIT_EXT_RAM_FOR_DATA=1")
sdk_compile_definitions("-DLV_USE_DEMO_WIDGETS=1")

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
        src/dualcore_widgets.c
        src/hpmlogo_alpha_180x120.c
        src/sec_core_img.c
        src/port/core_portme.c
        )

generate_ses_project()
