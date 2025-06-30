.. _i2c_component:

I2C Component
=============

Overview
--------

- The I2C component provides a higher-level API compared to the driver layer for configuring, initializing, and operating the I2C bus. It supports multiple I2C devices and offers both blocking and non-blocking data transfer functionalities. Below are the primary features of this component:

  - Multi-instance support
    - Supports multiple I2C instances (such as I2C0, I2C1, etc.), each with independent configuration parameters.
    - Manages configuration information for each I2C instance through the `hpm_i2c_context_t` structure, including address mode, speed mode, master/slave mode.

  - Blocking and non-blocking read/write operation interfaces
    - Supports both master and slave modes.
    - Provides non-blocking read/write functions.
    - Provides blocking read/write functions with timeout mechanisms to ensure timely error status returns in case of prolonged unresponsiveness.

Introduction to Relevant Enumerations and Structures
----------------------------------------------------

- Enumerations Introduction
  - I2C speed enumeration type, this enumeration defines three different I2C communication speed options: standard mode (100kHz), fast mode (400kHz), and fast plus mode (1MHz).

  .. code-block:: c

    typedef enum i2c_speed {
        i2c_speed_100khz,  /* Standard mode (100kHz) */
        i2c_speed_400khz,  /* Fast mode (400kHz) */
        i2c_speed_1Mhz,    /* Fast plus mode (1MHz) */
    } hpm_i2c_speed_t;

  - I2C communication mode enumeration type, defining two communication modes: master mode and slave mode.

  .. code-block:: c

    typedef enum i2c_communication_mode {
        i2c_master,
        i2c_slave,
    } hpm_i2c_communication_mode_t;

  - Address byte order enumeration type, this enumeration defines two different register address byte order options under master mode: little-endian and big-endian.

  .. code-block:: c

    typedef enum i2c_master_addr_endianness {
        i2c_master_addr_little_endian = 0,  /* Little endian */
        i2c_master_addr_big_endian,         /* Big endian */
    } hpm_i2c_master_addr_endianness_t;

- Structures Introduction

  - Initialization configuration structure, containing information such as communication mode, speed, whether to use 10-bit addressing, and slave device address.

  .. code-block:: c

    typedef struct i2c_initialize_config {
        hpm_i2c_communication_mode_t communication_mode; /* I2C communication mode */
        hpm_i2c_speed_t speed;                           /* I2C communication speed */
        bool is_10bit_addressing;                        /* Whether to use 10-bit addressing */
        uint8_t slave_address;                           /* Slave device address, only supported in slave mode */
    } hpm_i2c_initialize_config_t;

  - I2C context structure, containing members such as the I2C instance handle and initialization configuration. This structure needs to be defined in applications to save the I2C instance handle and initialization configuration.

  .. code-block:: c

    typedef struct i2c_context_t {
        I2C_Type *base;                                  /* Pointer to the I2C base register */
        hpm_i2c_initialize_config_t init_config;        /* Initialization configuration */
        hpm_i2c_master_addr_endianness_t addr_endianness;/* Master address endianness */
    } hpm_i2c_context_t;

API Call Flow Introduction
--------------------------

Defining an I2C Instance Variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Define an instance of the `hpm_i2c_context_t` structure. For example, use the `i2c_context` instance variable in the sample.

Default I2C Instance Variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Use the `hpm_i2c_get_default_init_context` API to initialize the `i2c_context` instance variable by assigning it to the `context` parameter of the API.

  - Prototype of `hpm_i2c_get_default_init_context` API:

  .. code-block:: c

    void hpm_i2c_get_default_init_context(hpm_i2c_context_t *context);

  - This function sets each member to default values:
    - 7-bit address mode
    - Master mode
    - 100kHz speed
    - Little-endian for address register transmission

I2C Initialization
~~~~~~~~~~~~~~~~~~

- Call the `hpm_i2c_initialize` API to initialize the I2C. During this process, the parameters of the `i2c_context` instance variable are assigned to the `context` and take effect.

- Prototype of `hpm_i2c_initialize` API:

  .. code-block:: c

    void hpm_i2c_initialize(hpm_i2c_context_t *context);

