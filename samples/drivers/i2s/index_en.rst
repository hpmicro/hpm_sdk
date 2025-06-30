.. _i2s:

I2S
===

Overview
--------

- This document introduces the main driver interfaces and usage of HPM I2S peripheral. For more details, please refer to the API documentation in `hpm_i2s_drv.h` and related user manuals.
- Supports multiple audio transmission protocols:

  - I2S Philips standard protocol
  - MSB justified/Left justified protocol
  - LSB justified/Right justified protocol
  - PCM protocol

- Supports master and slave modes:

  - Master mode: generates MCLK, BCLK, and FCLK clock signals
  - Slave mode: uses external clock source

- Supports multiple audio formats:

  - Sample depth (audio_depth):

    - Supports 16-bit, 24-bit, 32-bit
    - Represents the actual valid audio data width

  - Channel length (channel_length):

    - Supports 16-bit, 32-bit
    - Must be greater than or equal to sample depth
    - Represents the total width of each channel

  - Channel mode:

    - Standard mode: supports mono and stereo
    - TDM mode: supports up to 8 channels (4 pairs of stereo)

- Supports multiple sampling rates:

  - Clock division based on system audio master clock (MCLK)
  - Supports common sampling rates: 192kHz, 96kHz, 48kHz, 44.1kHz, 32kHz, 22.05kHz, 16kHz, 8kHz
  - Other sampling rates can be achieved through clock division configuration

- Supports DMA transfer
- Supports multiple interrupts:

  - FIFO threshold interrupt
  - FIFO full/empty interrupt

I2S Initialization
-------------------

- Ensure that the I2S clock source is enabled and the related I2S peripheral pins are initialized.

  - Use the `clock_add_to_group` function to add the I2S clock source to the clock group to ensure the I2S clock source is enabled.

- Related structure descriptions:

  - `i2s_config_t` structure for configuring basic I2S parameters

    .. code-block:: c

        typedef struct i2s_config {
            bool invert_mclk_out;           /* Enable MCLK output inversion */
            bool invert_mclk_in;            /* Enable MCLK input inversion */
            bool use_external_mclk;         /* Use external MCLK */
            bool invert_bclk_out;           /* Enable BCLK output inversion */
            bool invert_bclk_in;            /* Enable BCLK input inversion */
            bool use_external_bclk;         /* Use external BCLK */
            bool invert_fclk_out;           /* Enable FCLK output inversion */
            bool invert_fclk_in;            /* Enable FCLK input inversion */
            bool use_external_fclk;         /* Use external FCLK */
            bool enable_mclk_out;           /* Enable MCLK output */
            bool frame_start_at_rising_edge; /* Frame start edge selection */
            uint16_t tx_fifo_threshold;     /* TX FIFO threshold */
            uint16_t rx_fifo_threshold;     /* RX FIFO threshold */
        } i2s_config_t;

  - `i2s_transfer_config_t` structure for configuring I2S transfer parameters

    .. code-block:: c

        typedef struct i2s_transfer_config {
            uint32_t sample_rate;           /* Sample rate */
            bool enable_tdm_mode;           /* Enable TDM mode */
            uint8_t channel_num_per_frame;  /* Number of channels per frame */
            uint8_t channel_length;         /* Channel length: 16-bit or 32-bit */
            uint8_t audio_depth;            /* Audio depth: 16-bit, 24-bit, 32-bit */
            bool master_mode;               /* Master/Slave mode selection */
            uint8_t protocol;               /* Protocol selection */
            i2s_line_num_t data_line;       /* Data line selection */
            uint32_t channel_slot_mask;     /* Channel slot mask */
        } i2s_transfer_config_t;

    - `channel_num_per_frame` parameter description:

      Used to configure the number of channels per frame:

      - Parameter value must be even
      - Fixed to 2 in non-TDM mode
      - Can be configured as 2, 4, 6, 8, etc. in TDM mode
      - The specific channel enablement is configured through the `channel_slot_mask` parameter

    - `channel_slot_mask` parameter description:

      Used to configure the channel slot enable mask, each bit represents a channel slot:

      - In non-TDM mode (channel_num_per_frame = 2):

        - Mono: set to 0x1 (enable one channel only)
        - Stereo: set to 0x3 (enable two channels)

      - In TDM mode (e.g., channel_num_per_frame = 8):

        - Each bit corresponds to a channel
        - Examples:
          - 4 channels: set to 0xF (binary 1111)
          - 8 channels: set to 0xFF (binary 11111111)

