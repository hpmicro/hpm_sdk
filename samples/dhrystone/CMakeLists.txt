# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dhrystone)

set(opt_string "-O3 -fno-inline -flto -c -fmessage-length=0 -Wno-implicit-int -Wno-implicit-function-declaration -Wno-return-type -Wno-deprecated-non-prototype -fbuiltin -fno-builtin-printf")
if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(opt_string "${opt_string} -falign-functions=16 -falign-loops=4 -flate-loop-unroll -malign-branch -Wno-error=deprecated-non-prototype -Wno-error=implicit-int -Wno-error=implicit-function-declaration")
    sdk_zcc_ld_options(-O3)
    sdk_zcc_ld_options(-Wl,-mllvm,--riscv-default-unroll=true)
    sdk_zcc_ld_options(-Wl,-mllvm,--riscv-misched-load-clustering=true)
    sdk_zcc_ld_options(-Wl,-mllvm,--misfetch-cost=8)
else()
    sdk_clear_compile_options()
endif()

separate_arguments(opt_list UNIX_COMMAND ${opt_string})

foreach(opt ${opt_list})
sdk_compile_options(${opt})
endforeach()
sdk_get_compile_options(all_opts)
sdk_compile_definitions(LOOP_COUNT=18000000)
sdk_iar_options("--diag_suppress Pe260")
sdk_ld_options("-O3")

sdk_inc(src)
sdk_app_src(src/dhry_1.c)
sdk_app_src(src/dhry_2.c)
generate_ide_projects()
