# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED ENV{GNURISCV_TOOLCHAIN_PATH})
    message(FATAL_ERROR "GNURISCV_TOOLCHAIN_PATH is not set yet")
endif()

set(APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "application source directory")
set(APP_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "application binary directory")
set(__build_dir ${APP_BIN_DIR}/build_tmp)

if(NOT APP_NAME)
    set(APP_NAME demo)
endif()
set(APP_ELF_NAME ${APP_NAME}.elf)
set(APP_BIN_NAME ${APP_NAME}.bin)
set(APP_MAP_NAME ${APP_NAME}.map)
set(APP_ASM_NAME ${APP_NAME}.asm)

set(HPM_SDK_LIB hpm_sdk_lib)
set(HPM_SDK_LIB_ITF hpm_sdk_lib_itf)

# store all options
add_library(${HPM_SDK_LIB_ITF} INTERFACE)

add_library(app OBJECT "")

add_executable(${APP_ELF_NAME} $<TARGET_OBJECTS:app>)

message(STATUS "Application: ${APP_SRC_DIR}")

file(TO_CMAKE_PATH "${HPM_SDK_BASE}" PROJECT_SOURCE_DIR)

set(PROJECT_SOURCE_DIR ${HPM_SDK_BASE})
set(PROJECT_BINARY_DIR ${__build_dir})

