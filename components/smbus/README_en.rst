.. _smbus:

SMBus
=======

Overview
-----------

- The SMBus (System Management Bus) component extends the I2C bus protocol, providing system management bus specification-compliant communication interfaces. Key features include:

  - Protocol Compatibility

    - Supports standard protocol formats defined in SMBus 2.0 specification
    - Automatic generation/verification of packet CRC (PEC)
    - Supports 7-bit/10-bit device addressing

  - Transfer Mode Support

    - Single/dual-byte read/write operations in master/slave modes
    - Command-code based byte/word/block transfers (Block Write/Read) in master mode
    - Supports Normal mode (100kHz), Fast mode (400kHz) and High-speed mode (1MHz)

  - Data Integrity Assurance

    - Hardware-automated PEC checksum calculation
    - End-to-end data protection mechanism
    - CRC verification for multi-byte transfers

Key API Functions
---------------------

- Basic Transfer Interfaces

  - ``hpm_smbus_master_write_byte``: Master single-byte write (no command code)
  - ``hpm_smbus_master_read_byte``: Master single-byte read (no command code)
  - ``hpm_smbus_master_write``: Master multi-byte write (no command code)
  - ``hpm_smbus_master_read``: Master multi-byte read (no command code)
  - ``hpm_smbus_slave_write``: Slave multi-byte write (no command code)
  - ``hpm_smbus_slave_read``: Slave multi-byte read (no command code)

- Enhanced Transfer Interfaces

  - ``hpm_smbus_master_write_byte_in_command``: Master byte write with command code
  - ``hpm_smbus_master_read_byte_in_command``: Master byte read with command code
  - ``hpm_smbus_master_write_word_in_command``: Master word write with command code
  - ``hpm_smbus_master_read_word_in_command``: Master word read with command code
  - ``hpm_smbus_master_write_block_in_command``: Master block write with command code
  - ``hpm_smbus_master_read_block_in_command``: Master block read with command code

- Low-level Protocol Support

  - ``hpm_smbus_pec_crc8``: PEC checksum generation algorithm (CRC-8)

API Usage Flow
----------------

I2C Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Master Mode
""""""""""""""

- Call ``i2c_init_master`` API to initialize I2C in master mode. Defined in ``hpm_i2c_drv`` driver.

    - ``i2c_init_master`` API prototype:

        .. code-block:: c

            hpm_stat_t i2c_init_master(I2C_Type *ptr, uint32_t src_clk_in_hz, i2c_config_t *config);

        - Parameters:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - Pointer to I2C controller base address
                * - src_clk_in_hz
                  - uint32_t
                  - I2C clock source frequency
                * - config
                  - i2c_config_t*
                  - Pointer to I2C configuration struct

        - Return values:
            - ``status_success``: Success
            - ``status_invalid_argument``: Invalid parameters

- **Example**: Initialize I2C0 as master with 100KHz speed and 7-bit addressing:

    .. code-block:: c

        #define TEST_SMBUS                HPM_I2C0
        #define TEST_SMBUS_CLOCK_NAME     clock_i2c0

        i2c_config_t config;    /* Define I2C config struct */
        board_init_i2c_clock(TEST_SMBUS);
        init_i2c_pins(TEST_SMBUS);

        config.i2c_mode = i2c_mode_normal;
        config.is_10bit_addressing = false;
        freq = clock_get_frequency(TEST_SMBUS_CLOCK_NAME);
        stat = i2c_init_master(TEST_SMBUS, freq, &config);
        if (stat != status_success) {
            return stat;
        }

Slave Mode
""""""""""""""

- Call ``i2c_init_slave`` API to initialize I2C in slave mode. Defined in ``hpm_i2c_drv`` driver.

    - ``i2c_init_slave`` API prototype:

        .. code-block:: c

            hpm_stat_t i2c_init_slave(I2C_Type *ptr, uint32_t src_clk_in_hz, i2c_config_t *config);

        - Parameters:

           .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - Pointer to I2C controller base address
                * - src_clk_in_hz
                  - uint32_t
                  - I2C clock source frequency
                * - config
                  - i2c_config_t*
                  - Pointer to I2C configuration struct

        - Return values:
            - ``status_success``: Success
            - ``status_invalid_argument``: Invalid parameters

- **Example**: Initialize I2C0 as slave with 100KHz speed, 7-bit addressing, and slave address 0x16:

    .. code-block:: c

        #define TEST_SMBUS                HPM_I2C0
        #define TEST_SMBUS_CLOCK_NAME     clock_i2c0

        i2c_config_t config;    /* Define I2C config struct */
        board_init_i2c_clock(TEST_SMBUS);
        init_i2c_pins(TEST_SMBUS);
        config.i2c_mode = i2c_mode_slave;
        config.is_10bit_addressing = false;
        config.slave_address = 0x16;
        freq = clock_get_frequency(TEST_SMBUS_CLOCK_NAME);
        stat = i2c_init_slave(TEST_SMBUS, freq, &config);
        if (stat != status_success) {
            return stat;
        }

