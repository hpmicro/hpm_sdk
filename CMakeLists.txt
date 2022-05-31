# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.20)
    cmake_policy(SET CMP0116 OLD)
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

# to store all options
add_library(${HPM_SDK_LIB_ITF} INTERFACE)

add_library(${HPM_SDK_LIB} STATIC "")
target_link_libraries(${HPM_SDK_LIB} PUBLIC ${HPM_SDK_LIB_ITF})

include(cmake/extra_flags.cmake)

include(${HPM_SDK_BASE}/cmake/toolchain.cmake)
enable_language(C CXX ASM)

sdk_linker_global_symbols("_flash_size=${flash_size}")
sdk_linker_global_symbols("_extram_size=${extram_size}")
sdk_linker_global_symbols("_heap_size=${HEAP_SIZE}")
sdk_linker_global_symbols("_stack_size=${STACK_SIZE}")

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
    string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
    string(FIND ${build_type} "release" found)
    if (${found} GREATER_EQUAL 0)
        sdk_compile_options("-O3")
    else()
        sdk_compile_options("-g")
    endif()

    SET(FLASH_XIP 0)
    SET(FLASH_SDRAM_XIP 0)
    SET(LINK_TO_FLASH 0)
    SET(INCLUDE_BOOTHEADER 0)
    set(USE_PRESET_FLASH_LINKER 0)
    string(FIND ${build_type} "flash_" found)
    if(${found} GREATER_EQUAL 0)
        set(USE_PRESET_FLASH_LINKER 1)
        string(FIND ${build_type} "flash_xip" found)
        if(${found} GREATER_EQUAL 0)
            set(FLASH_XIP 1)
            set(LINK_TO_FLASH 1)
            set(INCLUDE_BOOTHEADER 1)
            sdk_compile_definitions("-DFLASH_XIP=1")
        endif()
        string(FIND ${build_type} "flash_sdram_xip" found)
        if(${found} GREATER_EQUAL 0)
            set(FLASH_SDRAM_XIP 1)
            set(LINK_TO_FLASH 1)
            set(INCLUDE_BOOTHEADER 1)
            sdk_compile_definitions("-DFLASH_XIP=1")
            sdk_compile_definitions("-DINIT_EXT_RAM_FOR_DATA=1")
        endif()

        string(FIND ${build_type} "flash_uf2" found)
        if(${found} GREATER_EQUAL 0)
            set(FLASH_UF2 1)
            set(LINK_TO_FLASH 1)
            set(EXCLUDE_BOOTHEADER 1)
            sdk_compile_definitions("-DFLASH_UF2=1")
        endif()

        string(FIND ${build_type} "flash_sdram_uf2" found)
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

    string(FIND ${build_type} "sec_core_img" found)
    if(${found} GREATER_EQUAL 0)
        set(GEN_SEC_CORE_IMG_C_ARRAY true)
    endif()

else()
    sdk_compile_options("-g")
endif()

if(NOT DEFINED USE_PRESET_FLASH_LINKER OR "${USE_PRESET_FLASH_LINKER}" EQUAL "0")
    if(DEFINED CUSTOM_LINKER_FILE)
        set(USE_CUSTOM_LINKER 1)
        set(LINKER_SCRIPT ${CUSTOM_LINKER_FILE})
    else()
        if(DEFINED USE_LINKER_TEMPLATE)
            set(USE_CUSTOM_LINKER 1)
            set(LINKER_SCRIPT ${PROJECT_BINARY_DIR}/linker.cmd)
        endif()
    endif()
endif()

if(${EXCLUDE_SDK_STARTUP})
    set(USE_CUSTOM_STARTUP 1)
endif()

if(DEFINED CUSTOM_STARTUP_FILE)
    set(USE_CUSTOM_STARTUP 1)
    sdk_src(${CUSTOM_STARTUP_FILE})
endif()

sdk_sys_inc(${SYSROOT_DIR}/include)

# parse module list file provided in soc folder, it will decide which modules to be enabled under drivers folder
import_soc_modules(soc/${HPM_SOC}/soc_modules.list)

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
       ${HPM_SDK_BASE}/soc/${HPM_SOC}/toolchains/gcc/linker.ld
       ${linker_script_dep}
       COMMAND ${CMAKE_C_COMPILER}
       -x assembler-with-cpp
       -MD -MF ${LINKER_SCRIPT}.dep -MT ${base_name}/${LINKER_SCRIPT}
       ${all_defs}
       -E ${HPM_SDK_BASE}/soc/${HPM_SOC}/toolchains/gcc/linker.ld
       -P
       -o ${LINKER_SCRIPT}
       WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
       VERBATIM)
   add_custom_target(${LINKER_TARGET} DEPENDS ${LINKER_SCRIPT})
endif()

sdk_ld_options("-T ${LINKER_SCRIPT}")

file(WRITE ${PROJECT_BINARY_DIR}/misc/empty.c "")
add_executable(${APP_ELF_NAME} ${PROJECT_BINARY_DIR}/misc/empty.c)

set_target_properties(${APP_ELF_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

if("${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc")
    target_link_libraries(${APP_ELF_NAME}
        "-Wl,-Map=${PROJECT_BINARY_DIR}/${APP_MAP_NAME}"
        ${HPM_SDK_LIB} ${HPM_SDK_LIB_ITF} hpm_sdk_nds_lib_itf app)
else()
    target_link_libraries(${APP_ELF_NAME}
    "-Wl,-Map=${PROJECT_BINARY_DIR}/${APP_MAP_NAME}"
    ${HPM_SDK_LIB} ${HPM_SDK_LIB_ITF} app)
endif()

add_custom_command(
    TARGET ${APP_ELF_NAME}
    COMMAND "${CROSS_COMPILE}objcopy" -O binary -S ${PROJECT_BINARY_DIR}/${APP_ELF_NAME} ${PROJECT_BINARY_DIR}/${APP_BIN_NAME}
)

add_custom_command(
    TARGET ${APP_ELF_NAME}
    COMMAND "${CROSS_COMPILE}objdump" -S -d ${PROJECT_BINARY_DIR}/${APP_ELF_NAME} > ${PROJECT_BINARY_DIR}/${APP_ASM_NAME}
)


if (DEFINED GEN_SEC_CORE_IMG_C_ARRAY)
    if(NOT DEFINED SEC_CORE_IMG_C_ARRAY_OUTPUT)
        set(SEC_CORE_IMG_C_ARRAY_OUTPUT "sec_core_img.c")
    endif()
    generate_bin2c_array(${SEC_CORE_IMG_C_ARRAY_OUTPUT})
endif()