- **Example**: Instantiate I2C0, set I2C0 to master mode, 1MHz speed, 7-bit address mode, and use big-endian for address register transmission.

  .. code-block:: c

    i2c_context.init_config.speed = i2c_speed_1Mhz;
    i2c_context.init_config.communication_mode = i2c_master;
    i2c_context.init_config.is_10bit_addressing = false;
    i2c_context.base = HPM_I2C0;
    i2c_context.addr_endianness = i2c_master_addr_big_endian;
    hpm_i2c_initialize(&i2c_context);

- **Example**: Instantiate I2C0, set I2C0 to slave mode, 1MHz speed, 10-bit address mode, with a slave device address of 0x16.

  .. code-block:: c

    i2c_context.init_config.speed = i2c_speed_1Mhz;
    i2c_context.init_config.communication_mode = i2c_slave;
    i2c_context.init_config.is_10bit_addressing = true;
    i2c_context.init_config.slave_address = 0x16;
    i2c_context.base = HPM_I2C0;
    hpm_i2c_initialize(&i2c_context);

DMA Configuration
~~~~~~~~~~~~~~~~~

- If using non-blocking read/write interfaces, this step is required.
- Use the `hpm_i2c_dma_mgr_install_callback` API to initialize DMA and register a callback function. When using non-blocking APIs for data transmission, this function must be called. If no callback function is needed, NULL can be passed as the callback argument.
- The main function of this function is to install the user-provided DMA transfer completion callback into the given I2C context. This ensures that after the DMA transfer completes, the system automatically calls the user's callback function for subsequent processing.

- Prototype of `hpm_i2c_dma_mgr_install_callback` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_dma_mgr_install_callback(hpm_i2c_context_t *context, hpm_i2c_dma_complete_cb complete)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `complete`
          - `hpm_i2c_dma_complete_cb`
          - Pointer to the callback function for DMA transfer completion. Called when the DMA transfer ends.

  - Return Values:

    - `status_success`: Success

    - `status_invalid_argument`: Invalid argument

    - `status_fail`: Other errors

- **Example**: How to use the `hpm_i2c_dma_mgr_install_callback` function to register a callback function

  .. code-block:: c

    /* Define the callback function */
    void my_dma_complete_callback(uint32_t channel) {
        printf("DMA transfer on channel %d completed.\n", channel);
    }
    int main(void) {
        hpm_i2c_context_t i2c_context;
        /* Initialize I2C context... Not listed here */

        /* Register DMA transfer completion callback */
        hpm_stat_t result = hpm_i2c_dma_install_callback(&i2c_context, my_dma_complete_callback);

        if (result == status_success) {
            printf("Callback installed successfully.\n");
            /* Continue with DMA setup and start transmission... */
        } else {
            printf("Failed to install callback. Error code: %d\n", result);
        }

        /* TODO */
    }

Read and Write Operations
~~~~~~~~~~~~~~~~~~~~~~~~~

