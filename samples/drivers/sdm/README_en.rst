.. _sdm:

SDM
======

Overview
-------------

The SDM sample project demonstrates the functionality of data sampling and amplitude detection for the data stream output by the Sigma-Delta modulator of SDM peripherals.

Key Features
---------------

- Signal Input Interface:

  - 4 independent CLK and DAT signal inputs
  - Each CLK/DAT pair can be independently configured for sampling mode
  - Supports multiple sampling modes:

    - Mode 0: Sample on MCLK rising edge
    - Mode 1: Sample on both MCLK rising and falling edges
    - Mode 2: Manchester encoding mode
    - Mode 3: Sample on MCLK falling edge
    - Mode 4: Sample every 2 MCLK rising edges
    - Mode 5: Sample every 2 MCLK falling edges

- Data Filter (PCM_CIC):

  - Supports 4 filter types:

    - Sinc1 (1st order)
    - Sinc2 (2nd order)
    - Sinc3 (3rd order)
    - SincFast (Fast 2nd order)

  - Configurable oversampling rate (1-256)
  - Built-in 16-depth 32-bit width FIFO
  - Synchronous sampling support
  - Gate control support (specific versions)
  - Optional timestamp output (specific versions)

- Amplitude Detector (AMP_CIC):

  - Supports the same 4 filter types
  - Independent amplitude monitoring channel
  - High/Low threshold detection
  - Zero-crossing detection
  - Configurable oversampling rate (1-32)
  - Real-time amplitude output

- Interrupt Support:

  - FIFO threshold interrupt
  - Data overflow interrupt
  - Data saturation interrupt
  - Amplitude limit interrupt

- Synchronization Features:

  - TRGMUX trigger input support
  - TRGMUX trigger output support

SDM Initialization
---------------------

- Structure Definitions:

  - `sdm_control_t` structure for basic SDM parameters:

    .. code-block:: c

        typedef struct {
            uint8_t clk_signal_sync;    /* Clock signal sync configuration */
            uint8_t data_signal_sync;   /* Data signal sync configuration */
            bool interrupt_en;          /* Interrupt enable */
        } sdm_control_t;

  - `sdm_channel_common_config_t` structure for channel common parameters:

    .. code-block:: c

        typedef struct {
            uint8_t sampling_mode;      /* Sampling mode */
            bool enable_err_interrupt;  /* Error interrupt enable */
            bool enable_data_ready_interrupt; /* Data ready interrupt enable */
        } sdm_channel_common_config_t;

  - `sdm_filter_config_t` structure for data filter parameters:

    .. code-block:: c

        typedef struct {
            uint8_t fifo_threshold;     /* FIFO threshold */
            uint8_t filter_type;        /* Filter type */
            uint16_t oversampling_rate; /* Oversampling rate */
            bool output_32bit;          /* 32-bit output enable */
            uint8_t sync_source;        /* Sync source selection */
            /* Other parameters omitted */
        } sdm_filter_config_t;

Data Filter Configuration
--------------------------

- Initialization API:

  .. code-block:: c

    void sdm_init_module(SDM_Type *ptr, sdm_control_t *control);
    void sdm_config_channel_common_setting(SDM_Type *ptr, uint8_t ch_index, sdm_channel_common_config_t *config);
    void sdm_config_channel_filter(SDM_Type *ptr, uint8_t ch_index, sdm_filter_config_t *filter_config);

- Example:

  .. code-block:: c

    /* SDM basic initialization */
    sdm_control_t control = {
        .clk_signal_sync = 0xf,
        .data_signal_sync = 0xf,
        .interrupt_en = true
    };
    sdm_init_module(HPM_SDM, &control);

    /* Configure channel sampling mode */
    sdm_channel_common_config_t ch_config = {
        .sampling_mode = sdm_sampling_rising_clk_edge,
        .enable_err_interrupt = true,
        .enable_data_ready_interrupt = true
    };
    sdm_config_channel_common_setting(HPM_SDM, channel, &ch_config);

    /* Configure data filter */
    sdm_filter_config_t filter_config;
    sdm_get_channel_default_filter_config(HPM_SDM, &filter_config);
    filter_config.filter_type = sdm_filter_sinc3;
    filter_config.oversampling_rate = 256;
    sdm_config_channel_filter(HPM_SDM, channel, &filter_config);

Amplitude Detector Configuration
--------------------------------

- Configuration API:

  .. code-block:: c

    void sdm_config_channel_comparator(SDM_Type *ptr, uint8_t ch_index, sdm_comparator_config_t *cmp_config);

