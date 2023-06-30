# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)

if(${CONFIG_AZURE_RTOS_USBX_DEVICE})
sdk_src(hpm_usbx_port.c)
endif()

if(${CONFIG_AZURE_RTOS_USBX_HOST})
sdk_src(usb_glue_hpm.c)
endif()