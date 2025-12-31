
# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
add_subdirectory_ifdef(CONFIG_EEPROM_DEVICE_I2C i2c)
add_subdirectory_ifdef(CONFIG_EEPROM_DEVICE_SPI spi)
