.. _i2s_over_spi:

I2S Emulation
=============

Overview
--------

- The ``i2s_over_spi`` component provides a set of API operations to emulate I2S over SPI for configuration, initialization, and read/write operations on the I2S bus. It supports various I2S devices and provides data transfer functionalities in both blocking and non-blocking modes. The key features of this component are as follows:

  - **Multi-instance Support**
    - Supports multiple SPI-emulated I2S instances (e.g., I2S0, I2S1, etc.), each with independent configuration parameters.
    - Manages configurations for each I2S instance via the ``hpm_i2s_over_spi_t`` structure, including emulation of bclk (bit clock), lrck (channel clock), mclk (master clock), and I2S data generation.

  - **Blocking and Non-blocking Read/Write Operation Interfaces**
    - **Supports only host mode.**
    - Provides **non-blocking read/write functions**.
    - Provides **blocking read/write functions** with timeout mechanisms to ensure timely error returns in case of prolonged unresponsiveness.

Structure Descriptions
--------------------------

- Structure Descriptions

  - GPTMR Timer Configuration Structure
    This structure is used for generating I2S clocks and encapsulates the critical hardware resources required for I2S protocol clock generation, including:

    - **bclk (Bit Clock)**: Controls the sampling frequency of data bits.
    - **lrck (Frame Clock)**: Determines the left-right channel switching frequency.
    - **mclk (Master Clock)**: Provides the system reference clock.

    .. code-block:: c

        typedef struct {
            GPTMR_Type *ptr;          /**< GPTMR hardware register pointer */
            clock_name_t clock_name;  /**< Clock source selection (e.g., clock_mclk) */
            uint8_t channel;          /**< Timer channel number (0 or 1) */
        } hpm_i2s_gptmr_context_t;

  - SPI Slave Context Configuration Structure
    This structure is used to emulate the I2S data channel via SPI, encapsulating the hardware resources required for I2S data transmission over the SPI interface:

    .. code-block:: c

        typedef struct {
            SPI_Type *ptr;          /**< SPI hardware register pointer */
            clock_name_t clock_name; /**< Clock source selection */
            uint16_t txdma_src;     /**< TX DMA request source number */
            uint16_t rxdma_src;     /**< RX DMA request source number */
            uint32_t cs_pin;        /**< Chip select pin number */
            void (*write_cs)(uint32_t cs_pin, uint8_t state); /**< Chip select control callback function */
        } hpm_i2s_spi_context_t;

  - DMA Context Configuration Structure
    This structure manages I2S data transfers and encapsulates DMA chain transfer resources, supporting multi-segment continuous transmission:

    .. code-block:: c

        typedef struct {
            dma_resource_t *resource;           /**< DMA channel resource pointer */
            dma_linked_descriptor_t *descriptors; /**< Linked descriptor array pointer */
        } hpm_i2s_dma_context_t;

  - I2S-over-SPI Context Structure
    Integrates all hardware resources and state information required to emulate I2S over SPI:

    .. code-block:: c

        typedef struct hpm_i2s_over_spi {
            /* Clock Generation Module */
            hpm_i2s_gptmr_context_t bclk;   /**< Bit clock generator (controls data rate) */
            hpm_i2s_gptmr_context_t lrck;   /**< Channel clock generator (controls sampling rate) */
            hpm_i2s_gptmr_context_t mclk;   /**< Master clock generator (drives codec) */

            /* Data Transfer Module */
            hpm_i2s_spi_context_t spi_slave;/**< SPI slave configuration (carries I2S data stream) */
            hpm_i2s_dma_context_t tx_dma;   /**< TX DMA resource (manages audio output) */
            hpm_i2s_dma_context_t rx_dma;    /**< RX DMA resource (manages audio input) */

            /* Status Control Module */
            bool i2s_rx;                    /**< Receive enable flag */
            i2s_rx_data_tc rx_callback;      /**< Receive completion callback pointer */
            hpm_i2s_gptmr_context_t transfer_time; /**< Transfer timer (records transmission duration) */
            bool has_done;                   /**< Transfer completion flag */
            void (*transfer_complete)(struct hpm_i2s_over_spi *i2s); /**< Global transfer completion callback */
        } hpm_i2s_over_spi_t;

I2S-over-SPI Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Call the ``hpm_i2s_master_over_spi_init`` API to initialize the I2S-over-SPI. During this process, the parameters of the ``i2s_device`` instance variable are assigned to the ``i2s`` parameter of the API and take effect.

