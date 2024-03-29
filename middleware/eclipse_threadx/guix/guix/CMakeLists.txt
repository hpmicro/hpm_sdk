cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

if(NOT DEFINED GX_INCLUDE_USER_DEFINE_FILE)
    sdk_compile_definitions(-DGX_INCLUDE_USER_DEFINE_FILE=1)
endif()
if(NOT DEFINED THREADX_ARCH)
    message(FATAL_ERROR "Error: THREADX_ARCH not defined")
endif()
if(NOT DEFINED THREADX_TOOLCHAIN)
    message(FATAL_ERROR "Error: THREADX_TOOLCHAIN not defined")
endif()

# Pick up the port specific stuff first
if(DEFINED GUIX_CUSTOM_PORT)
    add_subdirectory(${GUIX_CUSTOM_PORT} guix_port)
else()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ports/${THREADX_ARCH}/${THREADX_TOOLCHAIN})
endif()

# Then the common files
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/common)


# Include the user's override file if required
if (NOT GX_USER_FILE)
    message(STATUS "Using default gx_user.h file")
    set(GX_USER_FILE ${CMAKE_CURRENT_LIST_DIR}/common/inc/gx_user_sample.h)
else()
    message(STATUS "Using custom gx_user.h file from ${GX_USER_FILE}")
endif()

# set include file path to place configure file
set(generated_file_path "${__build_dir}/generated")
set(CUSTOM_INC_DIR ${generated_file_path}/include)
message(STATUS "copy gx_user.h to " ${CUSTOM_INC_DIR})
configure_file(${GX_USER_FILE} ${CUSTOM_INC_DIR}/gx_user.h COPYONLY)
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