.. _spi:

SPI
===

Overview
--------

- This document introduces the main driver interfaces and usage methods of the SPI peripheral for HPMicro's microcontrollers. For more details, refer to the API documentation in `hpm_spi_drv.h` and the relevant user manuals.
- Communication modes:

  - Supports SPI, DSPI, and QSPI
  - Supports master mode and slave mode

- Master SCLK clock characteristics:

  - Supports configuration of SPI clock polarity and phase (CPOL and CPHA)
  - CS and SCLK interface timing can be configured
  - SPI clock source and SCLK frequency ratio can be configured

- Data format:

  - Supports MSB and LSB transmission
  - Data unit length: 1 to 32 bits
  - In master mode, command phase, address phase, dummy phase, and data phase can be configured (each phase can be enabled or disabled).
  - In slave mode, pure data mode and non-pure data mode can be configured. In pure data mode, the slave transmits and receives data only in the data phase; in non-pure data mode, the slave transmits and receives in the command (8-bit) + dummy (8-bit) + data phase.
  - Command phase length is fixed at 8 bits
  - Address phase length can be configured as 8, 16, 24, or 32 bits
  - Data phase length can be configured. The maximum single transfer length depends on the `SPI_SOC_TRANSFER_COUNT_MAX` macro defined in each SoC's `hpm_soc_feature.h`.
  - Master mode supports up to 4 hardware CS chip selects. This feature is available if the `HPM_IP_FEATURE_SPI_CS_SELECT` macro is defined in `hpm_soc_ip_feature.h`.

- Data buffering:

  - SPI has separate hardware FIFO buffers for TX and RX. TX FIFO has a maximum depth of 8 words (32 bits wide), and RX FIFO has a maximum depth of 8 words (32 bits wide).
  - The exact depth can be referenced from the `SPI_SOC_FIFO_DEPTH` macro in `hpm_soc_feature.h` or obtained via the `spi_get_rx_fifo_size` and `spi_get_tx_fifo_size` APIs in `hpm_spi_drv.h`.

- Interrupts and DMA:

  - Slave command interrupt: triggered when a command is received in slave mode
  - SPI transfer completion interrupt
  - TXFIFO interrupt when valid data is less than or equal to the set threshold (supports DMA trigger)
  - RXFIFO interrupt when valid data is greater than or equal to the set threshold (supports DMA trigger)
  - TXFIFO underflow interrupt
  - RXFIFO overflow interrupt

SPI Initialization
--------------------

- Ensure the SPI clock source is enabled and the related SPI peripheral pins are initialized.

SPI SCLK Clock Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Only master mode is supported; slave mode does not require configuration.
- Use the `spi_master_timing_init` API to set the master SCLK clock frequency.
- Before setting the master SCLK clock frequency, it is recommended to use the `spi_master_get_default_timing_config` API to obtain the default SCLK clock configuration.

- Structure Introduction:

  - `spi_timing_config_t`:

    - A structure used to configure the master SCLK clock frequency.

    .. code-block:: c

        typedef struct {
            uint32_t clk_src_freq_in_hz; /*!< SPI clock source frequency */
            uint32_t sclk_freq_in_hz;    /*!< SPI SCLK clock frequency */
            uint8_t cs2sclk;             /*!< Minimum time from CS active to SCLK edge. SCLK_cycle * (CS2SCLK + 1) / 2 */
            uint8_t csht;                /*!< Minimum time from CS active to SCLK high. SCLK_cycle * (CSHT + 1) / 2 */
        } spi_master_timing_config_t;

        typedef struct {
            spi_master_timing_config_t master_config; /*!< SPI master mode configuration */
        } spi_timing_config_t;

    - `clk_src_freq_in_hz`: SPI clock source frequency in Hz. The SPI clock source frequency can be obtained via the `clock_get_frequency` API in `hpm_clock_drv.h`. The default SPI clock source is 80 MHz.
    - `sclk_freq_in_hz`: SPI SCLK clock frequency in Hz. Set according to actual requirements. Configuration conditions:

      - The clock source frequency `sclk_freq_in_hz` must be divisible by the target SCLK frequency `clk_src_freq_in_hz`.
      - The divider coefficient must be even, i.e., `clk_src_freq_in_hz / sclk_freq_in_hz` must be even. The maximum divider coefficient is 510.
      - If the target SCLK frequency is greater than or equal to the clock source frequency, the target SCLK frequency is forced to match the clock source.

      - Successful configuration examples:

        - Clock source 80 MHz, target SCLK 10 MHz → Divider = 8 (80 / 10 = 8).
        - Clock source 60 MHz, target SCLK 16 MHz → Divider = 6 (60 / 10 = 6).

      - Failed configuration examples:

        - Clock source 80 MHz, target 33 MHz → Not divisible.
        - Clock source 80 MHz, target 16 MHz → Divider = 5 (not even).
        - Clock source 80 MHz, target 1 kHz → Divider = 800 (exceeds 510 limit).

    - If `spi_master_timing_init` fails to meet the target SCLK frequency, adjust the SPI clock source frequency to satisfy the conditions.

    - `cs2sclk`: Minimum time from CS active to SCLK edge. `SCLK_cycle * (CS2SCLK + 1) / 2`.

      - Example: `cs2sclk = 1`, `SCLK_cycle = 10 ns` → Minimum time = 10 ns.

    - `csht`: Minimum CS high time. `SCLK_cycle * (CSHT + 1) / 2`.

      - Example: `csht = 1`, `SCLK_cycle = 10 ns` → Minimum high time = 10 ns.

