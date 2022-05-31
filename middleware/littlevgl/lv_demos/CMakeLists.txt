# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

if(DEFINED CONFIG_LV_DEMO)
sdk_compile_definitions("-DLV_DEMO_CONF_INCLUDE_SIMPLE=1")
add_subdirectory(src/${CONFIG_LV_DEMO})
endif()
