# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(src)
sdk_inc(src/portable)

sdk_src(src/tusb.c)
sdk_src(src/common/tusb_fifo.c)

sdk_src(src/class/cdc/cdc_device.c)
sdk_src(src/class/dfu/dfu_rt_device.c)
sdk_src(src/class/midi/midi_device.c)
sdk_src(src/class/usbtmc/usbtmc_device.c)
sdk_src(src/class/vendor/vendor_device.c)
sdk_src(src/class/msc/msc_device.c)
sdk_src(src/class/hid/hid_device.c)
sdk_src(src/device/usbd.c)
sdk_src(src/device/usbd_control.c)
sdk_src(src/portable/hpm/dcd_hpm.c)

sdk_src(src/class/msc/msc_host.c)
sdk_src(src/class/hid/hid_host.c)

sdk_src(src/host/hub.c)
sdk_src(src/host/usbh.c)
sdk_src(src/host/usbh_control.c)
sdk_src(src/portable/hpm/hcd_hpm.c)
