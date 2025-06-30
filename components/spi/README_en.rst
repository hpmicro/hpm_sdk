.. _spi_component:

SPI Component
==========================

Overview
--------

- The SPI component provides a higher-level set of APIs compared to the driver layer for configuring, initializing, and operating the SPI bus. It supports multiple SPI devices and offers data transmission functionalities in both blocking and non-blocking modes. Below are the key features of this component:

  - Multi-instance support

    - Supports multiple SPI instances (such as SPI0, SPI1, etc.), each with independent configuration parameters.

  - Blocking and non-blocking read/write operation interfaces

    - Supports both master and slave modes.

    - Provides non-blocking read/write functions.

    - Provides blocking read/write functions with timeout mechanisms to ensure timely error status returns in case of long unresponsiveness.

Introduction to Relevant Enums and Structs
------------------------------------------

- Enum introduction

  - SPI working mode enum type: This enum defines two different SPI working mode options, namely master mode and slave mode. This enum is defined in `hpm_spi_drv.h`.


    .. code-block:: c

       typedef enum {
           spi_master_mode = 0,
           spi_slave_mode
       } spi_mode_selection_t;



  - SPI data transfer format enum type: Supports three data formats—single-wire, dual-wire, and quad-wire. This enum is defined in `hpm_spi_drv.h`.


    .. code-block:: c

       typedef enum {
           spi_single_io_mode = 0,
           spi_dual_io_mode,
           spi_quad_io_mode,
       } spi_data_phase_format_t;



  - SPI CPOL enum type: This enum defines the idle state level of the SPI SCLK. Specifically, it defines low idle and high idle states. This enum is defined in `hpm_spi_drv.h`.


    .. code-block:: c

       typedef enum {
           spi_sclk_low_idle = 0,
           spi_sclk_high_idle
       } spi_sclk_idle_state_t;



  - SPI CPHA enum type: This enum specifies whether the SPI SCLK sampling occurs on odd or even clock edges. Specifically, it defines odd clock edges and even clock edges. This enum is defined in `hpm_spi_drv.h`.


    .. code-block:: c

       typedef enum {
           spi_sclk_sampling_odd_clk_edges = 0,
           spi_sclk_sampling_even_clk_edges
       } spi_sclk_sampling_clk_edges_t;



  - SPI bit order enum type: This enum specifies the bit order in SPI communication. `spi_msb_first` indicates most significant bit first, while `spi_lsb_first` indicates least significant bit first. This enum is defined in `hpm_spi_drv.h`.


    .. code-block:: c

       typedef enum {
           spi_msb_first = 0,
           spi_lsb_first,
       } spi_shift_direction_t;


- Struct introduction

  - Initialization configuration struct, used for configuring settings of the SPI. It includes parameters such as SPI operation mode, data line mode, clock polarity, clock phase, data shift direction, data length (in bits), and data merging mode.


    .. code-block:: c

       typedef struct {
           spi_mode_selection_t mode;               /*!< SPI working mode */
           spi_data_phase_format_t io_mode;         /*!< SPI data line mode */
           spi_sclk_idle_state_t clk_polarity;      /*!< Clock polarity (CPOL) */
           spi_sclk_sampling_clk_edges_t clk_phase; /*!< Clock phase (CPHA) */
           spi_shift_direction_t direction;         /*!< Data shift direction (MSB or LSB) */
           uint8_t data_len;                        /*!< Length in bits (1~32 bits) */
           bool data_merge;                         /*!< Data merging mode, only supported when data_len is 8 */
       } spi_initialize_config_t;



API Call Flow Introduction
--------------------------

Define SPI Initialization Variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Define a variable of the ``spi_initialize_config_t``  struct. For example, the ``init_config``  variable used in the sample.

Default SPI Initialization Variable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Use the ``hpm_spi_get_default_init_config``  API to assign default values to the ``init_config``  variable's parameters within the API's ``spi_initialize_config_t`` .

  - Prototype of the ``hpm_spi_get_default_init_config``  API:


    .. code-block:: c

       void hpm_spi_get_default_init_config(spi_initialize_config_t *config);


  - This function sets each member to default values:

    - Master operation mode

    - Single-wire data line mode

    - Clock polarity low idle

    - Clock phase sampling on odd clock edges

    - Data length of 8 bits

    - No data merging

    - Data transfer direction MSB first

