.. _i2c:

I2C
===

Overview
------------

- This section mainly introduces the key driver interfaces and invocation methods of the I2C peripheral in HPM. For more details, please refer to the API documentation in `hpm_i2c_drv.h` and the relevant user manual.
- Supports standard mode (100 Kb/s), fast mode (400 Kb/s), and enhanced fast mode (1 Mb/s).
- Supports 7-bit and 10-bit addressing.
- Supports master mode and slave mode.
- Supports various interrupts and DMA transfers.

  - Transfer complete interrupt
  - Byte receive interrupt
  - Byte send interrupt
  - Start signal interrupt
  - Stop signal interrupt
  - Arbitration loss interrupt
  - Address match interrupt
  - FIFO half full/half empty interrupt
  - FIFO full interrupt
  - FIFO empty interrupt

- Supports bus deadlock recovery. This feature is supported if the `HPM_IP_FEATURE_I2C_SUPPORT_RESET` macro is defined in the SOC's `hpm_soc_ip_feature.h`.
- Equipped with a 4-byte hardware FIFO buffer. The specific depth can be checked via the `I2C_SOC_FIFO_SIZE` macro in `hpm_soc_feature.h` or obtained using the `i2c_get_fifo_size` API interface in `hpm_i2c_drv.h`.
- Maximum single transfer length is 4096 bytes. The specific length can be checked via the `I2C_SOC_TRANSFER_COUNT_MAX` macro in `hpm_soc_feature.h`.

I2C Initialization
--------------------

- It is necessary to ensure that the I2C clock source is enabled and the related I2C peripheral pins are initialized.

  - The `clock_add_to_group` function can be used to add the I2C clock source to the clock group, ensuring the I2C clock source is enabled.

- Related structure description:

  - The `i2c_config_t` structure is used to configure I2C parameters

    .. code-block:: c

        /* I2C configuration structure */
        typedef struct {
            bool is_10bit_addressing;  /* Whether to use 10-bit addressing */
            uint8_t i2c_mode;          /* I2C speed mode */
        } i2c_config_t;

- Related enumeration value description:

  - The `i2c_mode_t` enumeration is used to configure I2C speed mode

    .. code-block:: c

        /* I2C speed mode */
        typedef enum i2c_mode {
            i2c_mode_normal,      /* Standard mode 100 Kb/s */
            i2c_mode_fast,        /* Fast mode 400 Kb/s */
            i2c_mode_fast_plus,  /* Fast plus mode 1 Mb/s */
        } i2c_mode_t;

Master Mode Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Initialization API function:

  .. code-block:: c

        hpm_stat_t i2c_init_master(I2C_Type *ptr, uint32_t src_clk_in_hz, i2c_config_t *config);

  - Parameter description:

    .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - ptr
              - I2C_Type*
              - Pointer to the base address of the I2C controller
            * - src_clk_in_hz
              - uint32_t
              - Clock source frequency of the I2C controller (Hz)
            * - config
              - i2c_config_t*
              - Pointer to the I2C configuration structure

  - Return value:

    .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Initialization successful
            * - status_i2c_not_supported
              - I2C controller not supported

  - The clock source frequency of the I2C controller can be obtained using the `clock_get_frequency` API.

    - Example:

      .. code-block:: c

            uint32_t i2c_clk_freq = clock_get_frequency(clock_i2c0);

- Example:

  - I2C0 uses standard mode, 100 Kb/s, 7-bit addressing

  .. code-block:: c

        /* I2C clock source enable and pin initialization (not shown here) */
        /* I2C0 initialization as master */

        i2c_config_t i2c_config;
        uint32_t i2c_clk_freq = clock_get_frequency(clock_i2c0);
        i2c_config.is_10bit_addressing = false;
        i2c_config.i2c_mode = i2c_mode_normal;
        hpm_stat_t status = i2c_init_master(HPM_I2C0, i2c_clk_freq, &i2c_config);


