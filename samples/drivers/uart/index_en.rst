.. _uart:

UART
====

Overview
-----------

- Key Features

  - This section introduces main driver interfaces and usage of HPMicro UART peripherals. For more details, refer to API documentation in `hpm_uart_drv.h` and related user manuals.

  - Supports 5 to 9 data bits with address matching. See :ref:`uart_9bit <uart_9bit>` example. 9-bit data mode availability depends on `HPM_IP_FEATURE_UART_9BIT_MODE` macro in SOC-specific `hpm_soc_ip_feature.h`

  - Supports DMA data transfer. See :ref:`uart_dma <use_dma_to_receive_and_send_uart_data>` example

  - Supports hardware RX idle and TX space detection. See :ref:`uart_hardware_rx_idle <uart_hardware_rx_idle_detection>` example. Availability depends on `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` macro in SOC-specific `hpm_soc_ip_feature.h`

  - Supports various interrupts. See :ref:`uart_irq <uart_interrupt>` example

  - Supports multiple error detection during reception (parity error, FIFO overflow, etc.). See :ref:`uart_rx_line_status <uart_rxline_status>` example

  - Configurable stop bits: 1, 1.5, 2 bits

  - Configurable parity: None, Odd, Even

  - Configurable baudrate

  - Hardware flow control support

  - Built-in up to 32-byte hardware FIFO for RX/TX (actual depth varies by SOC). Check `UART_SOC_FIFO_SIZE` macro in `hpm_soc_feature.h` or use `uart_intr_tx_slot_avail` API in `hpm_uart_drv.h`

UART Initialization
-------------------

- Ensure UART clock source is enabled and related pins are initialized

  - Use `clock_add_to_group` function to add UART clock source to clock group

- Enumerations:

  - `parity_setting_t` enumeration for parity configuration:

    .. code-block:: c

        typedef enum parity {

            /**< No parity */
            parity_none = 0,

            /**< Odd parity */
            parity_odd,

            /**< Even parity */
            parity_even,

            /**< Parity bit fixed to 1 */
            parity_always_1,

            /**< Parity bit fixed to 0 */
            parity_always_0,

        } parity_setting_t;

  - `num_of_stop_bits_t` enumeration for stop bits configuration:

    .. code-block:: c

        typedef enum num_of_stop_bits {

            /**< 1 stop bit */
            stop_bits_1 = 0,

            /**< 1.5 stop bits */
            stop_bits_1_5,

            /**< 2 stop bits */
            stop_bits_2,

        } num_of_stop_bits_t;

  - `word_length_t` enumeration for data bits configuration:

    .. code-block:: c

        typedef enum word_length {

            /**< 5 data bits */
            word_length_5_bits = 0,

            /**< 6 data bits */
            word_length_6_bits,

            /**< 7 data bits */
            word_length_7_bits,

            /**< 8 data bits */
            word_length_8_bits,

        } word_length_t;

  - `uart_rxline_idle_cond_t` enumeration for RX/TX line idle detection:

    - Available only when SOC supports RX Idle detection (check `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` in `hpm_soc_ip_feature.h`)

    .. code-block:: c

        typedef enum hpm_uart_rxline_idle_cond {

            /**< Treat as idle when RX line remains high beyond threshold duration */
            uart_rxline_idle_cond_rxline_logic_one = 0,

            /**< Treat as idle when RX state machine stays idle beyond threshold duration */
            uart_rxline_idle_cond_state_machine_idle = 1

        } uart_rxline_idle_cond_t;

