# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(toolchains)

if(NOT DEFINED USE_CUSTOM_STARTUP)
    sdk_gcc_startup_src(toolchains/gcc/start.S)
    sdk_ses_startup_src(toolchains/segger/startup.s)
    sdk_iar_startup_src(toolchains/iar/startup.s)
endif()

sdk_gcc_startup_src(toolchains/gcc/initfini.c)
sdk_gcc_src(
    toolchains/reset.c
    toolchains/trap.c
)

sdk_ses_src(
    toolchains/reset.c
    toolchains/trap.c
)

sdk_iar_src(
    toolchains/reset.c
    toolchains/trap.c
)

sdk_src(system.c)

# soc drivers
sdk_src (
    hpm_sysctl_drv.c
    hpm_clock_drv.c
    hpm_otp_drv.c
    hpm_pdgo_drv.c
    hpm_trgm_soc_drv.c
)

if(${INCLUDE_BOOTHEADER})
    sdk_inc(boot)
    sdk_src(boot/hpm_bootheader.c)
endif()

sdk_nds_compile_options(-mcpu=d45)
sdk_zcc_compile_options(-mtune=andes-d45-series)
sdk_zcc_ld_options(-mtune=andes-d45-series)

if(NOT HPM_SOC_DISABLE_B_EXT)
    get_toolchain_gcc_spec(spec)
    if(spec GREATER_EQUAL 20191213)
        set(RV_ARCH "${RV_ARCH}_zba_zbb_zbc_zbs" PARENT_SCOPE)
    endif()
endif()

set(SOC_LINKER_SCRIPT "" PARENT_SCOPE)
if(NOT DEFINED USE_CUSTOM_LINKER)
    if(${LINK_TO_FLASH})
        if(${INCLUDE_BOOTHEADER})
            if(${FLASH_XIP})
                set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash_xip.ld PARENT_SCOPE)
            else()
                if(${FLASH_XIP_HYBRID})
                    set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash_xip_hybrid.ld PARENT_SCOPE)
                endif()
            endif()
        else()
            if(${FLASH_UF2})
                set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash_uf2.ld PARENT_SCOPE)
            else()
                set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash.ld PARENT_SCOPE)
            endif()
        endif()
    else()
        set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/ram.ld PARENT_SCOPE)
    endif()
endif()