set(LIBRARY_OUTPUT_PATH ${APP_BIN_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${APP_BIN_DIR}/output)

# search board in extra_board_path if given
if(BOARD_SEARCH_PATH AND EXISTS ${BOARD_SEARCH_PATH})
    find_path(BOARD_SEARCH_DIR NAMES ${BOARD}.yaml PATHS ${BOARD_SEARCH_PATH}/* NO_DEFAULT_PATH)
    if(BOARD_SEARCH_DIR)
        set(BOARD_MESSAGE "Board (custom board): ${BOARD} from ${BOARD_SEARCH_PATH}")
        set(HPM_BOARD_DIR ${BOARD_SEARCH_PATH}/${BOARD})
    endif()
endif()

# search board in sdk
if(NOT HPM_BOARD_DIR OR NOT EXISTS ${HPM_BOARD_DIR})
    find_path(SDK_BOARD_DIR NAMES ${BOARD}.yaml PATHS ${HPM_SDK_BASE}/boards/* NO_DEFAULT_PATH)
    if(SDK_BOARD_DIR)
        set(BOARD_MESSAGE "Board: ${BOARD} from ${HPM_SDK_BASE}/boards")
        set(HPM_BOARD_DIR ${HPM_SDK_BASE}/boards/${BOARD})
    endif()
endif()

set(BOARD_YAML ${HPM_BOARD_DIR}/${BOARD}.yaml)
if(NOT HPM_BOARD_DIR OR NOT EXISTS ${BOARD_YAML})
    message(FATAL_ERROR "No board named '${BOARD}' found")
endif()
message(STATUS "${BOARD_MESSAGE}")

find_path(APP_YAML_PATH NAMES app.yaml PATHS ${APP_SRC_DIR} NO_DEFAULT_PATH)

if(NOT RV_ABI)
    set(RV_ABI "ilp32")
endif()

if(NOT RV_ARCH)
    set(RV_ARCH "rv32imac")
endif()

# add basic extentions
include(${HPM_SDK_BASE}/cmake/python.cmake)
include(${HPM_SDK_BASE}/cmake/cmake-ext.cmake)

if(APP_YAML_PATH)
    get_app_linked_project_name("${APP_YAML_PATH}/app.yaml" LINKED_PROJECT_NAME)
    get_app_linked_project_path("${APP_YAML_PATH}/app.yaml" LINKED_PROJECT_PATH)
    get_app_excluded_ides("${APP_YAML_PATH}/app.yaml" EXCLUDED_IDES)
endif()

# include more advanced extentions
include(${HPM_SDK_BASE}/cmake/toolchain.cmake)
include(${HPM_SDK_BASE}/cmake/ide.cmake)
include(${HPM_SDK_BASE}/cmake/extra_flags.cmake)
include(${HPM_SDK_BASE}/cmake/ccache.cmake)
include(${HPM_SDK_BASE}/cmake/version.cmake)

# distclean target
add_custom_target(
    distclean
    COMMAND ${CMAKE_COMMAND} -DBIN_DIR=${APP_BIN_DIR}
    -DSRC_DIR=${APP_SRC_DIR} -P ${HPM_SDK_BASE}/cmake/cleanup.cmake
)

if(NOT LOCALIZATION_DST)
    set(LOCALIZATION_DST ${APP_SRC_DIR})
endif()

if(NOT EXISTS ${LOCALIZATION_DST})
    message(FATAL_ERROR "localization dest folder: \"${LOCALIZATION_DST}\" can not be found")
endif()

add_custom_command(
    TARGET ${APP_ELF_NAME}
    COMMAND "${CROSS_COMPILE}objcopy" -O binary -S ${EXECUTABLE_OUTPUT_PATH}/${APP_ELF_NAME} ${EXECUTABLE_OUTPUT_PATH}/${APP_BIN_NAME}
)

add_custom_command(
    TARGET ${APP_ELF_NAME}
    COMMAND "${CROSS_COMPILE}objdump" -S -d ${EXECUTABLE_OUTPUT_PATH}/${APP_ELF_NAME} > ${EXECUTABLE_OUTPUT_PATH}/${APP_ASM_NAME}
)

if(APP_YAML_PATH)
    check_board_capability(${BOARD_YAML} "${APP_YAML_PATH}/app.yaml" result)
    if(${result} STREQUAL "1")
        message(FATAL_ERROR "${BOARD} can not support this sample")
    endif()
    if(${result} STREQUAL "2")
        message(FATAL_ERROR "Failed to build core1 project for this sample")
    endif()
    check_excluded_targets("${APP_YAML_PATH}/app.yaml" excluded_targets)
    # check if specific minimum SDK version is needed
    get_app_min_sdk_version("${APP_YAML_PATH}/app.yaml" app_min_sdk_version)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DREQUIRED_MINIMUM_SDK_VERSION=${app_min_sdk_version}
        -DHPM_SDK_BASE=${HPM_SDK_BASE}
        -DSDK_VERSION_MAJOR=${SDK_VERSION_MAJOR}
        -DSDK_VERSION_MINOR=${SDK_VERSION_MINOR}
        -DSDK_PATCHLEVEL=${SDK_PATCHLEVEL}
        -P ${HPM_SDK_BASE}/cmake/compare_target_sdk_version.cmake
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        RESULT_VARIABLE result
        ERROR_VARIABLE error_output
    )
    if(${result})
        message(FATAL_ERROR ${error_output})
    endif()
endif()

# default build type is set to debug
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE debug)
endif()

# target of "localize_sdk"
add_custom_target(
    localize_sdk
    COMMAND ${CMAKE_COMMAND}
    -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
    -DHPM_SDK_BASE=${HPM_SDK_BASE}
    -DSRC_DIR=${APP_SRC_DIR}
    -DDST_DIR=${LOCALIZATION_DST}
    -DBIN_DIR=${APP_BIN_DIR}
    -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DGENERATOR=${CMAKE_GENERATOR}
    -DBOARD=${BOARD}
    -P ${HPM_SDK_BASE}/cmake/localize_sdk.cmake
)

add_custom_target(
    unlocalize_sdk
    COMMAND ${CMAKE_COMMAND}
    -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
    -DHPM_SDK_BASE=${HPM_SDK_BASE}
    -DSRC_DIR=${APP_SRC_DIR}
    -DBIN_DIR=${APP_BIN_DIR}
    -DDST_DIR=${LOCALIZATION_DST}
    -DGENERATOR=${CMAKE_GENERATOR}
    -DUNLOCALIZE_PROJECT=1
    -P ${HPM_SDK_BASE}/cmake/localize_sdk.cmake
)

if((excluded_targets) AND (NOT ${CMAKE_BUILD_TYPE} STREQUAL ""))
    foreach(t IN ITEMS  ${excluded_targets})
        string(COMPARE EQUAL ${CMAKE_BUILD_TYPE} ${t} result)
        if (${result} EQUAL 1)
            message(FATAL_ERROR "\n!!! target ${CMAKE_BUILD_TYPE} has been excluded for this example\n")
        endif()
    endforeach()
endif()

get_soc_name_of_board(${BOARD_YAML} soc_name)
set(HPM_SOC ${soc_name})

# device name
get_device_name_of_board(${BOARD_YAML} device_name)
set(HPM_DEVICE_NAME ${device_name})

get_flash_size_of_board(${BOARD_YAML} flash_size)
get_extram_size_of_board(${BOARD_YAML} extram_size)

string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
if(NOT extram_size)
    string(FIND ${build_type} "sdram" found)
    if(${found} GREATER_EQUAL 0)
        message(FATAL_ERROR "\n!!! target ${build_type} is not supported for ${BOARD}\n")
    endif()
endif()

if(NOT HEAP_SIZE)
    SET(HEAP_SIZE 0x4000)    #segger default heap size
endif()
sdk_linker_global_symbols("_heap_size=${HEAP_SIZE}")

if(NOT STACK_SIZE)
    SET(STACK_SIZE 0x4000)    #segger default stack size
endif()
sdk_linker_global_symbols("_stack_size=${STACK_SIZE}")

# skip compiler check
set(CMAKE_C_COMPILER_FORCED 1)
set(CMAKE_CXX_COMPILER_FORCED 1)

enable_language(C CXX ASM)
add_subdirectory(${HPM_SDK_BASE} ${__build_dir})

if (DEFINED GEN_SEC_CORE_IMG_C_ARRAY)
    if(NOT DEFINED SEC_CORE_IMG_C_ARRAY_OUTPUT)
        set(SEC_CORE_IMG_C_ARRAY_OUTPUT "sec_core_img.c")
    endif()
    generate_bin2c_array(${SEC_CORE_IMG_C_ARRAY_OUTPUT})
endif()

# link final executable
target_link_libraries(app PUBLIC ${HPM_SDK_LIB_ITF} ${HPM_SDK_GCC_LIB_ITF})

if(${APP_SRC_DIR} STREQUAL ${APP_BIN_DIR})
    message(FATAL_ERROR "source directory is the same with binary directory.\
    please specify a build directory, e.g. cmake -Bbuild -H.")
endif()

