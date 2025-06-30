.. _serial_nor:

Serial Nor
==========================

Overview
--------

- The Serial Nor component is a universal driver for serial SPI Flash devices, seamlessly supporting various JEDEC-compliant serial SPI Flash chips like W25Q128, W25Q64, etc., enhancing portability and extensibility. Key features include:

  - Multi-instance support

    - Supports multiple flash instances with independent configuration parameters
    - Supports 24-bit and 32-bit addressing
    - Compatible with SPI/DSPI/QSPI interfaces
    - Supports read/write/erase operations
    - Each instance can be independently configured with SPI interface type, address width, data width, clock frequency, etc.
    - Enables concurrent operation of multiple flash instances

  - Blocking and non-blocking I/O interfaces

    - Provides non-blocking read/write functions
    - Provides blocking read/write functions with timeout mechanisms to ensure error reporting during prolonged unresponsiveness

Macro Definitions, Enums and Structs
------------------------------------

- Macro Definitions

  - Configuration macros for serial Nor Flash features (defined in ``hpm_serial_nor_host.h``):

  .. code-block:: c

        /**
        * @brief Enable single-wire SPI mode (MOSI+MISO)
        * @note Bit0: Controls single-wire transmission mode
        */
        #define SERIAL_NOR_HOST_SUPPORT_SINGLE_IO_MODE             (1UL << 0)

        /**
        * @brief Enable dual-wire SPI mode (using IO0 and IO1 as unidirectional data lines)
        * @note Bit1: Transmits 2 bits per clock cycle when enabled
        */
        #define SERIAL_NOR_HOST_SUPPORT_DUAL_IO_MODE               (1UL << 1)

        /**
        * @brief Enable quad-wire SPI mode (using IO0-IO3 as unidirectional data lines)
        * @note Bit2: Transmits 4 bits per clock cycle when enabled
        */
        #define SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE               (1UL << 2)

        /**
        * @brief Enable standard SPI interface protocol
        * @note Bit3: Indicates device uses basic SPI protocol
        */
        #define SERIAL_NOR_HOST_SUPPORT_SPI_INTERFACE              (1UL << 3)

        /**
        * @brief Enable DMA data transfer
        * @note Bit8: Enables DMA controller for large data transfers
        */
        #define SERIAL_NOR_HOST_SUPPORT_DMA                        (1UL << 8)

        /**
        * @brief Enable automatic chip select control
        * @note Bit9: Hardware automatically controls CS pin state when enabled
        */
        #define SERIAL_NOR_HOST_CS_CONTROL_AUTO                    (1UL << 9)

- Enumerations

  - I/O operation modes (defined in ``hpm_serial_nor_host.h``):

    .. code-block:: c

        typedef enum {
            single_io_mode = 0, /**< Single-wire SPI mode */
            dual_io_mode,       /**< Dual-wire mode using MOSI as D0, MISO as D1 */
            quad_io_mode        /**< Quad-wire QSPI mode */
        } hpm_serial_nor_seq_io_mode_t;

  - Address length modes (defined in ``hpm_serial_nor_host.h``):

    .. code-block:: c

        typedef enum {
            flash_addrlen_24bit = 0,  /**< 24-bit address mode (3-byte) */
            flash_addrlen_32bit       /**< 32-bit address mode (4-byte) */
        } hpm_serial_nor_seq_addr_bit_t;

  - Data transfer directions (defined in ``hpm_serial_nor_host.h``):

    .. code-block:: c

        typedef enum {
            write_direction = 0,  /**< Write operation for programming/erasing */
            read_direction        /**< Read operation */
        } hpm_serial_nor_seq_direction_t;

