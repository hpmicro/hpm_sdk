# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(SYSVIEW)
sdk_src(SYSVIEW/SEGGER_SYSVIEW.c)
sdk_src(Config/SEGGER_SYSVIEW_Config.c)

sdk_compile_definitions(-DCONFIG_SEGGER_SYSVIEW)

if(DEFINED CONFIG_FREERTOS AND NOT "${CONFIG_FREERTOS}" STREQUAL "0")
  add_subdirectory(Config/FreeRTOS)
elseif(DEFINED CONFIG_UCOS_III AND NOT "${CONFIG_UCOS_III}" STREQUAL "0")
  add_subdirectory(Config/uCOS-III)
else()
  add_subdirectory(Config/Baremetal)
endif()
