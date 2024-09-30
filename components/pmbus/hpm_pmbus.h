/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_PMBUS_H
#define HPM_PMBUS_H

#include "hpm_common.h"
#include "hpm_soc_feature.h"
#include "hpm_i2c_drv.h"
#include "hpm_pmbus_def.h"

/**
 * @brief PMBUS Complete Callback Configuration Structure
 *
 * This structure is used to configure the parameters for a PMBUS command completion callback,
 * including the command type, data length, read/write flag, PEC check flag, and data pointer.
 */
typedef struct {
    uint8_t command; /**< The PMBUS command type */
    uint32_t len;    /**< The length of the transferred data */
    bool read;       /**< Whether the operation is a read */
    bool pec_check;  /**< Whether PEC check is enabled */
    uint8_t *data;   /**< Pointer to the data buffer */
} hpm_pmbus_complete_cb_cfg_t;

/**
 * @brief Define a callback function type hpm_pmbus_complete_cb_t
 *
 * This callback function is called after an I2C operation completes, allowing users to implement specific completion handling logic.
 */
typedef void (*hpm_pmbus_complete_cb_t)(I2C_Type *base, hpm_pmbus_complete_cb_cfg_t *cfg);

/**
 * @brief Define the HPM PMBUS transaction type enumeration, used to distinguish between different PMBUS operation types.
 */
typedef enum {
    none            = 0,    /**< No operation */
    write_byte      = 1,    /**< Write a byte */
    read_byte       = 2,    /**< Read a byte */
    write_word      = 3,    /**< Write a word (two bytes) */
    read_word       = 4,    /**< Read a word (two bytes)  */
    write_block     = 5,    /**< Write a block */
    read_block      = 6,    /**< Read a block */
    send_byte       = 7,    /**< Send a byte */
    mfr_defined     = 8,    /**< Manufacturer-defined command */
    extended_command = 9,   /**< Extended command */
} hpm_pmbus_transaction_type;

/**
 * @brief Defines the PMBus command parameter structure.
 *
 * This structure encapsulates all parameters for a PMBus command, including the command code,
 * read/write transaction types, and data length. It provides a uniform way to communicate with
 * PMBus devices.
 *
 */
typedef struct {
     /**< The command code, used to identify specific PMBus commands. */
    uint8_t command_code;
     /**< The write transaction type, indicating the type of write operation. */
    hpm_pmbus_transaction_type write_transaction_type;
    /**< The read transaction type, indicating the type of read operation. */
    hpm_pmbus_transaction_type read_transaction_type;
    /**< The data length, indicating the amount of data associated with the command. */
    uint32_t data_length;
} hpm_pmbus_cmd_param_t;

/**
 * @brief  Define the HPM PMBUS Slave Configuration Structure
 *
 * This structure stores configuration information related to an HPM PMBUS slave
 *
 */
typedef struct {
    bool is_valid;        /**< A flag indicating whether the slave configuration is valid */
    uint8_t *wdata;       /**< Pointer to write data, used for writing data to the slave */
    uint8_t *rdata;       /**< Pointer to read data, used for reading data from the slave */
    uint32_t data_length; /**< Data length, representing the number of bytes to be written or read */
    hpm_pmbus_complete_cb_t callback; /**< Callback function pointer, called when data transfer completes */
} hpm_pmbus_slave_cfg_t;

/**
 * @brief PMBUS status
 */
enum {
    status_pmbus_not_support_cmd = MAKE_STATUS(status_group_pmbus, 1),
    status_pmbus_not_transaction_type = MAKE_STATUS(status_group_pmbus, 2),
};

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Handles the HPM PMBUS interrupt service request.
 *
 * This function serves as an interrupt service routine (ISR) handler for communication with PMBUS devices over I2C.
 *
 * @param ptr A pointer to the I2C
 *
 */
void hpm_pmbus_isr_handler(I2C_Type *ptr);