- Structures:

  - `uart_modem_config_t` structure for hardware flow control:

    .. code-block:: c

        typedef struct uart_modem_config {

            /**< Auto flow control enable flag */
            bool auto_flow_ctrl_en;

            /**< Loopback mode enable flag */
            bool loop_back_en;

            /**< Set RTS signal level high flag */
            bool set_rts_high;
        } uart_modem_config_t;

  - `uart_rxline_idle_config_t` structure for idle detection:

    - Available only when SOC supports RX Idle detection (check `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` in `hpm_soc_ip_feature.h`)

    .. code-block:: c

        typedef struct hpm_uart_rxline_idle_detect_config {

            /**< RX/TX line idle detection enable flag */
            bool detect_enable;

            /**< RX/TX line idle detection interrupt enable flag */
            bool detect_irq_enable;

            /**< RX/TX line idle detection condition */
            uart_rxline_idle_cond_t idle_cond;

            /**< RX/TX line idle detection threshold (bit time units) */
            uint8_t threshold;

        } uart_rxline_idle_config_t;

  - `uart_config_t` structure for UART configuration:

    .. code-block:: c

        typedef struct hpm_uart_config {
            /**< UART clock source frequency (Hz) */
            uint32_t src_freq_in_hz;

            /**< Baudrate */
            uint32_t baudrate;

            /**< Number of stop bits */
            uint8_t num_of_stop_bits;

            /**< Data bits */
            uint8_t word_length;

            /**< Parity configuration */
            uint8_t parity;

            /**< TX FIFO trigger level for interrupt/DMA */
            uint8_t tx_fifo_level;

            /**< RX FIFO trigger level for interrupt/DMA */
            uint8_t rx_fifo_level;

            /**< DMA enable flag */
            bool dma_enable;

            /**< FIFO enable flag */
            bool fifo_enable;

            /**< Hardware flow control configuration */
            uart_modem_config_t modem_config;

            /**< RX idle detection configuration (if supported) */
        #if defined(HPM_IP_FEATURE_UART_RX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_RX_IDLE_DETECT == 1)
            uart_rxline_idle_config_t  rxidle_config;
        #endif

            /**< TX idle detection configuration (if supported) */
        #if defined(HPM_IP_FEATURE_UART_TX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_TX_IDLE_DETECT == 1)
            uart_rxline_idle_config_t  txidle_config;
        #endif

            /**< RX enable configuration (if supported) */
        #if defined(HPM_IP_FEATURE_UART_RX_EN) && (HPM_IP_FEATURE_UART_RX_EN == 1)
            /**< Prevents false reception from RX pin glitches. Default RX signal is forced high during power-up */
            bool rx_enable;
        #endif
        } uart_config_t;

    - Member descriptions:

      - `fifo_enable`: Enables FIFO mode (true: FIFO mode, false: single-byte buffer mode)

      - `dma_enable`: Enables DMA transfer (triggers DMA request when FIFO reaches threshold)

      - `tx_fifo_level`: TX FIFO trigger level for interrupt/DMA (refer to `uart_fifo_trg_lvl_t`). Two FIFO threshold width modes.

        - **2-bit width thresholds** (0-3):

          - TX FIFO Thresholds:

            .. list-table::
                :header-rows: 1

                * - Enum
                  - 16-byte FIFO
                  - 32-byte FIFO
                * - uart_tx_fifo_trg_not_full
                  - Not full
                  - Not full
                * - uart_tx_fifo_trg_lt_three_quarters
                  - <12 bytes
                  - <24 bytes
                * - uart_tx_fifo_trg_lt_half
                  - <8 bytes
                  - <16 bytes
                * - uart_tx_fifo_trg_lt_one_quarter
                  - <4 bytes
                  - <8 bytes

          - Examples:

            - `tx_fifo_level = uart_tx_fifo_trg_lt_half`: Triggers DMA when TX FIFO < 8 bytes (16-byte FIFO)

        - **4-bit width thresholds** (0-31) for 1-32 byte granularity:

          - Requires `HPM_IP_FEATURE_UART_FINE_FIFO_THRLD` in `hpm_soc_ip_feature.h`

          - Examples:

            - `tx_fifo_level = 15`: Triggers DMA when TX FIFO <= 15 bytes (32-byte FIFO)

      - `rx_fifo_level`: RX FIFO trigger level for interrupt/DMA (refer to `uart_fifo_trg_lvl_t`)

        - **2-bit width thresholds** (0-3):

          - RX FIFO Thresholds:

            .. list-table::
                :header-rows: 1

                * - Enum
                  - 16-byte FIFO
                  - 32-byte FIFO
                * - uart_rx_fifo_trg_not_empty
                  - Not empty
                  - Not empty
                * - uart_rx_fifo_trg_gt_one_quarter
                  - >3 bytes
                  - >7 bytes
                * - uart_rx_fifo_trg_gt_half
                  - >7 bytes
                  - >15 bytes
                * - uart_rx_fifo_trg_gt_three_quarters
                  - >13 bytes
                  - >27 bytes

          - Examples:

            - `rx_fifo_level = uart_rx_fifo_trg_gt_one_quarter`: Triggers DMA when RX FIFO >3 bytes (16-byte FIFO)

        - **4-bit width thresholds** (0-31) for 1-32 byte granularity:

          - Requires `HPM_IP_FEATURE_UART_FINE_FIFO_THRLD` in `hpm_soc_ip_feature.h`

          - Examples:

            - `rx_fifo_level = 27`: Triggers DMA when RX FIFO >27 bytes (32-byte FIFO)

      - `rxidle_config`: RX idle detection parameters (SOC-dependent):

        - `detect_enable`: Enables RX idle detection
        - `detect_irq_enable`: Enables RX idle interrupt
        - `idle_cond`: Idle detection condition
        - `threshold`: Idle detection threshold (bit time units)

      - `txidle_config`: TX idle detection parameters (SOC-dependent), same as `rxidle_config`