SPI Initialization
~~~~~~~~~~~~~~~~~~

- Call the ``hpm_spi_initialize``  API to initialize the SPI. During this process, the parameters of the ``init_config``  variable are assigned and take effect in the API's ``config``  parameter.

- Prototype of the ``hpm_spi_initialize``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_initialize(SPI_Type *ptr, spi_initialize_config_t *config);


- **Example** : Instantiate SPI0, set SPI0 to master mode, quad-wire data line, CPOL high idle, CPHA sampling on odd clock edges, data transfer direction MSB first, data unit length of 16 bits.


    .. code-block:: c

           init_config.mode = spi_master_mode;
           init_config.io_mode = spi_quad_io_mode;
           init_config.clk_polarity = spi_sclk_high_idle;
           init_config.clk_phase = spi_sclk_sampling_odd_clk_edges;
           init_config.data_len = 16;
           init_config.direction = spi_msb_first;
           hpm_spi_initialize(HPM_SPI0, &init_config);


- **Example** : Instantiate SPI1, set SPI1 to slave mode, single-wire data line, CPOL low idle, CPHA sampling on even clock edges, data transfer direction LSB first, data unit length of 8 bits.


    .. code-block:: c

           init_config.mode = spi_slave_mode;
           init_config.io_mode = spi_dual_io_mode;
           init_config.clk_polarity = spi_sclk_low_idle;
           init_config.clk_phase = spi_sclk_sampling_even_clk_edges;
           init_config.data_len = 8;
           init_config.direction = spi_lsb_first;
           hpm_spi_initialize(HPM_SPI1, &init_config);



Set SPI Master SCLK Frequency
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Not required in slave mode.

- Call the ``hpm_spi_set_sclk_frequency``  API to set the SPI's SCLK frequency.

- Prototype of the ``hpm_spi_set_sclk_frequency``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_set_sclk_frequency(SPI_Type *ptr, uint32_t freq)


  - This function sets the specified SPI's SCLK frequency by selecting appropriate clock sources and division factors to ensure the SPI clock frequency is as close as possible to the requested frequency.

  - Parameter Description

        .. list-table::
           :widths: 25 20 55
           :header-rows: 1

           * - Parameter Name
             - Type
             - Description
           * - ptr
             - ``SPI_Type *``
             - Pointer to the SPI module register structure
           * - freq
             - ``uint32_t``
             - Target SPI clock frequency (unit: Hz)
  - Return Values

    - status_success: Successfully set the SPI clock frequency.

    - status_invalid_argument: Invalid argument provided (e.g., configuration object is NULL)

- **Example** : How to use the ``hpm_spi_set_sclk_frequency``  function to set the SPI1 master SCLK frequency to 1 MHz.


    .. code-block:: c

           uint32_t desired_freq = 1000000; /* Desired frequency is 1MHz */
           hpm_stat_t result = hpm_spi_set_sclk_frequency(HPM_SPI1, desired_freq);
           if (result == status_success) {
               printf("SPI SCLK frequency set successfully.\n");
           } else {
               printf("Failed to set SPI SCLK frequency.\n");
           }



DMA Configuration
~~~~~~~~~~~~~~~~~

- If using non-blocking read/write interfaces, this step is necessary.

- Use the ``hpm_spi_dma_mgr_install_callback``  API for DMA initialization and to register callback functions. This function must be called when using non-blocking APIs for data transmission. If no callback function is needed, NULL can be passed as the callback parameter.

- It is used to configure callback functions for DMA transfers in the SPI module. This function allows users to set completion callback functions for both TX and RX DMA channels separately to perform specific operations upon completion of DMA transfers.

- Prototype of the ``hpm_spi_dma_mgr_install_callback``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_dma_mgr_install_callback(SPI_Type *ptr, spi_dma_complete_cb tx_complete, spi_dma_complete_cb rx_complete)


    - Parameter Description

        .. list-table::
           :widths: 25 25 50
           :header-rows: 1

           * - Parameter Name
             - Type
             - Description
           * - ptr
             - ``SPI_Type*``
             - Pointer to the SPI module register structure
           * - tx_complete
             - ``spi_dma_complete_cb``
             - Callback function for TX DMA transfer completion
           * - rx_complete
             - ``spi_dma_complete_cb``
             - Callback function for RX DMA transfer completion

    - Return Values:

      - ``status_success`` : Success

      - ``status_invalid_argument`` : Invalid argument