- ``hpm_i2s_master_over_spi_init`` API prototype:

    .. code-block:: c

        hpm_stat_t hpm_i2s_master_over_spi_init(hpm_i2s_over_spi_t *i2s)

    - Parameter description:

        .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - i2s
              - hpm_i2s_over_spi_t*
              - Pointer to the I2S-over-SPI context structure containing I2S configuration and status information

    - Return values:

      - ``status_success``: Success
      - ``status_invalid_argument``: Invalid parameter
      - ``status_fail``: Other errors

    - **Example**: How to use ``hpm_i2s_master_over_spi_init`` to initialize I2S-over-SPI

        .. code-block:: c

            hpm_i2s_master_over_spi_init(&i2s_device);

I2S-over-SPI Receive Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Receive Configuration
"""""""""""""""""""""

- Call the ``hpm_i2s_master_over_spi_rx_config`` API to configure I2S receive parameters.

  - Configure I2S receive parameters, including protocol type, sampling rate, audio bit depth, and receive buffers.
  - After configuration, the I2S-over-SPI will initialize and prepare for receive operations based on the configuration.
  - This API supports single-buffer and dual-buffer modes.
  - In dual-buffer mode, I2S-over-SPI uses two buffers alternately to ensure data continuity and integrity.
  - In dual-buffer mode, the ``rx_callback`` callback function must determine which buffer's data is being processed and handle it accordingly.

- ``hpm_i2s_master_over_spi_rx_config`` API prototype:

    .. code-block:: c

        hpm_stat_t hpm_i2s_master_over_spi_rx_config(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz,
                                                uint32_t audio_depth, uint8_t *buffer0, uint8_t *buffer1, uint32_t size);

    - Parameter description:

        .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - i2s
              - hpm_i2s_over_spi_t*
              - Pointer to the I2S-over-SPI context structure
            * - protocol
              - uint8_t
              - Protocol type. Currently supports: ``I2S_PROTOCOL_MSB_JUSTIFIED``. Unsupported: ``I2S_PROTOCOL_PCM`` and ``I2S_PROTOCOL_I2S_PHILIPS``
            * - lrck_hz
              - uint32_t
              - Frame clock frequency (sampling rate) in Hz (e.g., 8000/48000)
            * - audio_depth
              - uint32_t
              - Audio bit depth. Currently supports: 16-bit and 32-bit
            * - buffer0
              - uint8_t*
              - Pointer to receive buffer 0
            * - buffer1
              - uint8_t*
              - Pointer to receive buffer 1 (used in dual-buffer mode)
            * - size
              - uint32_t
              - Buffer size in bytes

    - Return values:

        .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Configuration succeeded
            * - status_invalid_argument
              - Invalid parameters (including: invalid buffer pointer, unsupported protocol type, buffer size exceeds limits)

    - **Example**: How to configure I2S-over-SPI receive parameters

        .. code-block:: c

            #define RX_SIZE_MAX             (4096U)
            /* Dual-buffer configuration */
            ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) uint8_t rx_buffer0[RX_SIZE_MAX];
            ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) uint8_t rx_buffer1[RX_SIZE_MAX];

            hpm_i2s_master_over_spi_rx_config(&i2s_device,
                                            I2S_PROTOCOL_MSB_JUSTIFIED,
                                            48000,
                                            16,
                                            rx_buffer0,
                                            rx_buffer1,
                                            RX_SIZE_MAX);

Start Receiving
"""""""""""""""

- Call the ``hpm_i2s_master_over_spi_rx_start`` API to start the configured I2S receive process.

  - Start the I2S receive process to begin data reception.
  - This API starts the I2S-over-SPI receive operation based on the configuration and triggers the callback function upon completion.

- ``hpm_i2s_master_over_spi_rx_start`` API prototype:

    .. code-block:: c

        hpm_stat_t hpm_i2s_master_over_spi_rx_start(hpm_i2s_over_spi_t *i2s, i2s_rx_data_tc callback);

    - Parameter description:

        .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - i2s
              - hpm_i2s_over_spi_t*
              - Pointer to the I2S-over-SPI context structure (must be configured via ``rx_config``)
            * - callback
              - i2s_rx_data_tc
              - Callback function pointer triggered when DMA completes a buffer transfer

    - Return values:

        .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Receive process started successfully
            * - status_invalid_argument
              - Invalid parameters (e.g., null context pointer or callback pointer)

    - **Example**: How to start I2S-over-SPI receive using ``hpm_i2s_master_over_spi_rx_start``

        .. code-block:: c

            /* Define receive completion callback */
            void rx_done_callback(uint32_t buf_index) {
                printf("Buffer %d received\n", buf_index);
                /* Process the received data buffer. buf_index indicates the buffer index (e.g., 0 for buffer0, 1 for buffer1 in dual-buffer mode) */
            }

            int main(void) {
                /* Configure receive parameters... */
                hpm_i2s_master_over_spi_rx_config(&i2s_device, ...);

                /* Start receive and register callback */
                hpm_stat_t result = hpm_i2s_master_over_spi_rx_start(&i2s_device, rx_done_callback);
                if (result != status_success) {
                    printf("RX start failed: 0x%x\n", result);
                }
                /* TODO */
            }