Blocking Read and Write Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Master Mode, Divided into Direct Read/Write Operations and Address Register Read/Write Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **Direct Read/Write Operations**: Used for blocking read/write operations in I2C master mode. It will wait until data transmission is completed or a timeout occurs.
- These operations are divided into `hpm_i2c_master_write_blocking` and `hpm_i2c_master_read_blocking` APIs.

  - Prototype of `hpm_i2c_master_write_blocking` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_master_write_blocking(hpm_i2c_context_t *context, uint16_t device_address, uint8_t *buf, uint32_t size, uint32_t timeout)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `device_address`
          - `uint16_t`
          - Address of the target slave device to which data will be written. This value can be 7-bit or 10-bit based on the `is_10bit_addressing` flag in initialization configuration
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer containing data to be sent
        * - `size`
          - `uint32_t`
          - Number of bytes to send from the buffer
        * - `timeout`
          - `uint32_t`
          - Timeout in milliseconds. If the operation does not complete within this time, the function will return a timeout error

  - Return Values:

    - `status_success`: If the write operation completes successfully.

    - `status_invalid_argument`: If the provided `size` exceeds the maximum transfer count supported by the hardware.

    - `status_timeout`: If the operation does not complete within the specified `timeout`.

    - `status_i2c_no_addr_hit`: If no device responds at the target address.

    - `status_i2c_transmit_not_completed`: If the data counter does not match expectations before the transmission ends.

  - **Example**: An I2C master device sending some data to a slave device via the I2C bus:

  .. code-block:: c

    /* Define the I2C context and device address */
    hpm_i2c_context_t i2c_context;
    const uint16_t device_address = 0x3C; /* Example slave device address */
    /* Data to be sent and its size */
    uint8_t data_to_send[] = {0x01, 0x02, 0x03}; /* Example data */
    uint32_t size = sizeof(data_to_send); // Data size
    uint32_t timeout = 1000; /* Timeout set to 1 second */
    /* Initialize I2C context... Not listed here */
    /* Use the hpm_i2c_master_write_blocking function to read data from the device */
    hpm_stat_t result = hpm_i2c_master_write_blocking(
        &i2c_context,
        device_address,
        data_to_send,
        size,
        timeout
    );
    if (result == status_success) {
        /* Write successful */
        printf("Data successfully written to I2C slave.\n");
    } else {
        /* Write failed, handle error case */
        printf("Failed to write data to I2C slave. Error code: %d\n", result);
    }

  - Prototype of `hpm_i2c_master_read_blocking` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_master_read_blocking(hpm_i2c_context_t *context, uint16_t device_address, uint8_t *buf, uint32_t size, uint32_t timeout)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `device_address`
          - `uint16_t`
          - Address of the target slave device from which data will be read. This value can be 7-bit or 10-bit based on the `is_10bit_addressing` flag in initialization configuration
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer where received data will be stored
        * - `size`
          - `uint32_t`
          - Expected number of bytes to receive in the buffer
        * - `timeout`
          - `uint32_t`
          - Timeout in milliseconds. If the operation does not complete within this time, the function will return a timeout error

  - Return Values:

    - `status_success`: If the read operation completes successfully.

    - `status_invalid_argument`: If the provided `size` exceeds the maximum transfer count supported by the hardware.

    - `status_timeout`: If the operation does not complete within the specified `timeout`.

    - `status_i2c_no_addr_hit`: If no device responds at the target address.

    - `status_i2c_transmit_not_completed`: If the data counter does not match expectations before the transmission ends.

  - **Example**: An I2C master device reading some data from a slave device via the I2C bus:

  .. code-block:: c

    /* Define the I2C context and device address */
    hpm_i2c_context_t i2c_context;
    const uint16_t device_address = 0x3C; /* Example slave device address */
    /* Prepare the receive buffer and its size */
    uint8_t received_data[10]; /* Receive buffer */
    uint32_t size = sizeof(received_data); /* Buffer size */
    uint32_t timeout = 1000; /* Timeout set to 1 second */
    /* Initialize I2C context... Not listed here */
    /* Use the hpm_i2c_master_read_blocking function to read data from the device */
    hpm_stat_t result = hpm_i2c_master_read_blocking(
        &i2c_context,
        device_address,
        received_data,
        size,
        timeout
    );

    if (result == status_success) {
        /* Read successful */
        printf("Data successfully read from I2C slave.\n");
        /* Process received_data here */
    } else {
        /* Read failed, handle error case */
        printf("Failed to read data from I2C slave. Error code: %d\n", result);
    }

