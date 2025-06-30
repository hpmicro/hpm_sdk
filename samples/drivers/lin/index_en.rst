.. _lin:

LIN
===

Overview
--------

- This section introduces the main driver interfaces and usage methods of LIN peripheral. For more details, please refer to the API documentation in `hpm_lin_drv.h` and related user manuals.
- Supports LIN specification 2.2A, compatible with LIN 1.3
- Supports master and slave modes:

  - Master mode: operates at data rates from 1Kbps to 20Kbps
  - Slave mode: supports dynamic automatic baud rate detection

- Supports LIN frame format:

  - Header section (sent by master):

    - Break Field: low level exceeding 11 bits, marks frame start
    - Sync Field: fixed at 0x55, used for slave baud rate synchronization
    - Protected Identifier: 6-bit ID value and 2-bit checksum

  - Response section (sent by master or slave):

    - Data Field: 0~8 bytes of data
    - Checksum Field: 1 byte checksum

- Supports multiple features:

  - Bit error detection and warning
  - Checksum generation and verification (classic and enhanced modes)
  - Sleep mode entry and exit
  - Timeout error detection and warning
  - Data buffer up to 8 bytes

LIN Initialization
-------------------

- Ensure the LIN clock source is enabled and LIN peripheral pins are initialized.

  - Use the `clock_add_to_group` function to add the LIN clock source to the clock group.

- Structure Descriptions:

  - `lin_timing_t` structure for configuring LIN timing parameters

    .. code-block:: c

        typedef struct {
            uint32_t src_freq_in_hz;        /* Source clock frequency in Hz */
            uint32_t baudrate;              /* Baud rate */
        } lin_timing_t;

  - `lin_trans_config_t` structure for configuring LIN transfer parameters

    .. code-block:: c

        typedef struct {
            uint8_t id;                     /* ID */
            uint8_t *data_buff;             /* Data buffer */
            bool data_length_from_id;       /* Whether data length should be decoded from ID */
            uint8_t data_length;            /* Data length (used when data_length_from_id is false) */
            bool enhanced_checksum;         /* true: enhanced checksum; false: classic checksum */
            bool transmit;                  /* true: transmit operation; false: receive operation */
        } lin_trans_config_t;

Master Mode
^^^^^^^^^^^^

- Initialization API:

  .. code-block:: c

        hpm_stat_t lin_master_configure_timing(LIN_Type *ptr, lin_timing_t *timing);

  - Parameter description:

    .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - LIN_Type*
              - Pointer to LIN controller base address
            * - timing
              - lin_timing_t*
              - Pointer to LIN timing configuration structure

- Transfer API:

  - Non-blocking Transfer API:

    .. code-block:: c

        void lin_master_transfer(LIN_Type *ptr, lin_trans_config_t *config);

    - Parameter description:

      .. list-table::
              :header-rows: 1

              * - Parameter
                - Type
                - Description
              * - ptr
                - LIN_Type*
                - Pointer to LIN controller base address
              * - config
                - lin_trans_config_t*
                - Pointer to LIN transfer configuration structure

    .. note::
        This is a non-blocking API that returns immediately. Transfer completion status should be checked through interrupts.

  - Blocking Transfer APIs:

    .. code-block:: c

        hpm_stat_t lin_master_sent(LIN_Type *ptr, lin_trans_config_t *config);
        hpm_stat_t lin_master_receive(LIN_Type *ptr, lin_trans_config_t *config);

    These are blocking APIs that wait for transfer completion:

    - `lin_master_sent`: Master sends data, returns after transmission is complete or error occurs
    - `lin_master_receive`: Master receives data, returns after reception is complete or error occurs

Slave Mode
^^^^^^^^^^^

- Initialization API:

  .. code-block:: c

        hpm_stat_t lin_slave_configure_timing(LIN_Type *ptr, uint32_t src_freq_in_hz);

  - Parameter description:

    .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - LIN_Type*
              - Pointer to LIN controller base address
            * - src_freq_in_hz
              - uint32_t
              - Source clock frequency in Hz

- Transfer API:

  - Non-blocking Transfer API:

    .. code-block:: c

        void lin_slave_transfer(LIN_Type *ptr, lin_trans_config_t *config);

    - Parameter description:

      .. list-table::
              :header-rows: 1

              * - Parameter
                - Type
                - Description
              * - ptr
                - LIN_Type*
                - Pointer to LIN controller base address
              * - config
                - lin_trans_config_t*
                - Pointer to LIN transfer configuration structure

    .. note::
        This is a non-blocking API that returns immediately. Transfer completion status should be checked through interrupts.

  - Blocking Transfer APIs:

    .. code-block:: c

        hpm_stat_t lin_slave_sent(LIN_Type *ptr, lin_trans_config_t *config);
        hpm_stat_t lin_slave_receive(LIN_Type *ptr, lin_trans_config_t *config);

    These are blocking APIs that wait for transfer completion:

    - `lin_slave_sent`: Slave sends data, returns after transmission is complete or error occurs
    - `lin_slave_receive`: Slave receives data, returns after reception is complete or error occurs

Sleep and Wakeup
^^^^^^^^^^^^^^^^^

- Sleep Mode:

  - Master can send sleep request frame to make slaves enter sleep mode
  - Slave automatically enters sleep mode after bus inactivity for 4s~10s (configurable)
  - Use `lin_sleep` function to make LIN controller enter sleep mode

- Wakeup Function:

  - Any LIN node can wake up the bus by sending a wakeup signal
  - Use `lin_wakeup` function to send wakeup signal
  - Slave wakeup timeout for waiting master header is configurable (180ms~240ms)

Examples
---------

- Master Mode Example:

  .. code-block:: c

        /* LIN master initialization */
        lin_timing_t timing;
        lin_trans_config_t config;

        timing.src_freq_in_hz = clock_get_frequency(TEST_LIN_CLOCK_NAME);
        timing.baudrate = TEST_LIN_BAUDRATE;
        lin_master_configure_timing(TEST_LIN, &timing);

        /* Send data frame */
        config.id = TEST_LIN_TRANSMIT_ID;
        config.transmit = true;
        config.data_length = 8;
        config.enhanced_checksum = true;
        config.data_buff = sent_buff;
        lin_master_transfer(TEST_LIN, &config);

- Slave Mode Example:

  .. code-block:: c

        /* LIN slave initialization */
        uint32_t freq = clock_get_frequency(TEST_LIN_CLOCK_NAME);
        lin_slave_configure_timing(TEST_LIN, freq);

        /* Respond to master request in interrupt */
        if (status & LIN_STATE_DATA_REQ_MASK) {
            id = lin_get_id(TEST_LIN);
            config.id = id;
            config.transmit = true;
            config.data_length_from_id = true;
            config.enhanced_checksum = true;
            config.data_buff = sent_buff;
            lin_slave_transfer(TEST_LIN, &config);
        }

.. toctree::
   :maxdepth: 3

   master/README_en
   slave/README_en

