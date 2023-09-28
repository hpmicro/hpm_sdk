# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED FX_INCLUDE_USER_DEFINE_FILE)
    sdk_compile_definitions(-DFX_INCLUDE_USER_DEFINE_FILE=1)
endif()
option(FX_STANDALONE_ENABLE "Enable Filex in standalone mode" OFF)

if(NOT DEFINED THREADX_ARCH)
    message(FATAL_ERROR "Error: THREADX_ARCH not defined")
endif()
if(NOT DEFINED THREADX_TOOLCHAIN)
    message(FATAL_ERROR "Error: THREADX_TOOLCHAIN not defined")
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ports/${THREADX_ARCH}/${THREADX_TOOLCHAIN})

# Then the common files
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/common)


# Include the user's override file if required
if (NOT FX_USER_FILE)
    message(STATUS "Using default fx_user.h file")
    set(FX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/common/inc/fx_user_sample.h)
else()
    message(STATUS "Using custom fx_user.h file from ${FX_USER_FILE}")
endif()

# set include file path to place configure file
set(generated_file_path "${__build_dir}/generated")
set(CUSTOM_INC_DIR ${generated_file_path}/include)
message(STATUS "copy fx_user.h to " ${CUSTOM_INC_DIR})
configure_file(${FX_USER_FILE} ${CUSTOM_INC_DIR}/fx_user.h COPYONLY)
sdk_inc(${CUSTOM_INC_DIR})
