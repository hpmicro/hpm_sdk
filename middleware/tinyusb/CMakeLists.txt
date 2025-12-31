# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions(-DCFG_TUSB_MCU=OPT_MCU_HPM)

sdk_inc(src)
sdk_inc(hw)
sdk_inc(lib/embedded-cli)

sdk_src(hw/bsp/board.c)
sdk_src(hw/bsp/hpmicro/family.c)

if(CONFIG_USB_DEVICE_CDC OR CONFIG_USB_DEVICE_HID OR CONFIG_USB_DEVICE_MSC
  OR CONFIG_USB_DEVICE_AUDIO)
set(CONFIG_TINYUSB_DEVICE 1)
endif()

sdk_src(src/tusb.c)
sdk_src(src/common/tusb_fifo.c)

if(CONFIG_TINYUSB_DEVICE)
sdk_src_ifdef(CONFIG_USB_DEVICE_CDC src/class/cdc/cdc_device.c)
sdk_src_ifdef(CONFIG_USB_DEVICE_MSC src/class/msc/msc_device.c)
sdk_src_ifdef(CONFIG_USB_DEVICE_HID src/class/hid/hid_device.c)
sdk_src_ifdef(CONFIG_USB_DEVICE_AUDIO src/class/audio/audio_device.c)
sdk_src(src/device/usbd.c)
sdk_src(src/device/usbd_control.c)
sdk_src(src/portable/hpm/dcd_hpm.c)
endif()

if(CONFIG_TINYUSB_HOST)
sdk_src(src/portable/ehci/ehci.c)
sdk_src(src/portable/hpm/hcd_hpm.c)
sdk_src(src/class/msc/msc_host.c)
sdk_src(src/class/hid/hid_host.c)
sdk_src(src/class/cdc/cdc_host.c)
sdk_src(src/host/hub.c)
sdk_src(src/host/usbh.c)
endif()
