/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "board.h"
#include "hpm_owr_drv.h"

#define APP_OWR BOARD_OWR
#define DS18B20_SCRATCHPAD_MAX_COUNT (9U)
#define DS18B20_SCRATCHPAD_CONFIG_IDX (4U)
#define DS18B20_SCRATCHPAD_TEMP_LSB_IDX (0U)
#define DS18B20_SCRATCHPAD_TEMP_MSB_IDX (1U)
#define DS1B820_ROM_CODE_RES_COUNT (8U)

#define DS18B20_COMMAND_READ_ROM (0x33)
#define DS18B20_COMMAND_SKIP_ROM (0xcc)
#define DS18B20_COMMAND_READ_SCRATCHPAD (0xbe)
#define DS18B20_COMMAND_CONVERT_TEMP (0x44)

#define DS18B20_TEMP_RES_9BIT  (0.5f)
#define DS18B20_TEMP_RES_10BIT (0.25f)
#define DS18B20_TEMP_RES_11BIT (0.125f)
#define DS18B20_TEMP_RES_12BIT (0.0625f)

#pragma pack(1)
typedef struct {
  uint8_t family_code;
  uint8_t serial_num[6];
  uint8_t crc;
} ds18b20_read_rom_result_t;

typedef enum {
    ds18b20_thermometer_resolution_9bit = 0,
    ds18b20_thermometer_resolution_10bit,
    ds18b20_thermometer_resolution_11bit,
    ds18b20_thermometer_resolution_12bit
} ds18b20_thermometer_resolution_t;

hpm_stat_t ds18b20_skip_rom(void)
{
    /* skip ROM command */
    owr_write_data(APP_OWR, DS18B20_COMMAND_SKIP_ROM);

    return status_success;
}

hpm_stat_t ds18b20_read_rom(void)
{
    ds18b20_read_rom_result_t read_rom_result;
    uint8_t *p = (uint8_t *)&read_rom_result;

    /* read ROM command */
    owr_write_data(APP_OWR, DS18B20_COMMAND_READ_ROM);

    /* Read result */
    for (int i = 0; i < 8; i++) {
        /* read Data */
        owr_read_data(APP_OWR, (uint32_t *)p++);
    }

    printf("Family Code: %02x\n", read_rom_result.family_code);

    printf("Serial Num:");
    for (int i = 0; i < 6; i++) {
        printf("%02x ", read_rom_result.serial_num[i]);
    }
    printf("\n");

    printf("CRC: %02x\n", read_rom_result.crc);

    return status_success;
}

hpm_stat_t ds18b20_read_scratchpad(uint32_t *data)
{
    uint32_t *p = NULL;

    if (data == NULL) {
        return status_invalid_argument;
    }

    p = data;

    /* read ROM command */
    owr_write_data(APP_OWR, DS18B20_COMMAND_READ_SCRATCHPAD);

     /* read result */
    for (uint8_t i = 0; i < DS18B20_SCRATCHPAD_MAX_COUNT; i++) {
        /* read Data */
        owr_read_data(APP_OWR, p++);
    }

    return status_success;
}

hpm_stat_t ds18b20_read_config(uint32_t *data)
{
    uint32_t scratchpad[DS18B20_SCRATCHPAD_MAX_COUNT];
    uint8_t res;

    if (data == NULL) {
        return status_invalid_argument;
    }

    ds18b20_read_scratchpad(scratchpad);

    res = ((scratchpad[DS18B20_SCRATCHPAD_CONFIG_IDX] & (BIT6_MASK | BIT5_MASK)) >> 5);

    if (res == ds18b20_thermometer_resolution_12bit) {
        printf("Thermometer resolution: 12 bit\n");
    } else {
        printf("The current resolution is not the factory default value: %d!\n", res);
    }

    *data = res;

    return status_success;
}

hpm_stat_t ds18b20_calc_temp(int16_t data, uint8_t res, float *temp)
{
    float res_temp[] = {DS18B20_TEMP_RES_9BIT, DS18B20_TEMP_RES_10BIT, DS18B20_TEMP_RES_11BIT, DS18B20_TEMP_RES_12BIT};

    *temp = data * res_temp[res];

    printf("Current Temp: %.2f degree centigrade\n", *temp);

    return status_success;
}

hpm_stat_t ds18b20_read_temp(uint8_t *data)
{
    uint32_t scratchpad[DS18B20_SCRATCHPAD_MAX_COUNT];
    uint8_t temp_lsb, temp_msb;

    ds18b20_read_scratchpad(scratchpad);

    temp_lsb = scratchpad[DS18B20_SCRATCHPAD_TEMP_LSB_IDX];
    temp_msb = scratchpad[DS18B20_SCRATCHPAD_TEMP_MSB_IDX];

    *data++ = temp_lsb;
    *data   = temp_msb;

    return status_success;
}

hpm_stat_t ds18b20_convert_temp(void)
{
    uint32_t data;
    uint32_t timeout = 8; /* 800ms */

    /* convert temperature command */
    owr_write_data(APP_OWR, DS18B20_COMMAND_CONVERT_TEMP);

    while (timeout--) {
        board_delay_ms(100);
        owr_read_data(APP_OWR, &data);
        if (data) {
            break;
        }
    }

    if (timeout == 0) {
        return status_timeout;
    }

    return status_success;
}

int main(void)
{
    uint32_t status;
    owr_config_t config;
    uint32_t resolution;
    int16_t temp;
    float temp_value;

    board_init();

    printf("This is a onewire sample: DS18B20\n");

    /* clock initialization */
    board_init_owr_clock(APP_OWR);

    /* pin initialization */
    board_init_owr_pins(APP_OWR);

    /* software reset */
    owr_sw_reset(APP_OWR);

    /* set config */
    config.clock_source_frequency = BOARD_OWR_CLK;

    /* OWR initialization */
    owr_init(APP_OWR, &config);

    /* rpp */
    owr_reset_and_presence_pulses(APP_OWR);

    /* check if OWR slave is existent */
    owr_get_presence_status(APP_OWR, &status);

    if (status) {
        printf("OWR Slave Detected!\n");
    } else {
        printf("No OWR Slave!\n");
        while (1) {

        }
    }

    owr_reset_and_presence_pulses(APP_OWR);
    ds18b20_read_rom();

    owr_reset_and_presence_pulses(APP_OWR);
    ds18b20_skip_rom();
    ds18b20_read_config(&resolution);

    while (1) {
        owr_reset_and_presence_pulses(APP_OWR);
        ds18b20_skip_rom();
        ds18b20_convert_temp();
        owr_reset_and_presence_pulses(APP_OWR);
        ds18b20_skip_rom();
        ds18b20_read_temp((uint8_t *)&temp);
        ds18b20_calc_temp(temp, resolution, &temp_value);
    }
}