- Example:

  .. code-block:: c

    /* Configure amplitude detector */
    sdm_comparator_config_t cmp_config;
    sdm_get_channel_default_comparator_config(HPM_SDM, &cmp_config);
    cmp_config.high_threshold = threshold;
    cmp_config.en_high_threshold_int = true;
    cmp_config.filter_type = sdm_filter_sinc3;
    sdm_config_channel_comparator(HPM_SDM, channel, &cmp_config);

Data Transfer
----------------

- Data Reading API:

  .. code-block:: c

    int32_t sdm_get_channel_fifo_data(SDM_Type *ptr, uint8_t ch);
    int16_t sdm_get_channel_fifo_16bit_data(SDM_Type *ptr, uint8_t ch);
    uint16_t sdm_get_channel_comparator_data(SDM_Type *ptr, uint8_t ch);

- Example:

  .. code-block:: c

    /* Read 32-bit filtered data */
    int32_t data = sdm_get_channel_fifo_data(HPM_SDM, channel);

    /* Read 16-bit filtered data */
    int16_t data = sdm_get_channel_fifo_16bit_data(HPM_SDM, channel);

    /* Read amplitude detection result */
    uint16_t amplitude = sdm_get_channel_comparator_data(HPM_SDM, channel);

Synchronous Sampling
--------------------

- Synchronous Sampling Configuration:

  1. Select sync signal source
  2. Configure TRGM module connection
  3. Enable sync functionality

- Example:

  .. code-block:: c

    /* Configure synchronous sampling */
    filter_config.sync_source = sync_src;
    filter_config.fifo_clean_on_sync = 1;
    filter_config.wtsyncen = 1;
    filter_config.wtsynaclr = 1;
    sdm_config_channel_filter(HPM_SDM, channel, &filter_config);

    /* Configure TRGM connection */
    trgm_output_update_source(TRGM, output_index, input_src);

Sample Workflow
----------------

This example demonstrates four working modes:

1. Polling Mode Sampling
   Configuration Steps:

   - Initialize SDM module, configure clock and data synchronization
   - Configure channel sampling mode to rising edge sampling
   - Configure data filter:

     * Select Sinc3 filter type
     * Set oversampling rate to 256
     * Configure 32-bit data output

   - Enable SDM channel
   - Polling mode data reading:

     * Wait for data ready flag
     * Read sampling data from FIFO
     * Convert and display voltage value

2. Amplitude Detection Mode

   SDM uses the amplitude monitoring channel to monitor voltage over-limit events. The high voltage threshold for amplitude detection is set to SDM_AMPLITUDE_HIGH_THRESHOLD. When the input analog voltage increases from below this threshold to above it, an error interrupt is triggered.

   Configuration Steps:

   - Initialize SDM module, configure clock and data sync, and enable global interrupt
   - Configure channel rising edge sampling mode and enable error interrupt (amplitude over limit)
   - Configure amplitude detector:

     * Select Sinc3 filter type
     * Set oversampling rate
     * Configure high voltage threshold (240mV)
     * Enable high voltage threshold interrupt

   - Enable SDM channel
   - Wait for amplitude over-limit interrupt:

     * Read amplitude in interrupt handler
     * Display over-limit voltage value
     * Stop sampling and clear interrupt flag

3. Interrupt Mode Sampling

   Configuration Steps:

   - Initialize SDM module, configure clock and data sync, and enable global interrupt
   - Configure channel:

     * Set rising edge sampling mode
     * Enable error interrupt and data ready interrupt

   - Configure data filter:

     * Select Sinc3 filter type
     * Set oversampling rate to 256
     * Enable FIFO function and threshold

   - Enable SDM channel
   - Interrupt handling process:

     * FIFO threshold triggers interrupt
     * Read data from FIFO
     * Stop sampling when reaching specified data count

4. Synchronous Signal Sampling Mode

   In this example, GPTMR output is connected to SDM sync input through TRGMMUX configuration. When GPTMR output signal rises, it triggers SDM sync event.
   SDM samples data according to sync signals. When a sync event occurs, SDM starts sampling data into FIFO; when FIFO data reaches preset threshold, SDM triggers data ready interrupt to read data, and hardware clears sync event flag, stopping further sampling until next sync event.

   Configuration Steps:

   - Initialize SDM module, configure clock and data sync, and enable global interrupt
   - Configure SDM channel:

     * Set rising edge sampling mode
     * Enable error interrupt and data ready interrupt

   - Configure data filter:

     * Select Sinc3 filter type
     * Set oversampling rate to 256
     * Enable FIFO function and threshold
     * Select sync signal source
     * Enable synchronous sampling
     * Enable FIFO auto-clear
     * Enable data ready auto-clear sync flag

   - Enable SDM channel
   - Configure GPTMR to generate sync signal:

     * Set 4-second period
     * Configure 50% duty cycle

   - Configure TRGM module:

     * Connect GPTMR output to SDM sync input

   - Synchronous sampling process:

     * Wait for sync event trigger
     * Read data when FIFO reaches threshold
     * Auto-clear sync flag
     * Wait for next sync event

