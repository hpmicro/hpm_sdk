# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

# Summary of file content:
# This file defines various functions and properties for configuring the Segger Embedded Studio (SES) environment.
# It includes functions to set compile options and compile definitions for the SES interface library.
# Additionally, it defines properties for SES-specific libraries and options, such as debug target connection, linker options, and JLink speed.
# The functions provided in this file allow for the customization and configuration of the SES environment for different projects and targets.

# add ses library interface to store ses specific configurations
set(HPM_SDK_SES_LIB_ITF hpm_sdk_ses_lib_itf)
add_library(${HPM_SDK_SES_LIB_ITF} INTERFACE)

set(HPM_SDK_SES_LIB hpm_sdk_ses_lib)
define_property(GLOBAL PROPERTY ${HPM_SDK_SES_LIB} BRIEF_DOCS "ses library" FULL_DOCS "ses library")

set(HPM_SDK_SES_STARTUP_LIB hpm_sdk_ses_startup_lib)
define_property(GLOBAL PROPERTY ${HPM_SDK_SES_STARTUP_LIB} BRIEF_DOCS "ses startup library" FULL_DOCS "ses startup library")

set(SES_OPT_LIB_IO_TYPE LIBRARY_IO_TYPE)
set(SES_OPT_DBG_TGT_CONN debug_target_connection)
set(SES_OPT_LINKER_PRINTF_FP_ENABLED linker_printf_fp_enabled)
set(SES_OPT_LINKER_SCANF_FP_ENABLED linker_scanf_fp_enabled)
set(SES_OPT_LINKER_PRINTF_FMT_LEVEL linker_printf_fmt_level)
set(SES_OPT_LINKER_SCANF_FMT_LEVEL linker_scanf_fmt_level)
set(SES_OPT_DBG_JLINK_SPEED speed)
set(SES_DBG_CONN_GDB_SRV "GDB Server")
set(HPM_SDK_SES_OPTS hpm_sdk_ses_opts)
define_property(GLOBAL PROPERTY ${HPM_SDK_SES_OPTS} BRIEF_DOCS "ses options" FULL_DOCS "ses options")

# Set optimization level for file or directory in Segger Embedded Studio (SES)
set(HPM_SDK_SES_OPTIMIZATION_LEVEL hpm_sdk_ses_optimization_level)
define_property(GLOBAL PROPERTY ${HPM_SDK_SES_OPTIMIZATION_LEVEL} BRIEF_DOCS "ses optimization level" FULL_DOCS "ses optimization level")

# This is a private function to set the optimization level for a specific file or directory
# in the Segger Embedded Studio (SES) environment. It validates the provided optimization
# level against a list of valid levels and appends the configuration to a global property.
#
# :param path: The path to the source file or directory. If not absolute, it is resolved
#              relative to the current source directory.
# :param config_type: The configuration type (e.g., Debug, Release, Common).
# :param level: The optimization level to set. Must be one of the predefined valid levels.
# :raises FATAL_ERROR: If the provided optimization level is not valid.
# @private
function(__sdk_ses_set_optimization_level path config_type level)
    set(VALID_OPTIMIZATION_LEVELS "None" "Level 0" "Level 1" "Level 2 for speed" "Level 2 balanced" "Level 2 for size" "Level 3 for more speed")
    if(NOT level IN_LIST VALID_OPTIMIZATION_LEVELS)
        message(FATAL_ERROR "Segger: invalid optimization level: ${level}")
    endif()
    if(NOT IS_ABSOLUTE ${path})
        set(path ${CMAKE_CURRENT_SOURCE_DIR}/${path})
    endif()
    set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_SES_OPTIMIZATION_LEVEL} "'${path}':'${config_type}':'${level}'")
endfunction()

# Set optimization level for file or directory in Segger Embedded Studio (SES)
#
# This function sets the optimization level for all configurations in the SES project.
# It is a convenience wrapper around __sdk_ses_set_optimization_level.
#
# Example:
#   sdk_ses_set_optimization_level("path/to/file.c" "Level 2 for speed")
#
# :param path: Path to the source file or directory
# :param level: Optimization level to set
# @public
function(sdk_ses_set_optimization_level path level)
    __sdk_ses_set_optimization_level(${path} "Common" ${level})
endfunction()

# Set optimization level for Debug configuration in Segger Embedded Studio (SES)
#
# This function sets the optimization level specifically for the Debug configuration.
# It is a convenience wrapper around __sdk_ses_set_optimization_level.
#
# Example:
#   sdk_ses_set_optimization_level_debug("path/to/file.c" "Level 2 for speed")
#
# :param path: Path to the source file or directory
# :param level: Optimization level to set
# @public
function(sdk_ses_set_optimization_level_debug path level)
    __sdk_ses_set_optimization_level(${path} "Debug" ${level})
endfunction()

# Set optimization level for Release configuration in Segger Embedded Studio (SES)
#
# This function sets the optimization level specifically for the Release configuration.
# It is a convenience wrapper around __sdk_ses_set_optimization_level.
#
# Example:
#   sdk_ses_set_optimization_level_release("path/to/file.c" "Level 2 for speed")
#
# :param path: Path to the source file or directory
# :param level: Optimization level to set
# @public
function(sdk_ses_set_optimization_level_release path level)
    __sdk_ses_set_optimization_level(${path} "Release" ${level})
endfunction()

set(HPM_SDK_SES_SECTION_PLACEMENT hpm_sdk_ses_section_placement)
define_property(GLOBAL PROPERTY ${HPM_SDK_SES_SECTION_PLACEMENT} BRIEF_DOCS "ses section placement" FULL_DOCS "ses section placement")