- **Address Register Read/Write Operations**: Used for blocking operations in I2C master mode to write addresses and data to slave devices. It will wait until both address and data transmissions are completed or a timeout occurs.
- These operations are divided into `hpm_i2c_master_addr_write_blocking` and `hpm_i2c_master_addr_read_blocking` APIs.

  - Prototype of `hpm_i2c_master_addr_write_blocking` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_master_addr_write_blocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size, uint8_t *buf, uint32_t buf_size, uint32_t timeout)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `device_address`
          - `uint16_t`
          - Address of the target slave device to which data will be written. This value can be 7-bit or 10-bit based on the `is_10bit_addressing` flag in initialization configuration
        * - `addr`
          - `uint32_t`
          - Internal register or memory address of the slave device where the write operation will take place
        * - `addr_size`
          - `uint8_t`
          - Size of the address `addr` in bytes, typically 1 to 4 bytes
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer containing data to be sent
        * - `buf_size`
          - `uint32_t`
          - Number of bytes to send from the buffer
        * - `timeout`
          - `uint32_t`
          - Timeout in milliseconds. If the operation does not complete within this time, the function will return a timeout error

  - Return Values:

    - `status_success`: If the write operation completes successfully.

    - `status_invalid_argument`: If the provided `addr_size` or `buf_size` do not meet requirements, or if the total data length exceeds the maximum transfer count supported by the hardware.

    - `status_timeout`: If the operation does not complete within the specified `timeout`.

    - `status_i2c_no_addr_hit`: If no device responds at the target address.

  - **Example**: An I2C master device writing some data to a specific register of a slave device via the I2C bus:

  .. code-block:: c

    const uint16_t device_address = 0x3C; /* Example slave device address */
    /* Data to be sent and its size */
    uint8_t data_to_send[] = {0x01, 0x02, 0x03}; /* Example data */
    uint32_t addr = 0x00; /* Target register address */
    uint8_t addr_size = 1; /* Register address size is 1 byte */
    uint32_t size = sizeof(data_to_send); // Data size
    uint32_t timeout = 1000; /* Timeout set to 1 second */
    /* Initialize I2C context... Not listed here */
    /* Use the hpm_i2c_master_write_blocking function to write data to the device */
    hpm_stat_t result = hpm_i2c_master_addr_write_blocking(
        &i2c_context,
        device_address,
        addr,
        addr_size,
        data_to_send,
        size,
        timeout
    );
    if (result == status_success) {
        // Write successful
        printf("Data successfully written to I2C slave.\n");
    } else {
        // Write failed, handle error case
        printf("Failed to write data to I2C slave. Error code: %d\n", result);
    }

  - Prototype of `hpm_i2c_master_addr_read_blocking` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_master_addr_read_blocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size, uint8_t *buf, uint32_t buf_size, uint32_t timeout)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `device_address`
          - `uint16_t`
          - Address of the target slave device from which data will be read. This value can be 7-bit or 10-bit based on the `is_10bit_addressing` flag in initialization configuration
        * - `addr`
          - `uint32_t`
          - Internal register or memory address of the slave device where the read operation will take place
        * - `addr_size`
          - `uint8_t`
          - Size of the address `addr` in bytes, typically 1 to 4 bytes
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer where received data will be stored
        * - `buf_size`
          - `uint32_t`
          - Expected number of bytes to receive in the buffer
        * - `timeout`
          - `uint32_t`
          - Timeout in milliseconds. If the operation does not complete within this time, the function will return a timeout error

  - Return Values:

    - `status_success`: If the read operation completes successfully.

    - `status_invalid_argument`: If the provided `addr_size` or `buf_size` do not meet requirements, or if the total data length exceeds the maximum transfer count supported by the hardware.

    - `status_timeout`: If the operation does not complete within the specified `timeout`.

    - `status_i2c_no_addr_hit`: If no device responds at the target address.

  - **Example**: An I2C master device reading some data from a specific register of a slave device via the I2C bus:

  .. code-block:: c

    /* Define the I2C context and device address */
    hpm_i2c_context_t i2c_context;
    const uint16_t device_address = 0x3C; /* Example slave device address, modify according to actual device */

    /* Data to be read and related information */
    uint8_t received_data[4]; /* Receive buffer size, adjust as needed */
    uint32_t addr = 0x01; /* Target register address, modify as needed */
    uint8_t addr_size = 1; /* Register address size is 1 byte */
    uint32_t size = sizeof(received_data); /* Data size */
    uint32_t timeout = 1000; /* Timeout set to 1 second */

    /* Initialize I2C context... Not listed here */

    /* Use the assumed hpm_i2c_master_addr_read_blocking function to read data from the device */
    /* Note: Here it is assumed there exists a function named hpm_i2c_master_addr_read_blocking that allows specifying a register address */
    hpm_stat_t result = hpm_i2c_master_addr_read_blocking(
        &i2c_context,
        device_address,
        addr,
        addr_size,
        received_data,
        size,
        timeout
    );

    if (result == status_success) {
        // Read successful
        printf("Data successfully read from I2C slave.\n");
        for (int i = 0; i < size; i++) {
            printf("Received byte %d: 0x%02X\n", i, received_data[i]);
        }
    } else {
        // Read failed, handle error case
        printf("Failed to read data from I2C slave. Error code: %d\n", result);
    }

