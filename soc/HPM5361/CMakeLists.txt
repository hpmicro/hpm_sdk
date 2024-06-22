# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(toolchains)

if(NOT DEFINED USE_CUSTOM_STARTUP)
    sdk_gcc_src(toolchains/gcc/start.S)
    sdk_ses_src(toolchains/segger/startup.s)
    sdk_iar_src(toolchains/iar/startup.s)
endif()

sdk_src(
    toolchains/reset.c
    toolchains/trap.c
    system.c
)

sdk_gcc_src(toolchains/gcc/initfini.c)

# soc drivers
sdk_src (
    hpm_sysctl_drv.c
    hpm_l1c_drv.c
    hpm_clock_drv.c
    hpm_otp_drv.c
)

if(${INCLUDE_BOOTHEADER})
    sdk_inc(boot)
    sdk_src(boot/hpm_bootheader.c)
endif()

sdk_nds_compile_options(-mcpu=d25)

if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    sdk_compile_options(--target=riscv32-unknown-elf)
    sdk_compile_options(-march=rv32imafc_zp052b_xandes)
    sdk_compile_options(-mabi=ilp32f)
    sdk_compile_options(-mtune=andes-kavalan)
endif()

sdk_zcc_ld_options(--target=riscv32-unknown-elf)
sdk_zcc_ld_options(-march=rv32imafc_zp052b_xandes)
sdk_zcc_ld_options(-mabi=ilp32f)
sdk_zcc_ld_options(-mtune=andes-kavalan)
sdk_zcc_ld_options(-Wl,-mllvm,--align-all-nofallthru-blocks=2)
sdk_zcc_ld_options(-falign-functions=4)
sdk_zcc_ld_options(-flate-loop-unroll)
sdk_zcc_ld_options(-Wl,-mllvm,--jump-is-expensive=true)


set(SOC_LINKER_SCRIPT "" PARENT_SCOPE)
if(NOT DEFINED USE_CUSTOM_LINKER)
    if(${LINK_TO_FLASH})
        if(${INCLUDE_BOOTHEADER})
            if(${FLASH_XIP})
                set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash_xip.ld PARENT_SCOPE)
            else()
                if(${FLASH_SDRAM_XIP})
                    set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash_sdram_xip.ld PARENT_SCOPE)
                endif()
            endif()
        else()
            if(${FLASH_UF2})
                set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash_uf2.ld PARENT_SCOPE)
            else()
                if(${FLASH_SDRAM_UF2})
                    set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash_sdram_uf2.ld PARENT_SCOPE)
                else()
                    set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/flash.ld PARENT_SCOPE)
                endif()
            endif()
        endif()
    else()
        set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/gcc/ram.ld PARENT_SCOPE)
    endif()
endif()