- Structure Definitions

  - hpm_nor_host_dma_control_t: Host DMA control parameters structure. Implementation varies based on SERIAL_NOR_USE_DMA_MGR:

    - When SERIAL_NOR_USE_DMA_MGR=0: Manual DMA channel and base address configuration
    - When SERIAL_NOR_USE_DMA_MGR=1: DMA Manager auto-allocation

    .. code-block:: c

        typedef struct {
            uint8_t rx_dma_req;  /* RX DMA request line number (e.g. HPM_DMA_SRC_SPI2_RX) */
            uint8_t tx_dma_req;  /* TX DMA request line number (e.g. HPM_DMA_SRC_SPI2_TX) */

        #if (SERIAL_NOR_USE_DMA_MGR == 0)
            uint8_t rx_dma_ch;   /* Manually assigned RX DMA channel */
            uint8_t tx_dma_ch;   /* Manually assigned TX DMA channel */
            void *dma_base;      /* DMA controller base (e.g. HPM_DMA) */
            void *dmamux_base;   /* DMAMUX controller base (e.g. HPM_DMAMUX) */
        #else
            dma_resource_t txdma_resource; /* TX DMA resource from DMA Manager */
            dma_resource_t rxdma_resource; /* RX DMA resource from DMA Manager */
        #endif
        } hpm_nor_host_dma_control_t;

  - hpm_nor_host_param_t: Host controller hardware configuration parameters

    .. code-block:: c

        typedef struct {
            uint8_t pin_or_cs_index;      /* Physical pin number or CS index */
            hpm_nor_host_dma_control_t dma_control; /* DMA config (requires SERIAL_NOR_HOST_SUPPORT_DMA) */
            uint32_t clock_name;          /* Clock source name (e.g. clock_spi0) */
            uint32_t frequency;           /* Communication frequency (Hz) */
            uint32_t transfer_max_size;   /* Max single transfer size (bytes) */
            void *host_base;              /* Host controller base address */

            /**
            * @brief CS signal control callback
            * @param cs_pin CS pin number
            * @param state Pin state (0: low, 1: high)
            */
            void (*set_cs)(uint32_t cs_pin, uint8_t state);

            /**
            * @brief Frequency setup callback
            * @param host Host controller instance
            * @param freq Target frequency value
            */
            void (*set_frequency)(void *host, uint32_t freq);
        } hpm_nor_host_param_t;

- hpm_sfdp_read_para_t: Read operation timing parameters (supports different read modes)

    .. code-block:: c

        typedef struct {
            uint8_t read_cmd;            /* Base read command (e.g. 0x03: Normal, 0x6B: Quad Fast) */
            uint8_t data_dummy_count;    /* Dummy cycles before data phase (set per flash spec) */
            hpm_serial_nor_seq_addr_bit_t addr_bit;         /* Address length mode */
            hpm_serial_nor_seq_io_mode_t data_phase_format;  /* Data phase I/O mode */
            hpm_serial_nor_seq_io_mode_t addr_phase_format;  /* Address phase I/O mode */
        } hpm_sfdp_read_para_t;

  - hpm_sfdp_program_para_t: Page programming parameters

    .. code-block:: c

        typedef struct {
            bool has_4b_addressing_inst_table; /* Supports 4-byte addressing instructions (>128Mb) */
            bool support_1_4_4_page_program;  /* Supports 1-4-4 mode (1cmd +4addr +4data) */
            bool support_1_1_4_page_program;  /* Supports 1-1-4 mode (1cmd +1addr +4data) */
            uint8_t page_program_cmd;         /* Page program command (e.g. 0x02: Standard) */
        } hpm_sfdp_program_para_t;

  - hpm_serial_nor_host_param_t: Host parameter container

    .. code-block:: c

        typedef struct {
            uint32_t flags;              /* Feature flags (SERIAL_NOR_HOST_SUPPORT_* macros) */
            hpm_nor_host_param_t param;  /* Hardware params (pins/DMA/clocks) */
            void *user_data;             /* User context pointer for callbacks */
        } hpm_serial_nor_host_param_t;

  - hpm_serial_nor_transfer_seq_t: NOR flash operation sequence descriptor

    .. code-block:: c

        typedef struct {
            uint8_t use_dma;  /* DMA selector (0:disable 1:enable) */

            /* Command phase */
            struct {
                uint8_t cmd;  /* Operation command (e.g. 0x03: Read) */
            } cmd_phase;

            /* Address phase */
            struct {
                bool enable;  /* Enable address phase */
                hpm_serial_nor_seq_addr_bit_t addr_bit;     /* Address length */
                hpm_serial_nor_seq_io_mode_t addr_io_mode;  /* Address I/O mode */
                uint32_t addr;               /* Target address */
            } addr_phase;

            /* Dummy phase */
            struct {
                uint8_t dummy_count;  /* Dummy cycles (per flash spec) */
            } dummy_phase;

            /* Data phase */
            struct {
                hpm_serial_nor_seq_direction_t direction;  /* Data direction */
                hpm_serial_nor_seq_io_mode_t data_io_mode;  /* Data I/O mode */
                uint32_t len;    /* Data length (bytes) */
                uint8_t *buf;    /* Data buffer pointer */
            } data_phase;
        } hpm_serial_nor_transfer_seq_t;

  - serial_nor_host_ops_t: Host controller operations

    .. code-block:: c

        typedef struct {
            hpm_stat_t (*init)(void *host);
            hpm_stat_t (*transfer)(void *host, hpm_serial_nor_transfer_seq_t *seq);
            void (*set_cs)(uint32_t cs_pin, uint8_t state);
            void (*set_frequency)(void *host, uint32_t freq);
            void *user_data;
        } serial_nor_host_ops_t;

  - hpm_serial_nor_host_t: Complete host instance

    .. code-block:: c

        typedef struct {
            hpm_serial_nor_host_param_t host_param;  /* Host configuration */
            serial_nor_host_ops_t host_ops;          /* Hardware operations */
            void *user_data;                        /* Extension pointer */
        } hpm_serial_nor_host_t;

  - hpm_serial_nor_t: Full device descriptor

    .. code-block:: c

        typedef struct {
            hpm_serial_nor_host_t host;          /* Host driver instance */
            hpm_sfdp_read_para_t nor_read_para;  /* Read timing config */
            hpm_sfdp_program_para_t nor_program_para; /* Program config */
            hpm_serial_nor_info_t flash_info;    /* Physical characteristics */
        } hpm_serial_nor_t;

