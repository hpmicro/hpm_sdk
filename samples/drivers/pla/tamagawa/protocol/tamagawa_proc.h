/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_TAMAGAWA_PROC_H
#define HPM_TAMAGAWA_PROC_H
#include "hpm_common.h"

/**
 * @brief Data ID code
 *
 */
typedef enum tmgw_cmd_type {
    tmgw_cmd_id0 = 0,
    tmgw_cmd_id1,
    tmgw_cmd_id2,
    tmgw_cmd_id3,
    tmgw_cmd_id7,
    tmgw_cmd_id8,
    tmgw_cmd_idc,
    tmgw_cmd_wr_eeprom,
    tmgw_cmd_rd_eeprom,
} tmgw_cmd_type_t;

#define TMGW_CMD_NUM         (9)
#define TMGW_CMD_SEND_FRAME_MAX_LEN  (8)
#define TMGW_CMD_RECV_FRAME_MAX_LEN  (16)

/**
 * @brief Protocol-related configuration information
 *
 */
typedef struct tmgw_cmd_cfg {
    uint16_t tx_num; /**< tx_num contain 1byte_crc */
    uint16_t rx_num;    /**< Receive data length */
    volatile uint16_t *send_buf;    /**< send buffer */
    volatile uint16_t *receive_buf; /**< receive buffer */
} tmgw_cmd_cfg_t;

/**
 * @brief Configuration information about Tamagawa encoders
 *
 */
typedef struct tmgw_dev_cfg {
    uint8_t run_cmd;    /**< @ref tmgw_cmd_type_t */
    uint32_t comm_time_delay;   /**< Real-time communication delays */
    uint32_t comm_time_delay_config;    /**< Communication delay compensation value */
    uint32_t comm_time_delay_init;  /**< Initial value of communication delay compensation */
    volatile uint32_t *hall_buf;    /**< Pointing to the communication delay time storage location */
    uint16_t copy_buf[TMGW_CMD_RECV_FRAME_MAX_LEN]; /**< Receive data copy area */
    tmgw_cmd_cfg_t *buf_cfg;    /**< @ref tmgw_cmd_cfg_t */
} tmgw_dev_cfg_t;

/**
 * @brief Valid information after protocol parsing
 *
 */
typedef struct tmgw_proc_message {
    uint32_t abs;   /**< Absolute position */
    uint32_t abm;   /**< Number of Laps */
    uint32_t id;    /**< id */
    uint8_t almc;   /**< almc */
    uint8_t encoder_error;  /**< encoder error and communivation error, ea0 ea1 ca0 ca1*/
    uint8_t eeprom_addr;    /**< eeprom address */
    uint8_t eeprom_data;    /**< eeprom data */
    uint8_t cmd;    /**< data id */
    bool is_eeprom_busy; /**< message by edf an adf bit, while this bit enable, eeprom operation is fail */
    bool is_completed;  /**< Complete a data communication */
    bool is_crc_error;  /**< crc orror */
    bool is_cmd_error;  /**< Inconsistency between requested and received instructions */
} tmgw_proc_message_t;

/**
 * @brief Send write to eeprom command
 *
 * @param[in] addr eeprom address
 * @param[in] data data
 * @param[inout] cfg @ref tmgw_cmd_cfg_t
 */
void tmgw_cmd_send_write_eeprom(uint8_t addr, uint8_t data, tmgw_cmd_cfg_t *cfg);

/**
 * @brief Send read eeprom command
 *
 * @param[in] addr eeprom address
 * @param[inout] cfg @ref tmgw_cmd_cfg_t
 */
void tmgw_cmd_send_read_eeprom(uint8_t addr, tmgw_cmd_cfg_t *cfg);

/**
 * @brief Initialization data
 *
 * @param[inout] cfg @ref tmgw_cmd_cfg_t
 * @return true Data initialisation successful
 * @return false Data initialisation failure
 */
bool tmgw_cmd_init_data(tmgw_cmd_cfg_t *cfg);

/**
 * @brief Initialisation of qei capture pulse data
 *
 * @param[out] qei_buf Qei Buffer
 * @param[in] cfg @ref tmgw_cmd_cfg_t
 */
void tmgw_cmd_qei_phcmp_init(uint32_t (*qei_buf)[2], tmgw_cmd_cfg_t *cfg);

/**
 * @brief crc checks
 *
 * @param[in] data buffer
 * @param[in] cfg @ref tmgw_cmd_cfg_t
 * @return uint8_t crc value
 */
uint8_t tmgw_cmd_crc8_calibration(uint16_t *data, tmgw_cmd_cfg_t *cfg);

/**
 * @brief Processing of incoming messages
 *
 * @param[inout] status Is the communication complete
 * @param[in] dev @ref tmgw_dev_cfg_t
 * @param[out] msg @ref tmgw_proc_message_t
 * @return true Protocol parsed correctly
 * @return false Protocol parsing error
 */
bool tmgw_message_process(volatile uint32_t *status, tmgw_dev_cfg_t *dev, tmgw_proc_message_t *msg);

#endif
