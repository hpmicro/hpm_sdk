# Copyright (c) 2023-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_inc(port)
sdk_src(eeprom_emulation.c)
sdk_src(port/hpm_nor_flash.c)
