/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "eeprom_hal.h"
#include "e2p_emulation.h"

#define BOARD_APP_EEPROM_EMU_START_ADDR   BOARD_FLASH_SIZE - BOARD_APP_EEPROM_EMU_ERASE_SIZE * BOARD_APP_EEPROM_EMU_SECTOR_COUNT * 2
#define BOARD_APP_EEPROM_EMU_ERASE_SIZE   4096
#define BOARD_APP_EEPROM_EMU_SECTOR_COUNT 32

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
    hpm_eeprom_write_block(device, 0x42, 1, &write_buf[0]);
    if (!verify_data(device, 0x42, &write_buf[0], 1, "Single Byte I/O")) {
        return false;
    }

    printf("Testing a small, non-aligned write...\n");
    fill_random_data(write_buf, 7);
    hpm_eeprom_write_block(device, 5, 7, write_buf);
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

    e2p_emu_xpi_host_config_t emu_host = {
        .base = BOARD_APP_XPI_NOR_XPI_BASE,
    };

    e2p_emu_xpi_attribute_config_t emu_config = {
        .flash_base_addr = BOARD_FLASH_BASE_ADDRESS,
        .version = 0x4553,
        .opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR,
        .opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0,
        .opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1,
        .start_addr = BOARD_APP_EEPROM_EMU_START_ADDR,
        .erase_size = BOARD_APP_EEPROM_EMU_ERASE_SIZE,
        .sector_cnt = BOARD_APP_EEPROM_EMU_SECTOR_COUNT,
    };

    hpm_eeprom_device_t e2p_temp = {
        .host = &emu_host,
        .flag = EEPROM_FLAG_EMULATION_XPI,
        .config = &emu_config,
    };

    status = e2p_emulation_xpi_register_param(&e2p_temp);
    if (status != status_success) {
        printf("eeprom emulation get config failed!\n");
        return 0;
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