Slave Mode Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Initialization API function:

  .. code-block:: c

        hpm_stat_t i2c_init_slave(I2C_Type *ptr, uint32_t src_clk_in_hz, i2c_config_t *config, const uint16_t slave_address);

  - Parameter description:

    .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - ptr
              - I2C_Type*
              - Pointer to the base address of the I2C controller
            * - src_clk_in_hz
              - uint32_t
              - Clock source frequency of the I2C controller (Hz)
            * - config
              - i2c_config_t*
              - Pointer to the I2C configuration structure
            * - slave_addr
              - uint8_t
              - Slave address

  - Return value:

    .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Initialization successful
            * - status_i2c_not_supported
              - I2C controller not supported

  - The clock source frequency of the I2C controller can be obtained using the `clock_get_frequency` API.

- Example:

  - I2C0 uses standard mode, 100 Kb/s, 7-bit addressing, slave address 0x10

    .. code-block:: c

        /* I2C clock source enable and pin initialization (not shown here) */
        /* I2C0 initialization as slave */
        i2c_config_t i2c_config;
        uint32_t i2c_clk_freq = clock_get_frequency(clock_i2c0);
        i2c_config.is_10bit_addressing = false;
        i2c_config.i2c_mode = i2c_mode_normal;
        hpm_stat_t status = i2c_init_slave(HPM_I2C0, i2c_clk_freq, &i2c_config, 0x10);

I2C Data Transmission
-------------------------

Polling Transfer
^^^^^^^^^^^^^^^^^^^

Master Mode
""""""""""""