- Initialization APIs:

  - `uart_default_config` function for default parameters:

    .. code-block:: c

        void uart_default_config(UART_Type *ptr, uart_config_t *config);

    - Parameters:

      .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type*
          - UART controller base address
        * - config
          - uart_config_t*
          - Pointer to configuration structure

    - Default values:
      - Baudrate: 115200
      - 8 data bits
      - 1 stop bit
      - No parity
      - FIFO enabled
      - DMA disabled
      - RX idle detection enabled (if supported)

  - Core initialization function:

    .. code-block:: c

        hpm_stat_t uart_init(UART_Type *ptr, uart_config_t *config);

    - Parameters:

      .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type*
          - UART controller base address
        * - config
          - uart_config_t*
          - Pointer to configuration structure

    - Return values:

      .. list-table::
        :header-rows: 1

        * - Value
          - Description
        * - status_success
          - Initialization successful
        * - status_uart_no_suitable_baudrate_parameter_found
          - Invalid baudrate parameters
        * - status_invalid_argument
          - Invalid configuration

    - **Notes**:

      1. Ensure clock source is properly configured
      2. Verify parameters are within valid ranges
      3. Enable SOC-specific macros for advanced features

    - Example:

      .. code-block:: c

          void uart_init_sample(void)
          {
              uart_config_t config;
              uart_default_config(HPM_UART0, &config);
              config.src_freq_in_hz = BOARD_UART_CLK_FREQ;
              config.baudrate = 115200;
              hpm_stat_t status = uart_init(HPM_UART0, &config);
              if (status != status_success) {
                  printf("UART initialization failed\n");
              }
          }


UART Transmission
-----------------

- Supports FIFO and DMA transmission modes

Transmission
^^^^^^^^^^^^

- Supports polling mode and interrupt-driven FIFO transmission
- For interrupt mode:
  - Enable UART interrupts using `intc_m_enable_irq` API
  - Set interrupt priority using `intc_m_enable_irq_with_priority` API

Polling Mode
""""""""""""

- Single-byte transmit API:

  .. code-block:: c

      hpm_stat_t uart_send_byte(UART_Type *ptr, uint8_t c);

  - Parameters:

    .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type*
          - Pointer to UART controller base address
        * - data
          - uint8_t
          - Data to transmit

  - Return values:

    .. list-table::
        :header-rows: 1

        * - Value
          - Description
        * - status_success
          - Transmission successful
        * - status_timeout
          - Transmission timeout

  - **Note**: This API blocks until FIFO transmission completes

  - Example:

    .. code-block:: c

        void send_data_sample(void)
        {
            uint8_t data = 0x12;
            hpm_stat_t status = uart_send_byte(uart0, data);
            if (status == status_success) {
                printf("Data sent successfully\n");
            } else {
                printf("Data transmission failed\n");
            }
        }