- Related APIs:

  - `spi_master_get_default_timing_config`:

    - Obtains the default SCLK clock configuration for SPI master mode.

    .. code-block:: c

        void spi_master_get_default_timing_config(spi_master_timing_config_t *config);

    - Parameter Description:

    .. list-table::
         :header-rows: 1

         * - Parameter Name
           - Type
           - Description
         * - config
           - spi_master_timing_config_t*
           - Pointer to the SPI master mode SCLK clock configuration structure, containing SCLK frequency and configuration details.

    - Return Value:

      - None.

  - `spi_master_timing_init`:

    - Sets the SCLK clock frequency for SPI master mode.

    .. code-block:: c

        hpm_stat_t spi_master_timing_init(SPI_Type *ptr, spi_master_timing_config_t *config);

    - Parameter Description:

    .. list-table::
         :header-rows: 1

         * - Parameter Name
           - Type
           - Description
         * - ptr
           - SPI_Type*
           - Pointer to the SPI controller base address, specifying the SPI controller to configure.
         * - config
           - spi_master_timing_config_t*
           - Pointer to the SPI master mode SCLK clock configuration structure, containing SCLK frequency and configuration details.

    - Return Value:

      - `hpm_stat_t`:

        - `status_success`: Configuration succeeded.
        - `status_invalid_argument`: Invalid parameter.

- Example:

  - Refer to the master mode example in the `SPI example`.

    .. code-block:: c

        #include "hpm_clock_drv.h"
        #include "hpm_spi_drv.h"

        void spi_master_timing_init_example(void)
        {
            spi_master_timing_config_t config;
            uint32_t clock_freq_in_hz;
            /* Initialize SPI controller pins (not shown) */

            /* Enable SPI clock source */
            clock_add_to_group(clock_spi1, 0); /* Default 80 MHz */

            /* Get SPI clock source frequency */
            clock_freq_in_hz = clock_get_frequency(clock_spi1);

            /* Get default SCLK configuration for SPI master mode */
            spi_master_get_default_timing_config(&config);

            /* Set SCLK clock frequency for SPI master mode */
            config.clk_src_freq_in_hz = clock_freq_in_hz;
            config.sclk_freq_in_hz = 10000000; /* Set SCLK to 10 MHz */
            config.cs2sclk = 1;               /* Set CS2SCLK to 1 */
            config.csht = 1;                  /* Set CSHT to 1 */
            if (spi_master_timing_init(HPM_SPI1, &config) != status_success) {
                /* Configuration failed */
                printf("spi_master_timing_init failed\n");
            }
        }

SPI Format Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Supports master mode and slave mode.
- Mainly configures SPI data format, including data bit width, data transfer order, command bit width, address bit width, dummy bit width, and data bit width.
- Before setting the SPI format configuration in master mode, it is recommended to use the `spi_master_get_default_format_config` API to obtain the default SPI format configuration.
- Before setting the SPI format configuration in slave mode, it is recommended to use the `spi_slave_get_default_format_config` API to obtain the default SPI format configuration.