Stop Receiving
""""""""""""""

- Call the ``hpm_i2s_master_over_spi_rx_stop`` API to stop the ongoing I2S receive process.
  - This API stops the I2S-over-SPI receive operation.

- ``hpm_i2s_master_over_spi_rx_stop`` API prototype:

    .. code-block:: c

        hpm_stat_t hpm_i2s_master_over_spi_rx_stop(hpm_i2s_over_spi_t *i2s);

    - Parameter description:

        .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - i2s
              - hpm_i2s_over_spi_t*
              - Pointer to the I2S-over-SPI context structure

    - Return values:

        .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Receive process stopped successfully
            * - status_invalid_argument
              - Invalid parameter (e.g., null context pointer)

    - **Example**: How to stop I2S-over-SPI receive using ``hpm_i2s_master_over_spi_rx_stop``

        .. code-block:: c

            /* Stop the receive process */
            hpm_stat_t result = hpm_i2s_master_over_spi_rx_stop(&i2s_device);
            if (result != status_success) {
                printf("Failed to stop RX: 0x%x\n", result);
            }

I2S-over-SPI Transmission Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Starting Transmission: Blocking vs. Non-blocking Interfaces
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

Non-blocking Transmission
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Call the ``hpm_i2s_master_over_spi_tx_buffer_nonblocking`` API to start a non-blocking transmission.
  - Initiates the I2S transmission process to send data.
  - This API starts the I2S-over-SPI transmission operation based on the configuration.

    - ``hpm_i2s_master_over_spi_tx_buffer_nonblocking`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_i2s_master_over_spi_tx_buffer_nonblocking(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz,
                                                            uint8_t audio_depth, uint8_t *data, uint32_t size);

    - Parameter description:

        .. list-table::
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - i2s
              - hpm_i2s_over_spi_t*
              - Pointer to the I2S-over-SPI context structure (must be initialized)
            * - protocol
              - uint8_t
              - Protocol type. Currently supports: ``I2S_PROTOCOL_MSB_JUSTIFIED``. Unsupported: ``I2S_PROTOCOL_PCM`` and ``I2S_PROTOCOL_I2S_PHILIPS``
            * - lrck_hz
              - uint32_t
              - Frame clock frequency (sampling rate) in Hz (e.g., 8000/16000)
            * - audio_depth
              - uint8_t
              - Audio bit depth. Currently supports: 16-bit and 32-bit
            * - data
              - uint8_t*
              - Pointer to the transmit data buffer
            * - size
              - uint32_t
              - Data length in bytes

    - Return values:

        .. list-table::
            :header-rows: 1

            * - Return Value
              - Description
            * - status_success
              - Transmission successfully started
            * - status_invalid_argument
              - Invalid parameters (e.g., invalid buffer pointer, unsupported protocol type, data length exceeds limits)

    - **Example**: How to use ``hpm_i2s_master_over_spi_tx_buffer_nonblocking`` for non-blocking transmission

        .. code-block:: c

            /* Non-blocking send of 16-bit audio data */
            ATTR_PLACE_AT_NONCACHEABLE uint8_t tx_buffer[TX_SIZE_MAX];

            hpm_stat_t result = hpm_i2s_master_over_spi_tx_buffer_nonblocking(&i2s_device,
                                                                            I2S_PROTOCOL_MSB_JUSTIFIED,
                                                                            16000,
                                                                            16,
                                                                            tx_buffer,
                                                                            TX_SIZE_MAX);
            if (result != status_success) {
                printf("Non-blocking transmission failed: 0x%x\n", result);
            }

- Use ``hpm_i2s_master_over_spi_tx_is_busy`` API to check the transmission status. It returns ``true`` while the transmission is ongoing.

    - ``hpm_i2s_master_over_spi_tx_is_busy`` API prototype:

        .. code-block:: c

            bool hpm_i2s_master_over_spi_tx_is_busy(hpm_i2s_over_spi_t *i2s);

        - Parameter description:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - i2s
                  - hpm_i2s_over_spi_t*
                  - Pointer to the I2S-over-SPI context structure

        - Return values:

            .. list-table::
                :header-rows: 1

                * - Return Value
                  - Description
                * - true
                  - Transmission is ongoing
                * - false
                  - Transmission completed or not started

        - **Example**: How to use ``hpm_i2s_master_over_spi_tx_is_busy`` to check transmission status

           .. code-block:: c

                /* Poll status during non-blocking transmission */
                while (hpm_i2s_master_over_spi_tx_is_busy(&i2s_device)) {
                }

Blocking Transmission
~~~~~~~~~~~~~~~~~~~~~~~

- Call the ``hpm_i2s_master_over_spi_tx_buffer_blocking`` API to execute a blocking transmission.

    - Initiates the I2S transmission process and blocks until the transmission completes.

- ``hpm_i2s_master_over_spi_tx_buffer_blocking`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_i2s_master_over_spi_tx_buffer_blocking(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz,
                                                        uint8_t audio_depth, uint8_t *data, uint32_t size, uint32_t timeout);

        - Parameter description:

            .. list-table::
                :header-rows: 1

                * - Parameter
                  - Type
                  - Description
                * - i2s
                  - hpm_i2s_over_spi_t*
                  - Pointer to the I2S-over-SPI context structure (must be initialized)
                * - protocol
                  - uint8_t
                  - Protocol type. Currently supports: ``I2S_PROTOCOL_MSB_JUSTIFIED``. Unsupported: ``I2S_PROTOCOL_PCM`` and ``I2S_PROTOCOL_I2S_PHILIPS``
                * - lrck_hz
                  - uint32_t
                  - Frame clock frequency (sampling rate) in Hz (e.g., 8000/16000)
                * - audio_depth
                  - uint8_t
                  - Audio bit depth. Currently supports: 16-bit and 32-bit
                * - data
                  - uint8_t*
                  - Pointer to the transmit data buffer
                * - size
                  - uint32_t
                  - Data length in bytes
                * - timeout
                  - uint32_t
                  - Timeout in milliseconds. If the operation does not complete within this time, the function will return a timeout error.

        - Return values:

            .. list-table::
                :header-rows: 1

                * - Return Value
                  - Description
                * - status_success
                  - Transmission completed successfully
                * - status_invalid_argument
                  - Invalid parameters (e.g., invalid buffer pointer, unsupported protocol type, data length exceeds limits)

    - **Example**: How to use ``hpm_i2s_master_over_spi_tx_buffer_blocking`` for blocking transmission

           .. code-block:: c

                /* Blocking send of 16-bit audio data */
                uint8_t tx_buffer[TX_SIZE_MAX];

                hpm_stat_t result = hpm_i2s_master_over_spi_tx_buffer_blocking(&i2s_device,
                                                                            I2S_PROTOCOL_MSB_JUSTIFIED,
                                                                            16000,
                                                                            16,
                                                                            tx_buffer,
                                                                            TX_SIZE_MAX,
                                                                            1000000);
                if (result != status_success) {
                    printf("Blocking transmission failed: 0x%x\n", result);
                }