API Call Flow Introduction
-------------------------------

Define Serial NOR Initialization Variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Define a ``hpm_serial_nor_t`` struct variable. Example:

.. code-block:: c

    hpm_serial_nor_t nor_flash_dev;

Initialize Serial NOR Host Parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Main initialization for flash device's host member:

    .. code-block:: c

        #define APP_SPI_DATA_LEN_IN_BITS   (8U)
        #define PORT_SPI_IO_MODE           SERIAL_NOR_HOST_SUPPORT_DUAL_IO_MODE
        #define PORT_SPI_BASE              HPM_SPI2
        #define PORT_SPI_CLK_NAME          clock_spi2
        #define PORT_SPI_NOR_DMA           HPM_HDMA
        #define PORT_SPI_NOR_DMAMUX        HPM_DMAMUX
        #define PORT_SPI_RX_DMA_REQ        HPM_DMA_SRC_SPI2_RX
        #define PORT_SPI_TX_DMA_REQ        HPM_DMA_SRC_SPI2_TX
        #define PORT_SPI_RX_DMA_CH         0
        #define PORT_SPI_TX_DMA_CH         1
        #define PORT_SPI_CLK_FREQUENCY     (40000000u)

        ATTR_WEAK hpm_stat_t serial_nor_get_board_host(hpm_serial_nor_host_t *host)
        {
            /* Host capability flags */
            host->host_param.flags =  PORT_SPI_IO_MODE |            /* Dual I/O mode */
                                    SERIAL_NOR_HOST_SUPPORT_DMA |  /* Enable DMA */
                                    SERIAL_NOR_HOST_SUPPORT_SPI_INTERFACE; /* SPI protocol */

            /* Hardware parameters */
            host->host_param.param.set_cs = board_write_spi_cs;     /* CS control callback */
            host->host_param.param.set_frequency = set_spi_clk_frequency; /* Clock config */
            host->host_param.param.clock_name = PORT_SPI_CLK_NAME;  /* Clock source */
            host->host_param.param.pin_or_cs_index = BOARD_SPI_CS_PIN; /* CS pin/index */
            host->host_param.param.host_base = PORT_SPI_BASE;       /* SPI base address */

        #if (SERIAL_NOR_USE_DMA_MGR == 0)
            /* Manual DMA resource configuration */
            host->host_param.param.dma_control.dma_base = PORT_SPI_NOR_DMA; /* DMA ctrl base */
            host->host_param.param.dma_control.dmamux_base = PORT_SPI_NOR_DMAMUX; /* DMAMUX */
            host->host_param.param.dma_control.rx_dma_ch = PORT_SPI_RX_DMA_CH; /* RX channel */
            host->host_param.param.dma_control.tx_dma_ch = PORT_SPI_TX_DMA_CH; /* TX channel */
        #endif

            /* Common DMA request lines */
            host->host_param.param.dma_control.rx_dma_req = PORT_SPI_RX_DMA_REQ; /* RX request */
            host->host_param.param.dma_control.tx_dma_req = PORT_SPI_TX_DMA_REQ; /* TX request */

            host->host_param.param.frequency = PORT_SPI_CLK_FREQUENCY; /* 40MHz clock */
            host->host_param.param.transfer_max_size = SPI_SOC_TRANSFER_COUNT_MAX; /* Max transfer */
            host->host_param.param.user_data = NULL; /* User context pointer */

            return status_success;
        }

        serial_nor_get_board_host(&nor_flash_dev.host);

