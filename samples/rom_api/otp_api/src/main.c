/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_romapi.h"


#define OTP_CHIP_UID_IDX_START  (8U)
#define OTP_CHIP_UID_IDX_END    (11U)

#define OTP_CHIP_UUID_IDX_START (88U)
#define OTP_CHIP_UUID_IDX_END   (91U)

#define OTP_SW_INFO_IDX_START   (72U)
#define OTP_SW_INFO_IDX_END     (79U)

void show_uuid(void);
void show_uid(void);

void program_sw_info(void);

int main(void)
{
    board_init();

    printf("--------------------------------------------------------------------------------\n");
    printf("This demo prints out the CHIP UID and UUID\n");
    printf("For OTP programming, please refer to program_sw_info() function\n");
    printf("Please make sure only the right OTP words will be programmed,\nor the chip may stop working\n");
    printf("---------------------------------------------------------------------------------\n");

    show_uid();
    show_uuid();

    while (1) {

    }



    return 0;
}


void show_uuid(void)
{
    uint32_t uuid_words[4];

    uint32_t word_idx = 0;
    for (uint32_t i = OTP_CHIP_UUID_IDX_START; i <= OTP_CHIP_UUID_IDX_END; i++) {
        uuid_words[word_idx++] = ROM_API_TABLE_ROOT->otp_driver_if->read_from_shadow(i);
    }

    printf("UUID: ");
    uint8_t *uuid_bytes = (uint8_t *)uuid_words;
    for (uint32_t i = 0; i < sizeof(uuid_words); i++) {
        printf("%02x ", uuid_bytes[i]);
    }
    printf("\n");
}

void show_uid(void)
{
    uint32_t uid_words[4];

    uint32_t word_idx = 0;
    for (uint32_t i = OTP_CHIP_UID_IDX_START; i <= OTP_CHIP_UID_IDX_END; i++) {
        uid_words[word_idx++] = ROM_API_TABLE_ROOT->otp_driver_if->read_from_shadow(i);
    }

    printf("UID:  ");
    uint8_t *uid_bytes = (uint8_t *)uid_words;
    for (uint32_t i = 0; i < sizeof(uid_words); i++) {
        printf("%02x ", uid_bytes[i]);
    }
    printf("\n");
}

void program_sw_info(void)
{


    /*****************************************************************************************
     *
     *  NOTE: Be aware that each OTP bit can only be programmed from 0 to 1,
     *       it can no longer be re-programmed to 0
     *
     *       Please refer to the below codes to program the corresponding OTP word
     *
     ******************************************************************************************/
    uint32_t otp_value = 0x12345768;
    ROM_API_TABLE_ROOT->otp_driver_if->program(OTP_SW_INFO_IDX_START, &otp_value, 1);
}