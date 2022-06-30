# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause


sdk_inc(${TOOLCHAIN_HOME}/lib/gcc/${CROSS_COMPILE_TARGET}/${COMPILER_VERSION}/include)
sdk_sys_inc(${TOOLCHAIN_HOME}/${CROSS_COMPILE_TARGET}/include/c++/${COMPILER_VERSION})
sdk_compile_options("-Wall")
sdk_compile_options("-Wno-format")
sdk_compile_options("-fomit-frame-pointer")
sdk_compile_options("-fno-builtin")
sdk_compile_options("-ffunction-sections")
sdk_compile_options("-fdata-sections")

if("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
# By default, Andes' gcc toolchain uses lld as linker
include(${HPM_SDK_BASE}/cmake/toolchain/lld.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "gcc")
include(${HPM_SDK_BASE}/cmake/toolchain/ld.cmake)
if(RV_ABI)
    sdk_compile_options("-mabi=${RV_ABI}")
    sdk_ld_options("-mabi=${RV_ABI}")
endif()

if(RV_ARCH)
    sdk_compile_options("-march=${RV_ARCH}")
    sdk_ld_options("-march=${RV_ARCH}")
endif()
if(NOT HPM_SDK_NO_NANO_SPECS)
    sdk_ld_options("--specs=nano.specs")
    sdk_ld_options("-u _printf_float")
    sdk_ld_options("-u _scanf_float")
endif()

endif()

function (generate_bin2c_array c_array_path)
    add_custom_command(
        TARGET ${APP_ELF_NAME}
        COMMAND "python" $ENV{HPM_SDK_BASE}/scripts/bin2c.py ${APP_BIN_NAME} sec_core_img > ${c_array_path}
    )
endfunction ()