- When the DMA Manager component is enabled, there is no need to manually configure the DMA channel; only the DMA request line needs to be configured.
- The SPI clock source must be enabled, and the SPI-related pins must be initialized.

serial_nor Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~

- Call ``hpm_serial_nor_init`` API to initialize serial_nor device. During this process, parameters from ``nor_flash_dev`` will be applied to the ``flash`` parameter, and return NOR flash device information via ``hpm_serial_nor_info_t``.

    - ``hpm_serial_nor_init`` API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_init(hpm_serial_nor_t *flash, hpm_serial_nor_info_t *info);

    - Parameter description

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Pointer to NOR Flash device instance
            * - info
              - ``hpm_serial_nor_info_t *``
              - [out] Output parameter for flash physical characteristics

    - Return value

      - ``status_success``: Initialization successful
      - ``status_invalid_argument``: Invalid parameter
      - ``status_spi_nor_flash_not_identified``: Flash chip not recognized

    - Core functionality:

        - Hardware interface initialization:
            - Bind SPI host operation functions
            - Initialize SPI controller hardware
            - Configure initial communication frequency (10MHz for SFDP reading)

        - Flash parameter identification:
            - Read SFDP parameter table (JEDEC standard)
            - Parse physical parameters: page/sector/block sizes
            - Get capacity info (24-bit/32-bit addressing)

        - Operation mode configuration:
            - Auto-attempt quad mode enablement (QE bit setting)
            - Configure optimal read/write timing
            - Setup DMA transfer mode (if enabled)

        - Information synchronization:
            - Output parsed flash parameters via info
            - Return initialization status (success/failure reason)

- **Example**: Initialize serial_nor and retrieve flash info

    .. code-block:: c

            hpm_stat_t stat;
            hpm_serial_nor_info_t flash_info;
            stat = hpm_serial_nor_init(&nor_flash_dev, &flash_info);
            if (stat != status_success) {
                printf("SPI NOR Flash initialization failed\n");
            } else {
                printf("SPI NOR Flash initialized successfully\n");
                if (hpm_serial_nor_get_info(&nor_flash_dev, &flash_info) == status_success) {
                    printf("Flash SFDP version: %d\n", flash_info.sfdp_version);
                    printf("Flash size: %d KB\n", flash_info.size_in_kbytes);
                    printf("Page size: %d Bytes\n", flash_info.page_size);
                    printf("Sector size: %d KB\n", flash_info.sector_size_kbytes);
                    printf("Block size: %d KB\n", flash_info.block_size_kbytes);
                    printf("Sector erase command: 0x%02x\n", flash_info.sector_erase_cmd);
                    printf("Block erase command: 0x%02x\n", flash_info.block_erase_cmd);
                }
            }

serial_nor Check NOR Flash Busy Status
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- During NOR Flash operations (e.g. non-blocking erase/program), need to check device busy status.

- Use ``hpm_serial_nor_is_busy`` API to check if device is busy:

    - API prototype:

      .. code-block:: c

            hpm_stat_t hpm_serial_nor_is_busy(hpm_serial_nor_t *flash);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance pointer

    - Return values:

      - ``status_spi_nor_flash_is_busy``: Device busy (erase/program in progress)
      - ``status_success``: Device ready
      - ``status_invalid_argument``: Invalid parameter (NULL pointer)

- **Example**:

    .. code-block:: c

            /* Wait erase operation complete */
            while(hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
                hpm_spi_nor_udelay(100); /* Delay 100us */
            }
            printf("Flash operation completed\n");

