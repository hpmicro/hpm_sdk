# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dhrystone)

if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(zcc_opt_string "-mllvm --binop-rhs-first=true -falign-functions=16 -falign-loops=4 -flate-loop-unroll -malign-branch -Wno-error=deprecated-non-prototype -Wno-error=implicit-int -Wno-error=implicit-function-declaration")
    sdk_zcc_ld_options(-O3)
    sdk_zcc_ld_options("-fuse-size-lib")
    sdk_zcc_ld_options(-Wl,-mllvm,--riscv-default-unroll=true)
    sdk_zcc_ld_options(-Wl,-mllvm,--riscv-misched-load-clustering=true)
    sdk_zcc_ld_options(-Wl,-mllvm,--ldstmemcpy-glue-max=4)
    separate_arguments(zcc_opt_list UNIX_COMMAND ${zcc_opt_string})
    foreach(opt ${zcc_opt_list})
        sdk_zcc_compile_options(${opt})
    endforeach()
else()
    sdk_clear_compile_options()
endif()

set(opt_string "-O3 -fno-inline -fmessage-length=0 -Wno-implicit-int -Wno-implicit-function-declaration -Wno-return-type -Wno-deprecated-non-prototype -Wno-strict-prototypes -fno-builtin-printf -fbuiltin")
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