- **Example** : How to use the ``hpm_spi_dma_mgr_install_callback``  function to register callback functions.


    .. code-block:: c

       /**
       * Example TX DMA transfer completion callback function.
       */
       void tx_dma_complete_callback(void *context) {
           printf("TX DMA transfer completed.\n");
       }

       /**
       * Example RX DMA transfer completion callback function.
       */
       void rx_dma_complete_callback(void *context) {
           printf("RX DMA transfer completed.\n");
       }
       int main(void) {
           /* Initialize SPI... Not listed */

           /* Register DMA transfer completion callbacks */
           hpm_stat_t result = hpm_spi_dma_mgr_install_callback(spi_ptr, tx_dma_complete_callback, rx_dma_complete_callback);

           if (result == status_success) {
               /* Successfully installed DMA transfer completion callback functions */
               printf("DMA callback functions installed successfully.\n");
           } else {
               /* Failed to install DMA transfer completion callback functions */
               printf("Failed to install DMA callback functions.\n");
           }

           /* TODO */
       }



Read/Write Operations Supporting Master and Slave Modes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Blocking Read/Write Operations, Divided into Full-Duplex and Half-Duplex Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **Full-Duplex Read/Write Operation** : Used for blocking simultaneous read/write operations. It waits until data transmission is complete or a timeout occurs, provided by the ``hpm_spi_transmit_receive_blocking``  API.

- ``hpm_spi_transmit_receive_blocking``  API

  - Prototype of the ``hpm_spi_transmit_receive_blocking``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_transmit_receive_blocking(SPI_Type *ptr, uint8_t *wbuff, uint8_t *rbuff, uint32_t size, uint32_t timeout)


    - Parameter Description

        .. list-table::
           :widths: 20 15 65
           :header-rows: 1

           * - Parameter Name
             - Type
             - Description
           * - ptr
             - ``SPI_Type*``
             - Pointer to the SPI module register structure
           * - wbuff
             - ``uint8_t*``
             - Pointer to the buffer containing data to send
           * - rbuff
             - ``uint8_t*``
             - Pointer to the buffer to receive data
           * - size
             - ``uint32_t``
             - Size of data to transmit (in bytes)
           * - timeout
             - ``uint32_t``
             - Timeout in milliseconds. If the operation does not complete within this time, the function will return a timeout error

    - Return Values:

      - ``status_success`` : Successfully completed data transmission

      - ``status_invalid_argument`` : Invalid argument provided

      - ``status_timeout`` : If the operation did not complete within the specified timeout

    - **Example** : An SPI1 master or slave device simultaneously sending and receiving some data over the SPI bus.


    .. code-block:: c

       uint8_t tx_buffer[4] = {0x01, 0x02, 0x03, 0x04};  /* Transmit buffer */
       uint8_t rx_buffer[4];  /* Receive buffer */
       uint32_t transfer_size = 4;  /* Data size to transfer */
       uint32_t timeout_ms = 1000;  /* Timeout of 1 second */

       /* Initialize SPI... Not listed */

       hpm_stat_t result = hpm_spi_transmit_receive_blocking(HPM_SPI1, tx_buffer, rx_buffer, transfer_size, timeout_ms);
       if (result == status_success) {
           /* Successfully completed data transmission */
           printf("Data transfer completed successfully.\n");
           printf("Received data: ");
           for (uint32_t i = 0; i < transfer_size; i++) {
               printf("%02X ", rx_buffer[i]);
           }
           printf("\n");
       } else {
           /* Data transmission failed */
           printf("Failed to complete data transfer.\n");
       }


- **Half-Duplex Read/Write Operation** : Used for blocking half-duplex read/write operations. It waits until data transmission is complete or a timeout occurs, provided by the ``hpm_spi_transmit_blocking``  and ``hpm_spi_receive_blocking``  APIs.