# Set section placement for file or directory in Segger Embedded Studio (SES)
#
# This function sets the placement of different sections in the SES project.
# It validates the section type and stores the placement information in a global property.
# The section type is case-sensitive.
#
# Example:
#   sdk_ses_set_section_placement("path/to/file.c" "Debug" "text" "my_section")
#
# :param path: Path to the source file or directory
# :param config_type: Configuration type (e.g. Debug, Release)
# :param section_type: Type of section (data, rodata, text, isr, bss, or vector)
# :param section_name: Name of the section to place
# @private
function(__sdk_ses_set_section_placement path config_type section_type section_name)
    set(VALID_SECTION_TYPES "data" "const" "code" "isr" "bss" "vector")
    string(TOLOWER "${section_type}" section_type_lower)
    if(NOT section_type_lower IN_LIST VALID_SECTION_TYPES)
        message(FATAL_ERROR "Segger: invalid section type: ${section_type}")
    endif()
    if(NOT IS_ABSOLUTE ${path})
        set(path ${CMAKE_CURRENT_SOURCE_DIR}/${path})
    endif()
    set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_SES_SECTION_PLACEMENT} "'${path}':'${config_type}':${section_type_lower}:${section_name}")
endfunction()

# Set code section placement for Segger Embedded Studio (SES)
#
# This function sets the code section placement for all configurations in the SES project.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_code_placement("path/to/file.c" "my_code_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_code_placement path section_name)
    __sdk_ses_set_section_placement(${path} "Common" "code" ${section_name})
endfunction()

# Set code section placement for Debug configuration in Segger Embedded Studio (SES)
#
# This function sets the code section placement specifically for the Debug configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_code_placement_debug("path/to/file.c" "my_code_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_code_placement_debug path section_name)
    __sdk_ses_set_section_placement(${path} "Debug" "code" ${section_name})
endfunction()

# Set code section placement for Release configuration in Segger Embedded Studio (SES)
#
# This function sets the code section placement specifically for the Release configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_code_placement_release("path/to/file.c" "my_code_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_code_placement_release path section_name)
    __sdk_ses_set_section_placement(${path} "Release" "code" ${section_name})
endfunction()

# Set data section placement for Segger Embedded Studio (SES)
#
# This function sets the data section placement for all configurations in the SES project.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_data_placement("path/to/file.c" "my_data_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_data_placement path section_name)
    __sdk_ses_set_section_placement(${path} "Common" "data" ${section_name})
endfunction()

# Set data section placement for Debug configuration in Segger Embedded Studio (SES)
#
# This function sets the data section placement specifically for the Debug configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_data_placement_debug("path/to/file.c" "my_data_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_data_placement_debug path section_name)
    __sdk_ses_set_section_placement(${path} "Debug" "data" ${section_name})
endfunction()

# Set data section placement for Release configuration in Segger Embedded Studio (SES)
#
# This function sets the data section placement specifically for the Release configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_data_placement_release("path/to/file.c" "my_data_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_data_placement_release path section_name)
    __sdk_ses_set_section_placement(${path} "Release" "data" ${section_name})
endfunction()

# Set const section placement for Segger Embedded Studio (SES)
#
# This function sets the const section placement for all configurations in the SES project.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_const_placement("path/to/file.c" "my_const_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_const_placement path section_name)
    __sdk_ses_set_section_placement(${path} "Common" "const" ${section_name})
endfunction()

# Set const section placement for Debug configuration in Segger Embedded Studio (SES)
#
# This function sets the const section placement specifically for the Debug configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_const_placement_debug("path/to/file.c" "my_const_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_const_placement_debug path section_name)
    __sdk_ses_set_section_placement(${path} "Debug" "const" ${section_name})
endfunction()

# Set const section placement for Release configuration in Segger Embedded Studio (SES)
#
# This function sets the const section placement specifically for the Release configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_const_placement_release("path/to/file.c" "my_const_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_const_placement_release path section_name)
    __sdk_ses_set_section_placement(${path} "Release" "const" ${section_name})
endfunction()

# Set ISR (Interrupt Service Routine) section placement for Segger Embedded Studio (SES)
#
# This function sets the ISR section placement for all configurations in the SES project.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_isr_placement("path/to/file.c" "my_isr_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_isr_placement path section_name)
    __sdk_ses_set_section_placement(${path} "Common" "isr" ${section_name})
endfunction()

# Set ISR section placement for Debug configuration in Segger Embedded Studio (SES)
#
# This function sets the ISR section placement specifically for the Debug configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_isr_placement_debug("path/to/file.c" "my_isr_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_isr_placement_debug path section_name)
    __sdk_ses_set_section_placement(${path} "Debug" "isr" ${section_name})
endfunction()

# Set ISR section placement for Release configuration in Segger Embedded Studio (SES)
#
# This function sets the ISR section placement specifically for the Release configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_isr_placement_release("path/to/file.c" "my_isr_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_isr_placement_release path section_name)
    __sdk_ses_set_section_placement(${path} "Release" "isr" ${section_name})
endfunction()

# Set BSS (Block Started by Symbol) section placement for Segger Embedded Studio (SES)
#
# This function sets the BSS section placement for all configurations in the SES project.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_bss_placement("path/to/file.c" "my_bss_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_bss_placement path section_name)
    __sdk_ses_set_section_placement(${path} "Common" "bss" ${section_name})
endfunction()