- Provides read/write APIs without address operations, with address operations, and a custom sequence transfer API.

  - Read/Write APIs without address operations:

    .. code-block:: c

        hpm_stat_t i2c_master_write(I2C_Type *ptr, const uint16_t device_address, uint8_t *buf, const uint32_t size);
        hpm_stat_t i2c_master_read(I2C_Type *ptr, const uint16_t device_address, uint8_t *buf, const uint32_t size);

    - Parameter Description: The parameters for read/write APIs are consistent, as follows:

      .. list-table::
              :header-rows: 1

              * - Parameter Name
                - Type
                - Description
              * - ptr
                - I2C_Type*
                - Pointer to the base address of the I2C controller
              * - device_address
                - uint16_t
                - Device address
              * - buf
                - uint8_t*
                - Data buffer pointer
              * - size
                - uint32_t
                - Data length

    - Return Value:

      .. list-table::
              :header-rows: 1

              * - Return Value
                - Description
              * - status_success
                - Transfer successful
              * - status_invalid_argument
                - Invalid parameter
              * - status_timeout
                - Transfer timeout
              * - status_i2c_transmit_not_completed
                - Transfer not completed
              * - status_i2c_no_addr_hit
                - Address not hit (no device with this address on the bus)
              * - status_fail
                - Transfer failed

    - Note:

      - This API can be used to scan for the existence of a specific slave device address. The `buf` parameter can be set to `NULL`, and the `size` parameter can be set to `0`.

        - Example:

          .. code-block:: c

            /* Scan if I2C0 has a device with address 0x10 */
            uint8_t buf[1];
            hpm_stat_t status = i2c_master_read(HPM_I2C0, 0x10, NULL, 0);
            if (status == status_success) {
                /* Device with address 0x10 exists */
                printf("HPM_I2C0 has device address 0x10.\n");
            }

    - Example:

      - Master writes 10 bytes of data to the device with address 0x10

        .. code-block:: c

            uint8_t buf[10];
            hpm_stat_t status = i2c_master_write(HPM_I2C0, 0x10, buf, 10);
            if (status == status_success) {
                printf("HPM_I2C0 write 10 bytes to device address 0x10 success.\n");
            }

      - Master reads 10 bytes of data from the device with address 0x10

        .. code-block:: c

            uint8_t buf[10];
            hpm_stat_t status = i2c_master_read(HPM_I2C0, 0x10, buf, 10);
            if (status == status_success) {
                printf("HPM_I2C0 read 10 bytes from device address 0x10 success.\n");
            }

  - Read/Write APIs with address operations:

    .. code-block:: c

        hpm_stat_t i2c_master_address_write(I2C_Type *ptr, const uint16_t device_address, uint8_t *addr, uint32_t addr_size_in_byte, uint8_t *buf, const uint32_t size_in_byte);
        hpm_stat_t i2c_master_address_read(I2C_Type *ptr, const uint16_t device_address, uint8_t *addr, uint32_t addr_size_in_byte, uint8_t *buf, const uint32_t size_in_byte);

    - Parameter Description: The parameters for read/write APIs are consistent, as follows:

      .. list-table::
              :header-rows: 1

              * - Parameter Name
                - Type
                - Description
              * - ptr
                - I2C_Type*
                - Pointer to the base address of the I2C controller
              * - device_address
                - uint16_t
                - Device address
              * - addr
                - uint8_t*
                - Address buffer pointer
              * - addr_size_in_byte
                - uint32_t
                - Address length
              * - buf
                - uint8_t*
                - Data buffer pointer
              * - size_in_byte
                - uint32_t
                - Data length
    - Return Value:

      .. list-table::
              :header-rows: 1

              * - Return Value
                - Description
              * - status_success
                - Transfer successful
              * - status_invalid_argument
                - Invalid parameter
              * - status_timeout
                - Transfer timeout
              * - status_i2c_transmit_not_completed
                - Transfer not completed
              * - status_i2c_no_addr_hit
                - Address not hit (no device with this address on the bus)
              * - status_fail
                - Transfer failed
    - Example:

      - Master writes 10 bytes of data to the register at address 0x01 of the device with address 0x10

        .. code-block:: c

            uint8_t addr[1] = {0x01};
            uint8_t buf[10];
            hpm_stat_t status = i2c_master_address_write(HPM_I2C0, 0x10, addr, 1, buf, 10);
            if (status == status_success) {
                printf("HPM_I2C0 write 10 bytes to device address 0x10 success.\n");
            }

      - Master reads 10 bytes of data from the register at address 0x02 of the device with address 0x10

        .. code-block:: c

            uint8_t addr[1] = {0x02};
            uint8_t buf[10];
            hpm_stat_t status = i2c_master_address_read(HPM_I2C0, 0x10, addr, 1, buf, 10);
            if (status == status_success) {
                printf("HPM_I2C0 read 10 bytes from device address 0x10 success.\n");
            }

    - Note: The address length and data length must not exceed `I2C_SOC_TRANSFER_COUNT_MAX` bytes.

  - Custom sequence transfer API:

    .. code-block:: c

        hpm_stat_t i2c_master_transfer(I2C_Type *ptr, const uint16_t device_address, uint8_t *buf, const uint32_t size,  uint16_t flags);

    - Parameter Description:

      .. list-table::
              :header-rows: 1

              * - Parameter Name
                - Type
                - Description
              * - ptr
                - I2C_Type*
                - Pointer to the base address of the I2C controller
              * - device_address
                - uint16_t
                - Device address
              * - buf
                - uint8_t*
                - Data buffer pointer
              * - size
                - uint32_t
                - Data length
              * - flags
                - uint16_t
                - Transfer flags

    - Return Value:

      .. list-table::
              :header-rows: 1

              * - Return Value
                - Description
              * - status_success
                - Transfer successful
              * - status_invalid_argument
                - Invalid parameter
              * - status_timeout
                - Transfer timeout
              * - status_i2c_transmit_not_completed
                - Transfer not completed
              * - status_i2c_no_addr_hit
                - Address not hit (no device with this address on the bus)
              * - status_fail
                - Transfer failed

    - Flags: The related macro definitions can be found in `hpm_i2c_drv.h`

      .. list-table::
              :header-rows: 1

              * - Flag
                - Description
                - Notes
              * - I2C_WR
                - Write data
                - Cannot be set with I2C_RD
              * - I2C_RD
                - Read data
                - Cannot be set with I2C_WR
              * - I2C_ADDR_10BIT
                - 10-bit address
                - None
              * - I2C_NO_START
                - No start signal
                - None
              * - I2C_NO_ADDRESS
                - No address
                - None
              * - I2C_NO_READ_ACK
                - No acknowledgment
                - None
              * - I2C_NO_STOP
                - No stop signal
                - None
              * - I2C_WRITE_CHECK_ACK
                - Check acknowledgment during write
                - None

    - Example:

      - In some sensors' repeated Start operations, a command must be written first, followed by reading data. The custom sequence transfer API can be used in this case.

        .. code-block:: c

            /* Write command */
            uint8_t cmd = 0x01;
            /* Write data, do not send stop signal to keep the bus occupied */
            hpm_stat_t status = i2c_master_transfer(HPM_I2C0, 0x10, &cmd, 1, I2C_WR | I2C_NO_STOP);
            if (status == status_success) {
                printf("HPM_I2C0 write 1 byte to device address 0x10 success.\n");
            }
            /* Read data */
            uint8_t buf[10];
            status = i2c_master_transfer(HPM_I2C0, 0x10, buf, 10, I2C_RD);
            if (status == status_success) {
                printf("HPM_I2C0 read 10 bytes from device address 0x10 success.\n");
            }