Slave Mode
^^^^^^^^^^

- Used for blocking read/write operations in I2C slave mode. It will wait until data transmission is completed or a timeout occurs.
- These operations are divided into `hpm_i2c_slave_write_blocking` and `hpm_i2c_slave_read_blocking` APIs.

  - Prototype of `hpm_i2c_slave_write_blocking` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_slave_write_blocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size, uint32_t timeout)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer containing data to be sent
        * - `size`
          - `uint32_t`
          - Number of bytes to send from the buffer
        * - `timeout`
          - `uint32_t`
          - Timeout in milliseconds. If the operation is not completed within this time, the function will return a timeout error

  - Return Values:

    - `status_success`: If the write operation completes successfully.

    - `status_invalid_argument`: If the provided size exceeds the maximum transfer count supported by the hardware.

    - `status_timeout`: If the operation is not completed within the specified timeout.

    - `status_i2c_transmit_not_completed`: If the data counter does not match expectations before the end of the transmission.


  - **Example**: An I2C slave device wants to respond to a write request from a master device and send some data:

  .. code-block:: c

    hpm_i2c_context_t context;
    /* Initialize I2C context... */
    uint8_t data_to_send[] = {0x01, 0x02, 0x03};
    hpm_stat_t result = hpm_i2c_slave_write_blocking(&context, data_to_send, sizeof(data_to_send), 1000);
    if (result == status_success) {
         /* Successfully handled the write request */
    } else {
        /* Handle error condition */
    }

  - Prototype of `hpm_i2c_slave_read_blocking` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_slave_read_blocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size, uint32_t timeout)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer where received data will be stored
        * - `size`
          - `uint32_t`
          - Expected number of bytes to receive in the buffer
        * - `timeout`
          - `uint32_t`
          - Timeout in milliseconds. If the operation is not completed within this time, the function will return a timeout error

  - Return Values:

    - `status_success`: If the read operation completes successfully.

    - `status_invalid_argument`: If the provided size exceeds the maximum transfer count supported by the hardware.

    - `status_timeout`: If the operation is not completed within the specified timeout.

    - `status_i2c_transmit_not_completed`: If the data counter does not match expectations before the end of the transmission.

  - **Example**: An I2C slave device wants to respond to a read request from a master device and receive some data:

  .. code-block:: c

    hpm_i2c_context_t context;
    /* Initialize I2C context... */
    uint8_t received_data[10];
    hpm_stat_t result = hpm_i2c_slave_read_blocking(&context, received_data, sizeof(received_data), 1000);
    if (result == status_success) {
         /* Successfully handled the read request */
    } else {
        /* Handle error condition */
    }

Non-blocking Read and Write Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Used for non-blocking read/write operations in I2C master and slave modes. It allows continuing with other tasks without waiting for the operation to complete.
- Before using non-blocking read/write interfaces, the **DMA Configuration** process mentioned above must be completed.

