# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

# Set CMake policy CMP0079 to NEW
#
# This policy controls how CMake handles the INTERFACE_LINK_LIBRARIES target property
# when it is set to an empty string. By setting this policy to NEW, CMake will use the
# new behavior, which allows the INTERFACE_LINK_LIBRARIES property to be explicitly empty.
# This can be useful for ensuring that no additional libraries are linked by default.
#
# For more information, see the CMake documentation on policies:
# https://cmake.org/cmake/help/latest/policy/CMP0079.html
cmake_policy(SET CMP0079 NEW)

# Include hex conversion utilities
include(${CMAKE_CURRENT_LIST_DIR}/hex.cmake)

# Add source file to sdk core (HPM_SDK_LIB)
#
# This function adds source files to the HPM SDK core library (HPM_SDK_LIB).
# It accepts one or more source file paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# Directories cannot be added as source files.
#
# Example:
#   sdk_src(SOURCE_FILE)
#   sdk_src(file1.c file2.c)
#   sdk_src(${PROJECT_SOURCE_DIR}/src/file.c)
#
# :param SOURCE_FILE: source files to be added to HPM_SDK_LIB
# @public
#
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

# Add include path
#
# This function adds include directories to the HPM SDK interface library (HPM_SDK_LIB_ITF).
# It accepts one or more include directory paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# The include paths are added with INTERFACE visibility, meaning they will be
# propagated to targets that link against the HPM_SDK_LIB_ITF target.
#
# Example:
#   sdk_inc(INC_PATH)
#   sdk_inc(include)
#   sdk_inc(include1 include2)
#   sdk_inc(${PROJECT_SOURCE_DIR}/include)
#
# :param INC_PATH: include directories to be added with INTERFACE visibility
# @public
#
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

# Add system include path
#
# This function adds system include directories to the HPM SDK interface library (HPM_SDK_LIB_ITF).
# It accepts one or more include directory paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# The include paths are added with SYSTEM INTERFACE visibility, indicating that
# they are system include paths and will be propagated to targets that link
# against the HPM_SDK_LIB_ITF target. This can be useful for suppressing warnings
# from third-party headers.
#
# Example:
#   sdk_sys_inc(SYS_INC_PATH)
#   sdk_sys_inc(system_include)
#   sdk_sys_inc(system_include1 system_include2)
#   sdk_sys_inc(${PROJECT_SOURCE_DIR}/system_include)
#
# :param SYS_INC_PATH: system include directories to be added with SYSTEM INTERFACE visibility
# @public
#
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

# Get all compiler options
#
# This function retrieves all compiler options configured for the current project
# and returns them as a single space-separated string.
#
# Example:
#   sdk_get_compile_options(opts)
#
# :param opts: The variable to store the space-separated compiler options
# @public
#
function(sdk_get_compile_options opts)
    get_target_property(all_opts ${HPM_SDK_LIB_ITF} INTERFACE_COMPILE_OPTIONS)
    string(JOIN " " output_opts ${all_opts})
    set(${opts} ${output_opts} PARENT_SCOPE)
endfunction()

# Define global symbols for linker
#
# This function defines global symbols for the linker in the HPM SDK interface library (HPM_SDK_LIB_ITF).
# It accepts one or more symbol names as arguments. These symbols are passed to the linker using
# the `-Xlinker --defsym` option, which allows defining symbols with specific values.
# This can be useful for setting global variables or constants at link time.
#
# Example:
#   sdk_linker_global_symbols(SYMBOL_NAME)
#   sdk_linker_global_symbols(SYMBOL1 SYMBOL2)
#
# :param syms: symbols for linker to be defined globally
# @public
#
function(sdk_linker_global_symbols)
    foreach(sym ${ARGN})
        target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE "-Xlinker --defsym=${sym}")
        get_property(link_symbols TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_LINK_SYMBOLS)
        list(APPEND link_symbols "${sym}")
        set_target_properties(${HPM_SDK_LIB_ITF} PROPERTIES INTERFACE_LINK_SYMBOLS "${link_symbols}")
    endforeach()
endfunction()

