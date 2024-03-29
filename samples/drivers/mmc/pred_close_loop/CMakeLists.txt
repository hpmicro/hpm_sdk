# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mmc_pred_close_loop)

sdk_inc(src)
sdk_app_src(src/pwm.c)
sdk_app_src(src/qeo.c)
sdk_app_src(src/sei.c)
sdk_app_src(src/mmc.c)
generate_ide_projects()