- **Important Notes**

    - This API doesn't implement retry mechanism internally, caller must implement polling logic

serial_nor Erase Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Includes both blocking and non-blocking interfaces. Non-blocking erase supports sector/block operations,
  while blocking erase supports sector/block/chip erase and address-range based erase.

Non-Blocking Erase Interfaces
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sector Erase
""""""""""""
- Use ``hpm_serial_nor_erase_sector_nonblocking`` API for non-blocking sector erase:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_sector_nonblocking(hpm_serial_nor_t *flash, uint32_t sector_addr);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance
            * - sector_addr
              - ``uint32_t``
              - [in] Target sector address (must be sector-aligned)

    - Return values:

        - ``status_success``: Erase command sent successfully
        - ``status_invalid_argument``: Invalid parameters (NULL pointer/address misaligned)
        - ``status_spi_nor_flash_is_busy``: Flash device busy

- **Example**:

    .. code-block:: c

           /* Erase sector starting at 0x8000 */
            hpm_stat_t status = hpm_serial_nor_erase_sector_nonblocking(&nor_flash_dev, 0x8000);

            if (status == status_success) {
                /* Perform other tasks here */
                while(hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
                    /* Wait for erase completion */
                }
                printf("Sector erase completed\n");
            } else {
                printf("Erase failed: 0x%08X\n", status);
            }

- **Important Notes**

    - Must ensure sector_addr is integer multiple of sector size (flash_info.sector_size_kbytes)
    - Should check flash readiness via hpm_serial_nor_is_busy() before calling

Block Erase
"""""""""""

- Use ``hpm_serial_nor_erase_block_nonblocking`` API for non-blocking block erase:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_block_nonblocking(hpm_serial_nor_t *flash, uint32_t block_addr);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance pointer
            * - block_addr
              - ``uint32_t``
              - [in] Target block address (must be block-aligned, get block size via flash_info.block_size_kbytes)

    - Return values:
        - ``status_success``: Erase command sent successfully
        - ``status_invalid_argument``: Invalid parameters (NULL pointer/address misaligned)
        - ``status_spi_nor_flash_is_busy``: Flash device busy

    - **Example**:

        .. code-block:: c

            /* Erase block starting at 0x10000 */
            hpm_stat_t status = hpm_serial_nor_erase_block_nonblocking(&nor_flash_dev, 0x10000);
            if (status == status_success) {
                /* Perform other tasks here */
                while(hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
                    /* Wait for erase completion */
                }
                printf("Block erase completed\n");
            } else {
                printf("Erase failed: 0x%08X\n", status);
            }

- **Important Notes**

    - Must ensure block_addr is integer multiple of block size (flash_info.block_size_kbytes)
    - Should check flash readiness via hpm_serial_nor_is_busy() before calling

Blocking Erase Interfaces
^^^^^^^^^^^^^^^^^^^^^^^^^

Chip Erase
""""""""""
- Use ``hpm_serial_nor_erase_chip`` API for full chip erase:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_chip(hpm_serial_nor_t *flash);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance

    - Return values:

        - ``status_success``: Erase operation completed
        - ``status_invalid_argument``: Invalid parameters (NULL pointer)
        - ``status_spi_nor_flash_is_busy``: Device busy state timeout

- **Example**:

    .. code-block:: c

        /* Perform full chip erase */
        hpm_stat_t status = hpm_serial_nor_erase_chip(&nor_flash_dev);
        if (status == status_success) {
            printf("Full chip erase succeeded\n");
        } else {
            printf("Erase failed: 0x%08X\n", status);
        }

- **Important Notes**
    - This operation will erase ALL data on the chip - use with extreme caution!
    - Typical erase time reference: MX25L25635F takes ~150 seconds

Sector Erase
""""""""""""
- Use ``hpm_serial_nor_erase_sector_blocking`` API for sector erase:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_sector_blocking(hpm_serial_nor_t *flash, uint32_t sector_addr);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance
            * - sector_addr
              - ``uint32_t``
              - [in] Target sector address (must be sector-aligned, get sector size via flash_info.sector_size_kbytes)

    - Return values:

        - ``status_success``: Sector erase completed
        - ``status_invalid_argument``: Invalid parameters (NULL pointer/address misaligned)
        - ``status_spi_nor_flash_is_busy``: Flash device busy state timeout

    - **Example**:

        .. code-block:: c

            /* Erase sector starting at 0x8000 */
            hpm_stat_t status = hpm_serial_nor_erase_sector_blocking(&nor_flash_dev, 0x8000);
            if (status == status_success) {
                printf("Sector erase succeeded\n");
            }

    - **Important Notes**

        - Must ensure sector_addr is integer multiple of sector size (flash_info.sector_size_kbytes)