Master Mode, Divided into Direct Read/Write Operations and Address Register Read/Write Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **Direct Read/Write Operations**: Used for non-blocking read/write operations in I2C master mode.
- These operations are divided into `hpm_i2c_master_write_nonblocking` and `hpm_i2c_master_read_nonblocking` APIs.

  - Prototype of `hpm_i2c_master_write_nonblocking` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_master_write_nonblocking(hpm_i2c_context_t *context, uint16_t device_address, uint8_t *buf, uint32_t size)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `device_address`
          - `uint16_t`
          - Address of the target slave device to which data will be written. This value can be 7-bit or 10-bit based on the `is_10bit_addressing` flag in initialization configuration
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer containing data to be sent
        * - `size`
          - `uint32_t`
          - Number of bytes to send from the buffer

  - Return Values:

    - `status_success`: If the write operation starts successfully.

    - `status_invalid_argument`: If the provided size exceeds the maximum transfer count supported by the hardware.

    - `status_i2c_no_addr_hit`: If no device responds at the target address.

  - **Example**: An I2C master device wants to send some data to a slave device over the I2C bus:

  .. code-block:: c

    /* Define I2C context and device address */
    hpm_i2c_context_t i2c_context;
    const uint16_t device_address = 0x3C; /* Example slave device address */
    /* Data to send and its size */
    uint8_t data_to_send[] = {0x01, 0x02, 0x03}; /* Example data */
    uint32_t size = sizeof(data_to_send); // Data size
    /* Initialize I2C context... Not listed here */
    /* DMA configuration... Not listed here */
    /* Use hpm_i2c_master_read_blocking function to read data from the device */
    hpm_stat_t result = hpm_i2c_master_write_nonblocking(
        &i2c_context,
        device_address,
        data_to_send,
        size
    );
    if (result == status_success) {
        printf("Data transmission started successfully\n");
    } else {
        printf("Failed to start data transmission. Error code: %d\n", result);
    }

    /* TODO Since this is a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, wait for this transmission to complete */

  - Prototype of `hpm_i2c_master_read_nonblocking` API:

  .. code-block:: c

    hpm_stat_t hpm_i2c_master_read_nonblocking(hpm_i2c_context_t *context, uint16_t device_address, uint8_t *buf, uint32_t size)

  - Parameter Description:

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to the I2C context structure containing configuration and status
        * - `device_address`
          - `uint16_t`
          - Address of the target slave device from which data will be read. This value can be 7-bit or 10-bit based on the `is_10bit_addressing` flag in initialization configuration
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer where received data will be stored
        * - `size`
          - `uint32_t`
          - Expected number of bytes to receive in the buffer

  - Return Values:

    - `status_success`: If the read operation starts successfully.

    - `status_invalid_argument`: If the provided size exceeds the maximum transfer count supported by the hardware.

    - `status_i2c_no_addr_hit`: If no device responds at the target address.

  - **Example**: An I2C master device wants to read some data from a slave device over the I2C bus:

  .. code-block:: c

    /* Define I2C context and device address */
    hpm_i2c_context_t i2c_context;
    const uint16_t device_address = 0x3C; /* Example slave device address */
    /* Prepare buffer for receiving data and its size */
    uint8_t received_data[10]; /* Buffer for received data */
    uint32_t size = sizeof(received_data); /* Buffer size */
    /* Initialize I2C context... Not listed here */
    /* DMA configuration... Not listed here */
    /* Use hpm_i2c_master_read_blocking function to read data from the device */
    hpm_stat_t result = hpm_i2c_master_read_nonblocking(
        &i2c_context,
        device_address,
        received_data,
        size
    );

    if (result == status_success) {
        printf("Data transmission started successfully\n");
    } else {
        printf("Failed to start data transmission. Error code: %d\n", result);
    }

    /* TODO Since this is a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, wait for this transmission to complete */

