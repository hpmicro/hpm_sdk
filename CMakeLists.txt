# Copyright 2021-2022,2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0079 NEW)

project(hpm_sdk)

add_library(${HPM_SDK_LIB} STATIC "")
target_link_libraries(${HPM_SDK_LIB} PUBLIC ${HPM_SDK_LIB_ITF})

add_library(${HPM_SDK_GCC_LIB} STATIC "")
target_link_libraries(${HPM_SDK_GCC_LIB} PUBLIC ${HPM_SDK_GCC_LIB_ITF} ${HPM_SDK_LIB_ITF})

add_library(${HPM_SDK_GCC_STARTUP_LIB} STATIC "")
target_link_libraries(${HPM_SDK_GCC_STARTUP_LIB} PUBLIC ${HPM_SDK_GCC_LIB_ITF} ${HPM_SDK_LIB_ITF})

target_link_libraries(${HPM_SDK_LIB} PUBLIC ${HPM_SDK_GCC_LIB_ITF})
if("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
    target_link_libraries(${HPM_SDK_LIB} PUBLIC ${HPM_SDK_NDSGCC_LIB_ITF})
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    target_link_libraries(${HPM_SDK_LIB} PUBLIC ${HPM_SDK_ZCC_LIB_ITF})
    target_link_libraries(${HPM_SDK_GCC_LIB} PUBLIC ${HPM_SDK_ZCC_LIB_ITF})
    target_link_libraries(${HPM_SDK_GCC_STARTUP_LIB} PUBLIC ${HPM_SDK_ZCC_LIB_ITF})
endif()

if(flash_size)
    sdk_linker_global_symbols("_flash_size=${flash_size}")
endif()

if(extram_size)
    sdk_linker_global_symbols("_extram_size=${extram_size}")
endif()

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
    string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
    string(FIND ${build_type} "release" found)
    if (${found} GREATER_EQUAL 0)
        sdk_compile_options("-O3")
    else()
        sdk_compile_options("-g")
    endif()
endif()

string(TOLOWER ${HPM_BUILD_TYPE} hpm_build_type)
SET(FLASH_XIP 0)
SET(FLASH_SDRAM_XIP 0)
SET(LINK_TO_FLASH 0)

if(NOT DEFINED INCLUDE_BOOTHEADER)
    SET(INCLUDE_BOOTHEADER 0)
endif()

string(FIND ${hpm_build_type} "flash_" found)
if(${found} GREATER_EQUAL 0)
    string(FIND ${hpm_build_type} "flash_xip" found)
    if(${found} GREATER_EQUAL 0)
        string(FIND ${hpm_build_type} "flash_xip_hybrid" found)
        if(${found} GREATER_EQUAL 0)
            set(FLASH_XIP_HYBRID 1)
        else()
            set(FLASH_XIP 1)
        endif()
        set(LINK_TO_FLASH 1)
        set(INCLUDE_BOOTHEADER 1)
        sdk_compile_definitions("-DFLASH_XIP=1")
    endif()
    string(FIND ${hpm_build_type} "flash_sdram_xip" found)
    if(${found} GREATER_EQUAL 0)
        set(FLASH_SDRAM_XIP 1)
        set(LINK_TO_FLASH 1)
        set(INCLUDE_BOOTHEADER 1)
        sdk_compile_definitions("-DFLASH_XIP=1")
        sdk_compile_definitions("-DINIT_EXT_RAM_FOR_DATA=1")
    endif()

    string(FIND ${hpm_build_type} "flash_uf2" found)
    if(${found} GREATER_EQUAL 0)
        set(FLASH_UF2 1)
        set(LINK_TO_FLASH 1)
        set(EXCLUDE_BOOTHEADER 1)
        sdk_compile_definitions("-DFLASH_UF2=1")
    endif()

    string(FIND ${hpm_build_type} "flash_sdram_uf2" found)
    if(${found} GREATER_EQUAL 0)
        set(FLASH_SDRAM_UF2 1)
        set(LINK_TO_FLASH 1)
        set(EXCLUDE_BOOTHEADER 1)
        sdk_compile_definitions("-DFLASH_UF2=1")
        sdk_compile_definitions("-DINIT_EXT_RAM_FOR_DATA=1")
    endif()

    if(DEFINED EXCLUDE_BOOTHEADER)
        if (${EXCLUDE_BOOTHEADER})
            set(INCLUDE_BOOTHEADER 0)
        endif()
    endif()
endif()

string(FIND ${hpm_build_type} "sec_core_img" found)
if(${found} GREATER_EQUAL 0)
    set(BUILD_FOR_SECONDARY_CORE true CACHE INTERNAL "")
    set(GEN_SEC_CORE_IMG_C_ARRAY true PARENT_SCOPE)
endif()

set(USE_PRESET_FLASH_LINKER 0)
if(DEFINED CUSTOM_GCC_LINKER_FILE)
    set(USE_CUSTOM_LINKER 1)
    if(NOT IS_ABSOLUTE ${CUSTOM_GCC_LINKER_FILE})
        message(FATAL_ERROR "Custom ld script has to be absolute path not relative: ${CUSTOM_GCC_LINKER_FILE}")
    else()
        set(LINKER_SCRIPT ${CUSTOM_GCC_LINKER_FILE})
    endif()
elseif(DEFINED USE_LINKER_TEMPLATE)
    set(USE_CUSTOM_LINKER 1)
    set(LINKER_SCRIPT ${PROJECT_BINARY_DIR}/linker.cmd)
else()
    set(USE_PRESET_FLASH_LINKER 1)
endif()

if(${EXCLUDE_SDK_STARTUP})
    set(USE_CUSTOM_STARTUP 1)
endif()

if(DEFINED CUSTOM_GCC_STARTUP_FILE)
    set(USE_CUSTOM_STARTUP 1)
    sdk_gcc_startup_src(${CUSTOM_GCC_STARTUP_FILE})
endif()

if(DEFINED CUSTOM_SES_STARTUP_FILE)
    set(USE_CUSTOM_STARTUP 1)
    sdk_ses_src(${CUSTOM_SES_STARTUP_FILE})
endif()

if(DEFINED CUSTOM_IAR_STARTUP_FILE)
    set(USE_CUSTOM_STARTUP 1)
    sdk_iar_src(${CUSTOM_IAR_STARTUP_FILE})
endif()

# parse module list file provided in soc folder, it will decide which modules to be enabled under drivers folder
import_soc_modules(soc/${HPM_SOC_SERIES}/${HPM_SOC}/soc_modules.list)

add_subdirectory(arch)
add_subdirectory(boards)
add_subdirectory(soc)
add_subdirectory(drivers)
add_subdirectory(utils)
add_subdirectory(components)
add_subdirectory(middleware)

if(DEFINED USE_LINKER_TEMPLATE)
   set(linker_script_dep DEPFILE ${PROJECT_BINARY_DIR}/${LINKER_SCRIPT}.dep)
   get_directory_property(compile_defs COMPILE_DEFINITIONS)
   foreach(def ${compile_defs})
       list(APPEND all_defs -D${def})
   endforeach()
   get_filename_component(base_name ${CMAKE_CURRENT_BINARY_DIR} NAME)
   set(LINKER_TARGET linker)
   add_custom_command(
       OUTPUT ${LINKER_SCRIPT}
       DEPENDS
       ${HPM_SDK_BASE}/soc/${HPM_SOC_SERIES}/${HPM_SOC}/toolchains/gcc/linker.ld
       ${linker_script_dep}
       COMMAND ${CMAKE_C_COMPILER}
       -x assembler-with-cpp
       -MD -MF ${LINKER_SCRIPT}.dep -MT ${base_name}/${LINKER_SCRIPT}
       ${all_defs}
       -E ${HPM_SDK_BASE}/soc/${HPM_SOC_SERIES}/${HPM_SOC}/toolchains/gcc/linker.ld
       -P
       -o ${LINKER_SCRIPT}
       WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
       VERBATIM)
   add_custom_target(${LINKER_TARGET} DEPENDS ${LINKER_SCRIPT})
endif()

if(NOT EXISTS ${LINKER_SCRIPT})
    message(FATAL_ERROR "Can not locate GNU ld linker script: ${LINKER_SCRIPT}")
else()
    message(STATUS "GNU ld linker script: ${LINKER_SCRIPT}")
endif()
sdk_ld_options("-T ${LINKER_SCRIPT}")

set(generated_file_path "${PROJECT_BINARY_DIR}/generated")

# generate SDK version file
execute_process(
  COMMAND ${CMAKE_COMMAND} -DHPM_SDK_BASE=${HPM_SDK_BASE}
    -DOUT_FILE=${generated_file_path}/include/hpm_sdk_version.h
    -DSDKVERSION=${SDKVERSION}
    -DSDK_VERSION_NUMBER=${SDK_VERSION_NUMBER}
    -DSDK_VERSION_MAJOR=${SDK_VERSION_MAJOR}
    -DSDK_VERSION_MINOR=${SDK_VERSION_MINOR}
    -DSDK_PATCHLEVEL=${SDK_PATCHLEVEL}
    -DSDK_VERSION_STRING=${SDK_VERSION_STRING}
    -P ${HPM_SDK_BASE}/cmake/gen_version_h.cmake
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
sdk_inc(${generated_file_path}/include)

set_target_properties(${APP_ELF_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

if(NOT DEFINED HPM_SDK_LD_NO_WHOLE_SDK)
    set(HPM_SDK_LD_NO_WHOLE_SDK 0)
endif()

set(HPM_SDK_WHOLE_ARCHIVE_OPT "-Wl,--whole-archive")
if(${HPM_SDK_LD_NO_WHOLE_SDK})
    set(HPM_SDK_WHOLE_ARCHIVE_OPT "")
endif()

if("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
    target_link_libraries(${HPM_SDK_LIB} PUBLIC ${HPM_SDK_NDSGCC_LIB_ITF})
    target_link_libraries(${APP_ELF_NAME}
        "-Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${APP_MAP_NAME}"
        ${HPM_SDK_WHOLE_ARCHIVE_OPT}
        ${HPM_SDK_NDSGCC_LIB_ITF}
        ${HPM_SDK_GCC_STARTUP_LIB}
        ${HPM_SDK_GCC_LIB} ${HPM_SDK_GCC_LIB_ITF}
        ${HPM_SDK_LIB} ${HPM_SDK_LIB_ITF} app
        "-Wl,--no-whole-archive")
elseif("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    target_link_libraries(${HPM_SDK_LIB} PUBLIC ${HPM_SDK_ZCC_LIB_ITF})
    target_link_libraries(${APP_ELF_NAME}
        "-Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${APP_MAP_NAME}"
        ${HPM_SDK_WHOLE_ARCHIVE_OPT}
        ${HPM_SDK_ZCC_LIB_ITF}
        ${HPM_SDK_GCC_STARTUP_LIB}
        ${HPM_SDK_GCC_LIB} ${HPM_SDK_GCC_LIB_ITF}
        ${HPM_SDK_LIB} ${HPM_SDK_LIB_ITF} app
        "-Wl,--no-whole-archive")
else()
    target_link_libraries(${APP_ELF_NAME}
        "-Wl,-Map=${EXECUTABLE_OUTPUT_PATH}/${APP_MAP_NAME}"
        ${HPM_SDK_WHOLE_ARCHIVE_OPT}
        ${HPM_SDK_GCC_STARTUP_LIB}
        ${HPM_SDK_GCC_LIB} ${HPM_SDK_GCC_LIB_ITF}
        ${HPM_SDK_LIB} ${HPM_SDK_LIB_ITF} app
        "-Wl,--no-whole-archive")
endif()
