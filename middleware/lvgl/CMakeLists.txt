# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

file(GLOB_RECURSE LVGL_SRC lvgl/src/*.c)
sdk_inc(lvgl)
sdk_src(${LVGL_SRC})

if(NOT CONFIG_LVGL_CUSTOM_PORTABLE)
    sdk_inc(porting)
    sdk_src(porting/hpm_lvgl.c)
endif()

if(DEFINED CONFIG_LV_DEMO)
    file(GLOB_RECURSE LVGL_DEMO_SRC lvgl/demos/*.c)
    sdk_src(${LVGL_DEMO_SRC})
endif()
