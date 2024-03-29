# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)

if(${CONFIG_ECLIPSE_THREADX_USBX_DEVICE})
sdk_src(hpm_usbx_port.c)
endif()
if(${CONFIG_ECLIPSE_THREADX_USBX_HOST})
sdk_src(__wrap_ux_hcd_ehci_port_reset.c)
sdk_src(__wrap_ux_hcd_ehci_initialize.c)
endif()

if(${CONFIG_ECLIPSE_THREADX_USBX_HOST})
sdk_src(usb_glue_hpm.c)
endif()