- Multi-byte transmit API:

  .. code-block:: c

      hpm_stat_t uart_send_data(UART_Type *ptr, uint8_t *source, uint32_t size_in_byte);

  - Parameters:

    .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type*
          - Pointer to UART controller
        * - source
          - uint8_t*
          - Data buffer to transmit
        * - size_in_byte
          - uint32_t
          - Data length in bytes

  - Return values:

    .. list-table::
        :header-rows: 1

        * - Value
          - Description
        * - status_success
          - Transmission successful
        * - status_timeout
          - Transmission timeout

  - **Note**: This API blocks until FIFO transmission completes

  - Example:

    .. code-block:: c

        void send_data_sample(void)
        {
            uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
            hpm_stat_t status = uart_send_data(uart0, data, sizeof(data));
            if (status == status_success) {
                printf("Data sent successfully\n");
            } else {
                printf("Data transmission failed\n");
            }
        }

Interrupt Mode
""""""""""""""

- HPM UART features up to 32-byte TX FIFO
- Implement interrupt-driven transmission using TX empty interrupt

- Non-blocking single-byte write API:

  .. code-block:: c

      void uart_write_byte(UART_Type *ptr, uint8_t c)

  - Parameters:

    .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type*
          - UART controller base address
        * - data
          - uint8_t
          - Data to transmit

  - Example:

    .. code-block:: c

        #define UART_TX_SEND_SIZE 100
        uint8_t tx_buffer[UART_TX_SEND_SIZE];
        uint32_t tx_buffer_index = 0;
        uint8_t tx_fifo_size = 0;

        /* ISR implementation */
        void uart0_isr(void)
        {
            uint8_t irq_id = uart_get_irq_id(TEST_UART);
            if (irq_id & uart_intr_id_tx_slot_avail) {
                uart_disable_irq(HPM_UART0, uart_intr_tx_slot_avail);

                uint8_t send_size = MIN(tx_fifo_size, UART_TX_SEND_SIZE - tx_buffer_index);
                while (send_size--) {
                    uart_write_byte(HPM_UART0, tx_buffer[tx_buffer_index++]);
                }

                if (tx_buffer_index < UART_TX_SEND_SIZE) {
                    uart_enable_irq(HPM_UART0, uart_intr_tx_slot_avail);
                }
            }
        }

        void send_data_sample(void)
        {
            uart_disable_irq(HPM_UART0, uart_intr_tx_slot_avail);
            tx_fifo_size = uart_get_fifo_size(HPM_UART0, uart_intr_tx_slot_avail);

            uint8_t send_size = MIN(tx_fifo_size, UART_TX_SEND_SIZE);
            while (tx_buffer_index < send_size) {
                uart_write_byte(HPM_UART0, tx_buffer[tx_buffer_index++]);
            }

            uart_enable_irq(HPM_UART0, uart_intr_tx_slot_avail);
        }

  - **Note**:

    - `uart_intr_tx_slot_avail` indicates TX FIFO empty interrupt
    - Use `uart_enable_rxline_idle_detection` for actual transmission completion detection

DMA Mode
""""""""
  - Refer to :ref:`uart_dma <use_dma_to_receive_and_send_uart_data>` example

UART Reception
--------------

- Supports FIFO and DMA reception modes

Reception
^^^^^^^^^

- Supports polling mode and interrupt-driven FIFO reception
- For interrupt mode:
  - Enable UART interrupts using `intc_m_enable_irq` API
  - Set interrupt priority using `intc_m_enable_irq_with_priority` API

Polling Mode
""""""""""""

