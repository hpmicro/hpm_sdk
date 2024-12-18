# Copyright (c) 2021,2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(${HPM_SOC_SERIES}/${HPM_SOC})
sdk_inc(${HPM_SOC_SERIES}/ip)
add_subdirectory(${HPM_SOC_SERIES}/${HPM_SOC})

if(NOT "${SOC_LINKER_SCRIPT}" STREQUAL "")
    set(LINKER_SCRIPT ${SOC_LINKER_SCRIPT} PARENT_SCOPE)
endif()

if("${TOOLCHAIN_VARIANT}" STREQUAL "gcc")
    sdk_compile_options("-mabi=${RV_ABI}")
    sdk_ld_options("-mabi=${RV_ABI}")

    sdk_compile_options("-march=${RV_ARCH}")
    sdk_ld_options("-march=${RV_ARCH}")
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    sdk_compile_options("-mabi=${RV_ABI}")
    sdk_ld_options("-mabi=${RV_ABI}")

    if(CONFIG_HPM_ENABLE_DSP)
        set(RV_ARCH "${RV_ARCH}_zp052b")
    endif()
    sdk_compile_options("-march=${RV_ARCH}_xandes")
    sdk_ld_options("-march=${RV_ARCH}_xandes")
endif()

sdk_zcc_ld_options(-Wl,-mllvm,--align-all-nofallthru-blocks=2)
sdk_zcc_ld_options(-falign-functions=4)
sdk_zcc_ld_options(-flate-loop-unroll)
sdk_zcc_ld_options(-Wl,-mllvm,--jump-is-expensive=true)
