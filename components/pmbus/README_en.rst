.. _pmbus:

PMBus
=======

Overview
---------

- PMBus is a bus protocol used for managing and monitoring various physical devices in computer systems, such as processors, memory, storage devices, etc. The PMBus protocol defines a set of standard commands and registers for reading/writing device status information and performing management operations. PMBus is typically compatible with SMBus, enabling deployment across various computer systems.

- The primary objectives of the PMBus protocol are:

  - Provide a unified interface for communication between different physical devices
  - Support multiple device types including processors, memory modules, storage devices, and network interfaces
  - Enable hot-plug detection and event handling
  - Implement device fault diagnosis and recovery mechanisms

- PMBus implementation consists of:

  - Hardware components: Physical buses and devices
  - Software components: Drivers and library functions

    - Drivers handle protocol conversion between PMBus and SMBus (dependent on smbus component)
    - This component provides APIs for device status access and management operations

- Full support for 200+ standard command codes defined in PMBus 1.2 specification:

  - Voltage/current/temperature monitoring
  - Fault response mechanisms
  - Other power management features

- Command parameter configuration table (pmbus_cmd_param_table) definition:

.. code-block:: c

    typedef struct {
        uint8_t command_code;        /* PMBus command code */
        uint32_t data_length;        /* Data length (0 indicates no data) */
        pmbus_transaction_type_t read_transaction_type;  /* Read operation type (byte/word/block) */
        pmbus_transaction_type_t write_transaction_type; /* Write operation type */
    } hpm_pmbus_cmd_param_t;

- System Integration Features:

  - Master/Slave mode support (via underlying I2C/SMBus drivers)
  - Manufacturer extension command support (MFR_SPECIFIC_00-45)
  - Slave mode command handling through interrupt callback mechanism

Enumerations and Structures
------------------------------

- Enumerations

  - Transaction types in PMBUS communication, used to distinguish different data transmission types in PMBUS protocol. Each enumeration value corresponds to standard transaction types defined in PMBUS specification.

        .. code-block:: c

            typedef enum {
                none            = 0,    /**< No operation */
                write_byte      = 1,    /**< Write one byte */
                read_byte       = 2,    /**< Read one byte */
                write_word      = 3,    /**< Write one word (2 bytes) */
                read_word       = 4,    /**< Read one word (2 bytes) */
                write_block     = 5,    /**< Write data block */
                read_block      = 6,    /**< Read data block */
                send_byte       = 7,    /**< Send single byte */
                mfr_defined     = 8,    /**< Manufacturer-defined command */
                extended_command = 9,   /**< Extended command */
            } hpm_pmbus_transaction_type;

  - PMBUS status codes defining error states for PMBUS component.

        .. code-block:: c

            enum {
                status_pmbus_not_support_cmd = MAKE_STATUS(status_group_pmbus, 1),        /**< Unsupported PMBus command */
                status_pmbus_not_transaction_type = MAKE_STATUS(status_group_pmbus, 2),  /**< Unsupported transaction type */
            };

  - Callback function type definition for PMBUS operation completion.

        .. code-block:: c

            typedef void (*hpm_pmbus_complete_cb_t)(I2C_Type *base, hpm_pmbus_complete_cb_cfg_t *cfg);

        - Parameter description:

            .. list-table::
                :widths: 20 30 50
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - base
                  - ``I2C_Type *``
                  - Pointer to I2C controller base address
                * - cfg
                  - ``hpm_pmbus_complete_cb_cfg_t *``
                  - Pointer to completion callback configuration structure