- **Non-blocking Operation with Address Register Read/Write**: Used for non-blocking operations to write addresses and data to slave devices in I2C master mode.
- Divided into ``hpm_i2c_master_addr_write_nonblocking`` and ``hpm_i2c_master_addr_read_nonblocking`` APIs

  - ``hpm_i2c_master_addr_write_nonblocking`` API function prototype:

  .. code-block:: c

    hpm_stat_t hpm_i2c_master_addr_write_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size, uint8_t *buf, uint32_t buf_size)

  - Parameter Description

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to an I2C context structure that contains I2C configuration information and status.
        * - `device_address`
          - `uint16_t`
          - The address of the target slave device to which data will be written. Depending on the is_10bit_addressing flag in the initialization configuration, this value can be either 7-bit or 10-bit.
        * - `addr`
          - `uint8_t`
          - The register or memory address within the slave device where the write operation will take place.
        * - `addr_size`
          - `uint32_t`
          - The byte size of the address `addr`, typically ranging from 1 to 4 bytes.
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer containing the data to send.
        * - `size`
          - `uint32_t`
          - The number of bytes to send from the buffer.

  - Return Values:

    - ``status_success``: If the write operation completes successfully.

    - ``status_invalid_argument``: If the provided `addr_size` or `buf_size` does not meet requirements, or if the total data length exceeds the maximum transfer count supported by the hardware.

    - ``status_i2c_no_addr_hit``: If no device responds at the target address.

  - **Example**: An I2C master device writing some data to a register of a slave device over the I2C bus.

  .. code-block:: c

    const uint16_t device_address = 0x3C; /* Example slave device address */
    /* Data to send and its size */
    uint8_t data_to_send[] = {0x01, 0x02, 0x03}; /* Example data */
    uint32_t addr = 0x00; /* Target register address */
    uint8_t addr_size = 1; /* Size of register address is 1 byte */
    uint32_t size = sizeof(data_to_send); // Size of data
    /* Initialize I2C context... Not shown here */
    /* Configure DMA... Not shown here */

    hpm_stat_t result = hpm_i2c_master_addr_write_nonblocking(
        &i2c_context,
        device_address,
        addr,
        addr_size,
        data_to_send,
        size
    );
    if (result == status_success) {
        printf("Data transmission started successfully\n");
    } else {
        printf("Failed to start data transmission. Error code: %d\n", result);
    }

    /* TODO Since it's a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, waiting for this transmission to complete. */

  - ``hpm_i2c_master_addr_read_nonblocking`` API function prototype:

  .. code-block:: c

    hpm_stat_t hpm_i2c_master_addr_read_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size, uint8_t *buf, uint32_t buf_size)

  - Parameter Description

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to an I2C context structure that contains I2C configuration information and status.
        * - `device_address`
          - `uint16_t`
          - The address of the target slave device from which data will be read. Depending on the is_10bit_addressing flag in the initialization configuration, this value can be either 7-bit or 10-bit.
        * - `addr`
          - `uint8_t`
          - The register or memory address within the slave device where the read operation will take place.
        * - `addr_size`
          - `uint32_t`
          - The byte size of the address `addr`, typically ranging from 1 to 4 bytes.
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer where received data will be stored.
        * - `size`
          - `uint32_t`
          - The expected number of bytes to receive in the buffer.
        * - `timeout`
          - `uint32_t`
          - Timeout period in milliseconds. If the operation is not completed within this time, the function will return a timeout error.

  - Return Values:

    - ``status_success``: If the read operation completes successfully.

    - ``status_invalid_argument``: If the provided `addr_size` or `buf_size` does not meet requirements, or if the total data length exceeds the maximum transfer count supported by the hardware.

    - ``status_timeout``: If the operation is not completed within the specified timeout period.

    - ``status_i2c_no_addr_hit``: If no device responds at the target address.

  - **Example**: An I2C master device reading some data from a register of a slave device over the I2C bus.

  .. code-block:: c

    /* Define I2C context and device address */
    hpm_i2c_context_t i2c_context;
    const uint16_t device_address = 0x3C; /* Example slave device address, modify according to actual device */

    /* Data to be read and related information */
    uint8_t received_data[4]; /* Buffer size for receiving data, adjust as needed */
    uint32_t addr = 0x01; /* Target register address, modify as needed */
    uint8_t addr_size = 1; /* Register address size is 1 byte */
    uint32_t size = sizeof(received_data); /* Data size */
    uint32_t timeout = 1000; /* Set timeout to 1 second */

    /* Initialize I2C context... Not shown here */

    /* Use the assumed hpm_i2c_master_addr_read_nonblocking function to read data from the device */
    /* Note: Here it is assumed there exists a function named hpm_i2c_master_addr_read_nonblocking that allows specifying a register address */
    hpm_stat_t result = hpm_i2c_master_addr_read_nonblocking(
        &i2c_context,
        device_address,
        addr,
        addr_size,
        received_data,
        size
    );

    if (result == status_success) {
        // Read successful
        printf("Data successfully read from I2C slave.\n");
        for (int i = 0; i < size; i++) {
            printf("Received byte %d: 0x%02X\n", i, received_data[i]);
        }
    } else {
        // Read failed, handle error case
        printf("Failed to read data from I2C slave. Error code: %d\n", result);
    }

    /* TODO Since it's a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, waiting for this transmission to complete. */

Slave Mode
^^^^^^^^^^