# Set BSS section placement for Debug configuration in Segger Embedded Studio (SES)
#
# This function sets the BSS section placement specifically for the Debug configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_bss_placement_debug("path/to/file.c" "my_bss_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_bss_placement_debug path section_name)
    __sdk_ses_set_section_placement(${path} "Debug" "bss" ${section_name})
endfunction()

# Set BSS section placement for Release configuration in Segger Embedded Studio (SES)
#
# This function sets the BSS section placement specifically for the Release configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_bss_placement_release("path/to/file.c" "my_bss_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_bss_placement_release path section_name)
    __sdk_ses_set_section_placement(${path} "Release" "bss" ${section_name})
endfunction()

# Set vector section placement for Segger Embedded Studio (SES)
#
# This function sets the vector section placement for all configurations in the SES project.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_vector_placement("path/to/file.c" "my_vector_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_vector_placement path section_name)
    __sdk_ses_set_section_placement(${path} "Common" "vector" ${section_name})
endfunction()

# Set vector section placement for Debug configuration in Segger Embedded Studio (SES)
#
# This function sets the vector section placement specifically for the Debug configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_vector_placement_debug("path/to/file.c" "my_vector_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_vector_placement_debug path section_name)
    __sdk_ses_set_section_placement(${path} "Debug" "vector" ${section_name})
endfunction()

# Set vector section placement for Release configuration in Segger Embedded Studio (SES)
#
# This function sets the vector section placement specifically for the Release configuration.
# It is a convenience wrapper around sdk_ses_set_section_placement.
#
# Example:
#   sdk_ses_set_vector_placement_release("path/to/file.c" "my_vector_section")
#
# :param path: Path to the source file or directory
# :param section_name: Name of the section to place
# @public
function(sdk_ses_set_vector_placement_release path section_name)
    __sdk_ses_set_section_placement(${path} "Release" "vector" ${section_name})
endfunction()