- ``hpm_spi_transmit_blocking``  API

  - Used for sending data through the SPI interface. The function works in blocking mode, meaning it waits until data transmission completes or times out.

  - Prototype of the ``hpm_spi_transmit_blocking``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_transmit_blocking(SPI_Type *ptr, uint8_t *buff, uint32_t size, uint32_t timeout)


    - Parameter Description

        .. list-table::
           :widths: 20 15 65
           :header-rows: 1

           * - Parameter Name
             - Type
             - Description
           * - ptr
             - ``SPI_Type*``
             - Pointer to the SPI module register structure
           * - buff
             - ``uint8_t*``
             - Pointer to the buffer containing data to send
           * - size
             - ``uint32_t``
             - Size of data to transmit (in bytes)
           * - timeout
             - ``uint32_t``
             - Timeout in milliseconds. If the operation does not complete within this time, the function will return a timeout error

    - Return Values:

      - ``status_success`` : Successfully completed data transmission

      - ``status_invalid_argument`` : Invalid argument provided

      - ``status_timeout`` : If the operation did not complete within the specified timeout

    - **Example** : An SPI1 master or slave device sending some data over the SPI bus.


    .. code-block:: c

       uint8_t tx_buffer[4] = {0x01, 0x02, 0x03, 0x04};  /* Transmit buffer */
       uint32_t transfer_size = 4;  /* Data size to transfer */
       uint32_t timeout_ms = 1000;  /* Timeout of 1 second */

       /* Initialize SPI... Not listed */

       hpm_stat_t result = hpm_spi_transmit_blocking(HPM_SPI1, tx_buffer, transfer_size, timeout_ms);
       if (result == status_success) {
           /* Successfully completed data transmission */
           printf("Data transmission completed successfully.\n");
       } else {
           /* Data transmission failed */
           printf("Failed to complete data transmission.\n");
       }


- ``hpm_spi_receive_blocking``  API

  - Used for receiving data through the SPI interface. The function works in blocking mode, meaning it waits until data reception completes or times out.

  - Prototype of the ``hpm_spi_receive_blocking``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_receive_blocking(SPI_Type *ptr, uint8_t *buff, uint32_t size, uint32_t timeout)


    - Parameter Description

        .. list-table::
           :widths: 20 15 65
           :header-rows: 1

           * - Parameter Name
             - Type
             - Description
           * - ptr
             - ``SPI_Type*``
             - Pointer to the SPI module register structure
           * - buff
             - ``uint8_t*``
             - Pointer to the buffer to receive data
           * - size
             - ``uint32_t``
             - Size of data to receive (in bytes)
           * - timeout
             - ``uint32_t``
             - Timeout in milliseconds. If the operation does not complete within this time, the function will return a timeout error

    - Return Values:

      - ``status_success`` : Successfully completed data reception

      - ``status_invalid_argument`` : Invalid argument provided

      - ``status_timeout`` : If the operation did not complete within the specified timeout

    - **Example** : An SPI1 master or slave device receiving some data over the SPI bus.


    .. code-block:: c

       uint8_t rx_buffer[4];  /* Receive buffer */
       uint32_t transfer_size = 4;  /* Data size to transfer */
       uint32_t timeout_ms = 1000;  /* Timeout of 1 second */

       /* Initialize SPI... Not listed */
       hpm_stat_t result = hpm_spi_receive_blocking(HPM_SPI1, rx_buffer, transfer_size, timeout_ms);
       if (result == status_success) {
           /* Successfully completed data reception */
           printf("Data reception completed successfully.\n");
           printf("Received data: ");
           for (uint32_t i = 0; i < transfer_size; i++) {
               printf("%02X ", rx_buffer[i]);
           }
           printf("\n");
       } else {
           /* Data reception failed */
           printf("Failed to complete data reception.\n");
       }



Non-blocking Read/Write Operations, Divided into Full-Duplex and Half-Duplex Operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **Full-Duplex Read/Write Operation** : Used for non-blocking simultaneous read/write operations. It allows other tasks to continue executing without waiting for the operation to complete, provided by the ``hpm_spi_transmit_receive_nonblocking``  API.

