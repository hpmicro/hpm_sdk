/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "hpm_ecat_foe.h"
#include "hpm_l1c_drv.h"

#define FILE_NAME_MAX_SIZE  (32)
#define PASSWORD            (0x87654321)

uint32_t write_file_offset;
uint32_t read_file_size;
uint8_t *read_file_ptr;

bool foe_data_verification_pass;

uint32_t (*foe_get_file_addr)(uint16_t MBXMEM * name, uint16_t name_size);
uint32_t (*foe_get_file_size)(uint16_t MBXMEM * name, uint16_t name_size);
int8_t (*foe_prepare_data_space)(uint16_t MBXMEM * name, uint16_t name_size);
int8_t (*foe_store_data)(uint32_t offset, uint16_t MBXMEM * data, uint16_t size, uint8_t data_following);

uint16_t foe_read(uint16_t MBXMEM * name, uint16_t name_size, uint32_t password, uint16_t max_block_size, uint16_t *data)
{
    uint16_t readsize = 0;
    char read_file_name[FILE_NAME_MAX_SIZE];

    printf("Read file start\n");

    if ((name_size + 1) > FILE_NAME_MAX_SIZE) {
        printf("Error: File name size too long\n");
        return ECAT_FOE_ERRCODE_DISKFULL;
    }

    if (password != PASSWORD) {
        printf("Error: Invalid password\n");
        return ECAT_FOE_ERRCODE_NORIGHTS;
    }

    memcpy(read_file_name, name, name_size);
    read_file_name[name_size] = '\0';

    if (strcmp("app", read_file_name) == 0) { /* judge file name */
        /* cache invalidate for file in flash */
        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)FOE_FILE_ADDR);
        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)FOE_FILE_TOTAL_SIZE);
        uint32_t aligned_size = aligned_end - aligned_start;
        l1c_dc_invalidate(aligned_start, aligned_size);

        if (foe_get_file_addr != NULL) {
            read_file_ptr = (uint8_t *)foe_get_file_addr(name, name_size);
        } else {
            return ECAT_FOE_ERRCODE_NOTDEFINED;
        }
        if (foe_get_file_size != NULL) {
            read_file_size = foe_get_file_size(name, name_size);
        } else {
            return ECAT_FOE_ERRCODE_NOTDEFINED;
        }
    } else {
        printf("Error: read file name not supported\n");
        return ECAT_FOE_ERRCODE_NOTFOUND;
    }

    if (read_file_size == 0) {
        printf("Error: read file size is zero\n");
        return ECAT_FOE_ERRCODE_NOTFOUND;
    }


    readsize = max_block_size;
    bool read_all_data_in_first_packet = false;
    if (read_file_size < readsize) {
        readsize = (uint16_t)read_file_size;
        read_all_data_in_first_packet = true;
    }
    memcpy((uint8_t *)data, read_file_ptr, readsize);

    if (read_all_data_in_first_packet) {
        printf("Read file finish in first packet\n");
    }

    return readsize;
}

uint16_t foe_read_data(uint32_t offset, uint16_t max_block_size, uint16_t *data)
{
    uint32_t readsize = 0;

    if (read_file_size < offset) {
        return 0;
    }

    readsize = read_file_size - offset;
    if (readsize > max_block_size) {
        readsize = max_block_size;
    } else {
        printf("Read file finish\n");
    }

    memcpy((uint8_t *)data, read_file_ptr + offset, readsize);
    return (uint16_t)readsize;
}

uint16_t foe_write(uint16_t MBXMEM * name, uint16_t name_size, uint32_t password)
{
    char write_file_name[FILE_NAME_MAX_SIZE];

    printf("Write file start\n");

    if ((name_size + 1) > FILE_NAME_MAX_SIZE) {
        printf("Error: File name size too long\n");
        return ECAT_FOE_ERRCODE_DISKFULL;
    }

    if (password != PASSWORD) {
        printf("Error: Password invalid\n");
        return ECAT_FOE_ERRCODE_NORIGHTS;
    }

    memcpy(write_file_name, name, name_size);
    write_file_name[name_size] = '\0';

    if (strcmp("app", write_file_name) == 0) {
        if (foe_prepare_data_space != NULL) {
            int ret = foe_prepare_data_space(name, name_size);
            if (ret < 0) {
                return ECAT_FOE_ERRCODE_ACCESS;
            }
        }
    } else {
        printf("Error: write file name not supported\n");
        return ECAT_FOE_ERRCODE_NOTFOUND;
    }

    write_file_offset = 0;
    return 0;
}

uint16_t foe_write_data(uint16_t MBXMEM * data, uint16_t size, uint8_t data_following)
{
    if ((write_file_offset + size) > FOE_FILE_MAX_SIZE) {
        printf("Error: file is too large\n");
        return ECAT_FOE_ERRCODE_DISKFULL;
    }

    /* write file to flash */
    if (foe_store_data != NULL) {
        /* 0: data store success; -1 : data store fail or verify fail; 1 : data verify success when all data stored */
        int ret = foe_store_data(write_file_offset, data, size, data_following);
        if (ret < 0) {
            printf("Error: file write fail or verify fail\n");
            return ECAT_FOE_ERRCODE_ACCESS;
        }

        if (ret == 1) {
            foe_data_verification_pass = true;
        }
    } else {
        return ECAT_FOE_ERRCODE_NOTDEFINED;
    }


    if (data_following) {
        write_file_offset += size;
    } else {
        printf("Write file finish\n");
        write_file_offset = 0;
    }

    return 0;
}

