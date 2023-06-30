# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause


cmake_policy(SET CMP0079 NEW)

function(sdk_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_lib_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        target_sources(${HPM_SDK_LIB} PRIVATE ${path})
    endforeach()
endfunction()

function(sdk_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(${HPM_SDK_LIB_ITF} INTERFACE ${path})
    endforeach()
endfunction()

function(sdk_sys_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(${HPM_SDK_LIB_ITF} SYSTEM INTERFACE ${path})
    endforeach()
endfunction()

function(sdk_get_compile_options opts)
    get_target_property(all_opts ${HPM_SDK_LIB_ITF} INTERFACE_COMPILE_OPTIONS)
    string(JOIN " " output_opts ${all_opts})
    set(${opts} ${output_opts} PARENT_SCOPE)
endfunction()

function(sdk_linker_global_symbols)
    foreach(sym ${ARGN})
        target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE "-Xlinker --defsym=${sym}")
        get_property(link_symbols TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_LINK_SYMBOLS)
        list(APPEND link_symbols "${sym}")
        set_target_properties(${HPM_SDK_LIB_ITF} PROPERTIES INTERFACE_LINK_SYMBOLS "${link_symbols}")
    endforeach()
endfunction()

function(sdk_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

function(sdk_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

function(sdk_compile_definitions_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_compile_definitions(${ARGN})
    endif()
endfunction()

function(sdk_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE ${lib})
    endforeach()
endfunction()

function(sdk_link_libraries_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_link_libraries(${ARGN})
    endif()
endfunction()

function(sdk_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

function(sdk_ld_options_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_ld_options(${ARGN})
    endif()
endfunction()

function(get_board_info board_yaml info_type info_value)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_board_info.py
        ${board_yaml}
        ${info_type}
        RESULT_VARIABLE result
        OUTPUT_VARIABLE out
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    if(${out} STREQUAL "not found")
        set(out "0")
    endif()
    set(${info_value} ${out} PARENT_SCOPE)
endfunction()

function(get_openocd_probe_name_of_board board_yaml out)
    get_board_info(${board_yaml} openocd-probe r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

function(get_openocd_soc_name_of_board board_yaml out)
    get_board_info(${board_yaml} openocd-soc r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

function(get_soc_name_of_board board_yaml out)
    get_board_info(${board_yaml} soc r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

function(get_device_name_of_board board_yaml out)
    get_board_info(${board_yaml} device r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

function(get_compiler_version compiler version_text compiler_version)
    if("${compiler}" STREQUAL "gcc")
        execute_process(
            COMMAND
            ${PYTHON_EXECUTABLE}
            ${HPM_SDK_BASE}/scripts/get_gcc_version.py
            "${version_text}"
            RESULT_VARIABLE result
            OUTPUT_VARIABLE v
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        set(${compiler_version} ${v} PARENT_SCOPE)
    elseif("${compiler}" STREQUAL "clang")
        execute_process(
            COMMAND
            ${PYTHON_EXECUTABLE}
            ${HPM_SDK_BASE}/scripts/get_gcc_version.py
            "${version_text}"
            RESULT_VARIABLE result
            OUTPUT_VARIABLE v
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        set(${compiler_version} ${v} PARENT_SCOPE)

    else()
        message(FATAL_ERROR "Unsupported compiler ${compiler}")
    endif()
endfunction()

function(add_subdirectory_ifdef feature dir)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        add_subdirectory(${dir})
    endif()
endfunction()

function(sdk_inc_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_inc(${ARGN})
    endif()
endfunction()

function(sdk_inc_ifndef feature)
    if((NOT DEFINED ${feature}) OR (${feature} EQUAL 0))
        sdk_inc(${ARGN})
    endif()
endfunction()

function(sdk_src_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_src(${ARGN})
    endif()
endfunction()

function(sdk_src_ifndef feature)
    if((NOT DEFINED ${feature}) OR (${feature} EQUAL 0))
        sdk_src(${ARGN})
    endif()
endfunction()

function(sdk_clear_compile_options)
    set_target_properties(${HPM_SDK_LIB_ITF} PROPERTIES INTERFACE_COMPILE_OPTIONS "")
endfunction()

function(sdk_app_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(app PUBLIC ${path})
    endforeach()
endfunction()

function(sdk_app_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_app_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        target_sources(app PRIVATE ${path})
    endforeach()
endfunction()

function(check_board_capability board_yaml app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/check_board_cap.py
        ${board_yaml}
        ${app_yaml}
        RESULT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

function(get_flash_size_of_board board_yaml out)
    get_board_info(${board_yaml} flash_size r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

function(get_extram_size_of_board board_yaml out)
    get_board_info(${board_yaml} extram_size r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

function(get_ses_debug_auto_start_gdb_server app_yaml start)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_debug_info.py
        ${app_yaml}
        ses
        auto_start_gdb_server
        RESULT_VARIABLE result
        OUTPUT_VARIABLE auto_start
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${start} ${auto_start} PARENT_SCOPE)
endfunction()

function(get_ses_debug_gdb_server_port app_yaml port)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_debug_info.py
        ${app_yaml}
        ses
        gdb_server_port
        RESULT_VARIABLE result
        OUTPUT_VARIABLE p
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${port} ${p} PARENT_SCOPE)
endfunction()

function(get_ses_debug_gdb_server_reset_command app_yaml rst_cmd)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_debug_info.py
        ${app_yaml}
        ses
        gdb_server_reset_command
        RESULT_VARIABLE result
        OUTPUT_VARIABLE p
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${rst_cmd} ${p} PARENT_SCOPE)
endfunction()

function(check_excluded_targets app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/check_excluded_targets.py
        ${app_yaml}
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

function(import_soc_modules soc_module_list)
  file(
    STRINGS
    ${soc_module_list}
    MODULE_LIST
    REGEX "^CONFIG_"
    ENCODING "UTF-8"
  )

  foreach (m ${MODULE_LIST})
    string(REGEX MATCH "[^=]+" MODULE_NAME ${m})
    string(REGEX MATCH "=(.+$)" CONFIG_VALUE ${m})
    set(CONFIG_VALUE ${CMAKE_MATCH_1})

    if("${CONFIG_VALUE}" MATCHES "^\"(.*)\"$")
      set(CONFIG_VALUE ${CMAKE_MATCH_1})
    endif()

    set("${MODULE_NAME}" "${CONFIG_VALUE}" PARENT_SCOPE)
    if(("${CONFIG_VALUE}" STREQUAL "y") OR ("${CONFIG_VALUE}" STREQUAL "Y") OR ("${CONFIG_VALUE}" STREQUAL "1"))
        sdk_compile_definitions("-D${MODULE_NAME}=${CONFIG_VALUE}")
    endif()
  endforeach()
endfunction()

# raise fatal error if symbols given are all not set or set to 0 or n
function(sdk_raise_fatal_error_if_all_invalid)
  foreach(feature ${ARGN})
    if(NOT (${feature} AND (NOT ${${feature}} EQUAL 0)))
      message(FATAL_ERROR "ERROR: ${feature} needs to be set in ${ARGN}")
    endif()
  endforeach()
endfunction()

# raise fatal error if symbols given are all set or set to 1 or y
function(sdk_raise_fatal_error_if_all_valid)
  foreach(feature ${ARGN})
    if(${feature} AND (NOT ${${feature}} EQUAL 0))
      message(FATAL_ERROR "ERROR: ${feature} is not expected to be set in ${ARGN}")
    endif()
  endforeach()
endfunction()

# raise fatal error if at least one of them given symbols is set or set to 1 or y
function(sdk_raise_fatal_error_if_valid_at_least_one)
    foreach(feature ${ARGN})
        if((${feature}) AND (NOT ${${feature}} EQUAL 0))
            return()
        endif()
    endforeach()
    message(FATAL_ERROR "ERROR: at least one of ${ARGN} needs to set")
endfunction()

# NDSGCC sections
function(sdk_nds_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

function(sdk_nds_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

function(sdk_nds_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${lib})
    endforeach()
endfunction()

function(sdk_nds_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

function(add_subdirectory_ifndef feature dir)
    if(NOT DEFINED ${feature} OR "${${feature}}" EQUAL "0")
        add_subdirectory(${dir})
    endif()
endfunction()

function(sdk_gcc_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_lib_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        target_sources(${HPM_SDK_GCC_LIB} PRIVATE ${path})
    endforeach()
endfunction()