- Single-byte receive API:

  .. code-block:: c

      hpm_stat_t uart_receive_byte(UART_Type *ptr, uint8_t *byte);

  - Parameters:

    .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type*
          - Pointer to UART controller
        * - byte
          - uint8_t*
          - Pointer to received data

  - Return values:

    .. list-table::
        :header-rows: 1

        * - Value
          - Description
        * - status_success
          - Reception successful
        * - status_timeout
          - Reception timeout

  - **Note**: This API blocks until FIFO data is available

  - Example:

    .. code-block:: c

        void receive_data_sample(void)
        {
            uint8_t data;
            hpm_stat_t status = uart_receive_byte(uart0, &data);
            if (status == status_success) {
                printf("Received: 0x%02X\n", data);
            }
        }

- Multi-byte receive API:

  .. code-block:: c

      hpm_stat_t uart_receive_data(UART_Type *ptr, uint8_t *dest, uint32_t size_in_byte);

  - Parameters:

    .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type*
          - UART controller base address
        * - dest
          - uint8_t*
          - Receive buffer
        * - size_in_byte
          - uint32_t
          - Data length to receive

  - Return values:

    .. list-table::
        :header-rows: 1

        * - Value
          - Description
        * - status_success
          - Full data received
        * - status_fail
          - Reception failed

  - Example:

    .. code-block:: c

        void bulk_receive_sample(void)
        {
            uint8_t buffer[100];
            if (uart_receive_data(uart0, buffer, sizeof(buffer)) == status_success) {
                printf("Received %d bytes\n", sizeof(buffer));
            }
        }

Interrupt Mode
""""""""""""""

- HPM UART features up to 32-byte RX FIFO with two interrupt modes:
  - Data available interrupt: Triggers when FIFO reaches threshold
  - Timeout interrupt: Triggers when no data movement in FIFO for four character times

- Single-byte read API:

  .. code-block:: c

      uint8_t uart_read_byte(UART_Type *ptr);

  - Parameters:

    .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type*
          - UART controller base address

  - Return:
    - The received byte

  - Example: See :ref:`uart_rx_timeout <uart_rx_timeout>`

DMA Mode
""""""""
  - Refer to :ref:`uart_dma <use_dma_to_receive_and_send_uart_data>` example

Hardware Flow Control
-----------------------

- Supports CTS and RTS hardware flow control

- Structure definition:

  .. code-block:: c

        /* UART modem configuration structure */
        typedef struct uart_modem_config {
            /**< Auto flow control enable */
            bool auto_flow_ctrl_en;

            /**< Internal RX TX loopback enable */
            bool loop_back_en;

            /**< RTS signal level control */
            bool set_rts_high;
        } uart_modem_config_t;

  - **Important Notes**

    - Set `loop_back_en` to false when loopback functionality is not required
    - Hardware CTS flow control will be enabled when `auto_flow_ctrl_en` is set to true
    - For CTS-only configuration: set both `set_rts_high` and `auto_flow_ctrl_en` to true
    - For combined CTS/RTS configuration: set `set_rts_high` to false and `auto_flow_ctrl_en` to true

- Configuration API:

  - Enable hardware flow control:

    .. code-block:: c

        void uart_modem_config(UART_Type *ptr, uart_modem_config_t *config);

  - Parameter description:

    .. list-table::
        :header-rows: 1

        * - Parameter
          - Type
          - Description
        * - ptr
          - UART_Type *
          - Pointer to UART controller base address
        * - config
          - uart_modem_config_t *
          - Pointer to modem config structure containing flow control parameters

  - Example: Enable RTS/CTS hardware flow control with loopback disabled

    .. code-block:: c

        void uart_modem_config_sample(void)
        {
            uart_modem_config_t config = {
                .auto_flow_ctrl_en = true,
                .loop_back_en = false,
                .set_rts_high = false
            };
            uart_modem_config(HPM_UART0, &config);
        }