Basic Transfer Interfaces
"""""""""""""""""""""""""

- Supports single-byte read/write operations in master mode (no command code)

  - Master Mode

    - Master Single-byte Write

      - Call ``hpm_smbus_master_write_byte`` API for single-byte write in master mode.

        - ``hpm_smbus_master_write_byte`` API prototype:

            .. code-block:: c

                hpm_stat_t hpm_smbus_master_write_byte(I2C_Type *ptr, uint8_t slave_address, uint8_t data);

            - Parameters:

                .. list-table::
                    :header-rows: 1

                    * - Parameter
                      - Type
                      - Description
                    * - ptr
                      - I2C_Type*
                      - Pointer to I2C controller base address
                    * - slave_address
                      - uint8_t
                      - Slave device address
                    * - data
                      - uint8_t
                      - Data to write

            - Return values:

              - ``status_success``: Operation successful
              - ``status_invalid_argument``: Invalid parameters
              - ``status_timeout``: Operation timeout
              - ``status_fail``: General failure

        - **Example**: Write 0x55 to slave device 0x16 in master mode:

            .. code-block:: c

                /* Initialize I2C (omitted)... */
                hpm_stat_t stat;
                stat = hpm_smbus_master_write_byte(TEST_SMBUS, 0x16, 0x55);
                if (stat != status_success) {
                    printf("hpm_smbus_master_write_byte failed.\n");
                }

    - Master Single-byte Read

      - Call ``hpm_smbus_master_read_byte`` API for single-byte read in master mode.

        - ``hpm_smbus_master_read_byte`` API prototype:

            .. code-block:: c

                hpm_stat_t hpm_smbus_master_read_byte(I2C_Type *ptr, uint8_t slave_address, uint8_t *data);

            - Parameters:

                .. list-table::
                    :header-rows: 1

                    * - Parameter
                      - Type
                      - Description
                    * - ptr
                      - I2C_Type*
                      - Pointer to I2C controller base address
                    * - slave_address
                      - uint8_t
                      - Slave device address
                    * - data
                      - uint8_t*
                      - Pointer to store read data

            - Return values:

              - ``status_success``: Operation successful
              - ``status_invalid_argument``: Invalid parameters

        - **Example**: Read data from slave device 0x16 in master mode:

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data;
                /* Initialize I2C (omitted)... */
                stat = hpm_smbus_master_read_byte(TEST_SMBUS, 0x16, &data);
                if (stat != status_success) {
                    printf("hpm_smbus_master_read_byte failed.\n");
                }