- Structure Introduction:

  - `spi_master_format_config_t`:

    - A structure used to configure the SPI format in master mode.

      .. code-block:: c

         /*!< SPI format configuration structure */
        typedef struct {
            spi_master_format_config_t master_config;  /*!< SPI master mode configuration */
            spi_common_format_config_t common_config;  /*!< SPI common configuration */
        } spi_format_config_t;

        /*!< SPI master mode format configuration structure */
        typedef struct {
            uint8_t addr_len_in_bytes;  /*!< Address length in bytes */
        } spi_master_format_config_t;

        /*!< SPI common format configuration structure */
        typedef struct {
            uint8_t data_len_in_bits;  /*!< Data length in bits */
            bool data_merge;           /*!< Data merge mode */
            bool mosi_bidir;           /*!< MOSI bidirectional mode */
            bool lsb;                  /*!< LSB mode */
            uint8_t mode;              /*!< SPI mode */
            uint8_t cpol;              /*!< Clock polarity */
            uint8_t cpha;              /*!< Clock phase */
        } spi_common_format_config_t;

    - `spi_format_config_t` includes both `spi_master_format_config_t` and `spi_common_format_config_t` structures.

    - `spi_master_format_config_t` is used to configure the SPI format in master mode, including address length.

      - `addr_len_in_bytes`: Address length in bytes. Specifies the address length for the address phase. This parameter is not required if the address phase is disabled.

    - `spi_common_format_config_t` is used to configure the common SPI format, supporting both master and slave modes, including data length, data merge mode, MOSI bidirectional mode, LSB mode, SPI mode, clock polarity, and clock phase.

      - `data_len_in_bits`: Data unit length in bits. Specifies the data unit length for the data phase, supporting 1 to 32 bits.
      - `data_merge`: Data merge mode. **Valid only when the data unit length is 8 bits**, and the data size must be a multiple of 4. During transmission and reception, 8-bit data is merged into a 32-bit data unit to improve efficiency.
      - `mosi_bidir`: MOSI bidirectional mode. If set to `true`, MOSI operates in bidirectional mode; otherwise, it is unidirectional. **Supports only single-wire SPI mode**. Enables the MOSI pin as a bidirectional signal when enabled.
      - `lsb`: LSB mode. If set to `true`, data is transmitted in LSB-first order; otherwise, it is MSB-first.
      - `mode`: Specifies SPI master or slave mode using the `spi_master_mode` or `spi_slave_mode` enumeration.
      - `cpol`: Clock polarity. Specifies the SPI clock polarity using `spi_sclk_low_idle` or `spi_sclk_high_idle`.
      - `cpha`: Clock phase. Specifies the SPI clock phase using `spi_sclk_sampling_odd_clk_edges` or `spi_sclk_sampling_even_clk_edges`.

- Related APIs:

  - `spi_master_get_default_format_config`:

    - Obtains the default SPI format configuration for master mode.

       .. code-block:: c

            void spi_master_get_default_format_config(spi_format_config_t *config);

      - Parameter Description:

        .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - config
              - spi_format_config_t*
              - Pointer to the SPI master mode format configuration structure, containing SPI format configuration details.

      - Return Value:

        - None.

  - `spi_slave_get_default_format_config`:

    - Obtains the default SPI format configuration for slave mode.

      .. code-block:: c

            void spi_slave_get_default_format_config(spi_format_config_t *config);

      - Parameter Description:

        .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - config
              - spi_format_config_t*
              - Pointer to the SPI slave mode format configuration structure, containing SPI format configuration details.

      - Return Value:

        - None.

  - `spi_format_init`:

    - Sets the SPI format configuration.

      .. code-block:: c

            void spi_format_init(SPI_Type *ptr, spi_format_config_t *config);

      - Parameter Description:

        .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - ptr
              - SPI_Type*
              - Pointer to the SPI controller base address, specifying the SPI controller to configure.
            * - config
              - spi_format_config_t*
              - Pointer to the SPI format configuration structure, containing SPI format configuration details.

      - Return Value:

        - None.

