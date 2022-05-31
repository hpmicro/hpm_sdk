# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

string(REGEX REPLACE "{\"\'}" "" GNURISCV_TOOLCHAIN_PATH $ENV{GNURISCV_TOOLCHAIN_PATH})

if(NOT EXISTS ${GNURISCV_TOOLCHAIN_PATH})
  message(FATAL_ERROR "Nothing found at GNURISCV_TOOLCHAIN_PATH: '${GNURISCV_TOOLCHAIN_PATH}'")
endif()

set(TOOLCHAIN_HOME ${GNURISCV_TOOLCHAIN_PATH})

set(TOOLCHAIN_VARIANT gcc)
if(NOT "$ENV{HPM_SDK_TOOLCHAIN_VARIANT}" STREQUAL "")
  set(TOOLCHAIN_VARIANT $ENV{HPM_SDK_TOOLCHAIN_VARIANT})
endif()

# determine which toolchain will be used
if("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
  set(COMPILER gcc)
  set(LINKER ld)
  set(BINTOOLS gnu)
  set(C++ g++)
  set(CROSS_COMPILE_TARGET riscv32-elf)
  set(SYSROOT_TARGET       riscv32-elf)
  set(TOOLCHAIN_CMAKE gcc.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "gcc")
  set(COMPILER gcc)
  set(LINKER ld)
  set(BINTOOLS gnu)
  set(C++ g++)
  set(CROSS_COMPILE_TARGET riscv32-unknown-elf)
  set(SYSROOT_TARGET       riscv32-unknown-elf)
  set(TOOLCHAIN_CMAKE gcc.cmake)
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "nds-llvm")
  set(COMPILER clang)
  set(LINKER ld)
  set(BINTOOLS gnu)
  set(C++ clang++)
  set(CROSS_COMPILE_TARGET riscv32-elf)
  set(SYSROOT_TARGET       riscv32-elf)
  set(TOOLCHAIN_CMAKE llvm.cmake)
else()
  message(FATAL_ERROR "${TOOLCHAIN_VARIANT} is not supported")
endif()



set(CROSS_COMPILE ${TOOLCHAIN_HOME}/bin/${CROSS_COMPILE_TARGET}-)
set(SYSROOT_DIR   ${TOOLCHAIN_HOME}/${SYSROOT_TARGET})

message(STATUS "Found toolchain: gnu (${GNURISCV_TOOLCHAIN_PATH})")

set(CMAKE_SYSTEM_NAME Generic)
unset(CMAKE_C_COMPILER)
unset(CMAKE_CXX_COMPILER)
set(CC ${COMPILER})

find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}${CC}   PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)

if(CMAKE_C_COMPILER STREQUAL CMAKE_C_COMPILER-NOTFOUND)
    message(FATAL_ERROR "It was unable to find the toolchain. Is the environment misconfigured?
User-configuration:
GNURISCV_TOOLCHAIN_PATH: ${GNURISCV_TOOLCHAIN_PATH}
Internal variables:
CROSS_COMPILE: ${CROSS_COMPILE}
TOOLCHAIN_HOME: ${TOOLCHAIN_HOME}
")
endif()

execute_process(
  COMMAND ${CMAKE_C_COMPILER} --version
  RESULT_VARIABLE ret
  OUTPUT_VARIABLE version_text 
  ERROR_QUIET
  )
if(ret)
  message(FATAL_ERROR "Executing the below command failed. Are permissions set correctly?
'${CMAKE_C_COMPILER} --version'
"
    )
endif()

get_compiler_version(${COMPILER} "${version_text}" ver)

set(COMPILER_VERSION ${ver})

set(CXX ${C++})
find_program(CMAKE_CXX_COMPILER ${CROSS_COMPILE}${CXX}   PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)
# include toolchain specific settings
include(${HPM_SDK_BASE}/cmake/toolchain/${TOOLCHAIN_CMAKE})