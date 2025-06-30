# Copyright (c) 2021,2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(SES_TOOLCHAIN_VARIANT "Andes")

set(RV_ABI "ilp32d")
set(RV_ARCH "rv32gc")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(coremark)

set(opt_string "-O3 -funroll-all-loops -finline-limit=600 -fno-code-hoisting -fselective-scheduling -fno-forward-propagate -fno-tree-ter -fipa-pta -fno-tree-vrp")
separate_arguments(opt_list UNIX_COMMAND ${opt_string})

if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(zcc_opt_string "-O3 -flto -falign-functions=4 -flate-loop-unroll -malign-branch")
    separate_arguments(zcc_opt_list UNIX_COMMAND ${zcc_opt_string})
    foreach(opt ${zcc_opt_list})
        sdk_zcc_compile_options(${opt})
    endforeach()
    sdk_zcc_ld_options(-O3)
    sdk_zcc_ld_options("-fuse-size-lib")
    sdk_zcc_ld_options(-Wl,-mllvm,--riscv-misched-load-clustering=true)
    sdk_zcc_ld_options(-Wl,-mllvm,--misfetch-cost=8)
    sdk_zcc_ld_options(-Wl,-mllvm,--misched-topdown=true)
    sdk_zcc_ld_options(-Wl,-mllvm,--disable-constant-hoisting=true)
    sdk_zcc_ld_options(-Wl,--andes-relax-gp)

    foreach(opt ${opt_list})
        sdk_ses_compile_options(${opt})
    endforeach()
else()
    foreach(opt ${opt_list})
        sdk_compile_options(${opt})
    endforeach()
endif()

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

generate_ide_projects()