- Example:

  - Refer to the `SPI example` for implementation details.

  - In master mode, configure SPI format: LSB mode, data length 8 bits, clock polarity low, clock phase odd (CPOL = 0, CPHA = 0).

     .. code-block:: c

        #include "hpm_clock_drv.h"
        #include "hpm_spi_drv.h"
        void spi_format_init_example(void)
        {
            spi_format_config_t format_config;
            /* Initialize SPI controller pins (not shown) */
            /* Enable SPI clock source (not shown) */
            /* Set SPI master mode SCLK frequency (not shown) */

            /* Get default SPI format configuration for master mode */
            spi_master_get_default_format_config(&format_config);
            format_config.common_config.data_len_in_bits = 8; /* Set data length to 8 bits */
            format_config.common_config.lsb = true;           /* Set LSB mode */
            format_config.common_config.cpol = spi_sclk_low_idle; /* Set clock polarity to low */
            format_config.common_config.cpha = spi_sclk_sampling_odd_clk_edges; /* Set clock phase to odd */
            format_config.common_config.mode = spi_master_mode; /* Set SPI master mode */
            /* Apply SPI format configuration */
            spi_format_init(HPM_SPI1, &format_config);
        }

  - In master mode, configure SPI format: MSB mode, address phase with 3-byte address (24-bit address), data length 16 bits, clock polarity high, clock phase even (CPOL = 1, CPHA = 1).

      .. code-block:: c

            #include "hpm_clock_drv.h"
            #include "hpm_spi_drv.h"
            void spi_format_init_example(void)
            {
                spi_format_config_t format_config;
                /* Initialize SPI controller pins (not shown) */
                /* Enable SPI clock source (not shown) */
                /* Set SPI master mode SCLK frequency (not shown) */

                /* Get default SPI format configuration for master mode */
                spi_master_get_default_format_config(&format_config);
                format_config.common_config.data_len_in_bits = 16; /* Set data length to 16 bits */
                format_config.common_config.lsb = false;           /* Set MSB mode */
                format_config.common_config.cpol = spi_sclk_high_idle; /* Set clock polarity to high */
                format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges; /* Set clock phase to even */
                format_config.common_config.mode = spi_master_mode; /* Set SPI master mode */
                format_config.master_config.addr_len_in_bytes = 3; /* Set address phase address length to 3 bytes */
                /* Apply SPI format configuration */
                spi_format_init(HPM_SPI1, &format_config);
            }

  - In slave mode, configure SPI format: MSB mode, data length 8 bits, clock polarity low, clock phase odd (CPOL = 0, CPHA = 0).

     .. code-block:: c

            #include "hpm_clock_drv.h"
            #include "hpm_spi_drv.h"
            void spi_format_init_example(void)
            {
                spi_format_config_t format_config;
                /* Initialize SPI controller pins (not shown) */
                /* Enable SPI clock source (not shown) */
                /* Set SPI master mode SCLK frequency (not shown) */

                /* Get default SPI format configuration for slave mode */
                spi_slave_get_default_format_config(&format_config);
                format_config.common_config.data_len_in_bits = 8; /* Set data length to 8 bits */
                format_config.common_config.lsb = false;           /* Set MSB mode */
                format_config.common_config.cpol = spi_sclk_low_idle; /* Set clock polarity to low */
                format_config.common_config.cpha = spi_sclk_sampling_odd_clk_edges; /* Set clock phase to odd */
                format_config.common_config.mode = spi_slave_mode; /* Set SPI slave mode */
                /* Apply SPI format configuration */
                spi_format_init(HPM_SPI1, &format_config);
            }

SPI Data Transmission
----------------------------

- Related structure introduction:

  .. code-block:: c

        /*!< SPI transmission configuration structure */
        typedef struct {
            bool cmd_enable;         /* !< Command phase enable */
            bool addr_enable;        /*!< Address phase enable */
            uint8_t addr_phase_fmt;  /*!< Address phase format */
            bool token_enable;       /*!< Token phase enable */
            uint8_t token_value;     /*!< Token value */
        } spi_master_control_config_t;

        /*!< SPI slave mode control configuration structure */
        typedef struct {
            bool slave_data_only;    /*!< Slave data-only mode */
        } spi_slave_control_config_t;

        /*!< SPI transmission configuration structure */
        typedef struct {
            bool tx_dma_enable;      /*!< Transmit DMA enable */
            bool rx_dma_enable;      /*!< Receive DMA enable */
            uint8_t trans_mode;      /*!< Transmission mode */
            uint8_t data_phase_fmt;  /*!< Data phase format */
            uint8_t dummy_cnt;       /*!< Dummy count */
        #if defined(HPM_IP_FEATURE_SPI_CS_SELECT) && (HPM_IP_FEATURE_SPI_CS_SELECT == 1)  /* Current SOC supports hardware CS */
            uint8_t cs_index;        /*!< CS index */
        #endif
        } spi_common_control_config_t;

        /*!< SPI control configuration structure */
        typedef struct {
            spi_master_control_config_t master_config;  /*!< SPI master mode control configuration */
            spi_slave_control_config_t  slave_config;   /*!< SPI slave mode control configuration */
            spi_common_control_config_t common_config;  /*!< SPI common configuration */
        } spi_control_config_t;