- ``hpm_spi_transmit_receive_nonblocking``  API

  - Prototype of the ``hpm_spi_transmit_receive_nonblocking``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_transmit_receive_nonblocking(SPI_Type *ptr, uint8_t *wbuff, uint8_t *rbuff, uint32_t size);


    - Parameter Description

        .. list-table::
           :widths: 20 15 65
           :header-rows: 1

           * - Parameter Name
             - Type
             - Description
           * - ptr
             - ``SPI_Type*``
             - Pointer to the SPI module register structure
           * - wbuff
             - ``uint8_t*``
             - Pointer to the buffer containing data to send
           * - rbuff
             - ``uint8_t*``
             - Pointer to the buffer to receive data
           * - size
             - ``uint32_t``
             - Size of data to transfer (in bytes)

    - Return Values:

      - ``status_success`` : Successfully configured non-blocking data transmission/reception

      - ``status_invalid_argument`` : Invalid argument provided

    - **Example** : An SPI1 master or slave device simultaneously sending and receiving some data over the SPI bus.


    .. code-block:: c

       uint8_t tx_buffer[4] = {0x01, 0x02, 0x03, 0x04};  /* Transmit buffer */
       uint8_t rx_buffer[4];  /* Receive buffer */
       uint32_t transfer_size = 4;  /* Data size to transfer */
       uint32_t timeout_ms = 1000;  /* Timeout of 1 second */

       /* Initialize SPI... Not listed */

       hpm_stat_t result = hpm_spi_transmit_receive_nonblocking(spi_ptr, tx_buffer, rx_buffer, transfer_size);
       if (result == status_success) {
           /* Successfully configured non-blocking transmission/reception */
           printf("Non-blocking data transmission and reception configured successfully.\n");
       } else {
           /* Failed to configure non-blocking transmission/reception */
           printf("Failed to configure non-blocking data transmission and reception.\n");
       }

       /* TODO Since it's a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, wait for this transmission to complete */


- **Half-Duplex Read/Write Operation** : Used for non-blocking half-duplex read/write operations, provided by the ``hpm_spi_transmit_nonblocking``  and ``hpm_spi_receive_nonblocking``  APIs.

- ``hpm_spi_transmit_nonblocking``  API

  - Used for non-blocking data transmission through the SPI interface. The function does not wait for the transmission to complete but returns immediately, allowing background processing of data transmission.

  - Prototype of the ``hpm_spi_transmit_nonblocking``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_transmit_nonblocking(SPI_Type *ptr, uint8_t *buff, uint32_t size)


    - Parameter Description

        .. list-table::
           :widths: 20 15 65
           :header-rows: 1

           * - Parameter Name
             - Type
             - Description
           * - ptr
             - ``SPI_Type*``
             - Pointer to the SPI module register structure
           * - buff
             - ``uint8_t*``
             - Pointer to the buffer containing data to send
           * - size
             - ``uint32_t``
             - Size of data to transmit (in bytes)

    - Return Values:

      - ``status_success`` : Successfully completed data transmission

      - ``status_invalid_argument`` : Invalid argument provided

    - **Example** : An SPI1 master or slave device sending some data over the SPI bus.


    .. code-block:: c

       uint8_t tx_buffer[4] = {0x01, 0x02, 0x03, 0x04};  /* Transmit buffer */
       uint32_t transfer_size = 4;  /* Data size to transfer */
       uint32_t timeout_ms = 1000;  /* Timeout of 1 second */

       /* Initialize SPI... Not listed */

       hpm_stat_t result = hpm_spi_transmit_nonblocking(HPM_SPI1, tx_buffer, transfer_size);
       if (result == status_success) {
           /* Successfully configured non-blocking transmission */
           printf("Non-blocking data transmission configured successfully.\n");
       } else {
           /* Failed to configure non-blocking transmission */
           printf("Failed to configure non-blocking data transmission.\n");
       }
       /* TODO Since it's a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, wait for this transmission to complete */


