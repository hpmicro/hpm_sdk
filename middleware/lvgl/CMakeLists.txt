# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

file(GLOB_RECURSE LVGL_SRC lvgl/src/*.c)
sdk_inc(lvgl)
sdk_src(${LVGL_SRC})

sdk_inc(porting)
sdk_src(porting/lv_adapter.c)

if(DEFINED CONFIG_LV_DEMO)
    file(GLOB_RECURSE LVGL_DEMO_SRC lvgl/demos/${CONFIG_LV_DEMO}/*.c)
    sdk_src(${LVGL_DEMO_SRC})
endif()