Master Mode Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Initialization function API:

  .. code-block:: c

        void i2s_init(I2S_Type *ptr, i2s_config_t *config);

  - Parameter description:

    .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - I2S_Type*
              - Pointer to I2S controller base address
            * - config
              - i2s_config_t*
              - Pointer to I2S configuration structure

- Transfer configuration function API:

  .. code-block:: c

        hpm_stat_t i2s_config_tx(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config);
        hpm_stat_t i2s_config_rx(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config);
        hpm_stat_t i2s_config_transfer(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config);

  - Parameter description:

    .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - I2S_Type*
              - Pointer to I2S controller base address
            * - mclk_in_hz
              - uint32_t
              - MCLK frequency (Hz)
            * - config
              - i2s_transfer_config_t*
              - Pointer to I2S transfer configuration structure

  - Return values:

    .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Configuration successful
            * - status_invalid_argument
              - Invalid parameter

- Example:

  .. code-block:: c

        /* Step 1: I2S initialization, configure as master mode */
        i2s_config_t i2s_config;
        i2s_transfer_config_t transfer;
        uint32_t mclk_freq;

        /* Step 2: Get and set default configuration */
        i2s_get_default_config(HPM_I2S0, &i2s_config);
        i2s_config.enable_mclk_out = true;          /* Enable MCLK output */
        i2s_init(HPM_I2S0, &i2s_config);

        /* Step 3: Configure transfer parameters */
        i2s_get_default_transfer_config(&transfer);
        transfer.sample_rate = 48000;               /* Set sample rate to 48kHz */
        transfer.audio_depth = i2s_audio_depth_16_bits;    /* 16-bit audio data */
        transfer.channel_num_per_frame = 2;         /* 2 channels (stereo) */
        transfer.channel_length = i2s_channel_length_32_bits;  /* 32-bit channel length */
        transfer.master_mode = true;                /* Master mode */
        transfer.protocol = I2S_PROTOCOL_I2S_PHILIPS;  /* Standard I2S protocol */
        transfer.data_line = I2S_DATA_LINE_0;      /* Use data line 0 */
        transfer.channel_slot_mask = 0x3;          /* Enable two channels */

        /* Step 4: Get MCLK frequency */
        mclk_freq = clock_get_frequency(clock_i2s0);

        /* Step 5: Configure and start transfer */
        hpm_stat_t status = i2s_config_tx(HPM_I2S0, mclk_freq, &transfer);
        if (status != status_success) {
            printf("I2S TX config failed\n");
        }

Slave Mode Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Transfer configuration function API:

  .. code-block:: c

        hpm_stat_t i2s_config_tx_slave(I2S_Type *ptr, i2s_transfer_config_t *config);
        hpm_stat_t i2s_config_rx_slave(I2S_Type *ptr, i2s_transfer_config_t *config);
        hpm_stat_t i2s_config_transfer_slave(I2S_Type *ptr, i2s_transfer_config_t *config);

  - Parameter description:

    .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - I2S_Type*
              - Pointer to I2S controller base address
            * - config
              - i2s_transfer_config_t*
              - Pointer to I2S transfer configuration structure

  - Return values:

    .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Configuration successful
            * - status_invalid_argument
              - Invalid parameter

- Example:

  .. code-block:: c

        /* Step 1: I2S initialization, configure as slave mode */
        i2s_config_t i2s_config;
        i2s_transfer_config_t transfer;

        /* Step 2: Get and set default configuration */
        i2s_get_default_config(HPM_I2S0, &i2s_config);
        i2s_config.use_external_mclk = true;
        i2s_config.use_external_bclk = true;
        i2s_config.use_external_fclk = true;
        i2s_init(HPM_I2S0, &i2s_config);

        /* Step 3: Configure transfer parameters */
        i2s_get_default_transfer_config(&transfer);
        transfer.sample_rate = 48000;
        transfer.audio_depth = i2s_audio_depth_16_bits;
        transfer.channel_num_per_frame = 2;
        transfer.channel_length = i2s_channel_length_32_bits;
        transfer.master_mode = false;
        transfer.protocol = I2S_PROTOCOL_I2S_PHILIPS;
        transfer.data_line = I2S_DATA_LINE_0;
        transfer.channel_slot_mask = 0x3;

        /* Step 4: Configure and start transfer */
        hpm_stat_t status = i2s_config_tx_slave(HPM_I2S0, &transfer);
        if (status != status_success) {
            printf("I2S TX slave config failed\n");
        }

