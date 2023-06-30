# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(DEFINED CONFIG_HPM_SPI_SDCARD)
sdk_src(hpm_spi_sdcard.c)
else()
sdk_src(hpm_sdmmc_host.c)
sdk_src(hpm_sdmmc_common.c)
sdk_src(hpm_sdmmc_sd.c)
sdk_src(hpm_sdmmc_emmc.c)
endif()