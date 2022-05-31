# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(SES_TOOLCHAIN_VARIANT "Andes")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(coremark)

set(opt_string "-O3 -funroll-all-loops -finline-limit=600 -ftree-dominator-opts -fno-if-conversion2 -fselective-scheduling -fno-code-hoisting -mstrict-align")
separate_arguments(opt_list UNIX_COMMAND ${opt_string})

foreach(opt ${opt_list})
sdk_compile_options(${opt})
endforeach()
sdk_get_compile_options(all_opts)
sdk_compile_definitions(COMPILER_FLAGS="${all_opts}")
sdk_app_inc(src src/port)
sdk_app_src(src/core_list_join.c
            src/core_main.c
            src/core_matrix.c
            src/core_state.c
            src/core_util.c
            src/port/core_portme.c)

# Set optimized compile option for SES
sdk_ses_compile_options(-mabi=ilp32d)
sdk_ses_compile_options(-march=rv32gc)
generate_ses_project()
