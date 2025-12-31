/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _EEPROM_DEVICE_H
#define _EEPROM_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief eeprom device flag definition
 */
#define EEPROM_FLAG_EMULATION_XPI 0x0
#define EEPROM_FLAG_DEVICE_I2C    0x4
#define EEPROM_FLAG_DEVICE_SPI    0x5

/**
 * @brief List of supported I2C EEPROM Devices
 */
#define E2P_DEVICE_AT24C01 0
#define E2P_DEVICE_AT24C02 0
#define E2P_DEVICE_24xx01 0
#define E2P_DEVICE_24xx02 0
#define E2P_DEVICE_NV24C02 1
/* NV24C04 use 9 bits address but the a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_NV24C04 2
/* NV24C08 use 10 bits address but the a9, a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_NV24C08 3
/* NV24C16 use 11 bits address but the a10, a9, a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_NV24C16 4
#define E2P_DEVICE_M24C01 5
#define E2P_DEVICE_M24C02 5
/* M24C04 use 9 bits address but the a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_M24C04 6
/* 24x04 use 9 bits address but the a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_24xx04 6
/* AT24C04 use 9 bits address but the a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_AT24C04 6
/* M24C08 use 10 bits address but the a9 a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_M24C08 7
/* 24xx08 use 10 bits address but the a9, a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_24xx08 7
/* AT24C08 use 10 bits address but the a9, a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_AT24C08 7
/* M24C16 use 11 bits address but the a10, a9, a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_M24C16 8
/* 24xx16 use 11 bits address but the a10, a9, a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_24xx16 8
/* AT24C16 use 11 bits address but the a10, a9, a8 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_AT24C16 8
#define E2P_DEVICE_NV24C32 9
#define E2P_DEVICE_AT24C32 10
#define E2P_DEVICE_AT24C64 10
#define E2P_DEVICE_M24C32 11
#define E2P_DEVICE_24xxC64 11
#define E2P_DEVICE_NV24C64 11
#define E2P_DEVICE_M24128 12
#define E2P_DEVICE_M24256 12
#define E2P_DEVICE_AT24C128 12
#define E2P_DEVICE_AT24C256 12
#define E2P_DEVICE_24xx128 12
#define E2P_DEVICE_24xx256 12
#define E2P_DEVICE_NV24C128 12
#define E2P_DEVICE_NV24C256 12
#define E2P_DEVICE_AT24C512 13
#define E2P_DEVICE_M24512 13
#define E2P_DEVICE_24xx512 13
/* AT24CM01 use 17 bits address but the a16 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_AT24CM01 14
/* M24M01 use 17 bits address but the a16 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_M24M01 14
/* AT24CM02 use 18 bits address but the a17, a16 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_AT24CM02 15
/* M24M02 use 18 bits address but the a17, a16 will set at device address bits, check datasheet for details */
#define E2P_DEVICE_M24M02 15

/**
 * @brief List of supported SPI EEPROM Devices
 */
#define E2P_DEVICE_AT25010B 0
#define E2P_DEVICE_AT25020B 0
/* AT25040B use 9 bits address but the MSB transmitted on instruction sequcence, check datasheet for details */
#define E2P_DEVICE_AT25040B 1
#define E2P_DEVICE_M95010 2
#define E2P_DEVICE_M95020 2
/* M95040 use 9 bits address but the MSB transmitted on instruction sequcence, check datasheet for details */
#define E2P_DEVICE_M95040 3
#define E2P_DEVICE_S_25C010A 4
#define E2P_DEVICE_S_25C020A 4
/* S-25C040A use 9 bits address but the MSB transmitted on instruction sequcence, check datasheet for details */
#define E2P_DEVICE_S_25C040A 5
#define E2P_DEVICE_25xx010A 6
#define E2P_DEVICE_25xx020A 6
#define E2P_DEVICE_NV25010A 6
#define E2P_DEVICE_NV25020A 6
/* 25xx040A use 9 bits address but the MSB transmitted on instruction sequcence, check datasheet for details */
#define E2P_DEVICE_25xx040A 7
/* NV25040 use 9 bits address but the MSB transmitted on instruction sequcence, check datasheet for details */
#define E2P_DEVICE_NV25040A 7
#define E2P_DEVICE_25xx080A 8
#define E2P_DEVICE_25xx160A 8
#define E2P_DEVICE_M95080 9
#define E2P_DEVICE_M95160 9
#define E2P_DEVICE_M95320 9
#define E2P_DEVICE_M95640 9
#define E2P_DEVICE_AT25080B 9
#define E2P_DEVICE_AT25160B 9
#define E2P_DEVICE_AT25320B 9
#define E2P_DEVICE_AT25640B 9
#define E2P_DEVICE_S_25C080A 10
#define E2P_DEVICE_25xx080B 11
#define E2P_DEVICE_25xx160B 11
#define E2P_DEVICE_25xx320A 11
#define E2P_DEVICE_25xx640A 11
#define E2P_DEVICE_NV25080 11
#define E2P_DEVICE_NV25160 11
#define E2P_DEVICE_NV25320 11
#define E2P_DEVICE_NV25640 11
#define E2P_DEVICE_S_25C160A 12
#define E2P_DEVICE_S_25C320A 12
#define E2P_DEVICE_S_25C640A 12
#define E2P_DEVICE_S_25C128A 13
#define E2P_DEVICE_25xx128 14
#define E2P_DEVICE_25xx256 14
#define E2P_DEVICE_NV25128 14
#define E2P_DEVICE_NV25256 14
#define E2P_DEVICE_S_25C256A 14
#define E2P_DEVICE_AT25128B 15
#define E2P_DEVICE_AT25256B 15
#define E2P_DEVICE_M95128 15
#define E2P_DEVICE_M95256 15
#define E2P_DEVICE_S_25C512A 16
#define E2P_DEVICE_NV25512 16
#define E2P_DEVICE_M95512 17
#define E2P_DEVICE_25x512 18
#define E2P_DEVICE_M95M01 19
#define E2P_DEVICE_NV25M01 20
#define E2P_DEVICE_S_25CM01A 20
#define E2P_DEVICE_M95M02 21
#define E2P_DEVICE_25xx1024 22
#define E2P_DEVICE_M95M04 23

#define ARG_UNUSED(x) (void)(x)

/* eeprom device name max len */
#define E2P_DEVICE_NAME_MAX_LEN 20

/**
 * @brief eeprom device structure
 */
typedef struct {
    uint32_t flag;                   /* eeprom device flag */
    char device_name[E2P_DEVICE_NAME_MAX_LEN];  /* eeprom device name */
    void *host;                     /* Pointer to eeprom device host configuration structure */
    void *private;                 /* reserved */
    void *config;               /* Pointer to eeprom device attribute structure */
    void *ops;                   /* Pointer to eeprom device operations structure */
} hpm_eeprom_device_t;

/**
 * @brief eeprom driver operations structure
 */
typedef struct {
    hpm_stat_t (*init)(hpm_eeprom_device_t *device); /* eeprom init function */
    hpm_stat_t (*deinit)(hpm_eeprom_device_t *device); /* eeprom deinit function */
    hpm_stat_t (*write)(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data); /* eeprom write function */
    hpm_stat_t (*read)(hpm_eeprom_device_t *device, uint32_t data_addr, uint32_t len, uint8_t *data); /* eeprom read function */
} e2p_device_ops_t;

#ifdef __cplusplus
}
#endif

#endif