CTS Flow Control Implementation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- To monitor CTS status via interrupts:

  - **Critical Requirements**

    - Hardware flow control **MUST** be disabled (`auto_flow_ctrl_en = false`) when using interrupt mode
    - When hardware flow control is enabled, modem status interrupts will be blocked

  - Example: CTS status monitoring via interrupt

    .. code-block:: c

            SDK_DECLARE_EXT_ISR_M(IRQn_UART0, uart_isr)

            void uart_isr(void)
            {
                uint32_t irq_id = uart_get_irq_id(HPM_UART0);
                if (irq_id == uart_intr_id_modem_stat) {
                    /* Read and clear modem status register */
                    uint8_t cts_status = (uart_get_modem_status(HPM_UART0)
                                       & UART_MSR_DCTS_MASK) >> UART_MSR_DCTS_SHIFT;
                    printf("CTS state: 0x%02x\r\n", cts_status);
                }
            }

            void uart_modem_config_sample(void)
            {
                /* Enable modem status interrupt */
                uart_enable_irq(HPM_UART0, uart_intr_modem_stat);
                /* Set interrupt priority */
                intc_m_enable_irq_with_priority(IRQn_UART0, 2);
                /* Configure modem without auto flow control */
                uart_modem_config_t config = {
                    .auto_flow_ctrl_en = false,
                    .loop_back_en = false
                };
                uart_modem_config(HPM_UART0, &config);
            }

Interrupt
^^^^^^^^^^^^^^

- The UART interrupt corresponding to the PLIC controller needs to be enabled. Use the `intc_m_enable_irq` API to enable the UART interrupt.
- The priority of the UART interrupt needs to be configured in the PLIC controller. Use the `intc_m_enable_irq_with_priority` API to configure the priority of the UART interrupt.
- Each time a corresponding interrupt event occurs, the `uart_get_irq_id` API can be called in the interrupt handler to obtain the interrupt ID. For example, if the relevant interrupt is enabled as `uart_intr_rx_data_avail_or_timeout`, call `uart_get_irq_id` to check if the interrupt ID is `uart_intr_id_rx_data_avail`.
- The UART peripheral supports the following interrupts, which can be viewed from the `uart_intr_enable` enumeration type.

  .. code-block:: c

        typedef enum uart_intr_enable {
            uart_intr_rx_data_avail_or_timeout = UART_IER_ERBI_MASK,   /* Interrupt when received valid data reaches FIFO threshold or FIFO timeout */
            uart_intr_tx_slot_avail = UART_IER_ETHEI_MASK,             /* Interrupt when transmit FIFO is empty */
            uart_intr_rx_line_stat = UART_IER_ELSI_MASK,               /* Receive line status interrupt */
            uart_intr_modem_stat = UART_IER_EMSI_MASK,                 /* Flow control status interrupt */

            /* If the current SOC supports UART idle interrupt */
        #if defined(HPM_IP_FEATURE_UART_RX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_RX_IDLE_DETECT == 1)
            uart_intr_rx_line_idle = UART_IER_ERXIDLE_MASK,           /* Receive idle interrupt */
        #endif
            /* If the current SOC supports UART idle interrupt */
        #if defined(HPM_IP_FEATURE_UART_TX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_TX_IDLE_DETECT == 1)
            uart_intr_tx_line_idle = UART_IER_ETXIDLE_MASK,           /* Transmit idle interrupt */
        #endif

            /* If the current SOC supports receive address match function */
        #if defined(HPM_IP_FEATURE_UART_ADDR_MATCH) && (HPM_IP_FEATURE_UART_ADDR_MATCH == 1)
            uart_intr_addr_match = UART_IER_EADDRM_MASK,                   /*  Receive address match interrupt */
            uart_intr_addr_match_and_rxidle = UART_IER_EADDRM_IDLE_MASK,   /*  Receive address match and receive idle interrupt */
            uart_intr_addr_datalost = UART_IER_EDATLOST_MASK,              /*  Receive address match data lost interrupt */
        #endif

        /*  If the current SOC supports receive status detection */
        #if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
            uart_intr_errf = UART_IER_LSR_ERRF_IRQ_EN_MASK,               /*  Receive status error interrupt */
            uart_intr_break_err = UART_IER_LSR_BREAK_IRQ_EN_MASK,         /*  Transmission break interrupt */
            uart_intr_framing_err = UART_IER_LSR_FRAMING_IRQ_EN_MASK,     /*  Frame format error interrupt */
            uart_intr_parity_err = UART_IER_LSR_PARITY_IRQ_EN_MASK,       /*  Parity error interrupt */
            uart_intr_overrun = UART_IER_LSR_OVERRUN_IRQ_EN_MASK,         /*  Receive data overrun interrupt */
        #endif
        } uart_intr_enable_t;

