/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_I2C_COMPONENTS_H
#define HPM_I2C_COMPONENTS_H

#include "hpm_i2c_drv.h"
#ifdef HPMSOC_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_misc.h"
#include "hpm_l1c_drv.h"

#ifndef USE_I2C_DMA_MGR
#define USE_I2C_DMA_MGR      (0U)
#endif

#if USE_I2C_DMA_MGR
#include "hpm_dma_mgr.h"
typedef void (*hpm_i2c_dma_complete_cb)(uint32_t channel);
#endif

/**
 * Define an enumeration type for I2C communication speeds
 *
 * The I2C protocol allows different communication speeds.
 * This enumeration defines the three most common speed options.
 */
typedef enum i2c_speed {
    i2c_speed_100khz,  /* normal mode, speed is 100kHz*/
    i2c_speed_400khz,  /* Fast mode, speed is 400kHz */
    i2c_speed_1Mhz,    /* Fast plus mode, speed is 1MHz */
} hpm_i2c_speed_t;

/**
 * Define the I2C communication mode enumeration type.
 *
 * I2C communication modes include master (i2c_master) and slave (i2c_slave).
 */
typedef enum i2c_communication_mode {
    i2c_master,
    i2c_slave,
} hpm_i2c_communication_mode_t;

typedef enum i2c_master_addr_endianness {
    i2c_master_addr_little_endian = 0,
    i2c_master_addr_big_endian,
} hpm_i2c_master_addr_endianness_t;

/**
 * Define the I2C initialization configuration structure
 * This structure configures the I2C communication mode, speed, addressing method, and slave address
 */
typedef struct i2c_initialize_config {
    hpm_i2c_communication_mode_t communication_mode; /* I2C communication mode */
    hpm_i2c_speed_t speed;                           /* I2C communication speed */
    bool is_10bit_addressing;                    /* Flag for 10-bit addressing mode */
    uint8_t slave_address;                       /* Slave address, only support slave mode */
} hpm_i2c_initialize_config_t;

typedef struct i2c_context_t {
    I2C_Type *base;
    hpm_i2c_initialize_config_t init_config;
    hpm_i2c_master_addr_endianness_t addr_endianness;
} hpm_i2c_context_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the default initialization context for I2C
 *
 * This function initializes the I2C context structure, setting the default address endianness to little-endian.
 *
 * @param context Pointer to the I2C context structure
 */
void hpm_i2c_get_default_init_context(hpm_i2c_context_t *context);