- Structures

  - PMBUS command completion callback configuration structure.

    .. code-block:: c

        typedef struct {
            uint8_t command; /**< Currently processed PMBus command code (e.g. VOUT_COMMAND=0x21) */
            uint32_t len;    /**< Valid data length in bytes */
            bool read;       /**< Operation direction: true=read, false=write */
            bool pec_check;  /**< Packet Error Checking (PEC) enabled */
            uint8_t *data;   /**< Data buffer pointer:
                                - Write: Points to sent data
                                - Read: Points to received data */
        } hpm_pmbus_complete_cb_cfg_t;

  - PMBus command parameter structure.

    .. code-block:: c

        typedef struct {
            /**< PMBus command code */
            uint8_t command_code;
            /**< Write transaction type */
            hpm_pmbus_transaction_type write_transaction_type;
            /**< Read transaction type */
            hpm_pmbus_transaction_type read_transaction_type;
            /**< Associated data length in bytes (0 indicates no data) */
            uint32_t data_length;
        } hpm_pmbus_cmd_param_t;

  - PMBus slave configuration structure.

    .. code-block:: c

        typedef struct {
            bool is_valid;        /**< Configuration validity flag */
            uint8_t *wdata;      /**< Write data pointer (Host→Slave) */
            uint8_t *rdata;      /**< Read data buffer pointer (Slave→Host) */
            uint32_t data_length; /**< Data transfer length in bytes */
            hpm_pmbus_complete_cb_t callback; /**< Transfer completion callback */
        } hpm_pmbus_slave_cfg_t;

API Workflow
---------------------

PMBus Initialization
^^^^^^^^^^^^^^^^^^^^

Master Mode
""""""""""""""""""""

- Use ``i2c_init_master`` API to initialize I2C in master mode (defined in ``hpm_i2c_drv`` driver)

    - API prototype:

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
              - Pointer to I2C configuration structure

    - Return values:
        - ``status_success``: Initialization successful
        - ``status_invalid_argument``: Invalid parameter

- **Example**: Initialize I2C0 in master mode (100KHz, 7-bit address)

    .. code-block:: c

        #define TEST_PMBUS                HPM_I2C0
        #define TEST_PMBUS_CLOCK_NAME     clock_i2c0

        i2c_config_t config;
        board_init_i2c_clock(TEST_PMBUS);
        init_i2c_pins(TEST_PMBUS);

        config.i2c_mode = i2c_mode_normal;
        config.is_10bit_addressing = false;
        uint32_t freq = clock_get_frequency(TEST_PMBUS_CLOCK_NAME);
        hpm_stat_t stat = i2c_init_master(TEST_PMBUS, freq, &config);
        if (stat != status_success) {
            return stat;
        }

Slave Mode
""""""""""""""""""""

- Requires ``i2c_init_slave`` for basic I2C configuration and ``hpm_pmbus_slave_init`` to enable PMBus-specific interrupts

    - Initialize I2C in slave mode using ``i2c_init_slave``:

        .. code-block:: c

            hpm_stat_t i2c_init_slave(I2C_Type *ptr, uint32_t src_clk_in_hz, i2c_config_t *config);

        - Parameters identical to master mode API
        - Return values:
            - ``status_success``: Initialization successful
            - ``status_invalid_argument``: Invalid parameter

    - PMBus slave initialization with ``hpm_pmbus_slave_init``:

        .. code-block:: c

            hpm_stat_t hpm_pmbus_slave_init(I2C_Type *ptr, uint8_t slave_addr);

        - Parameters:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - ptr
                  - I2C_Type*
                  - I2C controller instance
                * - slave_addr
                  - uint8_t
                  - 7-bit slave address

        - Enables critical interrupts:
            - Address match
            - Transfer completion

    - Interrupt handler prototype:

        .. code-block:: c

            void hpm_pmbus_isr_handler(I2C_Type *ptr);

- **Example**: Configure I2C0 as PMBus slave (0x16 address, 100KHz)

    .. code-block:: c

        #define TEST_PMBUS                HPM_I2C0
        #define TEST_PMBUS_CLOCK_NAME     clock_i2c0
        #define TEST_I2C_IRQ              IRQn_I2C0
        #define TEST_PMBUS_SLAVE_ADDRESS  (0x16U)

        SDK_DECLARE_EXT_ISR_M(TEST_I2C_IRQ, i2c_isr)
        void i2c_isr(void) {
            hpm_pmbus_isr_handler(TEST_PMBUS);
        }

        void init_slave() {
            i2c_config_t config;
            board_init_i2c_clock(TEST_PMBUS);
            init_i2c_pins(TEST_PMBUS);

            config.i2c_mode = i2c_mode_slave;
            config.is_10bit_addressing = false;
            config.slave_address = 0x16;

            uint32_t freq = clock_get_frequency(TEST_PMBUS_CLOCK_NAME);
            hpm_stat_t stat = i2c_init_slave(TEST_PMBUS, freq, &config);
            if (stat != status_success) {
                return stat;
            }

            intc_m_enable_irq_with_priority(TEST_I2C_IRQ, 1);
            hpm_pmbus_slave_init(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS);
        }

