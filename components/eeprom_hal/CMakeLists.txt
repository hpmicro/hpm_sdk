# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if (CONFIG_EEPROM_HAL_I2C)
    set(CONFIG_EEPROM_HAL 1)
    set(CONFIG_EEPROM_DEVICE 1)
    set(CONFIG_EEPROM_DEVICE_I2C 1)
endif ()

if (CONFIG_EEPROM_HAL_SPI)
    set(CONFIG_EEPROM_HAL 1)
    set(CONFIG_EEPROM_DEVICE 1)
    set(CONFIG_EEPROM_DEVICE_SPI 1)
endif ()

if (CONFIG_EEPROM_HAL_EMULATION)
    set(CONFIG_EEPROM_HAL 1)
endif ()

if (CONFIG_EEPROM_HAL)
    sdk_inc(.)
    sdk_src(eeprom_hal.c)
    add_subdirectory_ifdef(CONFIG_EEPROM_DEVICE e2p_device)
    add_subdirectory_ifdef(CONFIG_EEPROM_EMULATION e2p_emulation)
endif ()


