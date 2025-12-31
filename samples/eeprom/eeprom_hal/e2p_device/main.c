/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "eeprom_hal.h"
#include "e2p_device_i2c.h"

#define TEST_BUFFER_SIZE 256

#define RUN_TEST(test_func, test_name) \
    do { \
        printf("\n--- Running Test: %s ---\n", test_name); \
        if (test_func(device)) { \
            printf("--- [PASS] %s ---\n", test_name); \
        } else { \
            printf("--- [FAIL] %s ---\n", test_name); \
            all_tests_passed = false; \
        } \
    } while (0)

static void fill_random_data(uint8_t *buffer, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        buffer[i] = rand() % 256;
    }
}

static bool verify_data(hpm_eeprom_device_t *device, uint32_t addr, const uint8_t *expected_data, uint32_t len, const char *sub_test_name)
{
    uint8_t read_buf[TEST_BUFFER_SIZE];
    if (len > TEST_BUFFER_SIZE) {
        printf("ERROR: Verification length %u exceeds buffer size %d\n", len, TEST_BUFFER_SIZE);
        return false;
    }

    memset(read_buf, 0, len);
    hpm_stat_t status = hpm_eeprom_read_block(device, addr, len, read_buf);
    if (status != status_success) {
        printf("[FAIL] %s: Read operation failed.\n", sub_test_name);
        return false;
    }

    if (memcmp(expected_data, read_buf, len) != 0) {
        printf("[FAIL] %s: Data mismatch!\n", sub_test_name);
        return false;
    }

    printf("[PASS] %s: Data verified successfully.\n", sub_test_name);
    return true;
}

bool test_basic_io(hpm_eeprom_device_t *device)
{
    uint8_t write_buf[32];

    printf("Testing single byte write at a known-good address (e.g., 0x42)...\n");
    write_buf[0] = rand() % 256;
    hpm_stat_t status = hpm_eeprom_write_block(device, 0x42, 1, &write_buf[0]);
    if (status != status_success) {
        printf("[FAIL]: Write operation failed.\n");
        return false;
    }
    if (!verify_data(device, 0x42, &write_buf[0], 1, "Single Byte I/O")) {
        return false;
    }

    printf("Testing a small, non-aligned write...\n");
    fill_random_data(write_buf, 7);
    status = hpm_eeprom_write_block(device, 5, 7, write_buf);
    if (status != status_success) {
        printf("[FAIL]: Write operation failed.\n");
        return false;
    }
    if (!verify_data(device, 5, write_buf, 7, "Small Non-aligned Write")) {
        return false;
    }

    return true;
}

bool test_large_block_io(hpm_eeprom_device_t *device)
{
    uint8_t write_buf[TEST_BUFFER_SIZE];
    uint32_t write_len = 150;
    uint32_t test_addr = 10;

    fill_random_data(write_buf, write_len);
    printf("Writing a large block (%u bytes) from a non-aligned address (0x%X)...\n", write_len, test_addr);

    hpm_stat_t status = hpm_eeprom_write_block(device, test_addr, write_len, write_buf);
    if (status != status_success) {
        printf("[FAIL] Large block write operation failed.\n");
        return false;
    }

    return verify_data(device, test_addr, write_buf, write_len, "Large Block Verification");
}

int main(void)
{
    board_init();
    hpm_stat_t status;
    bool all_tests_passed = true;

    /* custom id should depend on hardware connect A2, A1, A0 */
    uint16_t custom_id = 0;
    uint32_t e2p_device_id = HPM_E2P_I2C_ADD_CUSTOM_ID_TO_DEV_ID(BOARD_EEPROM_ABSTRACT_ID, custom_id);
    e2p_device_i2c_attribute_t dev_attr;
    e2p_device_i2c_host_config_t host_info = {
        .base = BOARD_EEPROM_I2C_BASE,
        .clock_init = board_init_i2c_eeprom_clock,
        .pin_init = board_init_i2c_eeprom_pin,
    };

    hpm_eeprom_device_t e2p_temp = {
        .host = &host_info,
        .flag = EEPROM_FLAG_DEVICE_I2C,
        .config = &dev_attr,
    };

    status = e2p_device_i2c_register_param(&e2p_temp, &host_info, e2p_device_id);
    if (status != status_success) {
        printf("eeprom device get config failed, please check device if supported or not!\n");
        while (1) {
        }
    }

    printf("EEPROM HAL sample test start.\n");

    char device_name[] = {"e2p_test"};

    status = hpm_eeprom_register(&e2p_temp, device_name);
    if (status != status_success) {
        printf("eeprom device register failed!\n");
        while (1) {
        }
    }

    hpm_eeprom_device_t *device = hpm_eeprom_find(device_name);
    hpm_eeprom_init(device);

    RUN_TEST(test_basic_io, "Basic I/O (Small Writes)");
    RUN_TEST(test_large_block_io, "Large Block, Multi-Page, Non-Aligned I/O");

    if (all_tests_passed) {
        printf("Result: ALL TESTS PASSED!\n");
    } else {
        printf("Result: SOME TESTS FAILED!\n");
    }

    printf("EEPROM HAL sample test done.\n");

    while (1) {
    }
}