- Supports multi-byte read/write operations in both master/slave modes (no command code)

  - Master Mode

    - Master Multi-byte Write

      - Call ``hpm_smbus_master_write`` API for multi-byte write in master mode.

        - ``hpm_smbus_master_write`` API prototype:

            .. code-block:: c

                hpm_stat_t hpm_smbus_master_write(I2C_Type *ptr, uint8_t slave_address, uint8_t *data, uint32_t size);

            - Parameters:

               .. list-table::
                    :header-rows: 1

                    * - Parameter
                      - Type
                      - Description
                    * - ptr
                      - I2C_Type*
                      - Pointer to I2C controller base address
                    * - slave_address
                      - uint8_t
                      - Slave device address
                    * - data
                      - uint8_t*
                      - Data buffer to write
                    * - size
                      - uint32_t
                      - Data length in bytes

            - Return values:

              - ``status_success``: Operation successful
              - ``status_invalid_argument``: Invalid parameters
              - ``status_timeout``: Operation timeout
              - ``status_fail``: General failure

        - **Example**: Write 3 bytes to slave device 0x16:

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data[3] = {0x11, 0x22, 0x33};
                /* Initialize I2C (omitted)... */
                stat = hpm_smbus_master_write(TEST_SMBUS, 0x16, data, 3);
                if (stat != status_success) {
                    printf("hpm_smbus_master_write failed.\n");
                }

    - Master Multi-byte Read

      - Call ``hpm_smbus_master_read`` API for multi-byte read in master mode.

        - ``hpm_smbus_master_read`` API prototype:

            .. code-block:: c

                hpm_stat_t hpm_smbus_master_read(I2C_Type *ptr, uint8_t slave_address, uint8_t *data, uint32_t size);

            - Parameters:

              .. list-table::
                    :header-rows: 1

                    * - Parameter
                      - Type
                      - Description
                    * - ptr
                      - I2C_Type*
                      - Pointer to I2C controller base address
                    * - slave_address
                      - uint8_t
                      - Slave device address
                    * - data
                      - uint8_t*
                      - Buffer to store read data
                    * - size
                      - uint32_t
                      - Data length in bytes

            - Return values:

              - ``status_success``: Operation successful
              - ``status_invalid_argument``: Invalid parameters
              - ``status_timeout``: Operation timeout
              - ``status_fail``: General failure

        - **Example**: Read 3 bytes from slave device 0x16:

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data[3];
                /* Initialize I2C (omitted)... */
                stat = hpm_smbus_master_read(TEST_SMBUS, 0x16, data, 3);
                if (stat != status_success) {
                    printf("hpm_smbus_master_read failed.\n");
                }

  - Slave Mode

    - Slave Multi-byte Write

      - Call ``hpm_smbus_slave_write`` API for slave mode multi-byte write

        - ``hpm_smbus_slave_write`` API prototype:

            .. code-block:: c

                hpm_stat_t hpm_smbus_slave_write(I2C_Type *ptr, uint8_t *data, uint32_t size);

            - Parameters:

              .. list-table::
                    :header-rows: 1

                    * - Parameter
                      - Type
                      - Description
                    * - ptr
                      - I2C_Type*
                      - Pointer to I2C controller base address
                    * - data
                      - uint8_t*
                      - Data to write
                    * - size
                      - uint32_t
                      - Data length in bytes

            - Return values:

              - ``status_success``: Success
              - ``status_invalid_argument``: Invalid parameters
              - ``status_timeout``: Timeout
              - ``status_fail``: General failure

        - **Example**: Slave mode write operation when master accesses slave address:

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data[3];
                /* Initialize I2C (omitted)... */
                /* Wait for address match */
                do {
                    stat = hpm_smbus_slave_write(TEST_SMBUS, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
                } while (stat == status_fail);

    - Slave Multi-byte Read

      - Call ``hpm_smbus_slave_read`` API for slave mode multi-byte read

        - ``hpm_smbus_slave_read`` API prototype:

            .. code-block:: c

                hpm_stat_t hpm_smbus_slave_read(I2C_Type *ptr, uint8_t *data, uint32_t size);

            - Parameters:

              .. list-table::
                    :header-rows: 1

                    * - Parameter
                      - Type
                      - Description
                    * - ptr
                      - I2C_Type*
                      - Pointer to I2C controller base address
                    * - data
                      - uint8_t*
                      - Buffer to store received data
                    * - size
                      - uint32_t
                      - Data length in bytes

            - Return values:

              - ``status_success``: Success
              - ``status_invalid_argument``: Invalid parameters
              - ``status_timeout``: Timeout
              - ``status_fail``: General failure

        - **Example**: Slave mode read operation when master accesses slave address:

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data[3];
                /* Initialize I2C (omitted)... */
                /* Wait for address match */
                do {
                    stat = hpm_smbus_slave_read(TEST_SMBUS, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
                } while (stat == status_fail);


Enhanced Transfer Interfaces
""""""""""""""""""""""""""""

- Supports command-code based transfers in master mode (byte/word/block operations)

  - Master Byte Write with Command Code

    - Call ``hpm_smbus_master_write_byte_in_command`` API for byte write with command code.

      - ``hpm_smbus_master_write_byte_in_command`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_smbus_master_write_byte_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t data);

        - Parameters:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - Pointer to I2C controller base address
                * - slave_address
                  - uint8_t
                  - Slave device address
                * - command
                  - uint8_t
                  - Command code
                * - data
                  - uint8_t
                  - Data to write

        - Return values:

          - ``status_success``: Operation successful
          - ``status_invalid_argument``: Invalid parameters
          - ``status_timeout``: Operation timeout
          - ``status_fail``: General failure

      - **Example**: Write command 0x01 and data 0x55 to slave 0x16:

        .. code-block:: c

            hpm_stat_t stat;
            /* Initialize I2C (omitted)... */
            stat = hpm_smbus_master_write_byte_in_command(TEST_SMBUS, 0x16, 0x01, 0x55);
            if (stat != status_success) {
                printf("hpm_smbus_master_write_byte_in_command failed.\n");
            }

  - Master Byte Read with Command Code

    - Call ``hpm_smbus_master_read_byte_in_command`` API for byte read with command code.

      - ``hpm_smbus_master_read_byte_in_command`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_smbus_master_read_byte_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data);

        - Parameters:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - Pointer to I2C controller base address
                * - slave_address
                  - uint8_t
                  - Slave device address
                * - command
                  - uint8_t
                  - Command code
                * - data
                  - uint8_t*
                  - Pointer to store read data

        - Return values:

          - ``status_success``: Operation successful
          - ``status_invalid_argument``: Invalid parameters
          - ``status_timeout``: Operation timeout
          - ``status_fail``: General failure

      - **Example**: Read data with command 0x01 from slave 0x16:

        .. code-block:: c

            hpm_stat_t stat;
            uint8_t data;
            /* Initialize I2C (omitted)... */
            stat = hpm_smbus_master_read_byte_in_command(TEST_SMBUS, 0x16, 0x01, &data);
            if (stat != status_success) {
                printf("hpm_smbus_master_read_byte_in_command failed.\n");
            }

  - Master Word Write with Command Code

    - Call ``hpm_smbus_master_write_word_in_command`` API for word write with command code.

      - ``hpm_smbus_master_write_word_in_command`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_smbus_master_write_word_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint16_t data);

        - Parameters:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - Pointer to I2C controller base address
                * - slave_address
                  - uint8_t
                  - Slave device address
                * - command
                  - uint8_t
                  - Command code
                * - data
                  - uint16_t
                  - Data to write

        - Return values:

          - ``status_success``: Operation successful
          - ``status_invalid_argument``: Invalid parameters
          - ``status_timeout``: Operation timeout
          - ``status_fail``: General failure

      - **Example**: Write command 0x01 and data 0x5555 to slave 0x16:

        .. code-block:: c

            hpm_stat_t stat;
            /* Initialize I2C (omitted)... */
            stat = hpm_smbus_master_write_word_in_command(TEST_SMBUS, 0x16, 0x01, 0x5555);
            if (stat != status_success) {
                printf("hpm_smbus_master_write_word_in_command failed.\n");
            }

  - Master Word Read with Command Code

    - Call ``hpm_smbus_master_read_word_in_command`` API for word read with command code.

      - ``hpm_smbus_master_read_word_in_command`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_smbus_master_read_word_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint16_t *data);

        - Parameters:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - Pointer to I2C controller base address
                * - slave_address
                  - uint8_t
                  - Slave device address
                * - command
                  - uint8_t
                  - Command code
                * - data
                  - uint16_t*
                  - Pointer to store read data

        - Return values:

          - ``status_success``: Operation successful
          - ``status_invalid_argument``: Invalid parameters
          - ``status_timeout``: Operation timeout
          - ``status_fail``: General failure

      - **Example**: Read word data with command 0x01 from slave 0x16:

        .. code-block:: c

            hpm_stat_t stat;
            uint16_t data;
            /* Initialize I2C (omitted)... */
            stat = hpm_smbus_master_read_word_in_command(TEST_SMBUS, 0x16, 0x01, &data);
            if (stat != status_success) {
                printf("hpm_smbus_master_read_word_in_command failed.\n");
            }

  - Master Block Write with Command Code

    - Call ``hpm_smbus_master_write_block_in_command`` API for block write with command code.

      - ``hpm_smbus_master_write_block_in_command`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_smbus_master_write_block_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t size);

        - Parameters:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - Pointer to I2C controller base address
                * - slave_address
                  - uint8_t
                  - Slave device address
                * - command
                  - uint8_t
                  - Command code
                * - data
                  - uint8_t*
                  - Data buffer to write
                * - size
                  - uint32_t
                  - Data length in bytes

        - Return values:

          - ``status_success``: Operation successful
          - ``status_invalid_argument``: Invalid parameters
          - ``status_timeout``: Operation timeout
          - ``status_fail``: General failure

      - **Example**: Write command 0x01 and 3 bytes to slave 0x16:

        .. code-block:: c

            hpm_stat_t stat;
            uint8_t data[3] = {0x11, 0x22, 0x33};
            /* Initialize I2C (omitted)... */
            stat = hpm_smbus_master_write_block_in_command(TEST_SMBUS, 0x16, 0x01, data, 3);
            if (stat != status_success) {
                printf("hpm_smbus_master_write_block_in_command failed.\n");
            }

  - Master Block Read with Command Code

    - Call ``hpm_smbus_master_read_block_in_command`` API for block read with command code.

      - ``hpm_smbus_master_read_block_in_command`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_smbus_master_read_block_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t size);

        - Parameters:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - Pointer to I2C controller base address
                * - slave_address
                  - uint8_t
                  - Slave device address
                * - command
                  - uint8_t
                  - Command code
                * - data
                  - uint8_t*
                  - Buffer to store read data
                * - size
                  - uint32_t
                  - Data length in bytes

        - Return values:

          - ``status_success``: Operation successful
          - ``status_invalid_argument``: Invalid parameters
          - ``status_timeout``: Operation timeout
          - ``status_fail``: General failure

      - **Example**: Read 3 bytes with command 0x01 from slave 0x16:

        .. code-block:: c

            hpm_stat_t stat;
            uint8_t data[3];
            /* Initialize I2C (omitted)... */
            stat = hpm_smbus_master_read_block_in_command(TEST_SMBUS, 0x16, 0x01, data, 3);
            if (stat != status_success) {
                printf("hpm_smbus_master_read_block_in_command failed.\n");
            }

