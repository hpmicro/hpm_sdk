# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
add_subdirectory(lvgl)
add_subdirectory(lv_drivers)

if(DEFINED CONFIG_LV_DEMO)
add_subdirectory(lv_demos)
endif()
