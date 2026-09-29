# Copyright (c) 2021-2022,2025-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

# Set up interface libraries for different toolchains
# These libraries are used to define the interface for the toolchains
set(HPM_SDK_NDSGCC_LIB_ITF hpm_sdk_ndsgcc_lib_itf)
add_library(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE)

set(HPM_SDK_ZCC_LIB_ITF hpm_sdk_zcc_lib_itf)
add_library(${HPM_SDK_ZCC_LIB_ITF} INTERFACE)

set(HPM_SDK_GCC_LIB_ITF hpm_sdk_gcc_lib_itf)
set(HPM_SDK_GCC_LIB hpm_sdk_gcc_lib)
set(HPM_SDK_GCC_STARTUP_LIB hpm_sdk_gcc_startup_lib)
add_library(${HPM_SDK_GCC_LIB_ITF} INTERFACE)

# Clean up the GNURISCV_TOOLCHAIN_PATH environment variable by removing unwanted characters
string(REGEX REPLACE "{\"\'}" "" GNURISCV_TOOLCHAIN_PATH $ENV{GNURISCV_TOOLCHAIN_PATH})

# Check if the toolchain path exists
if(NOT EXISTS ${GNURISCV_TOOLCHAIN_PATH})
    message(FATAL_ERROR "Nothing found at GNURISCV_TOOLCHAIN_PATH: '${GNURISCV_TOOLCHAIN_PATH}'")
endif()

# Set the toolchain home directory
set(TOOLCHAIN_HOME ${GNURISCV_TOOLCHAIN_PATH})

# Determine the toolchain variant to be used
set(TOOLCHAIN_VARIANT gcc)
if(NOT "$ENV{HPM_SDK_TOOLCHAIN_VARIANT}" STREQUAL "")
    set(TOOLCHAIN_VARIANT $ENV{HPM_SDK_TOOLCHAIN_VARIANT})
endif()

# Set the target triplet, which defines the target architecture and platform
if(CUSTOM_TARGET_TRIPLET)
    set(TARGET_TRIPLET ${CUSTOM_TARGET_TRIPLET})
else()
    set(TARGET_TRIPLET riscv32-unknown-elf)
endif()

# Determine which toolchain will be used based on the TOOLCHAIN_VARIANT
if("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
    set(COMPILER gcc)
    set(LINKER ld)
    set(BINTOOLS gnu)
    set(C++ g++)
    set(CROSS_COMPILE_TARGET riscv32-elf)
    set(SYSROOT_TARGET riscv32-elf)
    set(TOOLCHAIN_CMAKE gcc.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(COMPILER zcc)
    set(LINKER ld.lld)
    set(BINTOOLS gnu)
    set(C++ z++)
    set(SYSROOT_TARGET riscv32-unknown-elf)
    set(TOOLCHAIN_CMAKE zcc.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "gcc")
    set(COMPILER gcc)
    set(LINKER ld)
    set(BINTOOLS gnu)
    set(C++ g++)
    set(CROSS_COMPILE_TARGET ${TARGET_TRIPLET})
    set(SYSROOT_TARGET ${TARGET_TRIPLET})
    set(TOOLCHAIN_CMAKE gcc.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "nds-llvm")
    set(COMPILER clang)
    set(LINKER ld)
    set(BINTOOLS gnu)
    set(C++ clang++)
    set(CROSS_COMPILE_TARGET riscv32-elf)
    set(SYSROOT_TARGET riscv32-elf)
    set(TOOLCHAIN_CMAKE llvm.cmake)
else()
   message(FATAL_ERROR "${TOOLCHAIN_VARIANT} is not supported")
endif()

# Set the cross compile path based on the toolchain variant
if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(CROSS_COMPILE ${TOOLCHAIN_HOME}/bin/)
else()
    set(CROSS_COMPILE ${TOOLCHAIN_HOME}/bin/${CROSS_COMPILE_TARGET}-)
endif()

# Print a message indicating the found toolchain
message(STATUS "Found toolchain: gnu (${GNURISCV_TOOLCHAIN_PATH})")

# Set CMake system name and unset compilers to avoid conflicts
set(CMAKE_SYSTEM_NAME Generic)
unset(CMAKE_C_COMPILER)
unset(CMAKE_CXX_COMPILER)
set(CC ${COMPILER})

# Find the C compiler in the specified toolchain path
find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}${CC}   PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)

# Check if the C compiler was found
if(CMAKE_C_COMPILER STREQUAL CMAKE_C_COMPILER-NOTFOUND)
    message(FATAL_ERROR "It was unable to find the toolchain. Is the environment misconfigured?
    User-configuration:
    GNURISCV_TOOLCHAIN_PATH: ${GNURISCV_TOOLCHAIN_PATH}
    Internal variables:
    CROSS_COMPILE: ${CROSS_COMPILE}
    TOOLCHAIN_HOME: ${TOOLCHAIN_HOME}
    ")
endif()

# Get the compiler version text by executing the compiler with the --version flag
execute_process(
    COMMAND ${CMAKE_C_COMPILER} --version
    RESULT_VARIABLE ret
    OUTPUT_VARIABLE version_text
    ERROR_QUIET
)
if(ret)
    message(FATAL_ERROR "Executing the below command failed. Are permissions set correctly?
    '${CMAKE_C_COMPILER} --version'
    ")
endif()

# @private
# Function to get the ABI for nds-gcc by executing the compiler with the --verbose flag
# Arguments:
#   nds_gcc_abi: The variable to store the extracted ABI
function(get_nds_gcc_abi nds_gcc_abi)
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} --verbose
        RESULT_VARIABLE ret
        ERROR_VARIABLE verbose_text
        OUTPUT_QUIET
    )
    STRING(REGEX REPLACE ".*--with-abi=([A-Za-z0-9]+).*" "\\1" out ${verbose_text})
    set(${nds_gcc_abi} ${out} PARENT_SCOPE)