- ``hpm_spi_receive_nonblocking``  API

  - Used for non-blocking data reception through the SPI interface. The function does not wait for the reception to complete but returns immediately, allowing background processing of data reception.

  - Prototype of the ``hpm_spi_receive_nonblocking``  API:


    .. code-block:: c

       hpm_stat_t hpm_spi_receive_nonblocking(SPI_Type *ptr, uint8_t *buff, uint32_t size)


    - Parameter Description

        .. list-table::
           :widths: 20 15 65
           :header-rows: 1

           * - Parameter Name
             - Type
             - Description
           * - ptr
             - ``SPI_Type*``
             - Pointer to the SPI module register structure
           * - buff
             - ``uint8_t*``
             - Pointer to the buffer to receive data
           * - size
             - ``uint32_t``
             - Size of data to receive (in bytes)

    - Return Values:

      - ``status_success`` : Successfully configured non-blocking reception

      - ``status_invalid_argument`` : Invalid argument provided

    - **Example** : An SPI1 master or slave device receiving some data over the SPI bus.


    .. code-block:: c

       uint8_t rx_buffer[4];  /* Receive buffer */
       uint32_t transfer_size = 4;  /* Data size to transfer */
       uint32_t timeout_ms = 1000;  /* Timeout of 1 second */

       /* Initialize SPI... Not listed */
       hpm_stat_t result = hpm_spi_receive_nonblocking(spi_ptr, rx_buffer, transfer_size);
       if (result == status_success) {
           /* Successfully configured non-blocking reception */
           printf("Non-blocking data reception configured successfully.\n");
       } else {
           /* Failed to configure non-blocking reception */
           printf("Failed to configure non-blocking data reception.\n");
       }
       /* TODO Since it's a non-blocking operation, the function returns immediately, allowing the program to continue executing other tasks. For example, wait for this transmission to complete */


.. note::

    - As the SPI component uses the DMA manager component, configurations such as DMA channels are allocated by the DMA manager. When using DMA, ensure that the allocated DMA channels do not conflict with those used by the SPI component.

    - The transmit DMA channel used by the SPI component can be obtained by calling the ``hpm_spi_get_tx_dma_resource``  API to get the SPI transmit DMA channel resources.

      - Prototype of the ``hpm_spi_get_tx_dma_resource``  API:


        .. code-block:: c

           dma_resource_t *hpm_spi_get_tx_dma_resource(SPI_Type *ptr)


    - The receive DMA channel used by the SPI component can be obtained by calling the ``hpm_spi_get_rx_dma_resource``  API to get the SPI receive DMA channel resources.

      - Prototype of the ``hpm_spi_get_rx_dma_resource``  API:


        .. code-block:: c

           dma_resource_t *hpm_spi_get_rx_dma_resource(SPI_Type *ptr)

    - **Example:** How to Use `hpm_spi_get_tx_dma_resource` and `hpm_spi_get_rx_dma_resource` Functions to Obtain DMA Channel Resources and Utilize Them for Data Transfer

        .. code-block:: c

            /* Initialize SPI... details omitted */
            /* Get transmit DMA channel resource */
            dma_resource_t *tx_dma_resource = hpm_spi_get_tx_dma_resource(HPM_SPI1);
            if (tx_dma_resource != NULL) {
                /* Successfully obtained transmit DMA channel resource */
                printf("TX DMA channel resource obtained successfully.\n");
                /* Print the DMA instance and channel used by the transmit DMA resource */
                printf("TX DMA instance: %d, TX DMA channel: %d\n", tx_dma_resource->dma_instance, tx_dma_resource->dma_channel);
                /* Change the TX DMA resource's interrupt priority to 1 */
                dma_mgr_enable_dma_irq_with_priority(tx_dma_resource, 1);
            }
            /* Get receive DMA channel resource */
            dma_resource_t *rx_dma_resource = hpm_spi_get_rx_dma_resource(HPM_SPI1);
            if (rx_dma_resource != NULL) {
                /* Successfully obtained receive DMA channel resource */
                printf("RX DMA channel resource obtained successfully.\n");
                /* Get the DMA instance and channel used by the receive DMA resource */
                printf("RX DMA instance: %d, TX DMA channel: %d\n", rx_dma_resource->dma_instance, rx_dma_resource->dma_channel);
                /* Change the RX DMA resource's interrupt priority to 1 */
                dma_mgr_enable_dma_irq_with_priority(rx_dma_resource, 1);
            }