- Functional description:

  - From **structure introduction**, the following features can be summarized:
  - Supports master and slave modes.
  - Supports DMA transfer and polling transfer.

  - In master mode, SPI data transmission sequence is command phase + address phase + token phase + dummy phase + data phase. Each phase can be enabled or disabled.

    - Command phase:

      - Command phase enable: Set `cmd_enable` to true to enable the command phase.
      - Command phase disable: Set `cmd_enable` to false to disable the command phase.
      - The command phase length is fixed at 8 bits. The command value can be set via the `spi_write_command` API interface.
      - **Regardless of whether the command phase is enabled or disabled, the command value must be set before transmission. It can be set via the `spi_write_command` API interface.**

    - Address phase:

      - Address phase enable: Set `addr_enable` to true to enable the address phase.
      - Address phase disable: Set `addr_enable` to false to disable the address phase.
      - The address length in the address phase can be configured as 8, 16, 24, or 32 bits.
      - The address length remains fixed for each transmission. It can be set via the `addr_len_in_bytes` member in `spi_format_init`.
      - The address value can be set via the `spi_write_address` API interface.
      - The address transmission supports SPI, DSPI, and QSPI formats. It can be configured via the `addr_phase_fmt` member.
        - `addr_phase_fmt` :

          - `spi_address_phase_format_single_io_mode` : SPI address format.
          - `spi_address_phase_format_dualquad_io_mode` : DSPI or QSPI address format. It matches the `data_phase_fmt` member. For example, if `data_phase_fmt` is set to `spi_quad_io_mode` (QSPI format), the address phase format will also be QSPI.

    - Token phase:

      - Token phase enable: Can be configured via the `spi_master_enable_token_transfer` API.
      - Token phase disable: Can be configured via the `spi_master_disable_token_transfer` API.
      - The token value can be set via the `spi_master_set_token_value` API interface. Only 0x00 and 0x69 are supported.

    - Dummy phase:

      - `dummy_cnt` : Sets the dummy count. The unit is `data_len_in_bits` in `spi_format_init`. For example, if `data_len_in_bits` is set to 8 bits and `dummy_cnt` is set to 2, the dummy phase length is 16 bits.
      - Only effective when `trans_mode` is set to `spi_trans_write_dummy_read`, `spi_trans_read_dummy_write`, `spi_trans_dummy_write`, or `spi_trans_dummy_read`.

    - Data phase:

      - `data_phase_fmt` : Sets the data phase format. Ten data transmission formats are supported. Refer to the `spi_trans_mode_t` enumeration type.

        - `spi_trans_write_read_together`: Simultaneous read/write transmission. Only supports SPI mode. For example, in SPI mode, the SPI controller sends and receives data simultaneously.
        - `spi_trans_write_only`: Sends data only.
        - `spi_trans_read_only`: Receives data only.
        - `spi_trans_write_read`: Sends data first, then receives data.
        - `spi_trans_read_write`: Receives data first, then sends data.
        - `spi_trans_write_dummy_read`: Sends data → sends dummy → receives data.
        - `spi_trans_read_dummy_write`: Receives data → receives dummy → sends data.
        - `spi_trans_no_data`: No data transmission. Only command and address phases are transmitted. Requires enabling command and address phases.
        - `spi_trans_dummy_write`: Sends dummy first, then sends data.
        - `spi_trans_dummy_read`: Sends dummy first, then receives data.

  - In slave mode, SPI data transmission is divided into data-only mode and non-data-only mode.

    - Data-only mode:

      - Slave data-only mode: Set `slave_data_only` to true for data-only mode.
      - In data-only mode, the slave transmits and receives data phases only. Only supports `spi_trans_write_read_together` transmission mode.

    - Non-data-only mode:

      - Slave non-data-only mode: Set `slave_data_only` to false for non-data-only mode.
      - In non-data-only mode, the slave transmits and receives command (8 bits) + dummy (8 bits) + data phase.