Slave Mode
""""""""""""

- Read and Write APIs

  .. code-block:: c

        hpm_stat_t i2c_slave_write(I2C_Type *ptr, uint8_t *buf, const uint32_t size);
        hpm_stat_t i2c_slave_read(I2C_Type *ptr, uint8_t *buf, const uint32_t size);

  - Parameter Description: The parameters for the read and write APIs are the same, as described below:

    .. list-table::
              :header-rows: 1

              * - Parameter Name
                - Type
                - Description
              * - ptr
                - I2C_Type*
                - Pointer to the base address of the I2C controller
              * - buf
                - uint8_t*
                - Data buffer pointer
              * - size
                - uint32_t
                - Data length

  - Return Value:

    .. list-table::
              :header-rows: 1

              * - Return Value
                - Description
              * - status_success
                - Transmission succeeded
              * - status_invalid_argument
                - Invalid argument
              * - status_timeout
                - Transmission timeout
              * - status_i2c_transmit_not_completed
                - Transmission not completed
              * - status_fail
                - Transmission failed

  - Examples:

    - Slave writes 10 bytes of data to the host

      .. code-block:: c

            uint8_t buf[10];

            /* Wait for host to write data */
            do {
                stat = i2c_slave_write(TEST_I2C, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
            } while (stat == status_fail);

    - Slave reads 10 bytes of data from the host

      .. code-block:: c

            uint8_t buf[10];

            /* Wait for host to read data */
            do {
                stat = i2c_slave_read(TEST_I2C, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
            } while (stat == status_fail);

Initiate DMA Transfer
^^^^^^^^^^^^^^^^^^^^^^

- This section does not handle parameters related to DMA transfer (such as DMA channel, DMA source address, and device address settings), but focuses solely on initiating I2C DMA transfers.
- For more details, refer to the DMA example in **I2C Examples**.
- After each transfer, the `CMPL` flag must be cleared; otherwise, it will affect the next DMA transfer. The `i2c_clear_status` API can be used to clear the `CMPL` flag.
- For APIs requiring DMA transmission and reception, please consult the I2C component documentation :ref:`i2c_components <i2c_component>`.

Master Mode
""""""""""""

- Read and Write DMA Initiation APIs:

  .. code-block:: c

        hpm_stat_t i2c_master_start_dma_write(I2C_Type *i2c_ptr, const uint16_t device_address, uint32_t size);
        hpm_stat_t i2c_master_start_dma_read(I2C_Type *i2c_ptr, const uint16_t device_address, uint32_t size);

  - Parameter Description: The parameters for read and write APIs are identical, as detailed below:

    .. list-table::
              :header-rows: 1

              * - Parameter Name
                - Type
                - Description
              * - i2c_ptr
                - I2C_Type*
                - Pointer to the base address of the I2C controller
              * - device_address
                - uint16_t
                - Device address
              * - size
                - uint32_t
                - Data length
  - Return Value:

    .. list-table::
              :header-rows: 1

              * - Return Value
                - Description
              * - status_success
                - Configuration succeeded
              * - status_fail
                - Configuration failed
              * - status_invalid_argument
                - Invalid argument
              * - status_timeout
                - Configuration timeout

  - Note: Before each DMA transfer, this API must be called to configure DMA transfer parameters. Refer to the DMA example in **I2C Examples**.

  - Example:

    - Master writes 10 bytes of data to a slave device with address 0x10

      .. code-block:: c

            uint8_t buf[10];
            hpm_stat_t status = i2c_master_start_dma_write(HPM_I2C0, 0x10, 10);
            if (status == status_success) {
                printf("HPM_I2C0 dma write config success.\n");
            } else {
                assert(0);
            }
            /* Configure DMA channel, DMA source address, etc., start DMA transfer. See `I2C Examples` for DMA configuration */

    - Master reads 10 bytes of data from a slave device with address 0x10

      .. code-block:: c

            uint8_t buf[10];
            hpm_stat_t status = i2c_master_start_dma_read(HPM_I2C0, 0x10, 10);
            if (status == status_success) {
                printf("HPM_I2C0 dma read config success.\n");
            } else {
                assert(0);
            }
            /* Configure DMA channel, DMA source address, etc., start DMA transfer. See `I2C Examples` for DMA configuration */

Slave Mode
""""""""""""

- Initiate DMA Transfer API:

  .. code-block:: c

        hpm_stat_t i2c_slave_dma_transfer(I2C_Type *i2c_ptr, uint32_t size);

  - Parameter Description:

    .. list-table::
              :header-rows: 1

              * - Parameter Name
                - Type
                - Description
              * - i2c_ptr
                - I2C_Type*
                - Pointer to the base address of the I2C controller
              * - size
                - uint32_t
                - Data length

  - Return Value:

    .. list-table::
              :header-rows: 1

              * - Return Value
                - Description
              * - status_success
                - Configuration succeeded
              * - status_fail
                - Configuration failed
              * - status_invalid_argument
                - Invalid argument
              * - status_timeout
                - Configuration timeout

  - Notes:

    - After configuring DMA parameters, use this API to set up DMA transfer parameters. Refer to the DMA example in **I2C Examples**.
    - Wait for address match before reading the transfer direction. If it's a read direction, configure DMA such as setting the source address to the I2C data register address, etc. Similarly for write direction.
    - Address match can be waited upon using an address match interrupt or by polling.
    - Polling can be done using the `i2c_get_status` API to get the address match status.
    - The transfer direction can be checked using the `i2c_is_writing` and `i2c_is_reading` APIs.

  - Example:

    - Slave polls to write 10 bytes of data to the master

      .. code-block:: c

            uint8_t buf[10];
            /* Wait for address match */
            while (!(i2c_get_status(HPM_I2C0) & I2C_STATUS_ADDRHIT_MASK)) {
            }
            /* Check writing direction */
            if (i2c_is_writing(HPM_I2C0)) {
                /* In slave mode, prepare for writing direction */
                /* Configure DMA channel, DMA source address, etc., start DMA transfer. See `I2C Examples` for DMA configuration */
            } else {
                assert(0);
            }
            hpm_stat_t status = i2c_slave_dma_transfer(HPM_I2C0, 10);
            if (status == status_success) {
                printf("i2c slave dma transfer success.\n");
            } else {
                printf("i2c slave dma transfer failed.\n");
                assert(0);
            }
            /* Wait for I2C transfer completion */
            do {
                status = i2c_get_status(ptr);
            } while (!(status & I2C_STATUS_CMPL_MASK));
            /* Clear CMPL flag to avoid affecting the next transfer */
            i2c_clear_status(ptr, status);

    - Slave polls to read 10 bytes of data from the master

    .. code-block:: c

            uint8_t buf[10];
            /* Wait for address match */
            while (!(i2c_get_status(HPM_I2C0) & I2C_STATUS_ADDRHIT_MASK)) {
            }
            /* Check reading direction */
            if (i2c_is_reading(HPM_I2C0)) {
                /* In slave mode, prepare for reading direction */
                /* Configure DMA channel, DMA source address, etc., start DMA transfer. See `I2C Examples` for DMA configuration */
            } else {
                assert(0);
            }
            hpm_stat_t status = i2c_slave_dma_transfer(HPM_I2C0, 10);
            if (status == status_success) {
                printf("i2c slave dma transfer success.\n");
            } else {
                printf("i2c slave dma transfer failed.\n");
                assert(0);
            }
            /* Wait for I2C transfer completion */
            do {
                status = i2c_get_status(ptr);
            } while (!(status & I2C_STATUS_CMPL_MASK));
            /* Clear CMPL flag to avoid affecting the next transfer */
            i2c_clear_status(ptr, status);

I2C Interrupt
--------------

- Refer to the **interrupt example** in the **I2C Examples**.
- The corresponding I2C interrupt in the PLIC controller must be enabled using the `intc_m_enable_irq` API.
- The priority of the I2C interrupt must be configured in the PLIC controller using the `intc_m_enable_irq_with_priority` API.
- To read the I2C interrupt enable settings, use the `i2c_get_irq_setting` API.
- Each time an interrupt event occurs, call the `i2c_get_status` API in the interrupt handler to check the interrupt status.
- Each time an interrupt event occurs, call the `i2c_clear_status` API in the interrupt handler to clear the interrupt flag.
- The I2C peripheral supports the following interrupts, which can be viewed in the macro definitions from `hpm_i2c_drv.h`.

  .. list-table::
        :header-rows: 1

        * - Interrupt
          - Description
        * - I2C_EVENT_TRANSACTION_COMPLETE
          - Transaction complete
        * - I2C_EVENT_BYTE_RECEIVED
          - Byte received
        * - I2C_EVENT_BYTE_TRANSMIT
          - Byte transmitted
        * - I2C_EVENT_START_CONDITION
          - Start condition
        * - I2C_EVENT_STOP_CONDITION
          - Stop condition
        * - I2C_EVENT_LOSS_ARBITRATION
          - Arbitration lost
        * - I2C_EVENT_ADDRESS_HIT
          - Address match detected
        * - I2C_EVENT_FIFO_HALF
          - FIFO half full/empty
        * - I2C_EVENT_FIFO_FULL
          - FIFO full
        * - I2C_EVENT_FIFO_EMPTY
          - FIFO empty

- Related APIs:

  - Enable I2C Interrupt:

    .. code-block:: c

        hpm_stat_t i2c_enable_irq(I2C_Type *ptr, uint32_t mask);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - ptr
              - I2C_Type*
              - Pointer to the base address of the I2C controller
            * - mask
              - uint32_t
              - Interrupt enable mask. Supported masks can be found in the above interrupt table.

  - Disable I2C Interrupt:

    .. code-block:: c

        hpm_stat_t i2c_disable_irq(I2C_Type *ptr, uint32_t mask);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - ptr
              - I2C_Type*
              - Pointer to the base address of the I2C controller
            * - mask
              - uint32_t
              - Interrupt enable mask. Supported masks can be found in the above interrupt table.

I2C Samples
----------------

.. toctree::
   :maxdepth: 3

   dma/master/README_en
   dma/slave/README_en
   interrupt/master/README_en
   interrupt/slave/README_en
   polling/master/README_en
   polling/slave/README_en