- UART interrupt status enumeration definitions

  .. code-block:: c

        typedef enum uart_intr_id {
            uart_intr_id_modem_stat = 0x0,      /*  MODEM status interrupt */
            uart_intr_id_tx_slot_avail = 0x2,   /*  Transmit FIFO empty interrupt, does not indicate transmission shift register completion. */
            uart_intr_id_rx_data_avail = 0x4,   /*  Receive FIFO data available interrupt */
            uart_intr_id_rx_line_stat = 0x6,    /*  Receive line status interrupt */
            uart_intr_id_rx_timeout = 0xc,      /*  Receive FIFO data timeout interrupt */
        } uart_intr_id_t;

  - `uart_intr_rx_data_avail_or_timeout`

    - Interrupt trigger condition: If FIFO is enabled, it triggers when the received data reaches the FIFO threshold. When no characters are moved in or out of the receive FIFO, and there is at least one byte in the receive FIFO for the last four character times, a timeout trigger occurs.
    - Interrupt flag clearing: Use the `uart_read_byte` API to read FIFO data for clearing. Clears the `uart_intr_id_rx_data_avail` status. In FIFO mode, FIFO data needs to be read until the remaining data is less than the FIFO threshold.

  - `uart_intr_tx_slot_avail`

    - Interrupt trigger condition: Triggers when the transmit FIFO is empty.
    - Interrupt flag clearing: Use the `uart_write_byte` API to write data to the FIFO.

  - `uart_intr_rx_line_stat`

    - Interrupt trigger condition: Triggers on parity error, frame error, overrun error, timeout error, etc.
    - Interrupt flag clearing: Use the `uart_get_status` API to clear. This API can also be used as a serial port receive status query interface.

  - `uart_intr_modem_stat`

    - Refer to the `CTS Hardware Flow Control` section

  - `uart_intr_rx_line_idle`

    - Interrupt trigger condition: Triggers when the receive line is idle
    - Interrupt flag clearing: Use the `uart_clear_rxline_idle_flag` API to clear.
    - **Note**  Only supported when the current SOC supports RX Idle detection. Refer to the `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_tx_line_idle`

    - Interrupt trigger condition: Triggers when the transmit line is idle
    - Interrupt flag clearing: Use the `uart_clear_txline_idle_flag` API to clear.
    - **Note**  Only supported when the current SOC supports TX Idle detection. Refer to the `HPM_IP_FEATURE_UART_TX_IDLE_DETECT` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_addr_match`

    - Interrupt trigger condition: Triggers when the first byte of received data matches the configured Address. Use the `uart_is_addr_match` API in the interrupt handler to determine if triggered.
    - Interrupt flag clearing: Use the `uart_clear_addr_match_flag` API to clear.
    - **Note**  Refer to the `HPM_IP_FEATURE_UART_ADDR_MATCH` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_addr_match_and_rxidle`

    - Interrupt trigger condition: Triggers when the first byte of received data matches the configured Address and the receive bus becomes idle. Use the `uart_is_addr_match_and_rxidle` API in the interrupt handler to determine if triggered.
    - Interrupt flag clearing: Use the `uart_clear_addr_match_and_rxidle_flag` API to clear.
    - **Note**  Refer to the `HPM_IP_FEATURE_UART_ADDR_MATCH` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_addr_datalost`

    - Interrupt trigger condition: Data before the address match is discarded, then an interrupt is generated. Use the `uart_is_data_lost` API in the interrupt handler to determine if triggered. For example, if two data sets are prepared, with data set 1 (without a matchable address) before data set 2 (with a matchable address), an interrupt is generated and data from data set 1 is discarded.
    - Interrupt flag clearing: Use the `uart_clear_data_lost_flag` API to clear.
    - **Note**  Refer to the `HPM_IP_FEATURE_UART_ADDR_MATCH` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_errf`

    - Interrupt trigger condition: Receive FIFO error flag. In FIFO mode, triggers on parity error, frame error, or transmission break error. Use the `uart_rx_is_fifo_error` API in the interrupt handler to determine if triggered.
    - Interrupt flag clearing: Use the `uart_clear_rx_fifo_error_flag` API to clear.
    - **Note**  Refer to the `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_framing_err`

    - Interrupt trigger condition: Triggers on frame format error. Use the `uart_rx_is_framing_error` API in the interrupt handler to determine if triggered.
    - Interrupt flag clearing: Use the `uart_clear_rx_framing_error_flag` API to clear.
    - **Note**  Refer to the `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_break_err`

    - Interrupt trigger condition: Triggers on transmission break error. Use the `uart_rx_is_break` API in the interrupt handler to determine if triggered.
    - Interrupt flag clearing: Use the `uart_clear_rx_break_flag` API to clear.
    - **Note**  Refer to the `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_parity_err`

    - Interrupt trigger condition: Triggers on parity error. Use the `uart_rx_is_parity_error` API in the interrupt handler to determine if triggered.
    - Interrupt flag clearing: Use the `uart_clear_rx_parity_error_flag` API to clear.
    - **Note**  Refer to the `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.

  - `uart_intr_overrun`

    - Interrupt trigger condition: Triggers on receive data overrun. Use the `uart_rx_is_overrun` API in the interrupt handler to determine if triggered.
    - Interrupt flag clearing: Use the `uart_clear_rx_overrun_flag` API to clear.
    - **Note**

      - Refer to the `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` macro definition in each SOC's `hpm_soc_ip_feature.h` for specific support.
      - On SOCs with a 32-byte FIFO, due to a 1-byte asynchronous FIFO, the interrupt triggers when 33 bytes of data are received.

- Related API interfaces:

  - Enable UART interrupt

    .. code-block:: c

            void uart_enable_irq(UART_Type *ptr, uint32_t irq_mask);

    - Parameter description:

      .. list-table::
            :header-rows: 1

            * - Parameter name
              - Type
              - Description
            * - ptr
              - UART_Type*
              - Pointer to the UART controller base address, used to specify the UART controller to configure.
            * - irq_mask
              - uint32_t
              - Interrupt mask, used to specify the interrupts to enable. The corresponding interrupt masks can be viewed from the `uart_intr_enable_t` enumeration type.

    - Return value:

      - None

  - Disable UART interrupt

    .. code-block:: c

            void uart_disable_irq(UART_Type *ptr, uint32_t irq_mask)

    - Parameter description:

      .. list-table::
            :header-rows: 1

            * - Parameter name
              - Type
              - Description
            * - ptr
              - UART_Type*
              - Pointer to the UART controller base address, used to specify the UART controller to configure.
            * - irq_mask
              - uint32_t
              - Interrupt mask, used to specify the interrupts to enable. The corresponding interrupt masks can be viewed from the `uart_intr_enable_t` enumeration type.

    - Return value:

      - None

UART samples
-------------------

.. toctree::
  :maxdepth: 3

  uart_9bit/README_en
  uart_dma/README_en
  uart_hardware_rx_idle/README_en
  uart_irq/README_en
  uart_lin/master/README_en
  uart_lin/slave/README_en
  uart_lin/slave_baudrate_adaptive/README_en
  uart_rx_line_status/README_en
  uart_rx_timeout/README_en
  uart_software_rx_idle/README_en
  uart_tamagawa/README_en