Notes
-------

- Since the I2S_over_SPI component uses the DMA Manager for channel allocation, ensure that DMA channels used by other components do **not** conflict with those used by I2S_over_SPI.

    - The TX DMA channel used by I2S_over_SPI can be found in the ``i2s_device`` instance's member variables, e.g., ``i2s_device.tx_dma.resource``.
    - The RX DMA channel used by I2S_over_SPI can be found in the ``i2s_device`` instance's member variables, e.g., ``i2s_device.rx_dma.resource``.

- **Example** : How to Use DMA Channel Resources

    .. code-block:: c

        hpm_i2s_over_spi_t i2s_device;
        /* Initialize I2S over SPI instance... details omitted */
        /* Print the DMA instance and channel used by the transmit DMA resource */
        printf("TX DMA instance: %d, TX DMA channel: %d\n", i2s_device.rx_dma.resource->dma_instance, i2s_device.rx_dma.resource->dma_channel);
        /* Change the TX DMA resource's interrupt priority to 1 */
        dma_mgr_enable_dma_irq_with_priority(i2s_device.rx_dma.resource, 1);
        /* Print the DMA instance and channel used by the receive DMA resource */
        printf("RX DMA instance: %d, RX DMA channel: %d\n", i2s_device.rx_dma.resource->dma_instance, i2s_device.rx_dma.resource->dma_channel);
        /* Change the RX DMA resource's interrupt priority to 1 */
        dma_mgr_enable_dma_irq_with_priority(i2s_device.rx_dma.resource, 1);