Read/Write Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Master Mode
"""""""""""""""

Write Operation
~~~~~~~~~~~~~~~~~~~~

- Use ``hpm_pmbus_master_write`` API for write operations

    - **Key features**:

        - Retrieves predefined command parameters from pmbus_cmd_param_table[command]:
            - Transaction type (byte/word/block)
            - Expected data length
            - Protocol parameters

        - Supports 5 write modes:
            - send_byte: Command byte only (no data)
            - write_byte: Command + 1 byte data
            - write_word: Command + 2 bytes data
            - write_block: Command + length byte + data block

        - Block write handling:
            - 0xFFFFFFFF: Dynamic length mode (uses caller's len parameter)
            - Other values: Use predefined fixed length
            - Auto-handles PMBus length byte prefix

        - Error codes:
            - status_pmbus_not_support_cmd: Unsupported command
            - status_pmbus_not_transaction_type: Invalid transaction type

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_pmbus_master_write(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t len);

    - Parameters:

        .. list-table::
            :widths: 15 15 70
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - I2C_Type*
              - I2C controller base address
            * - slave_address
              - uint8_t
              - 7-bit slave address
            * - command
              - uint8_t
              - PMBus command code (e.g. 0x21 for VOUT_COMMAND)
            * - data
              - uint8_t*
              - Data buffer (format depends on command type)
            * - len
              - uint32_t
              - Data length (effective for block writes only)

    - Returns:

        - ``status_success``: Operation successful
        - ``status_pmbus_not_support_cmd``: Unsupported PMBus command
        - ``status_pmbus_not_transaction_type``: Invalid transaction type

    - Example: Set output voltage to 3.3V (write word 0x0D00):

        .. code-block:: c

            /* PMBus initialization omitted */
            uint16_t voltage = 0x0D00;
            hpm_stat_t status = hpm_pmbus_master_write(I2C0, 0x5A, PMBUS_CODE_VOUT_COMMAND, (uint8_t*)&voltage, 2);

Read Operation
~~~~~~~~~~~~~~

- Use ``hpm_pmbus_master_read`` API for read operations

    - **Key features**:

        - Supports three read modes:

            - Byte read: Single byte status registers
            - Word read: 16-bit measurements (voltage/current)
            - Block read: Large data chunks (logs etc.)

        - Length handling:

            - Byte/word: Fixed 1/2 byte returns
            - Block read:

                - 0xFFFFFFFF: Dynamic length mode (uses caller's len parameter)
                - Other values: Use predefined length

        - Error codes:

            - status_pmbus_not_support_cmd: Unsupported command
            - status_pmbus_not_transaction_type: Invalid transaction type

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_pmbus_master_read(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t *len);

    - Parameters:

        .. list-table::
            :widths: 15 15 70
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - I2C_Type*
              - I2C controller base address
            * - slave_address
              - uint8_t
              - 7-bit slave address
            * - command
              - uint8_t
              - PMBus command code
            * - data
              - uint8_t*
              - Data receive buffer pointer
            * - len
              - uint32_t*
              - Input: buffer capacity / Output: actual data length

    - Returns:
        - ``status_success``: Operation successful
        - ``status_pmbus_not_support_cmd``: Unsupported PMBus command
        - ``status_pmbus_not_transaction_type``: Invalid transaction type

    - Example: Read output voltage:

        .. code-block:: c

            /* PMBus initialization omitted */
            uint8_t vout_data[2];
            uint32_t read_len = 2;
            hpm_stat_t status = hpm_pmbus_master_read(I2C0, 0x5A, PMBUS_CODE_VOUT_COMMAND, vout_data, &read_len);
            if (status == status_success) {
                float voltage = (vout_data[0] | (vout_data[1] << 8)) * 0.001; /* Convert to voltage value */
            }

Slave Mode
""""""""""""""""