Multi-line Transfer Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Multi-line transfer configuration structure:

  .. code-block:: c

        typedef struct i2s_multiline_transfer_config {
            uint32_t sample_rate;           /* Sample rate */
            bool enable_tdm_mode;           /* Enable TDM mode */
            uint8_t channel_num_per_frame;  /* Number of channels per frame */
            uint8_t channel_length;         /* Channel length: 16-bit or 32-bit */
            uint8_t audio_depth;            /* Audio depth: 16-bit, 24-bit, 32-bit */
            bool master_mode;               /* Master/Slave mode selection */
            uint8_t protocol;               /* Protocol selection */
            bool tx_data_line_en[4];        /* TX data line enable */
            bool rx_data_line_en[4];        /* RX data line enable */
            uint32_t tx_channel_slot_mask[4];/* Channel slot mask for each TX data line */
            uint32_t rx_channel_slot_mask[4];/* Channel slot mask for each RX data line */
        } i2s_multiline_transfer_config_t;

- Multi-line transfer configuration API:

  .. code-block:: c

        hpm_stat_t i2s_config_multiline_transfer(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_multiline_transfer_config_t *config);

  - Parameter description:

    .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - ptr
              - I2S_Type*
              - Pointer to I2S controller base address
            * - mclk_in_hz
              - uint32_t
              - MCLK frequency (Hz)
            * - config
              - i2s_multiline_transfer_config_t*
              - Pointer to I2S multi-line transfer configuration structure

  - Return values:

    .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Configuration successful
            * - status_invalid_argument
              - Invalid parameter

- Example:

  .. code-block:: c

        /* I2S multi-line transfer configuration */
        i2s_config_t i2s_config;
        i2s_multiline_transfer_config_t transfer;
        uint32_t mclk_freq;

        /* Get default configuration */
        i2s_get_default_config(HPM_I2S0, &i2s_config);
        i2s_config.enable_mclk_out = true;
        i2s_init(HPM_I2S0, &i2s_config);

        /* Configure multi-line transfer parameters */
        i2s_get_default_multiline_transfer_config(&transfer);
        transfer.sample_rate = audio_data.sample_rate;         /* Set sample rate */
        transfer.channel_num_per_frame = 2;                    /* Channels per frame */
        transfer.audio_depth = audio_data.audio_depth;         /* Set bit depth */
        transfer.channel_length = i2s_channel_length_32_bits;  /* Channel length */
        transfer.master_mode = true;                           /* Master mode */
        transfer.protocol = I2S_PROTOCOL_MSB_JUSTIFIED;        /* MSB justified protocol */

        /* Configure data lines 0-3 for transmission */
        transfer.tx_data_line_en[0] = true;
        transfer.tx_data_line_en[1] = true;
        transfer.tx_data_line_en[2] = true;
        transfer.tx_data_line_en[3] = true;
        transfer.tx_channel_slot_mask[0] = 0x3;  /* Data line 0 uses channels 0-1 */
        transfer.tx_channel_slot_mask[1] = 0x3;  /* Data line 1 uses channels 0-1 */
        transfer.tx_channel_slot_mask[2] = 0x3;  /* Data line 2 uses channels 0-1 */
        transfer.tx_channel_slot_mask[3] = 0x3;  /* Data line 3 uses channels 0-1 */

        /* Get MCLK frequency */
        mclk_freq = clock_get_frequency(clock_i2s0);

        /* Configure transfer parameters */
        hpm_stat_t status = i2s_config_multiline_transfer(HPM_I2S0, mclk_freq, &transfer);
        if (status != status_success) {
            printf("I2S multiline transfer config failed\n");
        }

I2S Data Transfer
------------------

Polling Transfer
^^^^^^^^^^^^^^^^^

- Send data API:

  .. code-block:: c

        void i2s_send_data(I2S_Type *ptr, i2s_line_num_t tx_line_index, uint32_t data);
        uint32_t i2s_send_buff(I2S_Type *ptr, i2s_line_num_t tx_line_index, uint8_t samplebits, uint8_t *src, uint32_t size);

- Receive data API:

  .. code-block:: c

        void i2s_receive_data(I2S_Type *ptr, i2s_line_num_t rx_line_index, uint32_t *data);
        uint32_t i2s_receive_buff(I2S_Type *ptr, i2s_line_num_t rx_line_index, uint8_t samplebits, uint8_t *dst, uint32_t size);

- Example:

  .. code-block:: c

        /* Send single data */
        uint32_t data = 0x12345678;
        i2s_send_data(HPM_I2S0, I2S_DATA_LINE_0, data);

        /* Send buffer data */
        uint8_t tx_buff[32];
        uint32_t sent = i2s_send_buff(HPM_I2S0, I2S_DATA_LINE_0, i2s_audio_depth_16_bits, tx_buff, sizeof(tx_buff));
        if (sent != sizeof(tx_buff)) {
            printf("I2S TX failed\n");
        }

        /* Receive single data */
        uint32_t rx_data;
        i2s_receive_data(HPM_I2S0, I2S_DATA_LINE_0, &rx_data);

        /* Receive buffer data */
        uint8_t rx_buff[32];
        uint32_t received = i2s_receive_buff(HPM_I2S0, I2S_DATA_LINE_0, i2s_audio_depth_16_bits, rx_buff, sizeof(rx_buff));
        if (received != sizeof(rx_buff)) {
            printf("I2S RX failed\n");
        }

