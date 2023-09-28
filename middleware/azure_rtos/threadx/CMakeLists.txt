cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

sdk_compile_definitions(-DCONFIG_DISABLE_GLOBAL_IRQ_ON_STARTUP=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)

if(NOT DEFINED TX_INCLUDE_USER_DEFINE_FILE)
    sdk_compile_definitions(-DTX_INCLUDE_USER_DEFINE_FILE=1)
endif()
if(NOT DEFINED THREADX_ARCH)
    message(FATAL_ERROR "Error: THREADX_ARCH not defined")
endif()
if(NOT DEFINED THREADX_TOOLCHAIN)
    message(FATAL_ERROR "Error: THREADX_TOOLCHAIN not defined")
endif()
message(STATUS "THREADX_ARCH: ${THREADX_ARCH}")
message(STATUS "THREADX_TOOLCHAIN: ${THREADX_TOOLCHAIN}")

# Define our target library and an alias for consumers

# Pick up the port specific variables and apply them

add_subdirectory(ports/${THREADX_ARCH}/${THREADX_TOOLCHAIN})
add_subdirectory(ports/common)
# Pick up the common stuff
add_subdirectory(common)

# Define the FreeRTOS adaptation layer
add_library(freertos-threadx EXCLUDE_FROM_ALL)
target_include_directories(freertos-threadx
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/utility/rtos_compatibility_layers/FreeRTOS
)
target_sources(freertos-threadx
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/utility/rtos_compatibility_layers/FreeRTOS/tx_freertos.c
)
target_link_libraries(freertos-threadx PUBLIC threadx)

# If the user provided an override, copy it to the custom directory
if (NOT TX_USER_FILE)
    message(STATUS "Using default tx_user.h file")
    set(TX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/common/inc/tx_user_sample.h)
else()
    message(STATUS "Using custom tx_user.h file from ${TX_USER_FILE}")
endif()

# set include file path to place configure file
set(generated_file_path "${__build_dir}/generated")
set(CUSTOM_INC_DIR ${generated_file_path}/include)
message(STATUS "copy tx_user.h to " ${CUSTOM_INC_DIR})
configure_file(${TX_USER_FILE} ${CUSTOM_INC_DIR}/tx_user.h COPYONLY)

sdk_inc(${CUSTOM_INC_DIR})

# Enable a build target that produces a ZIP file of all sources
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_IGNORE_FILES
  \\.git/
  \\.github/
  _build/
  \\.git
  \\.gitattributes
  \\.gitignore
  ".*~$"
)
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)