# Set compile options for Segger Embedded Studio (SES)
#
# This function sets compile options for the SES interface library.
# It accepts one or more compile options as arguments. These options are added
# with INTERFACE visibility, meaning they will be applied to any target that links
# against the SES interface library.
#
# Example:
#   sdk_ses_compile_options(-O2 -g)
#
# :param opts: Compile options to be added with INTERFACE visibility.
# @public
function(sdk_ses_compile_options)
    foreach(opt ${ARGN})
        target_compile_options(${HPM_SDK_SES_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Set compile definitions for Segger Embedded Studio (SES)
#
# This function sets compile definitions for the SES interface library.
# It accepts one or more preprocessor definitions as arguments. These definitions
# are added with INTERFACE visibility, meaning they will be applied to any target
# that links against the SES interface library.
#
# Example:
#   sdk_ses_compile_definitions(-DDEBUG -DVERSION=1)
#
# :param def: Compiler preprocessing definitions to be added with INTERFACE visibility.
# @public
function(sdk_ses_compile_definitions)
    foreach(def ${ARGN})
        target_compile_definitions(${HPM_SDK_SES_LIB_ITF} INTERFACE ${def})
    endforeach()
endfunction()

# Link libraries for Segger Embedded Studio (SES)
#
# This function links libraries to the SES interface library.
# It accepts one or more library names or paths as arguments. The libraries can be
# actual file paths or standard libraries provided by the toolchain.
#
# Example:
#   sdk_ses_link_libraries(libm)
#
# :param libs: Libraries to be linked with INTERFACE visibility.
# @public
function(sdk_ses_link_libraries)
    foreach(lib ${ARGN})
        target_link_libraries(${HPM_SDK_SES_LIB_ITF} INTERFACE ${lib})
    endforeach()
endfunction()

# Set linker options for Segger Embedded Studio (SES)
#
# This function sets linker options for the SES interface library.
# It accepts one or more linker options as arguments. These options are added
# with INTERFACE visibility, meaning they will be applied to any target that links
# against the SES interface library.
#
# Example:
#   sdk_ses_ld_options(-Wl,--start-group)
#
# :param opts: Linker options to be added with INTERFACE visibility.
# @private
function(sdk_ses_ld_options)
    foreach(opt ${ARGN})
        target_link_libraries(${HPM_SDK_SES_LIB_ITF} INTERFACE ${opt})
    endforeach()
endfunction()

# Link libraries for Segger Embedded Studio (SES)
#
# This function links libraries to the SES interface library. It accepts one or more
# library names or paths as arguments. If a library path is relative, it will be resolved
# relative to the current source directory. If a library cannot be found, a warning message
# will be displayed.
#
# Example:
#   sdk_ses_ld_lib(libm)
#
# :param libs: Libraries to be linked with INTERFACE visibility.
# @private
function(sdk_ses_ld_lib)
    foreach(l ${ARGN})
        if(IS_ABSOLUTE ${l})
            set(lib ${l})
        else()
            set(lib ${CMAKE_CURRENT_SOURCE_DIR}/${l})
        endif()
        if(NOT EXISTS ${lib})
            message(WARNING "Segger: library ${lib} can't be found")
        else()
            set_property(TARGET ${HPM_SDK_SES_LIB_ITF} APPEND PROPERTY INTERFACE_SES_LD_INPUTS ${lib})
        endif()
    endforeach()
endfunction()

# Link libraries for Segger Embedded Studio (SES) with condition
#
# This function links libraries to the SES interface library only if a specified
# feature is enabled. It accepts one or more library names or paths as arguments.
#
# Example:
#   sdk_ses_ld_lib_ifdef(FEATURE_X libm)
#
# :param feature: The feature flag to check.
# :param libs: Libraries to be linked with INTERFACE visibility.
# @private
function(sdk_ses_ld_lib_ifdef feature)
    if((${feature}) AND (NOT ${${feature}} EQUAL 0))
        sdk_ses_ld_lib(${ARGN})
    endif()
endfunction()

# Add include path for Segger Embedded Studio (SES)
#
# This function adds include directories to the SES interface library.
# It accepts one or more include directory paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
#
# Example:
#   sdk_ses_inc(include)
#
# :param inc: Include directories to be added with INTERFACE visibility.
# @public
function(sdk_ses_inc)
    foreach(inc ${ARGN})
        if(IS_ABSOLUTE ${inc})
            set(path ${inc})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${inc})
        endif()
        target_include_directories(${HPM_SDK_SES_LIB_ITF} INTERFACE ${path})
    endforeach()
endfunction()

# Add source file for Segger Embedded Studio (SES)
#
# This function adds source files to the SES library.
# It accepts one or more source file paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# Directories cannot be added as source files.
#
# Example:
#   sdk_ses_src(source_file.c)
#
# :param SOURCE_FILE: Source files to be added to the SES library.
# @public
function(sdk_ses_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "Segger: directory ${file} can't be added to sdk_ses_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_SES_LIB} ${path})
    endforeach()
endfunction()

# Add source file for Segger Embedded Studio (SES) startup
#
# This function adds source files to the SES startup library.
# It accepts one or more source file paths as arguments. If a relative path
# is provided, it will be resolved relative to the current source directory.
# Directories cannot be added as source files.
#
# Example:
#   sdk_ses_startup_src(startup_file.c)
#
# :param STARTUP_SOURCE_FILE: Source files to be added to the SES startup library.
# @public
function(sdk_ses_startup_src)
    foreach(file ${ARGN})
        if(IS_DIRECTORY ${file})
            message(FATAL_ERROR "Segger: directory ${file} can't be added to sdk_ses_startup_src")
        endif()
        if(IS_ABSOLUTE ${file})
            set(path ${file})
        else()
            set(path ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        endif()
        set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_SES_STARTUP_LIB} ${path})
    endforeach()
endfunction()

# Add source file (glob pattern) for Segger Embedded Studio (SES)
#
# This function adds source files to the SES library using a glob pattern.
# It accepts one or more glob patterns as arguments. The files matching the patterns
# are added to the SES library.
#
# Example:
#   sdk_ses_src_glob("src/**/*.c")
#
# :param SOURCE_FILE_GLOB: Glob patterns to specify source files to be added to SES.
# @public
function(sdk_ses_src_glob)
    foreach(g ${ARGN})
        file(GLOB_RECURSE src ${g})
        list(APPEND globbed_src ${src})
    endforeach()
    sdk_ses_src(${globbed_src})
endfunction()

# Add options for Segger Embedded Studio (SES) project
#
# This function adds options to the SES project configuration.
# It accepts a list of option key-value pairs, where each pair is split by a space.
#
# Example:
#   sdk_ses_options("option_a_name=option_a_value")
#
# :param option_key_value_list: List of option key-value pairs for the SES project.
# @public
function(sdk_ses_options)
    foreach(opt ${ARGN})
        set_property(GLOBAL APPEND PROPERTY ${HPM_SDK_SES_OPTS} ${opt})
    endforeach()
endfunction()

# Set library I/O type for Segger Embedded Studio (SES)
#
# This function sets the library I/O type option for the SES project.
#
# Example:
#   sdk_ses_opt_lib_io_type(STD)
#
# :param type: The library I/O type to set.
# @public
function(sdk_ses_opt_lib_io_type type)
    sdk_ses_options("${SES_OPT_LIB_IO_TYPE}=${type}")
endfunction()

# Set debug connection for Segger Embedded Studio (SES)
#
# This function sets the debug connection option for the SES project.
#
# Example:
#   sdk_ses_opt_debug_connection("GDB Server")
#
# :param conn: The debug connection type to set.
# @public
function(sdk_ses_opt_debug_connection conn)
    sdk_ses_options("${SES_OPT_DBG_TGT_CONN}=${conn}")
endfunction()

# Set J-Link speed for Segger Embedded Studio (SES)
#
# This function sets the J-Link speed option for the SES project.
#
# Example:
#   sdk_ses_opt_debug_jlink_speed(4000)
#
# :param speed: The J-Link speed to set.
# @public
function(sdk_ses_opt_debug_jlink_speed speed)
    sdk_ses_options("${SES_OPT_DBG_JLINK_SPEED}=${speed}")
endfunction()

# Generate Segger Embedded Studio (SES) project
#
# This function generates a Segger Embedded Studio (SES) project file.
# It collects various properties and configurations from the CMake project
# and writes them to a JSON file, which is then used to generate the SES project.
#
# Example:
#   generate_ses_project()
#
# :param None: This function does not take any parameters.
# @public
function(generate_ses_project)
    get_property(target_source_files TARGET app PROPERTY SOURCES)
    get_property(target_app_include_dirs TARGET app PROPERTY INCLUDE_DIRECTORIES)
    get_property(target_lib_sources TARGET ${HPM_SDK_LIB} PROPERTY SOURCES)
    get_property(target_include_dirs TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    get_property(target_defines TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_COMPILE_DEFINITIONS)
    get_property(target_link_sym TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_LINK_SYMBOLS)
    get_property(target_gcc_cflags TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_COMPILE_OPTIONS)
    get_property(target_ses_cflags TARGET ${HPM_SDK_SES_LIB_ITF} PROPERTY INTERFACE_COMPILE_OPTIONS)
    get_property(target_ses_ld_lib TARGET ${HPM_SDK_SES_LIB_ITF} PROPERTY INTERFACE_SES_LD_INPUTS)
    get_property(target_ld_lib TARGET ${HPM_SDK_LIB_ITF} PROPERTY INTERFACE_LINK_LIBRARIES)
    get_property(target_ses_source_files GLOBAL PROPERTY ${HPM_SDK_SES_LIB})
    get_property(target_ses_startup_source_files GLOBAL PROPERTY ${HPM_SDK_SES_STARTUP_LIB})
    get_property(target_ses_options GLOBAL PROPERTY ${HPM_SDK_SES_OPTS})
    get_property(target_gcc_source_files TARGET ${HPM_SDK_GCC_LIB} PROPERTY SOURCES)
    get_property(target_gcc_startup_source_files TARGET ${HPM_SDK_GCC_STARTUP_LIB} PROPERTY SOURCES)
    get_property(target_ses_include_dirs TARGET ${HPM_SDK_SES_LIB_ITF} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

    if(NOT SES_TOOLCHAIN_VARIANT)
        set(SES_TOOLCHAIN_VARIANT "Standard")
    endif()

    string(FIND ${SES_TOOLCHAIN_VARIANT} "Andes" exist)
    if(NOT ${exist} EQUAL -1)
        set(SES_USE_TOOLCHAIN_ANDES 1)
        if(NOT SES_COMPILER_VARIANT)
            set(SES_COMPILER_VARIANT "gcc")
            get_property(target_nds_cflags TARGET ${HPM_SDK_NDSGCC_LIB_ITF} PROPERTY INTERFACE_COMPILE_OPTIONS)
        endif()
        if(NOT SES_ASSEMBLER_VARIANT)
            set(SES_ASSEMBLER_VARIANT "gcc")
        endif()
        if(NOT SES_LINKER_VARIANT)
            set(SES_LINKER_VARIANT "gnu")
        endif()
    endif()

    if(NOT SES_COMPILER_VARIANT)
        set(SES_COMPILER_VARIANT "gcc")
    endif()

    if(NOT SES_ASSEMBLER_VARIANT)
        set(SES_ASSEMBLER_VARIANT "SEGGER")
    endif()

    if(NOT SES_LINKER_VARIANT)
        set(SES_LINKER_VARIANT "SEGGER")
    endif()

    # process linked libraries
    set(target_ses_ld_input "")
    if(target_ld_lib)
        foreach(f IN ITEMS ${target_ld_lib})
            if(EXISTS ${f})
                if("${target_ses_ld_input}" STREQUAL "")
                    set(target_ses_ld_input ${f})
                else()
                    set(target_ses_ld_input "${target_ses_ld_input};${f}")
                endif()
            endif()
        endforeach()
    endif()

    if(target_ses_ld_lib)
        foreach(f IN ITEMS ${target_ses_ld_lib})
            if("${target_ses_ld_input}" STREQUAL "")
                set(target_ses_ld_input ${f})
            else()
                set(target_ses_ld_input "${target_ses_ld_input};${f}")
            endif()
        endforeach()
    endif()
    list(REMOVE_DUPLICATES target_ses_ld_input)

    set(target_cflags "")
    set(gcc_opt_level "")
    foreach(f IN ITEMS ${target_gcc_cflags})
        string(FIND ${f} "-mabi=" exist)
        if(NOT ${exist} EQUAL -1)
            string(SUBSTRING ${f} 6 -1 SES_COMPILER_ABI)
            list(REMOVE_ITEM target_gcc_cflags ${f})
            continue()
        endif()
        string(FIND ${f} "-march=" exist)
        if(NOT ${exist} EQUAL -1)
            string(SUBSTRING ${f} 7 -1 SES_COMPILER_ARCH)
            list(REMOVE_ITEM target_gcc_cflags ${f})
            continue()
        endif()
        STRING(REGEX MATCH "-O[0-3s].*" exist ${f})
        if(NOT ${exist} EQUAL -1)
            STRING(REGEX REPLACE "(-O[0-3s])" "\\1" gcc_opt_level ${f})
            list(REMOVE_ITEM target_gcc_cflags ${f})
            continue()
        endif()
        STRING(REGEX MATCH "-D.*" exist ${f})
        if(NOT ${exist} EQUAL -1)
            if(NOT target_cflags)
                set(target_cflags ${f})
            else()
                set(target_cflags "${target_cflags};${f}")
            endif()
            list(REMOVE_ITEM target_gcc_cflags ${f})
            continue()
        endif()
        # all warnings are passed to ses
        STRING(REGEX MATCH "-W.*" exist ${f})
        if(NOT ${exist} EQUAL -1)
            if(NOT target_cflags)
                set(target_cflags ${f})
            else()
                set(target_cflags "${target_cflags};${f}")
            endif()
            list(REMOVE_ITEM target_gcc_cflags ${f})
            continue()
        endif()
        # convert lto to ses specific settings
        STRING(REGEX MATCH "-flto" exist ${f})
        if(NOT ${exist} EQUAL -1)
            if(${SES_COMPILER_VARIANT} STREQUAL "SEGGER")
                if("${ses_extra_options}" STREQUAL "")
                    set(ses_extra_options "link_time_optimization=Yes")
                else()
                    set(ses_extra_options "${ses_extra_options},link_time_optimization=Yes")
                endif()
            endif()
            # FIXME: -flto will cause build failure on buiding asm file calling external functions
            list(REMOVE_ITEM target_gcc_cflags ${f})
            continue()
        endif()
    endforeach()

    if (${SES_COMPILER_VARIANT} STREQUAL "gcc")
        if(target_gcc_cflags)
            if(NOT target_cflags)
                set(target_cflags ${target_gcc_cflags})
            else()
                set(target_cflags "${target_cflags};${target_gcc_cflags}")
            endif()
        endif()
        if(SES_USE_TOOLCHAIN_ANDES)
            if(target_nds_cflags)
                foreach(f IN ITEMS ${target_nds_cflags})
                    if(NOT target_cflags)
                        set(target_cflags ${f})
                    else()
                        set(target_cflags "${target_cflags};${f}")
                    endif()
                endforeach()
            endif()
            set(target_source_files "${target_source_files};${target_gcc_source_files}")
        else()
            set(target_source_files "${target_source_files};${target_ses_source_files}")
        endif()
    else()
        set(target_source_files "${target_source_files};${target_ses_source_files}")
    endif()

    if(${SES_LINKER_VARIANT} STREQUAL "gnu")
        set(target_source_files "${target_source_files};${target_gcc_startup_source_files}")
    else()
        set(target_source_files "${target_source_files};${target_ses_startup_source_files}")
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

        STRING(REGEX MATCH "-O[0-3s].*" exist ${f})
        if(NOT ${exist} EQUAL -1)
            STRING(REGEX REPLACE "(-O[0-3s])" "\\1" gcc_opt_level ${f})
            list(REMOVE_ITEM target_ses_cflags ${f})
            continue()
        endif()

        if(NOT target_cflags)
            set(target_cflags ${f})
        else()
            set(target_cflags "${target_cflags};${f}")
        endif()
    endforeach()

    # fallback option
    if(NOT SES_COMPILER_ABI)
        set(SES_COMPILER_ABI "ilp32")
    endif()
    if(NOT SES_COMPILER_ARCH)
        set(SES_COMPILER_ARCH "rv32imac")
    endif()

    STRING(REGEX REPLACE "rv32imafd(.*)" "rv32g\\1" SES_COMPILER_ARCH ${SES_COMPILER_ARCH})
    STRING(FIND ${SES_COMPILER_ARCH} "_" exist)
    if(NOT ${exist} EQUAL -1)
        set(target_compiler_arch_exts_csr 0)
        set(target_compiler_arch_exts_fencei 0)
        # isa extenions will be processed according to given arch, user should be responsible for ext settings
        STRING(REGEX REPLACE "(rv[A-Za-z0-9]+)_([A-Za-z0-9_]+)" "\\1;\\2" RESULT ${SES_COMPILER_ARCH})
        LIST(GET RESULT 0 target_compiler_isa)
        LIST(GET RESULT 1 SES_COMPILER_ARCH_EXTS)
    else()
        set(target_compiler_isa ${SES_COMPILER_ARCH})
        # zicsr and zifencei will be set if no isa extention specified
        set(target_compiler_arch_exts_csr 1)
        set(target_compiler_arch_exts_fencei 1)
    endif()

    set(post_build_command "")
    set(is_secondary_core 0)
    if (DEFINED BUILD_FOR_SECONDARY_CORE)
        if (BUILD_FOR_SECONDARY_CORE)
            if (NOT DEFINED SEC_CORE_IMG_C_ARRAY_OUTPUT)
                set (SEC_CORE_IMG_C_ARRAY_OUTPUT "sec_core_img.c")
            endif()
            set(post_build_command "${PYTHON_EXECUTABLE} $(HPM_SDK_BASE)/scripts/bin2c.py $(OutDir)/demo.bin sec_core_img > ${SEC_CORE_IMG_C_ARRAY_OUTPUT}")
            string(REPLACE "\\" "/" post_build_command ${post_build_command})
            set(is_secondary_core 1)
        endif()
    endif()

    set(target_link_symbols "")
    foreach(sym IN ITEMS ${target_link_sym})
        if(${SES_LINKER_VARIANT} STREQUAL "segger")
            string(FIND ${sym} "_heap_size" exist)
            if(NOT ${exist} EQUAL -1)
                continue()
            endif()
            string(FIND ${sym} "_stack_size" exist)
            if(NOT ${exist} EQUAL -1)
                continue()
            endif()
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
    if(${SES_LINKER_VARIANT} STREQUAL "gnu")
        set(target_linker ${target_linker_script})
    else()
        if(CUSTOM_SES_LINKER_FILE)
            set(target_linker ${CUSTOM_SES_LINKER_FILE})
        else()
            # if no specific linker file is set, try to locate it
            string(REPLACE ".ld" ".icf" target_linker ${target_linker_script})
            string(REPLACE "/gcc/" "/segger/" target_linker ${target_linker})
        endif()
    endif()

    foreach(opt IN ITEMS ${target_ses_options})
        string(REPLACE "\"" "" opt ${opt}) # remove any quotes
        string(REGEX MATCH "${SES_OPT_DBG_TGT_CONN}=" out ${opt})
        if(out)
            string(REGEX REPLACE "${SES_OPT_DBG_TGT_CONN}=(.*)" "\\1" debug_target_conn ${opt})
            continue()
        endif()
        string(REGEX MATCH "${SES_OPT_LIB_IO_TYPE}=" out ${opt})
        if(out)
            string(REGEX REPLACE "${SES_OPT_LIB_IO_TYPE}=(.*)" "\\1" lib_io_type ${opt})
            continue()
        endif()
        string(REGEX MATCH "${SES_OPT_DBG_JLINK_SPEED}=" out ${opt})
        if(out)
            string(REGEX REPLACE "${SES_OPT_DBG_JLINK_SPEED}=(.*)" "\\1" jlink_speed ${opt})
            continue()
        endif()
        if("${ses_extra_options}" STREQUAL "")
            set(ses_extra_options "${opt}")
        else()
            set(ses_extra_options "${ses_extra_options},${opt}")
        endif()
    endforeach()

    if(NOT lib_io_type)
        set(lib_io_type STD)
    endif()

    if(NOT debug_target_conn)
        set(debug_target_conn ${SES_DBG_CONN_GDB_SRV})
    endif()

    string(REGEX MATCH "GDB Server" is_gdb ${debug_target_conn})
    if(is_gdb)
        set(OPENOCD_READY 1)
        # get specified openocd config file for probe
        get_openocd_probe_name_of_board(${BOARD_YAML} OPENOCD_PROBE)
        if(NOT OPENOCD_PROBE)
            message(STATUS " Segger: openocd-probe was not correctly configured in ${BOARD_YAML}")
            set(OPENOCD_READY 0)
        endif()
        # get specified openocd config file for soc
        get_openocd_soc_name_of_board(${BOARD_YAML} OPENOCD_SOC)
        if(NOT OPENOCD_SOC)
            message(STATUS " Segger: openocd-soc was not correctly configured in ${BOARD_YAML}")
            set(OPENOCD_READY 0)
        endif()
        find_program(OPENOCD openocd)
        if(NOT EXISTS ${OPENOCD})
            message(STATUS " Segger: openocd can not be located")
            set(OPENOCD_READY 0)
        else()
            set(OPENOCD ${OPENOCD})
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
    endif()

    foreach (lib_src IN ITEMS ${target_lib_sources})
        set(target_sources "${target_sources},${lib_src}")
    endforeach ()

    if (NOT IS_ABSOLUTE ${target_linker})
        set(target_linker ${CMAKE_CURRENT_SOURCE_DIR}/${target_linker})
    endif()

    if(EXISTS ${target_linker})
        if(SES_LINKER_VARIANT)
            string(TOLOWER ${SES_LINKER_VARIANT} linker_variant_lower)
            if(${linker_variant_lower} STREQUAL "segger")
                string(REGEX MATCH "\.ld\$" is_valid_linker_script ${target_linker})
            endif()
            if(${linker_variant_lower} STREQUAL "gnu")
                string(REGEX MATCH "\.icf\$" is_valid_linker_script ${target_linker})
            endif()
            if(is_valid_linker_script)
                message(WARNING "\nSegger: ${SES_LINKER_VARIANT} linker is used, but linker file is \"${target_linker}\". It might fail on linking.\n")
            endif()
        endif()
        message(STATUS "Segger linker script: " ${target_linker})
    else()
        message(WARNING "Segger: can not generate project, due to missing linker script: ${target_linker}")
        return()
    endif()

    # Remove duplicates
    list(REMOVE_DUPLICATES target_sources)
    list(REMOVE_DUPLICATES target_include_dirs)

    set(target_sources_with_macro "")
    foreach (target_source IN ITEMS ${target_sources})
        if(NOT IS_ABSOLUTE ${target_source})
            set(target_source ${CMAKE_CURRENT_SOURCE_DIR}/${target_source})
        endif()
        if("${target_sources_with_macro}" STREQUAL "")
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

    foreach (target_source IN ITEMS ${target_ses_include_dirs})
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

    if(${SES_LINKER_VARIANT} STREQUAL "SEGGER")
        set(target_defines_escaped "${target_defines_escaped},HPM_SES_USE_SEGGER_LD=1")
    endif()

    set(target_register_definition "${HPM_SDK_BASE}/soc/${HPM_SOC_SERIES}/${HPM_SOC}/hpm_ses_reg.xml")
    set(target_cpu_register_definition "${HPM_SDK_BASE}/soc/${HPM_SOC_SERIES}/${HPM_SOC}/hpm_ses_riscv_cpu_regs.xml")

    string(REPLACE "\\"  "/" target_sources_with_macro ${target_sources_with_macro})
    string(REPLACE "\\"  "/" target_include_dirs_with_macro ${target_include_dirs_with_macro})
    string(REPLACE "\\"  "/" target_linker ${target_linker})
    string(REPLACE "\\"  "/" target_register_definition ${target_register_definition})
    string(REPLACE "\\"  "/" target_cpu_register_definition ${target_cpu_register_definition})
    string(REPLACE "\\"  "/" hpm_sdk_base_path ${HPM_SDK_BASE})
    string(REPLACE "\\"  "/" hpm_board_path ${HPM_BOARD_DIR})

    if(SES_COMPILER_ARCH_EXTS)
        STRING(REPLACE "_" ";" RESULT ${SES_COMPILER_ARCH_EXTS})
        FOREACH(i IN LISTS RESULT)
            if(${i} STREQUAL "zicsr")
                set(target_compiler_arch_exts_csr 1)
            endif()
            if(${i} STREQUAL "zifencei")
                set(target_compiler_arch_exts_fencei 1)
            endif()
            if(${i} STREQUAL "zba")
                set(target_compiler_arch_exts_zba 1)
            endif()
            if(${i} STREQUAL "zbb")
                set(target_compiler_arch_exts_zbb 1)
            endif()
            if(${i} STREQUAL "zbc")
                set(target_compiler_arch_exts_zbc 1)
            endif()
            if(${i} STREQUAL "zbs")
                set(target_compiler_arch_exts_zbs 1)
            endif()
            # process zcc arch string
            if(${i} STREQUAL "xandes")
                set(target_compiler_arch_exts_csr 1)
                set(target_compiler_arch_exts_fencei 1)
            endif()
        ENDFOREACH()
    endif()


    # Specify SES target device name to be defined in SES project file
    if (NOT HPM_DEVICE_NAME)
        set(SES_DEVICE_NAME "${HPM_SOC}XXXX")
    else()
        set(SES_DEVICE_NAME ${HPM_DEVICE_NAME})
    endif()
    message(STATUS "Segger device name: ${SES_DEVICE_NAME}")

    if(${is_secondary_core})
        set(SES_DEVICE_NAME "${HPM_DEVICE_NAME}_CPU1")
    endif()

    get_property(section_placement GLOBAL PROPERTY ${HPM_SDK_SES_SECTION_PLACEMENT})
    get_property(optimization_level GLOBAL PROPERTY ${HPM_SDK_SES_OPTIMIZATION_LEVEL})

    set(target_data "
        \"name\": \"${CMAKE_PROJECT_NAME}\",
        \"sources\": \"${target_sources_with_macro}\",
        \"includes\": \"${target_include_dirs_with_macro}\",
        \"defines\":\"${target_defines_escaped}\",
        \"linker\":\"${target_linker}\",
        \"link_symbols\":\"${target_link_symbols}\",
        \"sdk_base\":\"${hpm_sdk_base_path}\",
        \"board\":\"${BOARD}\",
        \"board_dir\":\"${hpm_board_path}\",
        \"soc\":\"${HPM_SOC}\",
        \"register_definition\":\"${target_register_definition}\",
        \"cpu_register_definition\":\"${target_cpu_register_definition}\",
        \"post_build_command\":\"${post_build_command}\",
        \"heap_size\":\"${HEAP_SIZE}\",
        \"stack_size\":\"${STACK_SIZE}\",
        \"cplusplus\":\"${CMAKE_CXX_STANDARD}\",
        \"gcc_opt_level\":\"${gcc_opt_level}\",
        \"target_device_name\":\"${SES_DEVICE_NAME}\",
        \"toolchain_variant\":\"${SES_TOOLCHAIN_VARIANT}\",
        \"compiler_variant\":\"${SES_COMPILER_VARIANT}\",
        \"assembler_variant\":\"${SES_ASSEMBLER_VARIANT}\",
        \"linker_variant\":\"${SES_LINKER_VARIANT}\",
        \"cflags\":\"${target_cflags}\",
        \"compiler_abi\":\"${SES_COMPILER_ABI}\",
        \"compiler_isa\":\"${target_compiler_isa}\",
        \"compiler_arch_exts_csr\":\"${target_compiler_arch_exts_csr}\",
        \"compiler_arch_exts_fencei\":\"${target_compiler_arch_exts_fencei}\",
        \"compiler_arch_exts_zba\":\"${target_compiler_arch_exts_zba}\",
        \"compiler_arch_exts_zbb\":\"${target_compiler_arch_exts_zbb}\",
        \"compiler_arch_exts_zbc\":\"${target_compiler_arch_exts_zbc}\",
        \"compiler_arch_exts_zbs\":\"${target_compiler_arch_exts_zbs}\",
        \"ses_link_input\":\"${target_ses_ld_input}\",
        \"enable_nds_dsp\":\"${enable_nds_dsp}\",
        \"enable_cpp_exceptions\":\"${enable_cpp_exceptions}\",
        \"library_io_type\":\"${lib_io_type}\",
        \"extra_ses_options\":\"${ses_extra_options}\",
        \"app_name\":\"${APP_NAME}\",
        \"section_placement\":\"${section_placement}\",
        \"optimization_level\":\"${optimization_level}\"
        ")

    string(REGEX MATCH "GDB Server" is_gdb ${debug_target_conn})
    if(is_gdb)
        if(${OPENOCD_READY})
            set(target_data "${target_data},
                \"openocd\":\"${OPENOCD}\",
                \"openocd_soc\":\"${OPENOCD_SOC}\",
                \"debug_probe\":\"${OPENOCD_PROBE}\",
                \"auto_start_gdb_server\":\"${AUTO_START_GDB_SRV}\",
                \"gdb_server_port\":\"${GDB_SERVER_PORT}\",
                \"gdb_server_reset_command\":\"${GDB_SERVER_RST_CMD}\"")
        else()
            message(STATUS " Segger: no debugger configuration is generated, due to missing openocd information. Please configure manually in Segger Embedded Studio")
        endif()
    else()
        set(target_data "${target_data},
            \"debug_target_connection\":\"${debug_target_conn}\"")

        if(jlink_speed)
            set(target_data "${target_data}, \"jlink_speed\":\"${jlink_speed}\"")
        endif()
    endif()

    set(SES_PROJECT_PATH "${CMAKE_CURRENT_BINARY_DIR}/segger_embedded_studio")
    set(SES_JSON_FILE_PATH "${SES_PROJECT_PATH}/${CMAKE_PROJECT_NAME}.json")
    set(SDK_SCRIPT_IDE_PATH "${HPM_SDK_BASE}/scripts/ide")
    file(WRITE ${SES_JSON_FILE_PATH}
        "{
            \"target\": {
                ${target_data}
            }
        }")

    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} ${SDK_SCRIPT_IDE_PATH}/segger/embedded_studio_proj_gen.py "${SES_JSON_FILE_PATH}" "${SES_PROJECT_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}"
        WORKING_DIRECTORY  ${SDK_SCRIPT_IDE_PATH}/segger
        )
endfunction ()