DMA Transfer
^^^^^^^^^^^^^

- I2S supports DMA transfer, which can be configured through the following steps:

  1. Enable I2S DMA request
  2. Configure DMAMUX, select I2S as DMA request source
  3. Configure DMA channel parameters
  4. Start DMA transfer

- Related API:

  .. code-block:: c

        static inline void i2s_enable_tx_dma_request(I2S_Type *ptr);
        static inline void i2s_enable_rx_dma_request(I2S_Type *ptr);
        static inline void i2s_disable_tx_dma_request(I2S_Type *ptr);
        static inline void i2s_disable_rx_dma_request(I2S_Type *ptr);

- Example:

  .. code-block:: c

        /* Step 1: Enable I2S DMA request */
        i2s_enable_tx_dma_request(HPM_I2S0);

        /* Step 2: Configure DMAMUX, select I2S as DMA request source */
        dmamux_config(HPM_DMAMUX, DMA_MUX_CHANNEL, I2S_DMA_REQ, true);

        /* Step 3: Configure DMA channel parameters */
        dma_channel_config_t config = {
            .src_width = DMA_TRANSFER_WIDTH_WORD,    /* Source data width: 32-bit */
            .dst_width = DMA_TRANSFER_WIDTH_WORD,    /* Destination data width: 32-bit */
            .src_addr = (uint32_t)tx_buff,          /* Source buffer address */
            .dst_addr = (uint32_t)&HPM_I2S0->TXD[0], /* I2S TX register address */
            .src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T,  /* Source burst size: 1 */
            .dst_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T,  /* Destination burst size: 1 */
            .size_in_byte = sizeof(tx_buff)         /* Total transfer size in bytes */
        };
        dma_setup_channel(HPM_DMA, DMA_CHANNEL, &config, true);

        /* Step 4: Start DMA transfer */
        dma_start_channel(HPM_DMA, DMA_CHANNEL);

I2S Interrupts
---------------

- I2S supports the following interrupts:

  .. list-table::
        :header-rows: 1

        * - Interrupt Type
          - Description
        * - TX_FIFO_THRESHOLD
          - TX FIFO below threshold interrupt
        * - RX_FIFO_THRESHOLD
          - RX FIFO above threshold interrupt
        * - TX_FIFO_EMPTY
          - TX FIFO empty interrupt
        * - RX_FIFO_FULL
          - RX FIFO full interrupt

- Related API:

  .. code-block:: c

        void i2s_enable_irq(I2S_Type *ptr, uint32_t mask);
        void i2s_disable_irq(I2S_Type *ptr, uint32_t mask);
        void i2s_clear_irq_status(I2S_Type *ptr, uint32_t mask);

- Example:

  .. code-block:: c

        /* Enable TX FIFO threshold interrupt */
        i2s_enable_irq(HPM_I2S0, i2s_tx_fifo_threshold_irq_mask);

        /* Configure interrupt priority and enable interrupt */
        intc_m_enable_irq_with_priority(IRQn_I2S0, 1);

        /* Interrupt handler */
        void isr_i2s(void)
        {
            uint32_t status = i2s_get_irq_status(HPM_I2S0);
            if (status & i2s_tx_fifo_threshold_irq_mask) {
                /* Handle TX FIFO threshold interrupt */
                i2s_clear_irq_status(HPM_I2S0, i2s_tx_fifo_threshold_irq_mask);
            }
        }

Important Notes
----------------

- I2S Data Width Alignment Rules:

  - When transferring data with different bit widths, I2S always keeps valid data in the high bits of the 32-bit interface register
  - For 16-bit data format:

    - For sending: 16-bit data needs to be left-shifted by 16 bits before writing to TX FIFO register
    - For receiving: data needs to be right-shifted by 16 bits after reading from RX FIFO register

  - Data shift requirements:

    - Data shift operation is required when using `i2s_send_data` and `i2s_receive_data` functions
    - Data shift operation is required when using DMA transfer
    - Shift bits = 32 - actual data width

.. toctree::
  :maxdepth: 3

  i2s/README_en
  i2s_dma/README_en
  i2s_interrupt/README_en
  i2s_master/README_en
  i2s_slave/README_en

