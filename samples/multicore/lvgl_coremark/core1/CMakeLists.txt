# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_COREMARK 1)

set(CUSTOM_LINKER_FILE $ENV{HPM_SDK_BASE}/samples/multicore/lvgl_coremark/linker/gcc/core1.ld)

set(SEC_CORE_IMG_C_ARRAY_OUTPUT $ENV{HPM_SDK_BASE}/samples/multicore/lvgl_coremark/core0/src/sec_core_img.c)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
    string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
    string(FIND ${build_type} "sec_core_img" found)
    if(${found} GREATER_EQUAL 0)
        set(IS_SEC_CORE_IMG true)
    endif()
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(coremark_core1)

set(opt_string "-O3 -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting")
separate_arguments(opt_list UNIX_COMMAND ${opt_string})

foreach(opt ${opt_list})
    sdk_compile_options(${opt})
endforeach()
sdk_get_compile_options(all_opts)
sdk_compile_definitions(COMPILER_FLAGS="${all_opts}")

sdk_inc(src/port)
sdk_app_src(src/port/core_portme.c)

generate_ses_project()