Slave Command Transaction Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Command Transaction Installation
++++++++++++++++++++++++++++++++

- Use ``hpm_pmbus_slave_command_transaction_install`` to configure slave command response

    - **Key features**:

        - Pre-bind PMBus commands with data buffers
        - Register transaction completion callback
        - Supports standard command codes (0x00-0xFF)
        - Auto-handles PEC checksum space allocation

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_pmbus_slave_command_transaction_install(
                I2C_Type *ptr,
                uint8_t command,
                uint8_t *wdata,
                uint8_t *rdata,
                uint32_t len,
                hpm_pmbus_complete_cb_t callback
            );

    - Parameters:

        .. list-table::
            :widths: 20 25 55
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - I2C_Type*
              - Bound I2C controller instance (e.g. HPM_I2C0)
            * - command
              - uint8_t
              - PMBus standard command code
            * - wdata
              - uint8_t*
              - Host write buffer (slave receive), must reserve PEC space
            * - rdata
              - uint8_t*
              - Host read buffer (slave transmit), must reserve PEC space
            * - len
              - uint32_t
              - Total buffer length (data + PEC checksum)
            * - callback
              - hpm_pmbus_complete_cb_t
              - Transaction completion callback pointer

    - Returns:
        - ``status_success``: Configuration successful
        - ``status_invalid_argument``: Null pointer/insufficient buffer
        - ``status_pmbus_not_support_cmd``: Unsupported command type

Command Transaction Uninstallation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Use ``hpm_pmbus_slave_command_transaction_uninstall`` to remove command configuration

    - **Key features**:

        - Release command slot resources
        - Clear associated callback functions
        - Disable specified command response

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_pmbus_slave_command_transaction_uninstall(
                I2C_Type *ptr,
                uint8_t command
            );

    - Parameters:

        .. list-table::
            :widths: 25 25 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - I2C_Type*
              - I2C controller instance (must match installation)
            * - command
              - uint8_t
              - PMBus command code to remove

    - Returns:
        - ``status_success``: Uninstallation successful
        - ``status_invalid_argument``: Invalid controller pointer

Application Example
~~~~~~~~~~~~~~~~~~~~

-  Slave Voltage Monitoring (Respond to READ_VOUT)

.. code-block:: c

    /* Command parameters */
    #define VOUT_CMD      PMBUS_CMD_READ_VOUT  /* 0x8B */
    #define DATA_BUF_LEN  3                   /* 2 data bytes + 1 PEC */

    uint8_t vout_data[DATA_BUF_LEN] = {0};    /* Voltage storage (little-endian) */

    /* Voltage read callback */
    void vout_callback(I2C_Type *base, hpm_pmbus_complete_cb_cfg_t *cfg)
    {
        if (cfg->read) {
            printf("Host read %d voltage bytes\n", cfg->len);
        } else {
            printf("Host wrote %d config bytes\n", cfg->len);
        }
    }

    /* System initialization */
    void pmbus_slave_init(void)
    {
        /* Initialize I2C slave mode */
        i2c_init_slave(TEST_PMBUS, freq, &config);
        hpm_pmbus_slave_init(TEST_PMBUS, 0x16);

        /* Install voltage read transaction */
        hpm_pmbus_slave_command_transaction_install(
            TEST_PMBUS,
            VOUT_CMD,
            NULL,       /* Ignore host write */
            vout_data,  /* Read data buffer */
            DATA_BUF_LEN,
            vout_callback
        );

        /* Set initial voltage (11.5V = 0x47E)
        vout_data[0] = 0xE0 | (0x47 & 0x1F);  /* Exponent */
        vout_data[1] = (0x47 >> 5);           /* Mantissa */
    }

    /* deinit */
    void pmbus_slave_deinit(void)
    {
        hpm_pmbus_slave_command_transaction_uninstall(TEST_PMBUS, VOUT_CMD);
    }