- Related API interfaces:

    - Polling data transfer API.

      .. code-block:: c

            hpm_stat_t spi_transfer(SPI_Type *ptr,
                        spi_control_config_t *config,
                        uint8_t *cmd, uint32_t *addr,
                        uint8_t *wbuff, uint32_t wcount, uint8_t *rbuff, uint32_t rcount);

      - Parameter description:

       .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - ptr
              - SPI_Type*
              - Pointer to SPI controller base address, used to specify the SPI controller to configure.
            * - config
              - spi_control_config_t*
              - Pointer to SPI control configuration structure, containing SPI control configuration information.
            * - cmd
              - uint8_t*
              - Pointer to command value, used to specify the command value.
            * - addr
              - uint32_t*
              - Pointer to address value, used to specify the address value.
            * - wbuff
              - uint8_t*
              - Pointer to transmit buffer, used to specify the transmit buffer.
            * - wcount
              - uint32_t
              - Number of bytes to transmit.
            * - rbuff
              - uint8_t*
              - Pointer to receive buffer, used to specify the receive buffer.
            * - rcount
              - uint32_t
              - Number of bytes to receive.

      - Return values:

        - `status_success`：Transfer successful.
        - `status_invalid_argument`：Invalid parameter.
        - `status_timeout`：Transfer timeout.
        - `status_fail`：Transfer error.

     - Notes:

        - If `cmd_enable` is set to false, `cmd` can be set to NULL.
        - If `addr_enable` is set to false, `addr` can be set to NULL.
        - If `data_phase_fmt` is set to `spi_trans_no_data`, `wcount` and `rcount` can be set to 0.

      Examples:

        - Master mode, single-line SPI, command phase enabled (command value 0x01), address phase enabled (SPI format, address value 0x02), transmission mode `spi_trans_write_read_together`, data transmit/receive length 5 bytes.

          .. code-block:: c

                #include "hpm_clock_drv.h"
                #include "hpm_spi_drv.h"
                void spi_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t cmd = 0x01;
                    uint32_t addr = 0x02;
                    uint8_t wbuff[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
                    uint8_t rbuff[5] = {0};
                    /* Initialize SPI controller pins, not shown here */
                    /* Enable SPI clock source, not shown here */
                    /* Set SPI master mode SCLK frequency, not shown here */
                    /* Set SPI master mode SPI format configuration, not shown here */
                    spi_master_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_read_together; /* Set transmission mode to simultaneous read/write */
                    control_config.common_config.data_phase_fmt = spi_single_io_mode; /* Set data phase format to single-line mode */
                    control_config.master_config.cmd_enable = true; /* Enable command phase */
                    control_config.master_config.addr_enable = true; /* Enable address phase */
                    control_config.master_config.addr_phase_fmt = spi_address_phase_format_single_io_mode; /* Set address phase format to SPI */
                    if (spi_transfer(HPM_SPI1, &control_config, &cmd, &addr, wbuff, 5, rbuff, 5)!= status_success) {
                        /* Transfer failed */
                        printf("spi_transfer failed\n");
                    }
                }

        - Master mode, four-line QSPI, command/address phases disabled, transmission mode `spi_trans_write_dummy_read`, transmit length 5 bytes, receive length 10 bytes, dummy count 2.

         .. code-block:: c

                void spi_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t wbuff[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
                    uint8_t rbuff[10] = {0};
                    /* Initialize SPI controller pins, not shown here */
                    /* Enable SPI clock source, not shown here */
                    /* Set SPI master mode SCLK frequency, not shown here */
                    /* Set SPI master mode SPI format configuration, not shown here */
                    spi_master_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_dummy_read; /* Set transmission mode to send dummy then receive data */
                    control_config.common_config.data_phase_fmt = spi_quad_io_mode; /* Set data phase format to quad-line mode */
                    control_config.master_config.cmd_enable = false; /* Disable command phase */
                    control_config.master_config.addr_enable = false; /* Disable address phase */
                    control_config.common_config.dummy_cnt = spi_dummy_count_2; /* Set dummy count to 2 */
                    if (spi_transfer(HPM_SPI1, &control_config, NULL, NULL, wbuff, 5, rbuff, 10)!= status_success) {
                        /* Transfer failed */
                        printf("spi_transfer failed\n");
                    }
                }

        - Slave mode, four-line QSPI, slave data-only mode, transmission mode `spi_trans_write_read_together`, data transmit/receive length 5 bytes.

        .. code-block:: c

                void spi_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t wbuff[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
                    uint8_t rbuff[5] = {0};
                    /* Initialize SPI controller pins, not shown here */
                    /* Enable SPI clock source, not shown here */
                    /* Set SPI slave mode SPI format configuration, not shown here */
                    spi_slave_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_read_together; /* Set transmission mode to simultaneous read/write */
                    control_config.common_config.data_phase_fmt = spi_quad_io_mode; /* Set data phase format to quad-line mode */
                    control_config.slave_config.slave_data_only = true; /* Enable slave data-only mode */
                    if (spi_transfer(HPM_SPI1, &control_config, NULL, NULL, wbuff, 5, rbuff, 5)!= status_success) {
                        /* Transfer failed */
                        printf("spi_transfer failed\n");
                    }
                }

    - DMA transfer initialization setup API

        - Used to set up SPI DMA transfer initialization, including enabling transmit/receive DMA, setting SPI controller transfer length, etc.
        - If the transfer length remains unchanged for each transmission, initialization is only required once. In master mode, subsequent transmissions can use the `spi_write_command` API interface.
        - DMA transmission is triggered when TXFIFO valid data is less than or equal to the set threshold. Threshold setting can be done via the `spi_set_tx_fifo_threshold` API interface.
        - DMA transmission is triggered when RXFIFO valid data is greater than or equal to the set threshold. Threshold setting can be done via the `spi_set_rx_fifo_threshold` API interface.
        - Use DMA for sending and receiving with the SPI component :ref:`spi_component <spi_component>`  API reference

        .. code-block:: c

            hpm_stat_t spi_setup_dma_transfer(SPI_Type *ptr, spi_control_config_t *config,
                        uint8_t *cmd, uint32_t *addr, uint32_t wcount, uint32_t rcount);

        - Parameter description:

            .. list-table::
                :header-rows: 1

                * - Parameter Name
                  - Type
                  - Description
                * - ptr
                  - SPI_Type*
                  - Pointer to SPI controller base address, used to specify the SPI controller to configure.
                * - config
                  - spi_control_config_t*
                  - Pointer to SPI control configuration structure, containing SPI control configuration information.
                * - cmd
                  - uint8_t*
                  - Pointer to command value, used to specify the command value.
                * - addr
                  - uint32_t*
                  - Pointer to address value, used to specify the address value.
                * - wcount
                  - uint32_t
                  - Number of bytes to transmit.
                * - rcount
                  - uint32_t
                  - Number of bytes to receive.

        - Return values:

            - `status_success`：DMA transfer initialization setup successful.
            - `status_invalid_argument`：Invalid parameter.
            - `status_fail`：DMA transfer initialization setup failed.

        Examples:

            - Master mode, dual-line DSPI, command phase enabled (command value 0x01), address phase enabled (DSPI format, address value 0x02), transmission mode `spi_trans_write_read`, data transmit/receive length 100 bytes.

              .. code-block:: c

                    #include "hpm_clock_drv.h"
                    #include "hpm_spi_drv.h"
                    void spi_setup_dma_transfer_example(void)
                    {
                        spi_control_config_t control_config;
                        uint8_t cmd = 0x01;
                        uint32_t addr = 0x02;
                        uint8_t wbuff[100] = {0};
                        uint8_t rbuff[100] = {0};
                        /* Initialize SPI controller pins, not shown here */
                        /* Enable SPI clock source, not shown here */
                        /* Set SPI master mode SCLK frequency, not shown here */
                        /* Set SPI master mode SPI format configuration, not shown here */
                        spi_master_get_default_format_config(&control_config);
                        control_config.common_config.trans_mode = spi_trans_write_read; /* Set transmission mode to send data first then receive data */
                        control_config.common_config.data_phase_fmt = spi_dual_io_mode; /* Set data phase format to dual-line mode */
                        control_config.master_config.cmd_enable = true; /* Enable command phase */
                        control_config.master_config.addr_enable = true; /* Enable address phase */
                        control_config.master_config.addr_phase_fmt = spi_address_phase_format_dualquad_io_mode; /* Set address phase format to DSPI */
                        if (spi_setup_dma_transfer(HPM_SPI1, &control_config, &cmd, &addr, 100, 100)!= status_success) {
                            /* DMA transfer initialization setup failed */
                            printf("spi_setup_dma_transfer failed\n");
                            return;
                        }
                        /* Configure DMA channel, source address, device address, etc., start DMA transfer, not shown here, refer to `SPI example` DMA related sections */
                    }

            - Master mode, four-line QSPI, command and address phases disabled, transmission mode `spi_trans_write_only`, data transmit length 100 bytes.

              .. code-block:: c

                    void spi_setup_dma_transfer_example(void)
                    {
                        spi_control_config_t control_config;
                        uint8_t wbuff[100] = {0};
                        /* Initialize SPI controller pins, not shown here */
                        /* Enable SPI clock source, not shown here */
                        /* Set SPI master mode SCLK frequency, not shown here */
                        /* Set SPI master mode SPI format configuration, not shown here */
                        spi_master_get_default_format_config(&control_config);
                        control_config.common_config.trans_mode = spi_trans_write_only; /* Set transmission mode to send data only */
                        control_config.common_config.data_phase_fmt = spi_quad_io_mode; /* Set data phase format to quad-line mode */
                        control_config.master_config.cmd_enable = false; /* Disable command phase */
                        control_config.master_config.addr_enable = false; /* Disable address phase */
                        if (spi_setup_dma_transfer(HPM_SPI1, &control_config, NULL, NULL, 100, 0)!= status_success) {
                            /* DMA transfer initialization setup failed */
                            printf("spi_setup_dma_transfer failed\n");
                            return;
                        }
                        /* Configure DMA channel, source address, device address, etc., start DMA transfer, not shown here, refer to `SPI example` DMA related sections */
                    }

            - Slave mode, four-line QSPI, slave data-only mode, transmission mode `spi_trans_write_read_together`, data transmit/receive length 100 bytes.

             .. code-block:: c

                    void spi_setup_dma_transfer_example(void)
                    {
                        spi_control_config_t control_config;
                        uint8_t wbuff[100] = {0};
                        uint8_t rbuff[100] = {0};
                        /* Initialize SPI controller pins, not shown here */
                        /* Enable SPI clock source, not shown here */
                        /* Set SPI slave mode SPI format configuration, not shown here */
                        spi_slave_get_default_format_config(&control_config);
                        control_config.common_config.trans_mode = spi_trans_write_read_together; /* Set transmission mode to simultaneous read/write */
                        control_config.common_config.data_phase_fmt = spi_quad_io_mode; /* Set data phase format to quad-line mode */
                        control_config.slave_config.slave_data_only = true; /* Enable slave data-only mode */
                        if (spi_setup_dma_transfer(HPM_SPI1, &control_config, NULL, NULL, 100, 100)!= status_success) {
                            /* DMA transfer initialization setup failed */
                            printf("spi_setup_dma_transfer failed\n");
                            return;
                        }
                        /* Configure DMA channel, source address, device address, etc., start DMA transfer, not shown here, refer to `SPI example` DMA related sections */
                    }

