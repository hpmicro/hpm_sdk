# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_src(diskio.c)

sdk_inc_ifdef(CONFIG_USB_FATFS usb)
sdk_src_ifdef(CONFIG_USB_FATFS_TINYUSB usb/hpm_fatfs_tinyusb.c)
sdk_src_ifdef(CONFIG_USB_FATFS_CHERRYUSB usb/hpm_fatfs_cherryusb.c)

if(NOT DEFINED CONFIG_HPM_SPI_SDCARD)
sdk_inc_ifdef(CONFIG_SDMMC sdxc)
sdk_src_ifdef(CONFIG_SDMMC sdxc/hpm_sdmmc_disk.c)
else()
sdk_inc_ifdef(CONFIG_HPM_SPI_SDCARD spi_sd)
sdk_src_ifdef(CONFIG_HPM_SPI_SDCARD spi_sd/hpm_spi_sd_disk.c)
endif()