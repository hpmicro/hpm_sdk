# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(freertos/inc)
sdk_inc(freertos/bt)
sdk_inc(freertos/bt/vhci)

if (CONFIG_ESP_HOSTED_SPI)
    sdk_src_glob(freertos/src/port_esp_hosted_host_spi.c)
endif()
if (CONFIG_ESP_HOSTED_SPI_HD)
    sdk_src_glob(freertos/src/port_esp_hosted_host_spi_hd.c)
endif()
sdk_src_glob(freertos/bt/*.c)
sdk_src_glob(freertos/src/port_esp_hosted_host_os.c)
sdk_src_glob(freertos/src/port_esp_hosted_host_transport_defaults.c)

sdk_compile_definitions(-DCONFIG_COMPILER_OPTIMIZATION_PERF=1)
