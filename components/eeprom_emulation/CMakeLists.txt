# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

set(EEPROM_USER_CONFIG_PATH ${APP_BIN_DIR}/..)
file(GLOB EEPROM_USER_CONFIG_EXIST "${EEPROM_USER_CONFIG_PATH}/user_config.h")

if(EEPROM_USER_CONFIG_EXIST)
    message(STATUS "eeprom emulation use custom config file")
else()
    sdk_inc(config)
    message(STATUS "eeprom emulation use default config file")
endif()


sdk_inc(.)
sdk_inc(port)
sdk_src(eeprom_emulation.c)
sdk_src(port/hpm_nor_flash.c)

