# Copyright (c) 2021,2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(SES_TOOLCHAIN_VARIANT "Andes")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(coremark)

if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(opt_string "-O3 -flto -falign-functions=4 -flate-loop-unroll -malign-branch")
    sdk_zcc_ld_options(-O3)
    sdk_zcc_ld_options("-fuse-size-lib")
    sdk_zcc_ld_options(-Wl,-mllvm,--misfetch-cost=8)
    sdk_zcc_ld_options(-Wl,-mllvm,--misched-topdown=true)
else()
    set(opt_string "-O3 -funroll-all-loops -finline-limit=600 -fno-code-hoisting -fselective-scheduling -fno-forward-propagate -fno-tree-ter -fipa-pta -fno-tree-vrp")
endif()
separate_arguments(opt_list UNIX_COMMAND ${opt_string})

foreach(opt ${opt_list})
sdk_compile_options(${opt})
endforeach()
sdk_get_compile_options(all_opts)
sdk_compile_definitions(COMPILER_FLAGS="${all_opts}")
sdk_nds_compile_options(-mcmov)
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
generate_ide_projects()