# Set compile options
#
# This function sets compile options for the HPM SDK interface library (HPM_SDK_LIB_ITF).
# It accepts one or more compile options as arguments. These options are added with
# INTERFACE visibility, meaning they will be applied to any target that links against
# the HPM_SDK_LIB_ITF target. This is useful for specifying compiler flags that should
# be used consistently across multiple projects or modules.
#
# Example:
#   sdk_compile_options(-Wall -Wextra)
#   sdk_compile_options(-O2)
#
# :param opts: compile options to be added with INTERFACE visibility
# @public
#
function(sdk_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set compile definitions
#
# This function sets compile definitions for the HPM SDK interface library (HPM_SDK_LIB_ITF).
# It accepts one or more preprocessor definitions as arguments. These definitions are added
# with INTERFACE visibility, meaning they will be applied to any target that links against
# the HPM_SDK_LIB_ITF target. This is useful for defining macros that should be available
# across multiple projects or modules.
#
# Example:
#   sdk_compile_definitions(DEBUG)
#   sdk_compile_definitions(ENABLE_FEATURE_X)
#
# :param def: compiler preprocessing definitions to be added with INTERFACE visibility
# @public
#
function(sdk_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

# Set compile definitions if feature is true
#
# This function sets compile definitions for the HPM SDK interface library (HPM_SDK_LIB_ITF)
# only if a specified feature is enabled. It accepts one or more preprocessor definitions
# as arguments. These definitions are added with INTERFACE visibility, meaning they will be
# applied to any target that links against the HPM_SDK_LIB_ITF target. This is useful for
# conditionally defining macros based on feature flags.
#
# Example:
#   sdk_compile_definitions_ifdef(FEATURE_X DEBUG)
#   sdk_compile_definitions_ifdef(FEATURE_Y ENABLE_FEATURE_Y)
#
# :param feature: the feature flag to check
# :param def: compiler preprocessing definitions to be added with INTERFACE visibility
# @public
#
function(sdk_compile_definitions_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_compile_definitions(${ARGN})
    endif()
endfunction()

# Link libraries
#
# This function links libraries to the HPM SDK interface library (HPM_SDK_LIB_ITF).
# It accepts one or more library names or paths as arguments. The libraries can be
# actual file paths (e.g., USER_LIB.a) or standard libraries provided by the toolchain
# (e.g., m). If a library path is relative, it will be resolved relative to the current
# source directory. This function ensures that the specified libraries are linked with
# INTERFACE visibility, meaning they will be linked to any target that links against
# the HPM_SDK_LIB_ITF target.
#
# Example:
#   sdk_link_libraries(libm)
#   sdk_link_libraries(${PROJECT_SOURCE_DIR}/libs/libcustom.a)
#
# :param libs: libraries to be linked with INTERFACE visibility
# @public
#
function(sdk_link_libraries)
    foreach(l ${ARGN})
        string(REGEX MATCH "\.[a-zA-Z]+$" is_file_name ${l})
        if(NOT ${is_file_name} STREQUAL "")
            # given library is an actual file name
            if(IS_ABSOLUTE ${l})
                set(lib ${l})
            else()
                set(lib ${CMAKE_CURRENT_SOURCE_DIR}/${l})
            endif()
            if(NOT EXISTS ${lib})
                message(WARNING "library ${lib} can't be found")
            else()
                target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE ${lib})
            endif()
        else()
            target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE ${l})
        endif()
    endforeach()
endfunction()

# Link libraries if feature is true
#
# This function links libraries to the HPM SDK interface library (HPM_SDK_LIB_ITF)
# only if a specified feature is enabled. It accepts one or more library names or paths
# as arguments. The libraries can be actual file paths (e.g., USER_LIB.a) or standard
# libraries provided by the toolchain (e.g., m). If a library path is relative, it will
# be resolved relative to the current source directory. This function ensures that the
# specified libraries are linked with INTERFACE visibility, meaning they will be linked
# to any target that links against the HPM_SDK_LIB_ITF target, but only if the feature
# flag is set.
#
# Example:
#   sdk_link_libraries_ifdef(FEATURE_X libm)
#   sdk_link_libraries_ifdef(FEATURE_Y ${PROJECT_SOURCE_DIR}/libs/libcustom.a)
#
# :param feature: the feature flag to check
# :param libs: libraries to be linked with INTERFACE visibility
# @public
#
function(sdk_link_libraries_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_link_libraries(${ARGN})
    endif()
endfunction()

# Set linker options
#
# This function sets linker options for the HPM SDK interface library (HPM_SDK_LIB_ITF).
# It accepts one or more linker options as arguments. These options are added with
# INTERFACE visibility, meaning they will be applied to any target that links against
# the HPM_SDK_LIB_ITF target. This is useful for specifying linker flags that should
# be used consistently across multiple projects or modules.
#
# Example:
#   sdk_ld_options(-Wl,--start-group)
#   sdk_ld_options(-Wl,--end-group)
#
# :param opts: linker options to be added with INTERFACE visibility
# @public
#
function(sdk_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set linker options if feature is true
#
# This function sets linker options for the HPM SDK interface library (HPM_SDK_LIB_ITF)
# only if a specified feature is enabled. It accepts one or more linker options as arguments.
# These options are added with INTERFACE visibility, meaning they will be applied to any target
# that links against the HPM_SDK_LIB_ITF target, but only if the feature flag is set.
# This is useful for conditionally applying linker flags based on feature flags.
#
# Example:
#   sdk_ld_options_ifdef(FEATURE_X -Wl,--start-group)
#   sdk_ld_options_ifdef(FEATURE_Y -Wl,--end-group)
#
# :param feature: the feature flag to check
# :param opts: linker options to be added with INTERFACE visibility
# @public
#
function(sdk_ld_options_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_ld_options(${ARGN})
    endif()
endfunction()
# Get board information from a board YAML file
#
# This function retrieves specific information from a board YAML file using a Python script.
# It takes the board YAML file path, the type of information to retrieve, and a variable to store the result.
# If the information is not found, it defaults to "0".
# The result is set in the parent scope.
#
# Example:
#   get_board_info(board.yaml soc soc_name)
#
# :param board_yaml: The path to the board YAML file.
# :param info_type: The type of information to retrieve (e.g., "soc", "device").
# :param info_value: The variable to store the retrieved information.
# @private
#
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

# Get the OpenOCD probe name from a board YAML file
#
# This function retrieves the OpenOCD probe name for a board from its YAML file.
# It uses the get_board_info function to obtain the probe name and sets it in the parent scope.
#
# Example:
#   get_openocd_probe_name_of_board(board.yaml probe_name)
#
# :param board_yaml: The path to the board YAML file.
# :param out: The variable to store the retrieved probe name.
# @private
#
function(get_openocd_probe_name_of_board board_yaml out)
    get_board_info(${board_yaml} openocd-probe r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# Get the OpenOCD SoC name from a board YAML file
#
# This function retrieves the OpenOCD SoC name for a board from its YAML file.
# It uses the get_board_info function to obtain the SoC name and sets it in the parent scope.
#
# Example:
#   get_openocd_soc_name_of_board(board.yaml soc_name)
#
# :param board_yaml: The path to the board YAML file.
# :param out: The variable to store the retrieved SoC name.
# @private
#
function(get_openocd_soc_name_of_board board_yaml out)
    get_board_info(${board_yaml} openocd-soc r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# Get the SoC name from a board YAML file
#
# This function retrieves the SoC name for a board from its YAML file.
# It uses the get_board_info function to obtain the SoC name and sets it in the parent scope.
#
# Example:
#   get_soc_name_of_board(board.yaml soc_name)
#
# :param board_yaml: The path to the board YAML file.
# :param out: The variable to store the retrieved SoC name.
# @private
#
function(get_soc_name_of_board board_yaml out)
    get_board_info(${board_yaml} soc r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# Get the device name from a board YAML file
#
# This function retrieves the device name for a board from its YAML file.
# It uses the get_board_info function to obtain the device name and sets it in the parent scope.
#
# Example:
#   get_device_name_of_board(board.yaml device_name)
#
# :param board_yaml: The path to the board YAML file.
# :param out: The variable to store the retrieved device name.
# @private
#
function(get_device_name_of_board board_yaml out)
    get_board_info(${board_yaml} device r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# Add a subdirectory if a feature is enabled
#
# This function adds subdirectories to the build if a specified feature is enabled.
# It takes a feature flag and a list of directories to add.
# If the feature is enabled, each directory in the list is added as a subdirectory.
#
# Example:
#   add_subdirectory_ifdef(FEATURE_X subdir1 subdir2)
#
# :param feature: The feature flag to check.
# :param ARGN: The list of directories to add as subdirectories.
# @public
#
function(add_subdirectory_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        foreach(d ${ARGN})
            add_subdirectory(${d})
        endforeach()
    endif()
endfunction()

# Add include paths if a feature is enabled
#
# This function adds include directories to the build if a specified feature is enabled.
# It takes a feature flag and a list of include paths to add.
# If the feature is enabled, each path in the list is added using sdk_inc.
#
# Example:
#   sdk_inc_ifdef(FEATURE_Y include1 include2)
#
# :param feature: The feature flag to check.
# :param ARGN: The list of include paths to add.
# @public
#
function(sdk_inc_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_inc(${ARGN})
    endif()
endfunction()

# Add include paths if a feature is not enabled
#
# This function adds include directories to the build if a specified feature is not enabled.
# It takes a feature flag and a list of include paths to add.
# If the feature is not defined or is set to 0, each path in the list is added using sdk_inc.
#
# Example:
#   sdk_inc_ifndef(FEATURE_Z include3 include4)
#
# :param feature: The feature flag to check.
# :param ARGN: The list of include paths to add.
# @public
#
function(sdk_inc_ifndef feature)
    if((NOT DEFINED ${feature}) OR (${feature} EQUAL 0))
        sdk_inc(${ARGN})
    endif()
endfunction()

# Add source files to the SDK core if a feature is enabled
#
# This function adds source files to the SDK core library if a specified feature is enabled.
# It takes a feature flag and a list of source files to add.
# If the feature is enabled, each file in the list is added using sdk_src.
#
# Example:
#   sdk_src_ifdef(FEATURE_A file1.c file2.c)
#
# :param feature: The feature flag to check.
# :param ARGN: The list of source files to add.
# @public
#
function(sdk_src_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_src(${ARGN})
    endif()
endfunction()

# Add source files to the SDK core if a feature is not enabled
#
# This function adds source files to the SDK core library if a specified feature is not enabled.
# It takes a feature flag and a list of source files to add.
# If the feature is not defined or is set to 0, each file in the list is added using sdk_src.
#
# Example:
#   sdk_src_ifndef(FEATURE_B file3.c file4.c)
#
# :param feature: The feature flag to check.
# :param ARGN: The list of source files to add.
# @public
#
function(sdk_src_ifndef feature)
    if((NOT DEFINED ${feature}) OR (${feature} EQUAL 0))
        sdk_src(${ARGN})
    endif()
endfunction()

# Clear all compile options for the HPM SDK interface library
#
# This function clears all compile options for the HPM SDK interface library (HPM_SDK_LIB_ITF).
# It sets the INTERFACE_COMPILE_OPTIONS property to an empty string, effectively removing any
# previously set compile options.
#
# Example:
#   sdk_clear_compile_options()
#
# @public
#
function(sdk_clear_compile_options)
    set_target_properties(${HPM_SDK_LIB_ITF} PROPERTIES INTERFACE_COMPILE_OPTIONS "")
endfunction()

# Add application-specific include paths
#
# This function adds include directories to the application target.
# It accepts one or more include directory paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# The include paths are added with PUBLIC visibility, meaning they will be
# propagated to targets that link against the application target.
#
# Example:
#   sdk_app_inc(app_include)
#   sdk_app_inc(app_include1 app_include2)
#
# :param INC_PATH: Include directories to be added with PUBLIC visibility.
# @public
#
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

# Add application source files
#
# This function adds source files to the application target.
# It accepts one or more source file paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# Directories cannot be added as source files.
#
# Example:
#   sdk_app_src(app_file.c)
#   sdk_app_src(app_file1.c app_file2.c)
#
# :param SOURCE_FILE: Source files to be added to the application.
# @public
#
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

# Check board capability based on board YAML and app YAML
#
# This function checks the board capability by executing a Python script with the provided
# board YAML and app YAML files. It also takes the application binary directory and CMake
# generator as arguments. The result of the script execution is stored in the specified result variable.
#
# Example:
#   check_board_capability(board.yaml app.yaml bin_dir "Ninja" result_var)
#
# :param board_yaml: The path to the board YAML file.
# :param app_yaml: The path to the application YAML file.
# :param app_bin_dir: The directory containing the application binaries.
# :param cmake_generator: The CMake generator being used.
# :param result: The variable to store the result of the capability check.
# @private
#
function(check_board_capability board_yaml app_yaml app_bin_dir cmake_generator result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/check_board_cap.py
        ${board_yaml}
        ${app_yaml}
        ${app_bin_dir}
        ${cmake_generator}
        RESULT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# Get flash size from board YAML
#
# This function retrieves the flash size from a board YAML file using the get_board_info function.
# The retrieved flash size is stored in the specified output variable.
#
# Example:
#   get_flash_size_of_board(board.yaml flash_size_var)
#
# :param board_yaml: The path to the board YAML file.
# :param out: The variable to store the retrieved flash size.
# @private
#
function(get_flash_size_of_board board_yaml out)
    get_board_info(${board_yaml} flash_size r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# Get on-board external RAM size
#
# This function retrieves the external RAM size from a board YAML file using the get_board_info function.
# The retrieved external RAM size is stored in the specified output variable.
#
# Example:
#   get_extram_size_of_board(board.yaml extram_size_var)
#
# :param board_yaml: The path to the board YAML file.
# :param out: The variable to store the retrieved external RAM size.
# @private
#
function(get_extram_size_of_board board_yaml out)
    get_board_info(${board_yaml} extram_size r)
    set(${out} ${r} PARENT_SCOPE)
endfunction()

# Retrieve auto-start GDB server setting for SES
#
# This function retrieves the auto-start GDB server setting from an application YAML file
# using a Python script. The result is stored in the specified output variable.
#
# Example:
#   get_ses_debug_auto_start_gdb_server(app.yaml auto_start_var)
#
# :param app_yaml: The path to the application YAML file.
# :param start: The variable to store the retrieved auto-start setting.
# @private
#
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

# Retrieve GDB server port for SES
#
# This function retrieves the GDB server port from an application YAML file
# using a Python script. The result is stored in the specified output variable.
#
# Example:
#   get_ses_debug_gdb_server_port(app.yaml port_var)
#
# :param app_yaml: The path to the application YAML file.
# :param port: The variable to store the retrieved GDB server port.
# @private
#
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

# Retrieve GDB server reset command for SES
#
# This function retrieves the GDB server reset command from an application YAML file
# using a Python script. The result is stored in the specified output variable.
#
# Example:
#   get_ses_debug_gdb_server_reset_command(app.yaml reset_cmd_var)
#
# :param app_yaml: The path to the application YAML file.
# :param rst_cmd: The variable to store the retrieved GDB server reset command.
# @private
#
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

# Check excluded targets
#
# This function checks for excluded targets in an application YAML file
# using a Python script. The result is stored in the specified output variable.
#
# Example:
#   check_excluded_targets(app.yaml result_var)
#
# :param app_yaml: The path to the application YAML file.
# :param result: The variable to store the result of the check.
# @private
#
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

# Import SoC modules and set compile definitions
#
# This function imports SoC modules from a list and sets compile definitions
# for each module. If a module is enabled (set to "y", "Y", or "1"), it adds
# a compile definition using sdk_compile_definitions.
#
# Example:
#   import_soc_modules(soc_module_list.txt)
#
# :param soc_module_list: The file containing the list of SoC modules.
# @private
#
function(import_soc_modules soc_module_list)
  file(
    STRINGS
    ${soc_module_list}
    MODULE_LIST
    REGEX "^HPMSOC_"
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

# Raise fatal error if all given features are not set or set to 0 or n
#
# This function checks a list of feature flags and raises a fatal error if all of them
# are either not set or set to 0 or n. It is useful for ensuring that at least one feature
# is enabled in a given set.
#
# Example:
#   sdk_raise_fatal_error_if_all_invalid(FEATUREA FEATUREB)
#
# :param ARGN: The list of feature flags to check.
# @public
#
function(sdk_raise_fatal_error_if_all_invalid)
  foreach(feature ${ARGN})
    if(NOT (${feature} AND (NOT ${${feature}} EQUAL 0)))
      message(FATAL_ERROR "ERROR: ${feature} needs to be set in ${ARGN}")
    endif()
  endforeach()
endfunction()

# Raise fatal error if all given features are set or set to 1 or y
#
# This function checks a list of feature flags and raises a fatal error if all of them
# are set to 1 or y. It is useful for ensuring that not all features are enabled simultaneously.
#
# Example:
#   sdk_raise_fatal_error_if_all_valid(FEATUREA FEATUREB)
#
# :param ARGN: The list of feature flags to check.
# @public
#
function(sdk_raise_fatal_error_if_all_valid)
  foreach(feature ${ARGN})
    if(${feature} AND (NOT ${${feature}} EQUAL 0))
      message(FATAL_ERROR "ERROR: ${feature} is not expected to be set in ${ARGN}")
    endif()
  endforeach()
endfunction()

# Raise fatal error if none of the given features is set or set to 1 or y
#
# This function checks a list of feature flags and raises a fatal error if none of them
# is set to 1 or y. It is useful for ensuring that at least one feature is enabled in a given set.
#
# Example:
#   sdk_raise_fatal_error_if_valid_at_least_one(FEATUREA FEATUREB)
#
# :param ARGN: The list of feature flags to check.
# @public
#
function(sdk_raise_fatal_error_if_valid_at_least_one)
    foreach(feature ${ARGN})
        if((${feature}) AND (NOT ${${feature}} EQUAL 0))
            return()
        endif()
    endforeach()
    message(FATAL_ERROR "ERROR: at least one of ${ARGN} needs to set")
endfunction()

#### NDSGCC sections ####

# Set compile options for the NDS toolchain
#
# This function sets compile options for the NDS toolchain by adding them with INTERFACE visibility
# to the HPM_SDK_NDSGCC_LIB_ITF target. This ensures that the options are applied to any target
# that links against the NDS toolchain interface library.
#
# Example:
#   sdk_nds_compile_options(-O2 -g)
#
# :param ARGN: The list of compile options for the NDS toolchain.
# @public
#
function(sdk_nds_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set compile definitions for the NDS toolchain
#
# This function sets compile definitions for the NDS toolchain by adding them with INTERFACE visibility
# to the HPM_SDK_NDSGCC_LIB_ITF target. This ensures that the definitions are applied to any target
# that links against the NDS toolchain interface library.
#
# Example:
#   sdk_nds_compile_definitions(-DDEBUG -DVERSION=1)
#
# :param ARGN: The list of compile definitions for the NDS toolchain.
# @public
#
function(sdk_nds_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

# Link libraries for Andes toolchains
#
# This function links libraries for the Andes toolchain by adding them with INTERFACE visibility
# to the HPM_SDK_NDSGCC_LIB_ITF target. This ensures that the libraries are linked to any target
# that links against the Andes toolchain interface library.
#
# Example:
#   sdk_nds_link_libraries(USER_LIB.a m)
#
# :param libs: Libraries to be linked, supporting both file paths (like USER_LIB.a) and standard
#              libraries provided by the toolchain (like m).
# @public
function(sdk_nds_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${lib})
    endforeach()
endfunction()

# Set linker options for Andes toolchains
#
# This function sets linker options for the Andes toolchain by adding them with INTERFACE visibility
# to the HPM_SDK_NDSGCC_LIB_ITF target. This ensures that the options are applied to any target
# that links against the Andes toolchain interface library.
#
# Example:
#   sdk_nds_ld_options(-Wl,--start-group -Wl,--end-group)
#
# :param opts: Linker options to be added with INTERFACE visibility.
# @public
function(sdk_nds_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_NDSGCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

#### ZCC sections ####

# Set compile options for ZCC toolchain
#
# This function sets compile options for the ZCC toolchain by adding them with INTERFACE visibility
# to the HPM_SDK_ZCC_LIB_ITF target. This ensures that the options are applied to any target
# that links against the ZCC toolchain interface library.
#
# Example:
#   sdk_zcc_compile_options(-O2 -g)
#
# :param opts: Options for the ZCC toolchain to be added with INTERFACE visibility.
# @public
function(sdk_zcc_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_ZCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set compile definitions for ZCC toolchain
#
# This function sets compile definitions for the ZCC toolchain by adding them with INTERFACE visibility
# to the HPM_SDK_ZCC_LIB_ITF target. This ensures that the definitions are applied to any target
# that links against the ZCC toolchain interface library.
#
# Example:
#   sdk_zcc_compile_definitions(-DDEBUG -DVERSION=1)
#
# :param defs: Definitions for the ZCC toolchain to be added with INTERFACE visibility.
# @public
function(sdk_zcc_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_ZCC_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

# Link libraries for ZCC toolchains
#
# This function links libraries for the ZCC toolchain by adding them with INTERFACE visibility
# to the HPM_SDK_ZCC_LIB_ITF target. This ensures that the libraries are linked to any target
# that links against the ZCC toolchain interface library.
#
# Example:
#   sdk_zcc_link_libraries(USER_LIB.a m)
#
# :param libs: Libraries to be linked, supporting both file paths (like USER_LIB.a) and standard
#              libraries provided by the toolchain (like m).
# @public
function(sdk_zcc_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(${HPM_SDK_ZCC_LIB_ITF} INTERFACE ${lib})
    endforeach()
endfunction()

# Set linker options for ZCC toolchains
#
# This function sets linker options for the ZCC toolchain by adding them with INTERFACE visibility
# to the HPM_SDK_ZCC_LIB_ITF target. This ensures that the options are applied to any target
# that links against the ZCC toolchain interface library.
#
# Example:
#   sdk_zcc_ld_options(-Wl,--start-group -Wl,--end-group)
#
# :param opts: Linker options to be added with INTERFACE visibility.
# @public
function(sdk_zcc_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_ZCC_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Add directory if feature is false
#
# This function adds a subdirectory to the build if a specified feature is not enabled.
# It takes a feature flag and a directory path. If the feature is not defined or is set to "0",
# the directory is added as a subdirectory.
#
# Example:
#   add_subdirectory_ifndef(FEATURE_X subdir)
#
# :param feature: The feature flag to check.
# :param dir: The directory to add as a subdirectory.
# @public
#
function(add_subdirectory_ifndef feature dir)
    if(NOT DEFINED ${feature} OR "${${feature}}" EQUAL "0")
        add_subdirectory(${dir})
    endif()
endfunction()

# Add include path for gcc
#
# This function adds include directories to the GCC interface library (HPM_SDK_GCC_LIB_ITF).
# It accepts one or more include directory paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# The include paths are added with INTERFACE visibility, meaning they will be
# propagated to targets that link against the HPM_SDK_GCC_LIB_ITF target.
#
# Example:
#   sdk_gcc_inc(INC_PATH)
#
# :param INC_PATH: Include directories to be added with INTERFACE visibility.
# @public
#
function(sdk_gcc_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(${HPM_SDK_GCC_LIB_ITF} INTERFACE ${path})
    endforeach()
endfunction()

# Add source specifically for gcc
#
# This function adds source files to the GCC library (HPM_SDK_GCC_LIB).
# It accepts one or more source file paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# Directories cannot be added as source files.
#
# Example:
#   sdk_gcc_src(SOURCE_FILE)
#
# :param SOURCE_FILE: Source files to be added to HPM_SDK_GCC_LIB.
# @public
#
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

# Add source specifically for gcc startup
#
# This function adds source files to the GCC startup library (HPM_SDK_GCC_STARTUP_LIB).
# It accepts one or more source file paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# Directories cannot be added as source files.
#
# Example:
#   sdk_gcc_startup_src(SOURCE_FILE)
#
# :param SOURCE_FILE: Source files to be added to HPM_SDK_GCC_STARTUP_LIB.
# @public
#
function(sdk_gcc_startup_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "directory ${file} can't be added to sdk_lib_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        target_sources(${HPM_SDK_GCC_STARTUP_LIB} PRIVATE ${path})
    endforeach()
endfunction()

# Get minimal SDK version needed by application
#
# This function retrieves the minimal SDK version required by an application
# from its YAML file using a Python script. The result is stored in the specified output variable.
#
# Example:
#   get_app_min_sdk_version(app.yaml min_sdk_version_var)
#
# :param app_yaml: The path to the application YAML file.
# :param result: The variable to store the retrieved minimal SDK version.
# @private
#
function(get_app_min_sdk_version app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_min_sdk_version.py
        ${app_yaml}
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# Get linked project path from application YAML
#
# This function retrieves the linked project path from an application YAML file
# using a Python script. The result is stored in the specified output variable.
#
# Example:
#   get_app_linked_project_path(app.yaml linked_project_path_var)
#
# :param app_yaml: The path to the application YAML file.
# :param result: The variable to store the retrieved linked project path.
# @private
#
function(get_app_linked_project_path app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_info.py
        ${app_yaml}
        "linked_project_path"
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# Get linked project name from application YAML
#
# This function retrieves the linked project name from an application YAML file
# using a Python script. The result is stored in the specified output variable.
#
# Example:
#   get_app_linked_project_name(app.yaml linked_project_name_var)
#
# :param app_yaml: The path to the application YAML file.
# :param result: The variable to store the retrieved linked project name.
# @private
#
function(get_app_linked_project_name app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_info.py
        ${app_yaml}
        "linked_project_name"
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# Get excluded IDEs from application YAML
#
# This function retrieves the list of excluded IDEs from an application YAML file
# using a Python script. The result is stored in the specified output variable.
#
# Example:
#   get_app_excluded_ides(app.yaml excluded_ides_var)
#
# :param app_yaml: The path to the application YAML file.
# :param result: The variable to store the retrieved list of excluded IDEs.
# @private
#
function(get_app_excluded_ides app_yaml result)
    execute_process(
        COMMAND
        ${PYTHON_EXECUTABLE}
        ${HPM_SDK_BASE}/scripts/get_app_info.py
        ${app_yaml}
        "excluded_ides"
        OUTPUT_VARIABLE r
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# Add source files (glob pattern) to the SDK core (HPM_SDK_LIB)
#
# This function adds source files to the HPM SDK core library (HPM_SDK_LIB) using a glob pattern.
# It accepts one or more glob patterns as arguments. The files matching the patterns are added
# to the SDK core library.
#
# Example:
#   sdk_src_glob("src/**/*.c")
#
# :param SOURCE_FILE_GLOB: Glob patterns to specify source files to be added to HPM_SDK_LIB,
#    such as "src/**/*.c" to add all .c files under the src directory recursively.
# @public
#
function(sdk_src_glob)
    foreach(g ${ARGN})
        file(GLOB_RECURSE src ${g})
        list(APPEND globbed_src ${src})
    endforeach()
    sdk_src(${globbed_src})
endfunction()

# Add application source files (glob pattern)
#
# This function adds source files to the application target using a glob pattern.
# It accepts one or more glob patterns as arguments. The files matching the patterns
# are added to the application target.
#
# Example:
#   sdk_app_src_glob("app/**/*.c")
#
# :param SOURCE_FILE_GLOB: Glob patterns to specify source files to be added to the application,
#    such as "app/**/*.c" to add all .c files under the app directory recursively.
# @public
#
function(sdk_app_src_glob)
    foreach(g ${ARGN})
        file(GLOB_RECURSE src ${g})
        list(APPEND globbed_src ${src})
    endforeach()
    sdk_app_src(${globbed_src})
endfunction()

# Add source files (glob pattern) for GCC
#
# This function adds source files to the GCC target using a glob pattern.
# It accepts one or more glob patterns as arguments. The files matching the patterns
# are added to the GCC target.
#
# Example:
#   sdk_gcc_src_glob("gcc/**/*.c")
#
# :param SOURCE_FILE_GLOB: Glob patterns to specify source files to be added to GCC,
#    such as "gcc/**/*.c" to add all .c files under the gcc directory recursively.
# @public
#
function(sdk_gcc_src_glob)
    foreach(g ${ARGN})
        file(GLOB_RECURSE src ${g})
        list(APPEND globbed_src ${src})
    endforeach()
    sdk_gcc_src(${globbed_src})
endfunction()

# Add source files (glob pattern) for SDK core if a feature is enabled
#
# This function adds source files to the SDK core library using a glob pattern,
# but only if a specified feature is enabled. It accepts a feature flag and one or more
# glob patterns as arguments. The files matching the patterns are added if the feature is enabled.
#
# Example:
#   sdk_src_glob_ifdef(FEATURE_X "src/**/*.c")
#
# :param feature: The feature flag to check.
# :param SOURCE_FILE_GLOB: Glob patterns to specify source files to be added to HPM_SDK_LIB,
#    such as "src/**/*.c" to add all .c files under the src directory recursively.
# @public
#
function(sdk_src_glob_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_src_glob(${ARGN})
    endif()
endfunction()

# Add source files (glob pattern) for SDK core if a feature is not enabled
#
# This function adds source files to the SDK core library using a glob pattern,
# but only if a specified feature is not enabled. It accepts a feature flag and one or more
# glob patterns as arguments. The files matching the patterns are added if the feature is not enabled.
#
# Example:
#   sdk_src_glob_ifndef(FEATURE_Y "src/**/*.c")
#
# :param feature: The feature flag to check.
# :param SOURCE_FILE_GLOB: Glob patterns to specify source files to be added to HPM_SDK_LIB,
#    such as "src/**/*.c" to add all .c files under the src directory recursively.
# @public
#
function(sdk_src_glob_ifndef feature)
    if((NOT DEFINED ${feature}) OR (${feature} EQUAL 0))
        sdk_src_glob(${ARGN})
    endif()
endfunction()

# Convert decimal size string to hex string
#
# This function converts a decimal size string to a hex string with "0x" prefix.
# It supports plain decimal numbers and hex strings. For human-readable sizes
# like "1K", "2M", use normalize_size_string_in_hex instead.
#
# Example:
#   decimal_to_hex(HEX_RESULT "1024")
#   message("1024 = ${HEX_RESULT}")  # Output: 1024 = 0x400
#
#   decimal_to_hex(HEX_RESULT "0x400")
#   message("0x400 = ${HEX_RESULT}")  # Output: 0x400 = 0x400
#
# :param output_var: The variable to store the hex result
# :param decimal_str: The decimal size string to convert (plain number or hex)
# @private
#
function(decimal_to_hex output_var decimal_str)
    # Check if input is already a hex string
    string(REGEX MATCH "^0x[0-9A-Fa-f]+$" is_hex "${decimal_str}")
    if(is_hex)
        # Input is already hex, return as-is
        set(${output_var} "${decimal_str}" PARENT_SCOPE)
        return()
    endif()

    # Check if input is a plain decimal number
    if("${decimal_str}" MATCHES "^[0-9]+$")
        # Convert to hex using existing to_hex function
        to_hex(${decimal_str} hex_result)
        set(${output_var} ${hex_result} PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Invalid decimal format: ${decimal_str}. Use normalize_size_string_in_hex for human-readable strings like '1K', '2M'.")
    endif()
endfunction()

# Convert hex string to decimal size string
#
# This function converts a hex string to a decimal size string.
# If the input is already a decimal string, it returns it as-is.
#
# Example:
#   hex_to_decimal(DEC_RESULT "0x400")
#   message("0x400 = ${DEC_RESULT}")  # Output: 0x400 = 1024
#
#   hex_to_decimal(DEC_RESULT "1024")
#   message("1024 = ${DEC_RESULT}")  # Output: 1024 = 1024
#
# :param output_var: The variable to store the decimal result
# :param hex_str: The hex string to convert (with or without 0x prefix)
# @private
#
function(hex_to_decimal output_var hex_str)
    # Check if input is already a decimal string
    if("${hex_str}" MATCHES "^[0-9]+$")
        # Input is already decimal, return as-is
        set(${output_var} "${hex_str}" PARENT_SCOPE)
        return()
    endif()

    # Ensure hex string has 0x prefix
    if(NOT "${hex_str}" MATCHES "^0x")
        set(hex_str "0x${hex_str}")
    endif()

    # Convert hex to decimal using existing from_hex function
    from_hex(${hex_str} decimal_result)
    set(${output_var} ${decimal_result} PARENT_SCOPE)
endfunction()

# Convert human-readable size string to decimal size (bytes)
#
# Example:
#   human_readable_size_to_decimal(DEC_RESULT "1K")
#   message("1K = ${DEC_RESULT}")  # Output: 1K = 1024
#
#   human_readable_size_to_decimal(DEC_RESULT "2M")
#   message("2M = ${DEC_RESULT}")  # Output: 2M = 2097152
#
#   human_readable_size_to_decimal(DEC_RESULT "1.5K")
#   message("1.5K = ${DEC_RESULT}")  # Output: 1.5K = 1536
#
# :param output_var: The variable to store the decimal result
# :param size_str: The human-readable size string to convert
# @private
function(human_readable_size_to_decimal output_var size_str)
    # Convert to uppercase for case-insensitive matching
    string(TOUPPER "${size_str}" size_str_upper)

    # Extract the unit (K, M, G) if present - handle with or without whitespace
    string(REGEX MATCH "[0-9]+\\s*[KMG]$" unit_match "${size_str_upper}")
    if(unit_match)
        # Extract just the unit letter
        string(REGEX REPLACE "[0-9\.]+\\s*([KMG])$" "\\1" unit "${size_str_upper}")
        # Remove the unit and any trailing spaces to get just the number
        string(REGEX REPLACE "\\s*[KMG]$" "" size_num "${size_str_upper}")
    else()
        # No unit found
        set(unit "")
        set(size_num "${size_str_upper}")
    endif()

    # Check if the remaining string is a valid number (integer or decimal)
    if(size_num MATCHES "^[0-9]+(\.[0-9]+)?$")
        # Default multiplier
        set(multiplier 1)
        if(unit STREQUAL "K")
            set(multiplier 1024)
        elseif(unit STREQUAL "M")
            math(EXPR multiplier "1024 * 1024")
        elseif(unit STREQUAL "G")
            math(EXPR multiplier "1024 * 1024 * 1024")
        elseif(NOT unit STREQUAL "")
            message(FATAL_ERROR "Unsupported unit: ${unit}")
        endif()

        # If decimal, multiply using expr and round down
        if(size_num MATCHES "^([0-9]+)\.([0-9]+)$")
            string(REGEX REPLACE "^([0-9]+)\.([0-9]+)$" "\\1;\\2" parts "${size_num}")
            list(GET parts 0 int_part)
            list(GET parts 1 dec_part)
            string(LENGTH "${dec_part}" dec_len)

            # Calculate decimal base using multiplication (10^dec_len)
            set(dec_base 1)
            while(dec_len GREATER 0)
                math(EXPR dec_base "${dec_base} * 10")
                math(EXPR dec_len "${dec_len} - 1")
            endwhile()

            message(STATUS "dec_base: ${dec_base}")
            math(EXPR dec_val "${dec_part} * ${multiplier} / ${dec_base}")
            math(EXPR int_val "${int_part} * ${multiplier}")
            math(EXPR bytes_value "${int_val} + ${dec_val}")
        else()
            math(EXPR bytes_value "${size_num} * ${multiplier}")
        endif()
        set(${output_var} "${bytes_value}" PARENT_SCOPE)
    else()
        # Check if it's a plain integer without unit
        if("${size_str}" MATCHES "^[0-9]+$")
            set(${output_var} "${size_str}" PARENT_SCOPE)
        else()
            message(FATAL_ERROR "Invalid size format: ${size_str}")
        endif()
    endif()
endfunction()

# Normalize size string to hex string
#
# This function takes a size string (decimal, hex, or human-readable like "1K", "2M", etc.)
# and returns the size as a hex string (e.g., "0x400", "0x200000").
#
# Example:
#   normalize_size_string_in_hex(HEX_RESULT "1024")   # HEX_RESULT = 0x400
#   normalize_size_string_in_hex(HEX_RESULT "1K")     # HEX_RESULT = 0x400
#   normalize_size_string_in_hex(HEX_RESULT "0x400")  # HEX_RESULT = 0x400
#
# :param output_var: The variable to store the hex result
# :param size_str: The input size string
# @private
function(normalize_size_string_in_hex output_var size_str)
    # Check if input is already a hex string
    string(REGEX MATCH "^0x[0-9A-Fa-f]+$" is_hex "${size_str}")
    if(is_hex)
        # Input is already hex, just ensure proper case for hex digits but keep 'x' lowercase
        string(TOUPPER "${size_str}" hex_result)
        string(REPLACE "0X" "0x" hex_result "${hex_result}")
        set(${output_var} "${hex_result}" PARENT_SCOPE)
        return()
    endif()

    # Check if input is a plain decimal number
    if("${size_str}" MATCHES "^[0-9]+$")
        decimal_to_hex(hex_result "${size_str}")
        set(${output_var} "${hex_result}" PARENT_SCOPE)
        return()
    endif()

    # Otherwise, treat as human-readable size string
    human_readable_size_to_decimal(decimal_result "${size_str}")
    decimal_to_hex(hex_result "${decimal_result}")
    set(${output_var} "${hex_result}" PARENT_SCOPE)
endfunction()
