# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PANEL 1)

set(SES_TOOLCHAIN_VARIANT "Andes")
set(CONFIG_LVGL 1)
if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt9xx")
endif()
set(CONFIG_HPM_TOUCH 1)
set(CONFIG_LV_DEMO widgets)
set(CONFIG_COREMARK 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lv_demo_dualcore_coremark)

sdk_compile_definitions(-DCONFIG_LV_TOUCH=1)
sdk_compile_definitions(-DLV_USE_DEMO_WIDGETS=1)
sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")
sdk_compile_definitions(-DMAIN_ENTRY=app_main)

if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(opt_string "-O3 -flto -falign-functions=4 -flate-loop-unroll")
    sdk_zcc_ld_options(-Wl,-mllvm,--align-all-nofallthru-blocks=2)
    sdk_zcc_ld_options(-falign-functions=4)
    sdk_zcc_ld_options(-flate-loop-unroll)
    sdk_zcc_ld_options(-Wl,-mllvm,--jump-is-expensive=true)
    sdk_zcc_ld_options(-O3)
    sdk_zcc_ld_options("-fuse-size-lib")
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

sdk_inc(src/port)
sdk_inc(src)
sdk_app_src(src/demo_dualcore_coremark.c
        src/dualcore_widgets.c
        src/generated/hpmlogo_180x120.c
        src/sec_core_img.c
        src/port/core_portme.c
        )

sdk_ses_compile_options(-mabi=ilp32d)
sdk_ses_compile_options(-march=rv32gc)
generate_ide_projects()
