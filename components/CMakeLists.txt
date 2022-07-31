# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(serial_nor)
sdk_inc(usb)

add_subdirectory(debug_console)
add_subdirectory_ifdef(CONFIG_CAMERA camera)
add_subdirectory_ifdef(CONFIG_CODEC codec)
add_subdirectory_ifdef(CONFIG_USB_HOST usb/host)
add_subdirectory_ifdef(CONFIG_USB_DEVICE usb/device)
add_subdirectory_ifdef(CONFIG_ENET_PHY_RTL8211 enet_phy/rtl8211)
add_subdirectory_ifdef(CONFIG_ENET_PHY_RTL8201 enet_phy/rtl8201)
add_subdirectory_ifdef(CONFIG_ENET_PHY_DP83867 enet_phy/dp83867)
add_subdirectory_ifdef(CONFIG_ENET_PHY_DP83848 enet_phy/dp83848)
add_subdirectory_ifdef(CONFIG_TOUCH touch)
add_subdirectory_ifdef(CONFIG_HPM_ADC adc)
add_subdirectory_ifdef(CONFIG_HPM_SPI spi)
add_subdirectory_ifdef(CONFIG_DMA_MANAGER dma_manager)