endfunction()

# Detect the ABI for andes toolchain explicitly if the toolchain variant is nds-gcc
if(${TOOLCHAIN_VARIANT} STREQUAL "nds-gcc")
    get_nds_gcc_abi(nds_gcc_abi)
endif()

# Set the RV_ABI if not already set
if(NOT RV_ABI)
    if(nds_gcc_abi)
        set(RV_ABI ${nds_gcc_abi})
    else()
        set(RV_ABI "ilp32")
    endif()
endif()

# Check if the specified RV_ABI matches the ABI provided by nds_gcc
if(nds_gcc_abi)
    if(NOT ${RV_ABI} STREQUAL ${nds_gcc_abi})
        message(FATAL_ERROR "Specified RV_ABI: ${RV_ABI} does not match abi provided by nds_gcc ${nds_gcc_abi}")
    endif()
endif()

# Set the RV_ARCH if not already set
if(NOT RV_ARCH)
    set(RV_ARCH "rv32imac")
endif()

# Probe whether the toolchain accepts extra march extensions on top of
# RV_ARCH. Toolchain configure-time options cannot answer this reliably:
# distro toolchains omit --with-isa-spec while still following the newer
# ISA spec, and a rejected march means the toolchain predates the
# extension and keeps those instructions in the base ISA, so the plain
# arch string stays correct as-is.
# Arguments:
#   result: Variable to receive TRUE/FALSE in the caller scope
#   ARGN: march extension suffix to probe, e.g. "_zicsr_zifencei"
function(check_march_ext_supported result)
    file(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/march_probe.c" "int main(void) { return 0; }\n")
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -march=${RV_ARCH}${ARGN} -mabi=${RV_ABI}
                -c "${CMAKE_BINARY_DIR}/CMakeFiles/march_probe.c"
                -o "${CMAKE_BINARY_DIR}/CMakeFiles/march_probe.o"
        RESULT_VARIABLE march_ext_rc
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(march_ext_rc EQUAL 0)
        set(${result} TRUE PARENT_SCOPE)
    else()
        set(${result} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Decide which ISA extensions the march string needs. Toolchains following
# ISA spec 20191213 split csr and fence.i out of the base ISA, so those
# instructions only assemble when _zicsr/_zifencei are listed explicitly;
# arch strings already carrying extensions, or implying them via g/f/d,
# need no suffix.
set(__isa_ext_suffix "")
if(NOT "${RV_ARCH}" MATCHES "_")
    set(need_csr ON)
    set(need_fencei ON)
    if("${RV_ARCH}" MATCHES "g")
        set(need_csr OFF)
        set(need_fencei OFF)
    elseif("${RV_ARCH}" MATCHES "[fd]")
        set(need_csr OFF)
    endif()
    if(need_csr AND NOT HPM_SDK_GCC_ISA_SPEC_NO_CSR)
        string(APPEND __isa_ext_suffix "_zicsr")
    endif()
    if(need_fencei AND NOT HPM_SDK_GCC_ISA_SPEC_NO_FENCEI)
        string(APPEND __isa_ext_suffix "_zifencei")
    endif()
endif()

if(__isa_ext_suffix AND NOT "${TOOLCHAIN_VARIANT}" STREQUAL "nds-llvm")
    check_march_ext_supported(__march_ext_ok "${__isa_ext_suffix}")
    if(__march_ext_ok)
        string(APPEND RV_ARCH "${__isa_ext_suffix}")
        message(STATUS "Toolchain accepts -march=${RV_ARCH}")
    else()
        message(STATUS "Toolchain keeps csr/fence.i in base ISA, using -march=${RV_ARCH}")
    endif()
endif()
unset(need_csr)
unset(need_fencei)
unset(__isa_ext_suffix)
unset(__march_ext_ok)

# Remove timestamp from libraries to ensure reproducible builds
set(CMAKE_ASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <LINK_FLAGS> <OBJECTS>")

# Set the C++ standard to 11 if not already set
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

# Set the C++ compiler
set(CXX ${C++})
# Find the C++ compiler in the specified toolchain path
find_program(CMAKE_CXX_COMPILER ${CROSS_COMPILE}${CXX}   PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)
# Include toolchain specific settings
include(${HPM_SDK_BASE}/cmake/toolchain/${TOOLCHAIN_CMAKE})

# Function to generate a C array from a binary file
# Arguments:
#   c_array_path: The path to the generated C array file
function (generate_bin2c_array c_array_path)
    add_custom_command(
        TARGET ${APP_ELF_NAME}
        COMMAND ${PYTHON_EXECUTABLE} $ENV{HPM_SDK_BASE}/scripts/bin2c.py ${EXECUTABLE_OUTPUT_PATH}/${APP_BIN_NAME} sec_core_img > ${c_array_path}
    )
endfunction ()
