# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if ((CONFIG_ESP_HOSTED_SPI_HD AND CONFIG_ESP_HOSTED_SPI_HD  EQUAL 1) OR
    (CONFIG_ESP_HOSTED_SPI AND CONFIG_ESP_HOSTED_SPI EQUAL 1))
if(NOT DEFINED CONFIG_HPM_SPI OR CONFIG_HPM_SPI EQUAL 0)
    message(FATAL_ERROR "Please enable SPI component in the middleware configuration")
endif()
if(NOT DEFINED CONFIG_DMA_MGR OR CONFIG_DMA_MGR EQUAL 0)
    message(FATAL_ERROR "Please enable DMA mgr component in the middleware configuration")
endif()
endif()

# common
add_subdirectory(common)

# host
add_subdirectory(host)

# esp_components
add_subdirectory(esp_components)

# port
add_subdirectory(porting)

