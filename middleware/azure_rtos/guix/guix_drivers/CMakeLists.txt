# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(DEFINED CONFIG_HPM_TOUCH)
    sdk_src(touch/guix_touch.c)
endif()

sdk_inc(touch)
sdk_inc(${HPM_SOC})
sdk_src_ifdef(CONFIG_AZURE_RTOS_GUIX_565RGB ${HPM_SOC}/guix_display_driver_565rgb.c)
sdk_src_ifdef(CONFIG_AZURE_RTOS_GUIX_32ARGB ${HPM_SOC}/guix_display_driver_32argb.c)