- Used for blocking read and write operations under I2C slave mode. It will wait until data transmission completes or times out.
- Divided into ``hpm_i2c_slave_write_nonblocking`` and ``hpm_i2c_slave_read_nonblocking`` APIs

  - ``hpm_i2c_slave_write_nonblocking`` API function prototype:

  .. code-block:: c

    hpm_stat_t hpm_i2c_slave_write_nonblocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size)

  - Parameter Description

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to an I2C context structure that contains I2C configuration information and status.
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer containing the data to send.
        * - `size`
          - `uint32_t`
          - The number of bytes to send from the buffer.

  - Return Values:

    - ``status_success``: If the write operation completes successfully.

    - ``status_invalid_argument``: If the provided `size` exceeds the maximum transfer count supported by the hardware.

  - **Example**: An I2C slave device wanting to respond to a write request from a master device and send some data:

  .. code-block:: c

    hpm_i2c_context_t context;
    /* Initialize I2C context... */
    /* Configure DMA... Not shown here */
    uint8_t data_to_send[] = {0x01, 0x02, 0x03};
    hpm_stat_t result = hpm_i2c_slave_write_nonblocking(&context, data_to_send, sizeof(data_to_send));
    if (result == status_success) {
         /* Successfully processed the write request */
    } else {
        /* Handle error condition */
    }
    /* TODO Since it's a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, waiting for this transmission to complete. */

  - ``hpm_i2c_slave_read_nonblocking`` API function prototype:

  .. code-block:: c

    hpm_stat_t hpm_i2c_slave_read_nonblocking(hpm_i2c_context_t *context, uint8_t *buf, uint32_t size)

  - Parameter Description

      .. list-table::
        :header-rows: 1

        * - Parameter Name
          - Type
          - Description
        * - `context`
          - `hpm_i2c_context_t*`
          - Pointer to an I2C context structure that contains I2C configuration information and status.
        * - `buf`
          - `uint8_t*`
          - Pointer to the buffer where received data will be stored.
        * - `size`
          - `uint32_t`
          - The expected number of bytes to receive in the buffer.

  - Return Values:

    - ``status_success``: If the read operation completes successfully.

    - ``status_invalid_argument``: If the provided `size` exceeds the maximum transfer count supported by the hardware.

  - **Example**: An I2C slave device wanting to respond to a read request from a master device and receive some data:

  .. code-block:: c

    hpm_i2c_context_t context;
    /* Initialize I2C context... */
    /* Configure DMA... Not shown here */
    uint8_t received_data[10];
    hpm_stat_t result = hpm_i2c_slave_read_nonblocking(&context, received_data, sizeof(received_data));
    if (result == status_success) {
         /* Successfully processed the read request */
    } else {
        /* Handle error condition */
    }
    /* TODO Since it's a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, waiting for this transmission to complete. */

Notes
-----

- Since the I2C component uses the DMA manager component, the DMA channel configurations are allocated by the DMA manager. When using DMA, ensure that the allocated DMA channels do not conflict with those used by the I2C component.
- The DMA channels used by the I2C component can be obtained by calling the `hpm_i2c_get_dma_mgr_resource` API.

  - `hpm_i2c_get_dma_mgr_resource` API function prototype:

  .. code-block:: c

    dma_resource_t *hpm_i2c_get_dma_mgr_resource(hpm_i2c_context_t *context)

- **Example:** How to Use `hpm_i2c_get_dma_mgr_resource` Functions to Obtain DMA Channel Resources and Utilize Them for Data Transfer

    .. code-block:: c

        hpm_i2c_context_t context;
        /* Initialize SPI... details omitted */
        /* Get DMA channel resource */
        dma_resource_t *i2c_dma_resource = hpm_spi_get_tx_dma_resource(&context);
        if (i2c_dma_resource != NULL) {
            /* Successfully obtained transmit DMA channel resource */
            printf("I2C DMA channel resource obtained successfully.\n");
            /* Print the DMA instance and channel used by the transmit DMA resource */
            printf("i2c DMA instance: %d, i2c DMA channel: %d\n", i2c_dma_resource->dma_instance, i2c_dma_resource->dma_channel);
            /* Change the i2c DMA resource's interrupt priority to 1 */
            dma_mgr_enable_dma_irq_with_priority(i2c_dma_resource, 1);
        }