Interrupt
--------------

- It is necessary to enable the SPI interrupt in the plic controller, using the `intc_m_enable_irq` API interface to enable the SPI interrupt.
- It is required to configure the priority of the SPI interrupt in the plic controller, using the `intc_m_enable_irq_with_priority` API interface to set the priority of the SPI interrupt.
- Each time an interrupt event occurs, it is necessary to call the `spi_clear_interrupt_status` API interface within the interrupt handling function to clear the interrupt flag.
- Each time an interrupt event occurs, it is possible to call the `spi_get_interrupt_status` API interface within the interrupt handling function to obtain the interrupt flag.
- The SPI peripheral supports the following interrupts, which can be viewed from the `spi_interrupt_t` enumeration type.

  .. code-block:: c

    typedef enum {
        spi_rx_fifo_overflow_int  = SPI_INTREN_RXFIFOORINTEN_MASK,   /* Receive FIFO overflow interrupt */
        spi_tx_fifo_underflow_int = SPI_INTREN_TXFIFOURINTEN_MASK,   /* Transmit FIFO underflow interrupt */
        spi_rx_fifo_threshold_int = SPI_INTREN_RXFIFOINTEN_MASK,     /* Receive FIFO threshold interrupt */
        spi_tx_fifo_threshold_int = SPI_INTREN_TXFIFOINTEN_MASK,     /* Transmit FIFO threshold interrupt */
        spi_end_int               = SPI_INTREN_ENDINTEN_MASK,        /* Transfer end interrupt */
        spi_slave_cmd_int         = SPI_INTREN_SLVCMDEN_MASK,        /* Slave command interrupt */
    #if defined(HPM_IP_FEATURE_SPI_CS_EDGE_DETECT_FOR_SLAVE) && (HPM_IP_FEATURE_SPI_CS_EDGE_DETECT_FOR_SLAVE == 1)  /* If the current SOC supports hardware CS edge detection */
        spi_slave_cs_edge_falling_int = SPI_INTREN_CS_NEGEN_MASK,    /* Slave CS falling edge interrupt */
        spi_slave_cs_edge_rising_int = SPI_INTREN_CS_POSEN_MASK,     /* Slave CS rising edge interrupt */
    #endif
    } spi_interrupt_t;

  - Among these, `spi_rx_fifo_overflow_int`, `spi_tx_fifo_underflow_int`, `spi_slave_cmd_int`, `spi_slave_cs_edge_falling_int`, and `spi_slave_cs_edge_rising_int` are only supported in slave mode.
  - `spi_rx_fifo_threshold_int`: When the effective data in the receive FIFO reaches or exceeds the threshold set by `spi_set_rx_fifo_threshold`, an interrupt will be triggered. For example, if the SPI unit width is 8 bits and the threshold set by `spi_set_rx_fifo_threshold` is 5 bytes, when the effective data in the receive FIFO is greater than or equal to 5 bytes, an interrupt will be triggered. At this point, the receive FIFO data can be read within the interrupt handling function.
  - `spi_tx_fifo_threshold_int`: When the remaining space in the transmit FIFO is less than or equal to the threshold set by `spi_set_tx_fifo_threshold`, an interrupt will be triggered. For example, if the SPI unit width is 8 bits and the threshold set by `spi_set_tx_fifo_threshold` is 5 bytes, when the remaining space in the transmit FIFO is less than or equal to 5 bytes, an interrupt will be triggered. At this point, data can be written into the transmit FIFO within the interrupt handling function.

