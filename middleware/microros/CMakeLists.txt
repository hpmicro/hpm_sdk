# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc_ifdef(CONFIG_MICROROS_FOXY microros_foxy/include)
include(CheckCSourceCompiles)
if("${TOOLCHAIN_VARIANT}" STREQUAL "gcc" OR "${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
sdk_zcc_compile_definitions(-D__STDC_WANT_LIB_EXT1__=1)
sdk_link_libraries_ifdef(CONFIG_MICROROS_FOXY "${CMAKE_CURRENT_SOURCE_DIR}/microros_foxy/libmicroros/libmicroros.a")
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
  
check_c_source_compiles("
  #if defined (__riscv_float_abi_soft)
    #define MICROROS_SOFT 1
  #else 
    #error toolchain not use soft-float
  #endif
  int main(void) { return 0; }" MICROROS_SOFT)

check_c_source_compiles("
  #if defined (__riscv_float_abi_single)
    #define MICROROS_SINGLE 1
  #else 
    #error toolchain not use single-float
  #endif
  int main(void) { return 0; }" MICROROS_SINGLE)

check_c_source_compiles("
  #if defined (__riscv_float_abi_double)
    #define MICROROS_DOUBLE 1
  #else 
    #error toolchain not use double-float
  #endif
  int main(void) { return 0; }" MICROROS_DOUBLE)
if(MICROROS_SOFT)
  sdk_link_libraries_ifdef(CONFIG_MICROROS_FOXY "${CMAKE_CURRENT_SOURCE_DIR}/microros_foxy/libmicroros/libmicroros.a")
endif()
if(MICROROS_SINGLE)
  sdk_link_libraries_ifdef(CONFIG_MICROROS_FOXY "${CMAKE_CURRENT_SOURCE_DIR}/microros_foxy/libmicroros_nds/libmicroros_f.a")
endif()
if(MICROROS_DOUBLE)
  sdk_link_libraries_ifdef(CONFIG_MICROROS_FOXY "${CMAKE_CURRENT_SOURCE_DIR}/microros_foxy/libmicroros_nds/libmicroros_d.a")
endif()
else()
message(FATAL_ERROR "No libmicroros lib for this toolchain")
endif()
 
add_subdirectory(lib)
sdk_inc(driver/inc)

file(GLOB_RECURSE MIRCO_ROS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/driver/src/*.c)
sdk_src(${MIRCO_ROS_SRC})
