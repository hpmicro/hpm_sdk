# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CAMERA)
set(CONFIG_CAMERA "ov7725")
endif()
set(CONFIG_HPM_CAMERA 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cam_example)

sdk_app_src(src/cam.c)
generate_ses_project()