/**
 * @brief Initialize the I2C interface
 *
 * This function initializes the I2C module based on the provided configuration,
 * setting it up for communication. It configures the I2C mode, speed, addressing method,
 * and initializes the I2C device as either a master or slave.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t" which contains
 *                     initialization configurations and base address information.
 *
 * @retval hpm_stat_t Returns the initialization status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_initialize(hpm_i2c_context_t *context);

/**
 * @brief Probes an I2C slave address
 *
 * This function sends a probe signal over the I2C bus to check if a slave device at the specified address is present.
 * It is primarily used to verify that a slave device is correctly connected to the I2C bus without performing any data transfer.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address address of the slave device to probe
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_probe_slave_address(hpm_i2c_context_t *context, uint16_t device_address);

/**
 * @brief I2C master write data to a device's address over I2C in blocking mode
 *
 * This function writes data to a specific device over the I2C bus in a blocking manner. It requires the base address of the I2C peripheral,
 * the device's I2C address, the address within the device, the address size, the data buffer, and the buffer size. The function ensures
 * that the data transfer is completed within the specified timeout period.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address Address of the I2C slave device (7-bit or 10-bit address, depending on the hardware)
 * @param [in] addr The address within the device where data writing should start
 * @param [in] addr_size The size of the address, indicating the width of the device's internal address (in bytes)
 * @param [in] buf Pointer to the buffer containing the data to be written
 * @param [in] buf_size The size of the buffer, indicating the length of the data to be written (in bytes)
 * @param [in] timeout The timeout period, within which the function must complete the data transfer
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_addr_write_blocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size, uint32_t timeout);

/**
 * @brief I2C master read data from a specified address of an I2C slave device in blocking mode.
 *
 * This function reads data from a specified address of an I2C slave device using the I2C bus in a blocking manner.
 * It first sends the slave device address, then the memory address to read from, and finally reads the data.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address Address of the I2C slave device (7-bit or 10-bit address, depending on the hardware).
 * @param [in] addr Internal address within the slave device to read from (can be a register or memory address).
 * @param [in] addr_size Size of the address in bytes, indicating the number of bits in the address.
 * @param [in] buf Pointer to the buffer where the read data will be stored.
 * @param [in] buf_size Number of bytes to read.
 * @param [in] timeout Timeout value in milliseconds. If the read operation does not complete within this time, the function returns a timeout error.
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_addr_read_blocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size, uint32_t timeout);

/**
 * @brief I2C master write data to an I2C slave device in blocking mode
 *
 * This function performs a blocking write operation for an I2C master device,
 * writing a specified number of bytes to the I2C slave device.
 * Blocking mode means the function will return only after the data transfer is complete,
 * and it will occupy CPU resources during the data transfer.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address Address of the I2C slave device (7-bit or 10-bit address, depending on the hardware)
 * @param [in] buf Pointer to the buffer containing the data to be written, data type is uint8_t
 * @param [in] size Number of bytes to write, data type is uint32_t
 * @param [in] timeout Timeout for the operation, in milliseconds. If the operation does not complete within this time, the function will return an error
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_write_blocking(hpm_i2c_context_t *context, uint16_t device_address,
                            uint8_t *buf, uint32_t size, uint32_t timeout);

/**
 * @brief I2C master read data from an I2C device in blocking mode
 *
 * This function reads a specified number of bytes from the I2C device with the given address
 * and stores them in the provided buffer. The read operation is blocking, meaning it will wait
 * until the read is complete or an error occurs.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address Address of the I2C slave device (7-bit or 10-bit address, depending on the hardware)
 * @param [in] buf Pointer to the buffer where the read data will be stored
 * @param [in] size Number of bytes to read
 * @param [in] timeout Timeout for the operation in milliseconds
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_read_blocking(hpm_i2c_context_t *context, const uint16_t device_address,
                           uint8_t *buf, const uint32_t size, uint32_t timeout);

/**
 * @brief I2C slave write operation in blocking mode
 *
 * This function handles the write operation for an I2C slave in a blocking manner.
 * It waits until all data is successfully written or an error occurs, or the specified timeout is exceeded.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] buf Pointer to the buffer where received data will be stored
 * @param [in] size Number of bytes to receive
 * @param [in] timeout Timeout value for the operation; if the operation does not complete within this time, the function returns a timeout error
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_slave_write_blocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size, uint32_t timeout);

/**
 * @brief I2C slave read operation in blocking mode.
 *
 * This function reads data from the I2C slave device using the specified I2C interface in a blocking mode.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] buf Pointer to the buffer where the read data will be stored.
 * @param [in] size Number of bytes to read from the I2C slave device.
 * @param [in] timeout Timeout value in milliseconds for the read operation. If the read operation does not complete within this time, it will be terminated.
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_slave_read_blocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size, uint32_t timeout);

#if USE_I2C_DMA_MGR
/**
 * @brief Install I2C DMA transfer complete callback function
 *
 * This function is used to install a callback function for I2C DMA transfer in the HPM (High-Performance Module).
 * When the DMA transfer is complete, the callback function will be called.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] complete Pointer to the callback function that will be called when the I2C DMA transfer is complete.
 *
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_dma_mgr_install_callback(hpm_i2c_context_t *context, hpm_i2c_dma_complete_cb complete);

/**
 * @brief I2C master write data to a specified device address on the I2C bus in non-blocking mode.
 *
 * This function is used to write data to a slave device with a specific device address on the I2C bus in non-blocking mode.
 * It allows the user to specify the buffer containing the data to be written, the size of the buffer, and the device address.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address Address of the I2C slave device (7-bit or 10-bit address, depending on the hardware)
 * @param [in] addr The address within the device where data writing should start
 * @param [in] addr_size The size of the address, indicating the width of the device's internal address (in bytes)
 * @param [in] buf Pointer to the buffer containing the data to be written
 * @param [in] buf_size The size of the buffer, indicating the length of the data to be written (in bytes)
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_addr_write_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size);

/**
 * @brief I2C master read data from a specified address of an I2C slave device in non-blocking mode.
 *
 * This function reads data from a specified address of an I2C slave device using the I2C bus in a non-blocking manner.
 * It first sends the slave device address, then the memory address to read from, and finally reads the data.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address Address of the I2C slave device (7-bit or 10-bit address, depending on the hardware).
 * @param [in] addr Internal address within the slave device to read from (can be a register or memory address).
 * @param [in] addr_size Size of the address in bytes, indicating the number of bits in the address.
 * @param [in] buf Pointer to the buffer where the read data will be stored.
 * @param [in] buf_size Number of bytes to read.
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_addr_read_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size);

/**
 * @brief I2C master write data to an I2C slave device in non-blocking mode
 *
 * This function performs a non-blocking write operation for an I2C master device,
 * writing a specified number of bytes to the I2C slave device.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address Address of the I2C slave device (7-bit or 10-bit address, depending on the hardware)
 * @param [in] buf Pointer to the buffer containing the data to be written, data type is uint8_t
 * @param [in] size Number of bytes to write, data type is uint32_t
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_write_nonblocking(hpm_i2c_context_t *context, uint16_t device_address,
                            uint8_t *buf, uint32_t size);

/**
 * @brief I2C master read data from an I2C device in non-blocking mode
 *
 * This function reads a specified number of bytes from the I2C device with the given address
 * and stores them in the provided buffer.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] device_address Address of the I2C slave device (7-bit or 10-bit address, depending on the hardware)
 * @param [in] buf Pointer to the buffer where the read data will be stored
 * @param [in] size Number of bytes to read
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_master_read_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address,
                           uint8_t *buf, const uint32_t size);

/**
 * @brief I2C slave write operation in non-blocking mode
 *
 * This function handles the write operation for an I2C slave in a non-blocking manner
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] buf Pointer to the buffer where received data will be stored
 * @param [in] size Number of bytes to receive
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_slave_read_nonblocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size);

/**
 * @brief I2C slave read operation in non-blocking mode.
 *
 * This function reads data from the I2C slave device using the specified I2C interface in a non-blocking mode.
 *
 * @param [in] context A pointer to the struct of "hpm_i2c_context_t"
 * @param [in] buf Pointer to the buffer where the read data will be stored.
 * @param [in] size Number of bytes to read from the I2C slave device.
 * @retval hpm_stat_t Operation status, indicating success or error codes
 */
hpm_stat_t hpm_i2c_slave_write_nonblocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size);

/**
 * Get the DMA manager resource for the I2C module
 *
 * This function retrieves the DMA resource associated with the I2C module using the provided context.
 * It is primarily used to configure DMA for I2C communication, enhancing data transfer efficiency.
 *
 * @param context Pointer to the I2C context, containing the base configuration information of the I2C module.
 * @return A pointer to the DMA resource if the configuration object exists; otherwise, returns NULL.
 */
dma_resource_t *hpm_i2c_get_dma_mgr_resource(hpm_i2c_context_t *context);

#endif
#ifdef __cplusplus
}
#endif

#endif
