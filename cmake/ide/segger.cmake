# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

# add ses library interface to store ses specific configurations
add_library(hpm_sdk_ses_lib_itf INTERFACE)

function(sdk_ses_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(hpm_sdk_ses_lib_itf INTERFACE ${opt})
    endforeach()
endfunction()

function(sdk_ses_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(hpm_sdk_ses_lib_itf INTERFACE ${def})
    endforeach()
endfunction()

function(sdk_ses_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(hpm_sdk_ses_lib_itf INTERFACE ${lib})
    endforeach()
endfunction()

function(sdk_ses_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(hpm_sdk_ses_lib_itf INTERFACE ${opt})
    endforeach()
endfunction()

function(sdk_ses_ld_lib)
    foreach(opt ${ARGN})
        set_property(TARGET hpm_sdk_ses_lib_itf APPEND PROPERTY INTERFACE_SES_LD_INPUTS ${opt})
    endforeach()
endfunction()


function (generate_ses_project)
    get_property(target_source_files TARGET app PROPERTY SOURCES)
    get_property(target_app_include_dirs TARGET app PROPERTY INCLUDE_DIRECTORIES)
    get_property(target_lib_sources TARGET ${HPM_SDK_LIB} PROPERTY SOURCES)
    get_property(target_include_dirs TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    get_property(target_defines TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_COMPILE_DEFINITIONS)
    get_property(target_link_sym TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_LINK_SYMBOLS)
    get_property(target_gcc_cflags TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_COMPILE_OPTIONS)
    get_property(target_ses_cflags TARGET hpm_sdk_ses_lib_itf PROPERTY INTERFACE_COMPILE_OPTIONS)
    get_property(target_ses_ld_lib TARGET hpm_sdk_ses_lib_itf PROPERTY INTERFACE_SES_LD_INPUTS)


    if(NOT SES_TOOLCHAIN_VARIANT)
        set(SES_TOOLCHAIN_VARIANT "Standard")
    endif()

    string(FIND ${SES_TOOLCHAIN_VARIANT} "Andes" exist)
    if (NOT ${exist} EQUAL -1)
        set(SES_USE_TOOLCHAIN_ANDES 1)
        get_property(target_nds_cflags TARGET hpm_sdk_nds_lib_itf PROPERTY INTERFACE_COMPILE_OPTIONS)
    endif()

    if(target_ses_ld_lib)
        foreach(f IN ITEMS ${target_ses_ld_lib})
            if("${target_ses_ld_input}" STREQUAL "")
                set(target_ses_ld_input ${f})
            else()
                set(target_ses_ld_input "${target_ses_ld_input};${f}")
            endif()
        endforeach()
    else()
        set(target_ses_ld_input "")
    endif()

    set(target_cflags "")
    if (SES_USE_TOOLCHAIN_ANDES)
        # if Andes toolchain has not been specified, cflags needs to be preserved
        foreach(f IN ITEMS ${target_gcc_cflags})
            string(FIND ${f} "-mabi=" exist)
            if(NOT ${exist} EQUAL -1)
                string(SUBSTRING ${f} 6 -1 SES_COMPILER_ABI)
                continue()
            endif()
            string(FIND ${f} "-march=" exist)
            if(NOT ${exist} EQUAL -1)
                string(SUBSTRING ${f} 7 -1 SES_COMPILER_ARCH)
                continue()
            endif()
            if("${target_cflags}" STREQUAL "")
                set(target_cflags ${f})
            else()
                set(target_cflags "${target_cflags};${f}")
            endif()
        endforeach()
        if(target_nds_cflags)
            foreach(f IN ITEMS ${target_nds_cflags})
                if("${target_cflags}" STREQUAL "")
                    set(target_cflags ${f})
                else()
                    set(target_cflags "${target_cflags};${f}")
                endif()
            endforeach()
        endif()
    endif()

    foreach(f IN ITEMS ${target_ses_cflags})
        string(FIND ${f} "-mabi=" exist)
        if(NOT ${exist} EQUAL -1)
            string(SUBSTRING ${f} 6 -1 SES_COMPILER_ABI)
            continue()
        endif()
        string(FIND ${f} "-march=" exist)
        if(NOT ${exist} EQUAL -1)
            string(SUBSTRING ${f} 7 -1 SES_COMPILER_ARCH)
            continue()
        endif()

        set(enable_nds_dsp 0)
        string(FIND ${f} "-mext-dsp" exist)
        if(NOT ${exist} EQUAL -1)
            set(enable_nds_dsp 1)
            continue()
        endif()

        set(enable_cpp_exceptions 0)
        string(FIND ${f} "-fexceptions" exist)
        if(NOT ${exist} EQUAL -1)
            set(enable_cpp_exceptions 1)
            continue()
        endif()

        if(NOT target_cflags)
            set(target_cflags ${f})
        else()
            set(target_cflags "${target_cflags};${f}")
        endif()
    endforeach()

    # fallback option
    if(NOT RV_ABI)
        set(RV_ABI ilp32)
    endif()
    if(NOT RV_ARCH)
        set(RV_ARCH rv32ima)
    endif()

    if(NOT SES_COMPILER_ABI)
        set(SES_COMPILER_ABI ${RV_ABI})
    endif()
    if(NOT SES_COMPILER_ARCH)
        STRING(REGEX REPLACE "imafd" "g" RV_ARCH ${RV_ARCH})
        STRING(REGEX REPLACE "imadf" "g" RV_ARCH ${RV_ARCH})
        set(SES_COMPILER_ARCH ${RV_ARCH})
    endif()

    set(post_build_command "")
    if (DEFINED IS_SEC_CORE_IMG)
        if (IS_SEC_CORE_IMG)
            if (NOT DEFINED SEC_CORE_IMG_C_ARRAY_OUTPUT)
                set (SEC_CORE_IMG_C_ARRAY_OUTPUT "sec_core_img.c")
            endif()
            set(post_build_command "${PYTHON_EXECUTABLE} $(HPM_SDK_BASE)/scripts/bin2c.py $(OutDir)/$(ProjectName).bin sec_core_img > ${SEC_CORE_IMG_C_ARRAY_OUTPUT}")
            string(REPLACE "\\" "/" post_build_command ${post_build_command})
        endif()
    endif()

    set(target_link_symbols "")
    foreach(sym IN ITEMS ${target_link_sym})
        string(FIND ${sym} "_heap_size" exist)
        if(NOT ${exist} EQUAL -1)
            continue()
        endif()
        string(FIND ${sym} "_stack_size" exist)
        if(NOT ${exist} EQUAL -1)
            continue()
        endif()
        if("${target_link_symbols}" STREQUAL "")
            set(target_link_symbols ${sym})
        else()
            set(target_link_symbols "${target_link_symbols},${sym}")
        endif()
    endforeach()

    set(target_sources "")
    foreach(src IN ITEMS ${target_source_files})
        if("${target_sources}" STREQUAL "")
            set(target_sources ${src})
        else()
            set(target_sources "${target_sources},${src}")
        endif()
    endforeach()

    get_property(target_linker_script TARGET ${APP_ELF_NAME} PROPERTY LINK_DEPENDS)
    if(SES_USE_TOOLCHAIN_ANDES)
        # if Andes toolchain has not been specified, gcc linker script should be used
        set(target_linker ${target_linker_script})
    else()
        if(SEGGER_LINKER_FILE)
            # SEGGER_LINKER_FILE is specified
            set(target_linker ${SEGGER_LINKER_FILE})
        else()
            # if SEGGER_LINKER_FILE is not specified, try to locate segger linker
            # located in segger folder naming with ".icf" rather than ".ld"
            string(REPLACE ".ld" ".icf" target_linker ${target_linker_script})
            string(REPLACE "/gcc/" "/segger/" target_linker ${target_linker})
        endif()
    endif()

    set(OPENOCD_READY 1)
    set(BOARD_YAML "${HPM_SDK_BASE}/boards/${BOARD}/${BOARD}.yaml")
    # get specified openocd config file for probe
    get_openocd_probe_name_of_board(${BOARD} OPENOCD_PROBE)
    if(NOT OPENOCD_PROBE)
        message(STATUS " Segger: openocd-probe was not correctly configured in ${BOARD_YAML}")
        set(OPENOCD_READY 0)
    endif()
    # get specified openocd config file for soc
    get_openocd_soc_name_of_board(${BOARD} OPENOCD_SOC)
    if(NOT OPENOCD_SOC)
        message(STATUS " Segger: openocd-soc was not correctly configured in ${BOARD_YAML}")
        set(OPENOCD_READY 0)
    endif()
    find_program(OPENOCD openocd)
    if(NOT EXISTS ${OPENOCD})
        message(STATUS " Segger: openocd can not be located")
        set(OPENOCD_READY 0)
    endif()

    if (APP_YAML_PATH)
        set(APP_YAML "${APP_YAML_PATH}/app.yaml")
        get_ses_debug_auto_start_gdb_server(${APP_YAML} AUTO_START_GDB_SRV)
        if(NOT DEFINED AUTO_START_GDB_SRV)
            message(STATUS " Segger: openocd auto start gdb server was not correctly configured in ${APP_YAML}")
        endif()

        get_ses_debug_gdb_server_port(${APP_YAML} GDB_SERVER_PORT)
        if(NOT DEFINED GDB_SERVER_PORT)
            message(STATUS " Segger: openocd gdb server port was not correctly configured in ${APP_YAML}")
        endif()

        get_ses_debug_gdb_server_reset_command(${APP_YAML} GDB_SERVER_RST_CMD)
        if(NOT DEFINED GDB_SERVER_RST_CMD)
            message(STATUS " Segger: openocd gdb server reset command was not correctly configured in ${APP_YAML}")
        endif()
    else()
        set(AUTO_START_GDB_SRV "Yes")
        set(GDB_SERVER_RST_CMD "reset halt")
        set(GDB_SERVER_PORT 3333)
    endif()

    foreach (lib_src IN ITEMS ${target_lib_sources})
        if(NOT SES_USE_TOOLCHAIN_ANDES)
            # if Andes toolchain has not been specified, files for gcc should be skipped
            string(FIND ${lib_src} "gcc" exist)
            if(NOT ${exist} EQUAL -1)
                continue()
            endif()
        endif()
        set(target_sources "${target_sources},${lib_src}")
    endforeach ()

    if(NOT SES_USE_TOOLCHAIN_ANDES)
        # if Andes toolchain has not been specified, it needs to use SES specific startup file
        set(target_sources "${target_sources},${HPM_SDK_BASE}/soc/${HPM_SOC}/toolchains/segger/startup.s")
    endif()

    if (NOT IS_ABSOLUTE ${target_linker})
        set(target_linker ${CMAKE_CURRENT_SOURCE_DIR}/${target_linker})
    endif()

    if(EXISTS ${target_linker})
        message(STATUS "Segger linker script: " ${target_linker})
    else()
        message(FATAL_ERROR "Segger: can not locate linker script: " ${target_linker})
    endif()

    # Remove duplicates
    list(REMOVE_DUPLICATES target_sources)
    list(REMOVE_DUPLICATES target_include_dirs)

    set(target_sources_with_macro "")
    foreach (target_source IN ITEMS ${target_sources})
        if(NOT IS_ABSOLUTE ${target_source})
            set(target_source ${CMAKE_CURRENT_SOURCE_DIR}/${target_source})
        endif()
        if("${target_source_with_macro}" STREQUAL "")
            set(target_sources_with_macro "${target_source}")
        else()
            set(target_sources_with_macro "${target_sources_with_macro},${target_source}")
        endif()
    endforeach ()

    list(REMOVE_DUPLICATES target_sources_with_macro)

    set(target_include_dirs_with_macro "")
    foreach (target_source IN ITEMS ${target_include_dirs})
        string(FIND ${target_source} $ENV{GNURISCV_TOOLCHAIN_PATH} position)
        if(${position} EQUAL 0)
            continue()
        endif()
        if(NOT IS_ABSOLUTE ${target_source})
            set(target_source ${CMAKE_CURRENT_SOURCE_DIR}/${target_source})
        endif()
        if("${target_include_dirs_with_macro}" STREQUAL "")
            set(target_include_dirs_with_macro ${target_source})
        else()
            set(target_include_dirs_with_macro "${target_include_dirs_with_macro},${target_source}")
        endif()
    endforeach ()

    foreach (target_source IN ITEMS ${target_app_include_dirs})
        string(FIND ${target_source} $ENV{GNURISCV_TOOLCHAIN_PATH} position)
        if(${position} EQUAL 0)
            continue()
        endif()
        if(NOT IS_ABSOLUTE ${target_source})
            set(target_source ${CMAKE_CURRENT_SOURCE_DIR}/${target_source})
        endif()
        if("${target_include_dirs_with_macro}" STREQUAL "")
            set(target_include_dirs_with_macro ${target_source})
        else()
            set(target_include_dirs_with_macro "${target_include_dirs_with_macro},${target_source}")
        endif()
    endforeach ()

    if(NOT "${target_defines}" STREQUAL "")
        set(target_defines_escaped "")
        foreach (defs IN ITEMS ${target_defines})
            string(REGEX REPLACE "\"" "\\\\\"" escaped ${defs})
            if("${target_defines_escaped}" STREQUAL "")
                set(target_defines_escaped ${escaped})
            else()
                set(target_defines_escaped "${target_defines_escaped},${escaped}")
            endif()
        endforeach()
    else()
        set(target_defines_escaped ${target_defines})
    endif()

    set(target_register_definition "${HPM_SDK_BASE}/soc/${HPM_SOC}/hpm_ses_reg.xml")
    set(target_cpu_register_definition "${HPM_SDK_BASE}/soc/${HPM_SOC}/hpm_ses_riscv_cpu_regs.xml")

    string(REPLACE "\\"  "/" target_sources_with_macro ${target_sources_with_macro})
    string(REPLACE "\\"  "/" target_include_dirs_with_macro ${target_include_dirs_with_macro})
    string(REPLACE "\\"  "/" target_linker ${target_linker})
    string(REPLACE "\\"  "/" target_register_definition ${target_register_definition})
    string(REPLACE "\\"  "/" target_cpu_register_definition ${target_cpu_register_definition})
    string(REPLACE "\\"  "/" hpm_sdk_base_path ${HPM_SDK_BASE})

    if(SES_COMPILER_ABI)
        set(target_compiler_abi ${SES_COMPILER_ABI})
    else()
        set(target_compiler_abi "ilp32")
    endif()

    if(SES_COMPILER_ARCH)
        set(target_compiler_arch ${SES_COMPILER_ARCH})
    else()
        set(target_compiler_arch "rv32ima")
    endif()

    # Specify SES target device name to be defined in SES project file
    if (NOT HPM_DEVICE_NAME)
        set(SES_DEVICE_NAME "${HPM_SOC}XXXX")
    else()
        set(SES_DEVICE_NAME ${HPM_DEVICE_NAME})
    endif()

    message(STATUS "Segger device name: ${SES_DEVICE_NAME}")
    if(${OPENOCD_READY})
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.json
            "{
                \"target\": {
                \"name\": \"${CMAKE_PROJECT_NAME}\",
                \"sources\": \"${target_sources_with_macro}\",
                \"includes\": \"${target_include_dirs_with_macro}\",
                \"defines\":\"${target_defines_escaped}\",
                \"linker\":\"${target_linker}\",
                \"link_symbols\":\"${target_link_symbols}\",
                \"sdk_base\":\"${hpm_sdk_base_path}\",
                \"board\":\"${BOARD}\",
                \"soc\":\"${HPM_SOC}\",
                \"openocd\":\"${OPENOCD}\",
                \"openocd_soc\":\"${OPENOCD_SOC}\",
                \"debug_probe\":\"${OPENOCD_PROBE}\",
                \"auto_start_gdb_server\":\"${AUTO_START_GDB_SRV}\",
                \"gdb_server_port\":\"${GDB_SERVER_PORT}\",
                \"gdb_server_reset_command\":\"${GDB_SERVER_RST_CMD}\",
                \"register_definition\":\"${target_register_definition}\",
                \"cpu_register_definition\":\"${target_cpu_register_definition}\",
                \"post_build_command\":\"${post_build_command}\",
                \"heap_size\":\"${HEAP_SIZE}\",
                \"stack_size\":\"${STACK_SIZE}\",
                \"cplusplus\":\"${CMAKE_CXX_STANDARD}\",
                \"segger_level_o3\":\"${SEGGER_LEVEL_O3}\",
                \"target_device_name\":\"${SES_DEVICE_NAME}\",
                \"toolchain_variant\":\"${SES_TOOLCHAIN_VARIANT}\",
                \"cflags\":\"${target_cflags}\",
                \"compiler_abi\":\"${target_compiler_abi}\",
                \"compiler_arch\":\"${target_compiler_arch}\",
                \"ses_link_input\":\"${target_ses_ld_input}\",
                \"enable_nds_dsp\":\"${enable_nds_dsp}\",
                \"enable_cpp_exceptions\":\"${enable_cpp_exceptions}\"
                }
            }")
    else()
        message(STATUS " Segger: no debugger configuration is generated, due to missing openocd information. Please configure manually in Segger Embedded Studio")
        file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.json
            "{
                \"target\": {
                \"name\": \"${CMAKE_PROJECT_NAME}\",
                \"sources\": \"${target_sources_with_macro}\",
                \"includes\": \"${target_include_dirs_with_macro}\",
                \"defines\":\"${target_defines_escaped}\",
                \"linker\":\"${target_linker}\",
                \"link_symbols\":\"${target_link_symbols}\",
                \"sdk_base\":\"${hpm_sdk_base_path}\",
                \"board\":\"${BOARD}\",
                \"soc\":\"${HPM_SOC}\",
                \"register_definition\":\"${target_register_definition}\",
                \"cpu_register_definition\":\"${target_cpu_register_definition}\",
                \"post_build_command\":\"${post_build_command}\",
                \"heap_size\":\"${HEAP_SIZE}\",
                \"stack_size\":\"${STACK_SIZE}\",
                \"cplusplus\":\"${CMAKE_CXX_STANDARD}\",
                \"segger_level_o3\":\"${SEGGER_LEVEL_O3}\",
                \"target_device_name\":\"${SES_DEVICE_NAME}\",
                \"toolchain_variant\":\"${SES_TOOLCHAIN_VARIANT}\",
                \"cflags\":\"${target_cflags}\",
                \"compiler_abi\":\"${target_compiler_abi}\",
                \"compiler_arch\":\"${target_compiler_arch}\",
                \"ses_link_input\":\"${target_ses_ld_input}\",
                \"enable_nds_dsp\":\"${enable_nds_dsp}\",
                \"enable_cpp_exceptions\":\"${enable_cpp_exceptions}\"
                }
            }")
    endif()

    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${HPM_SDK_BASE}/scripts/segger/embedded_studio_proj_gen.py "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.json" "${CMAKE_CURRENT_BINARY_DIR}/segger_embedded_studio" "${CMAKE_CURRENT_SOURCE_DIR}"
        WORKING_DIRECTORY  ${HPM_SDK_BASE}/scripts/segger
        )
endfunction ()