/**
 * @brief PMBus Slave Initialization
 *
 * This function initializes an I2C slave device to respond to a specific slave address and communicate according to the PMBus protocol.
 *
 * @param ptr Pointer to the base address of the I2C, pointing to the I2C slave device to be initialized
 * @param slave_addr Address of the slave device, used to identify the specific slave on the I2C bus
 *
 * @return hpm_stat_t Returns the status of the initialization operation, indicating success or failure reasons
 */
hpm_stat_t hpm_pmbus_slave_init(I2C_Type *ptr, uint8_t slave_addr);

/**
 * @brief Reads data from a PMBus slave device
 *
 * This function reads data from a specified PMBus slave device over the I2C bus. It first sends a command and then reads the response data.
 * Primarily used for communication with PMBus-compatible power management devices to retrieve device status or measurement data.
 *
 * @param ptr Pointer to the base address of the I2C controller
 * @param slave_address The 7-bit PMBus address of the slave device, used to select the correct slave device
 * @param command The PMBus command sent to the slave device, specifying the type of data or register to read
 * @param data Pointer to store the data returned from the slave device; this function will store the read data into the buffer pointed to by this pointer
 * @param len Pointer to a variable that specifies the maximum read length; this function will write the actual read length into this variable
 *
 * @return hpm_stat_t Returns the status of the read operation, indicating success or various error states
 */
hpm_stat_t hpm_pmbus_master_read(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t *len);

/**
 * @brief Writes data to a PMBus slave device
 *
 * function is used to send commands and data to a slave device over the I2C interface.
 * It is primarily intended for communication with PMBus-compatible devices, allowing the host
 * to send instructions and data to PMBus-compatible devices.
 *
 * @param ptr Pointer to the base address of the I2C controller
 * @param slave_address The 7-bit address of the slave device, used to address the slave device on the I2C bus
 * @param command The command or register address to be sent to the slave device
 * @param data Pointer to the data to be written to the slave device; the data will be written in order
 * @param len The length of the data to be written, in bytes
 *
 * @return Returns the result of the write operation: HPM_STAT_SUCCESS indicates success, other values indicate different error states
 *
 * Note: Before using this function, ensure that the I2C controller has been properly initialized, and the clock and interrupts (if needed) have also been configured.
 */
hpm_stat_t hpm_pmbus_master_write(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t len);

/**
 * @brief Installs a PMBus Slave Command Transaction
 *
 * This function installs a PMBus slave command transaction for writing and reading data.
 * The command is for the master, such as the read command, which is read for the master and write for the slave
 *
 * @param ptr Pointer to the base address of the I2C
 * @param command PMBus command byte specifying the PMBus command to be sen
 * @param wdata Pointer to the data to be written, containing the data to write to the slave
 * @param rdata Pointer to the buffer where the data read from the slave will be stored
 * @param len Length of the data to be transferred, in bytes
 * @param callback Pointer to the completion callback function that will be called when the transaction completes
 * @return hpm_stat_t Returns the installation status of the transaction, indicating success or the reason for failure
 */
hpm_stat_t hpm_pmbus_slave_command_transaction_install(I2C_Type *ptr, uint8_t command, uint8_t *wdata, uint8_t *rdata, uint32_t len, hpm_pmbus_complete_cb_t callback);

/**
 * @brief Uninstalls a specified PMBus slave command transaction.
 *
 * This function is used to uninstall a PMBus slave command transaction to reduce unnecessary resource usage.
 * In certain cases, if a specific slave command transaction is no longer needed, it can be uninstalled using this
 * function to free up related resources.
 *
 * @param ptr Pointer to the I2C
 * @param command The PMBus command to uninstall. This parameter specifies the exact command to be uninstalled.
 * @return hpm_stat_t Returns the operation status, indicating whether the uninstallation was successful.
 */
hpm_stat_t hpm_pmbus_slave_command_transaction_uninstall(I2C_Type *ptr, uint8_t command);

#ifdef __cplusplus
}
#endif

#endif
