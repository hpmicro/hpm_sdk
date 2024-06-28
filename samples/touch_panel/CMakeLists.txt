# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt9xx")
endif()
set(CONFIG_HPM_PANEL 1)
set(CONFIG_HPM_TOUCH 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(touch_pannel)

sdk_app_src(src/touch_panel.c)
generate_ide_projects()
