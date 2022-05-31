# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(diskio.c)

sdk_inc_ifdef(CONFIG_USB_FATFS usb)
sdk_src_ifdef(CONFIG_USB_FATFS usb/hpm_fatfs_usb.c)

sdk_inc_ifdef(CONFIG_SDMMC sdxc)
sdk_src_ifdef(CONFIG_SDMMC sdxc/hpm_sdmmc_disk.c)
