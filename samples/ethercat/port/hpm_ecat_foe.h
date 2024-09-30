/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_ECAT_FOE_H
#define HPM_ECAT_FOE_H

#include "ecat_def.h"
#include "applInterface.h"
#include "ecat_foe_support.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief ESC foe read request
 *
 * @param [in] name: file name pointer
 * @param [in] name_size: file name size
 * @param [in] password: password
 * @param [in] max_block_size: read data length
 * @param [in] data: file data pointer
 * @return the read data length
 */
uint16_t foe_read(uint16_t MBXMEM * name, uint16_t name_size, uint32_t password, uint16_t max_block_size, uint16_t *data);

/**
 * @brief ESC foe read data
 *
 * @param [in] offset: file data offset
 * @param [in] max_block_size: read data length
 * @param [in] data: file data pointer
 * @return the read data length
 */
uint16_t foe_read_data(uint32_t offset, uint16_t max_block_size, uint16_t *data);


/**
 * @brief ESC foe write request
 *
 * @param [in] name: file name pointer
 * @param [in] name_size: file name size
 * @param [in] password: password
 * @return 0: success, other: error code
 */
uint16_t foe_write(uint16_t MBXMEM * name, uint16_t name_size, uint32_t password);

/**
 * @brief ESC foe write data
 *
 * @param [in] data: file data pointer
 * @param [in] size: write data size
 * @param [in] data_following: following data flag
 * @return 0: success, other: error code
 */
uint16_t foe_write_data(uint16_t MBXMEM * data, uint16_t size, uint8_t data_following);

#ifdef __cplusplus
}
#endif


#endif /* HPM_ECAT_FOE_H */