Hardware Setting
--------------------

Please refer to :ref:`Pin Description <board_resource>` for specific board.
Different hardware settings are used depending on whether an AD sampling chip is on the development board:

- Without onboard AD sampling chip:

  - External sampling chip required
  - Connect CLK and DAT pins of sampling chip to SDM CLK and DAT pins on board

- With onboard AD sampling chip:

  - Simply connect analog signal to sampling chip input interface

Notes
----------

1. This example provides a conversion formula for input voltage and output value suitable for Sigma-Delta modulators such as AD7400 and NSI1306 (Full-scale ±320 mV). If using a different type of Sigma-Delta modulator, please check the conversion formula.

2. For synchronous sampling, note:

   - Initial samples after sync event may be inaccurate and should be ignored
   - Number of samples to ignore varies by filter type:

     * Sinc1: 0 samples
     * Sinc2: 1 sample
     * Sinc3: 2 samples
     * SincFast: 2 samples

3. For 16-bit data output mode:

   - Proper shift value configuration needed to avoid data overflow
   - Output range varies with different filter types and oversampling rates

Running the example
-----------------------

When running the project, you will see a menu interface through the serial port. Enter the corresponding number to execute different test functions:

.. code-block:: console

   sdm example
   *********************************************************************
   *                                                                   *
   *                         SDM Example Menu                          *
   * Please enter one of following SDM function test(e.g. 1 or 2 ...): *
   * 1 - SDM sample data in polling mode                               *
   * 2 - SDM amplitude threshold check                                 *
   * 3 - SDM sample data in interrupt mode                             *
   * 4 - SDM sample data with sync signal                              *
   *                                                                   *
   *********************************************************************

Each mode will display corresponding test results, including sampled voltage values or detected over-limit conditions.

1. SDM sample data in polling mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SDM Polling Data Valid Flag and Reading Sample Data from FIFO.

.. code-block:: console

   Selected option: 1

   sdm sample data in polling mode.
   203.57mV
   203.17mV
   203.31mV
   203.14mV
   203.01mV
   203.28mV
   203.17mV
   203.05mV
   203.19mV
   203.21mV
   203.02mV
   203.19mV
   203.43mV
   203.17mV
   203.29mV
   203.24mV


2. SDM amplitude threshold check
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SDM detects voltage over-limit events. The high voltage threshold for amplitude detection is set to SDM_AMPLITUDE_HIGH_THRESHOLD. When the input analog voltage increases from below this threshold to above it, an error interrupt is triggered.

.. code-block:: console

   Selected option: 2

   Set amplitude high threshold 240mV
   sdm channel comparator detected amplitude over the high threshold
   voltage: 253.75mV


3. SDM sample data in interrupt mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SDM samples data from FIFO with the data ready flag. When the data in the FIFO reaches a preset threshold, SDM triggers a data ready interrupt. During the interrupt handler, data is read, and stop the sampling when the number of data read reaches TEST_DATA_COUNT.

.. code-block:: console

   Selected option: 3

   sdm sample data in interrupt mode.
   203.39mV
   203.08mV
   202.96mV
   202.87mV
   202.85mV
   202.86mV
   202.86mV
   202.84mV
   202.81mV
   202.88mV
   203.01mV
   203.23mV
   203.08mV
   202.97mV
   202.92mV
   202.97mV


4. SDM sample data with sync signal
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this example, GPTMR output is connected to SDM sync input through TRGMMUX configuration. When GPTMR output signal rises, it triggers SDM sync event.
SDM samples data according to sync signals. When a sync event occurs, SDM starts sampling data into FIFO; when FIFO data reaches preset threshold, SDM triggers data ready interrupt to read data, and hardware clears sync event flag, stopping further sampling until next sync event.

.. code-block:: console

   Selected option: 4

   sdm sample data with sync signal. 1 time
   203.88mV
   203.84mV
   203.90mV
   203.87mV
   203.91mV
   203.92mV
   203.85mV
   203.85mV
   sdm sample data with sync signal. 2 time
   203.90mV
   203.96mV
   203.97mV
   203.93mV
   203.94mV
   203.86mV
   203.89mV
   203.90mV
   sdm sample data with sync signal. 3 time
   203.94mV
   204.03mV
   203.97mV
   203.91mV
   203.89mV
   203.87mV
   203.93mV
   203.97mV
   sdm sample data with sync signal. 4 time
   203.93mV
   204.13mV
   204.14mV
   204.16mV
   204.17mV
   204.13mV
   204.14mV
   204.12mV