- Related API interfaces:

  - Enable SPI Interrupt

    .. code-block:: c

        void spi_enable_interrupt(SPI_Type *ptr, uint32_t mask);

    - Parameter Description:

      .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - ptr
              - SPI_Type*
              - A pointer to the base address of the SPI controller, used to specify the SPI controller to be configured.
            * - mask
              - uint32_t
              - Interrupt mask, used to specify the interrupt to be enabled. Corresponding interrupt masks can be found in the `spi_interrupt_t` enumeration.

    - Return Value:

        - None

  - Disable SPI Interrupt

    .. code-block:: c

        void spi_disable_interrupt(SPI_Type *ptr, uint32_t mask);

    - Parameter Description:

     .. list-table::
            :header-rows: 1

            * - Parameter Name
              - Type
              - Description
            * - ptr
              - SPI_Type*
              - A pointer to the base address of the SPI controller, used to specify the SPI controller to be configured.
            * - mask
              - uint32_t
              - Interrupt mask, used to specify the interrupt to be disabled. Corresponding interrupt masks can be found in the `spi_interrupt_t` enumeration.

    - Return Value:

      - None

SPI samples
--------------------

.. toctree::
   :maxdepth: 3

   dma/master/README_en
   dma/slave/README_en
   interrupt/master/README_en
   interrupt/slave/README_en
   master_trans_large_amount_of_data/README_en
   polling/master/README_en
   polling/slave/README_en