Block Erase
"""""""""""
- Use ``hpm_serial_nor_erase_block_blocking`` API for blocking block erase:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_block_blocking(hpm_serial_nor_t *flash, uint32_t block_addr);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance pointer
            * - block_addr
              - ``uint32_t``
              - [in] Target block address (must be block-aligned)

    - Return values:

        - ``status_success``: Block erase completed
        - ``status_invalid_argument``: Invalid parameters (NULL pointer/address misaligned)
        - ``status_spi_nor_flash_is_busy``: Flash device busy timeout

    - **Example**:

        .. code-block:: c

            /* Erase block starting at 0x10000 */
            hpm_stat_t status = hpm_serial_nor_erase_block_blocking(&nor_flash_dev, 0x10000);
            if (status == status_success) {
                printf("Block erase succeeded\n");
            }

- **Important Notes**

    - Ensure block_addr is integer multiple of block size (flash_info.block_size_kbytes)
    - This operation will erase ALL data in specified block - use with caution!

Address-Range Erase
"""""""""""""""""""
- Use ``hpm_serial_nor_erase_blocking`` API for address-range erase:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_blocking(hpm_serial_nor_t *flash, uint32_t start, uint32_t length);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance
            * - start
              - ``uint32_t``
              - [in] Start address
            * - length
              - ``uint32_t``
              - [in] Erase length (bytes)

    - Return values:

        - ``status_success``: Erase operation completed
        - ``status_invalid_argument``: Invalid parameters
        - ``status_spi_nor_flash_is_busy``: Flash busy timeout

    - **Example**:

        .. code-block:: c

            /* Erase 1024 bytes starting at 0x20000 */
            hpm_stat_t status = hpm_serial_nor_erase_blocking(&nor_flash_dev, 0x20000, 1024);
            if (status == status_success) {
                printf("Address-range erase succeeded\n");
            } else {
                printf("Erase failed: 0x%08X\n", status);
            }

- **Important Notes**

    - Auto-adapt erase granularity: Prefer block erase (64KB) → sector erase (4KB)
    - Actual erased range will expand to sector/block boundaries

serial_nor Program Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Includes both blocking and non-blocking interfaces. Non-blocking programming supports page-level operations,
  while blocking programming supports page/address-range based writes.

Non-Blocking Program Interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Use ``hpm_serial_nor_page_program_nonblocking`` API for non-blocking page programming:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_page_program_nonblocking(hpm_serial_nor_t *flash, uint8_t *buf, uint32_t data_len, uint32_t address);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance
            * - buf
              - ``uint8_t *``
              - [in] Data buffer to be programmed
            * - data_len
              - ``uint32_t``
              - [in] Data length in bytes
            * - address
              - ``uint32_t``
              - [in] Target programming address

    - Return values:

        - ``status_success``: Program command sent successfully
        - ``status_invalid_argument``: Invalid parameters (NULL pointer/address misaligned/length exceeds limit)
        - ``status_spi_nor_flash_is_busy``: Device busy

    - **Example**:

        .. code-block:: c

            uint8_t write_buf[256];
            /* Fill data... */

            /* Non-blocking program at 0x1000 */
            hpm_stat_t status = hpm_serial_nor_page_program_nonblocking(&nor_flash_dev, write_buf, 256, 0x1000);
            if (status == status_success) {
                while(hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
                    /* Perform other tasks */
                }
                printf("Page programming completed\n");
            }

- **Important Notes**

    - Must ensure address is integer multiple of page size (flash_info.page_size)
    - Should check device readiness via hpm_serial_nor_is_busy() before calling
    - Data length must not exceed page size
    - Requires sector to be pre-erased before programming

Blocking Program Interface
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Call ``hpm_serial_nor_program_blocking`` API to program data of arbitrary length to specified address:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_program_blocking(hpm_serial_nor_t *flash, uint8_t *buf, uint32_t data_len, uint32_t address);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance
            * - buf
              - ``uint8_t *``
              - [in] Data buffer to be programmed
            * - data_len
              - ``uint32_t``
              - [in] Data length in bytes
            * - address
              - ``uint32_t``
              - [in] Target programming address

    - Return values:

        - ``status_success``: Data programming completed
        - ``status_invalid_argument``: Invalid parameters (NULL pointer/address out of range)
        - ``status_spi_nor_flash_is_busy``: Flash device busy timeout

    - **Example**:

        .. code-block:: c

            uint8_t write_buf[256] = {0x01, 0x23, 0x45, 0x67};
            hpm_stat_t status = hpm_serial_nor_program_blocking(&nor_flash_dev, write_buf, sizeof(write_buf), 0x8000);
            if (status == status_success) {
                printf("Data programmed successfully\n");
            }

serial_nor Read Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Use ``hpm_serial_nor_read`` API for data reading:

    - API prototype:

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_read(hpm_serial_nor_t *flash, uint8_t *buf, uint16_t data_len, uint32_t address);

    - Parameters:

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - Parameter
              - Type
              - Description
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] Initialized NOR Flash device instance
            * - buf
              - ``uint8_t *``
              - [in] Data buffer for read operation
            * - data_len
              - ``uint16_t``
              - [in] Data length in bytes
            * - address
              - ``uint32_t``
              - [in] Start address for reading

    - Return values:

        - ``status_success``: Data read successfully
        - ``status_invalid_argument``: Invalid parameters (NULL pointer/address out of range)
        - ``status_spi_nor_flash_is_busy``: Flash device busy timeout

    - **Example**:

        .. code-block:: c

            uint8_t read_buf[256];
            hpm_stat_t status = hpm_serial_nor_read(&nor_flash_dev, read_buf, sizeof(read_buf), 0x8000);
            if (status == status_success) {
                printf("Data read successfully\n");
            }

- **Important Notes**

    - Actual read speed depends on SPI clock configuration
    - Recommend 4-byte aligned data_len for optimal performance
    - Supports cross-page continuous reading (limited only by buffer size)
    - No pre-erase required before reading
    - Automatically handles address alignment and boundary crossing

.. note::

    - When DMA manager is enabled, the DMA channel allocation is managed by DMA manager.
      Avoid channel conflicts between serial_nor and other SPI components.

    - To get TX DMA resource used by serial_nor component, call ``hpm_serial_nor_get_tx_dma_mgr_resource`` API:

        - API prototype:

        .. code-block:: c

            dma_resource_t *hpm_serial_nor_get_tx_dma_mgr_resource(hpm_serial_nor_t *flash);

    - To get RX DMA resource used by serial_nor component, call ``hpm_serial_nor_get_rx_dma_mgr_resource`` API:

        - API prototype:

        .. code-block:: c

            dma_resource_t *hpm_serial_nor_get_rx_dma_mgr_resource(hpm_serial_nor_t *flash);

    - **Example**: How to use DMA resource APIs and configure DMA channels

        .. code-block:: c

            /* SPI initialization omitted... */

            /* Get TX DMA resource */
            dma_resource_t *tx_dma_resource = hpm_serial_nor_get_tx_dma_mgr_resource(&nor_flash_dev);
            if (tx_dma_resource != NULL) {
                printf("TX DMA channel resource obtained successfully.\n");
                printf("TX DMA instance: %d, TX DMA channel: %d\n",
                       tx_dma_resource->dma_instance,
                       tx_dma_resource->dma_channel);
                /* Set TX DMA interrupt priority to 1 */
                dma_mgr_enable_dma_irq_with_priority(tx_dma_resource, 1);
            }

            /* Get RX DMA resource */
            dma_resource_t *rx_dma_resource = hpm_serial_nor_get_rx_dma_mgr_resource(&nor_flash_dev);
            if (rx_dma_resource != NULL) {
                printf("RX DMA channel resource obtained successfully.\n");
                printf("RX DMA instance: %d, RX DMA channel: %d\n",
                       rx_dma_resource->dma_instance,
                       rx_dma_resource->dma_channel);
                /* Set RX DMA interrupt priority to 1 */
                dma_mgr_enable_dma_irq_with_priority(rx_dma_resource, 1);
            }
