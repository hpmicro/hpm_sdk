.. _serial_nor:

Serial Nor
==========================

概述
------

- Serial Nor组件是串行SPI Flash的通用驱动组件，可以无缝支持各种遵守JEDC标准的串行SPI Flash，如W25Q128、W25Q64等，提高了可移植性和扩展性。以下是该组件的主要特征：

  - 多实例支持

    - 支持多个flash实例对象，每个实例都有独立的配置参数。
    - 支持24bit和32bit地址。
    - 支持SPI/DSPI/QSPI接口。可通过不同CS使用多个flash实例。
    - 支持读写操作，包括读、写、擦除等。
    - 每个flash实例都可以独立配置，包括SPI接口类型、地址宽度、数据宽度、时钟频率等。
    - 支持多个flash实例的同时操作，每个实例可以独立配置和操作。

  - 阻塞与非阻塞读写操作接口

    - 提供了非阻塞式的读写函数。

    - 提供了阻塞式的读写函数，支持超时机制，确保在长时间未响应的情况下能够及时返回错误状态。

相关宏定义以及枚举和结构体介绍
------------------------------

- 宏定义

  - 定义了用于配置串行Nor Flash的功能支持的宏，例如：SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE | SERIAL_NOR_HOST_SUPPORT_DMA 表示同时支持四线模式和DMA传输。该宏在 ``hpm_serial_nor_host.h`` 中定义。

  .. code-block:: c

        /**
        * @brief 支持单线SPI模式（MOSI和MISO两条线）
        * @note 对应bit0，用于配置是否支持单线传输模式
        */
        #define SERIAL_NOR_HOST_SUPPORT_SINGLE_IO_MODE             (1UL << 0)

        /**
        * @brief 支持双线SPI模式（使用IO0和IO1两条单向数据线）
        * @note 对应bit1，启用后同一时钟周期可传输2位数据
        */
        #define SERIAL_NOR_HOST_SUPPORT_DUAL_IO_MODE               (1UL << 1)

        /**
        * @brief 支持四线SPI模式（使用IO0-IO3四条单向数据线）
        * @note 对应bit2，启用后同一时钟周期可传输4位数据
        */
        #define SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE               (1UL << 2)

        /**
        * @brief 支持标准SPI接口协议
        * @note 对应bit3，用于标识设备使用基础SPI协议通信
        */
        #define SERIAL_NOR_HOST_SUPPORT_SPI_INTERFACE              (1UL << 3)

        /**
        * @brief 支持DMA数据传输
        * @note 对应bit8，启用后可使用DMA控制器进行大数据量传输
        */
        #define SERIAL_NOR_HOST_SUPPORT_DMA                        (1UL << 8)

        /**
        * @brief 自动片选控制使能
        * @note 对应bit9，启用后硬件自动控制CS引脚电平状态
        */
        #define SERIAL_NOR_HOST_CS_CONTROL_AUTO                    (1UL << 9)


- 枚举介绍

  - serial_nor的IO操作模式，这个枚举用于表示串行NOR闪存（Serial NOR Flash）的三种I/O操作模式，分别是单I/O模式、双I/O模式和四I/O模式。该枚举在 ``hpm_serial_nor_host.h`` 中定义。

    .. code-block:: c

        typedef enum {
            single_io_mode = 0, /* 单线模式SPI */
            dual_io_mode,       /* 双线模式DSPI MOSI做D0, MISO做D1 */
            quad_io_mode        /* 四线模式QSPI */
        } hpm_serial_nor_seq_io_mode_t;


  - serial_nor的地址长度模式。用于配置闪存操作的地址字节长度，24位地址模式最大寻址16MB空间， 32位地址模式支持更大容量存储芯片。该枚举在 ``hpm_serial_nor_host.h`` 中定义。

    - 使用3字节(24bit)地址，最大支持寻址16MB存储空间。适用于W25Q16(2MB)、W25Q32(4MB)等中小容量闪存
    - 使用4字节(32bit)地址，可支持超过16MB的大容量闪存。适用于W25Q256(32MB)、W25Q512(64MB)等新型大容量存储芯片。

    .. code-block:: c

        typedef enum {
            flash_addrlen_24bit = 0,  /* 24位地址模式 (3字节地址) */
            flash_addrlen_32bit       /* 32位地址模式 (4字节地址) */
        } hpm_serial_nor_seq_addr_bit_t;

  -  serial_nor的数据传输方向，用于标识闪存操作序列中数据阶段的传输方向。该枚举在 ``hpm_serial_nor_host.h`` 中定义。

    .. code-block:: c

        typedef enum {
            write_direction = 0,  /* 写方向，对应闪存的编程/擦除操作 */
            read_direction        /* 读方向，对应闪存的读取操作 */
        } hpm_serial_nor_seq_direction_t;

- 结构体介绍

  - serial_nor主机DMA控制参数结构体。根据SERIAL_NOR_USE_DMA_MGR配置不同，结构体包含两种实现方式：

    - 当SERIAL_NOR_USE_DMA_MGR=0时，手动配置DMA通道和基地址
    - 当SERIAL_NOR_USE_DMA_MGR=1时，使用DMA管理器自动分配资源

    .. code-block:: c

        typedef struct {
            uint8_t rx_dma_req;  /* RX DMA请求线号 (如HPM_DMA_SRC_SPI2_RX) */
            uint8_t tx_dma_req;  /* TX DMA请求线号 (如HPM_DMA_SRC_SPI2_TX) */

        #if (SERIAL_NOR_USE_DMA_MGR == 0)
            uint8_t rx_dma_ch;   /* 手动分配的RX DMA通道号 */
            uint8_t tx_dma_ch;   /* 手动分配的TX DMA通道号 */
            void *dma_base;      /* DMA控制器基地址 (如HPM_DMA) */
            void *dmamux_base;   /* DMAMUX控制器基地址 (如HPM_DMAMUX) */
        #else
            dma_resource_t txdma_resource; /* 通过DMA管理器获取的发送通道资源 */
            dma_resource_t rxdma_resource; /* 通过DMA管理器获取的接收通道资源 */
        #endif
        } hpm_nor_host_dma_control_t;

  - serial_nor主机主机控制器参数结构体。包含主机控制器硬件相关的所有配置参数,每个NOR Flash实例需要独立配置该结构体.

    .. code-block:: c

        typedef struct {
            uint8_t pin_or_cs_index;      /* 物理引脚号或CS索引 (当使用硬件CS控制时为CS号) */
            hpm_nor_host_dma_control_t dma_control; /* DMA控制参数，需与SERIAL_NOR_HOST_SUPPORT_DMA配合使用 */
            uint32_t clock_name;          /* 时钟源名称 (如clock_spi0) */
            uint32_t frequency;           /* 通信频率 (单位：Hz，最大值由具体硬件决定) */
            uint32_t transfer_max_size;   /* SPI外设单次传输最大数据量 (单位：字节) */
            void *host_base;              /* 主机控制器基地址 (如SPI0/QSPI0寄存器基地址) */

            /**
            * @brief 片选信号控制回调函数
            * @param cs_pin 片选引脚号
            * @param state 引脚状态 (0: 拉低，1: 拉高)
            */
            void (*set_cs)(uint32_t cs_pin, uint8_t state);

            /**
            * @brief 频率设置回调函数
            * @param host 主机控制器实例
            * @param freq 目标频率值
            */
            void (*set_frequency)(void *host, uint32_t freq);
        } hpm_nor_host_param_t;

  - serial_nor主机读取参数结构体， 用于配置闪存读取操作的时序参数，支持不同模式的读取命令，如标准SPI读取、双线读取、四线快速读取等。

    .. code-block:: c

        typedef struct {
            uint8_t read_cmd;            /* 基础读取命令 (如0x03: Normal Read, 0x6B: Quad Fast Read) */
            uint8_t data_dummy_count;    /* 数据阶段前的空周期数 (根据闪存规格书设置) */
            hpm_serial_nor_seq_addr_bit_t addr_bit;         /* 地址长度模式 (24/32 bit) */
            hpm_serial_nor_seq_io_mode_t data_phase_format;  /* 数据阶段IO模式 (单/双/四线) */
            hpm_serial_nor_seq_io_mode_t addr_phase_format;  /* 地址阶段IO模式 (单/双/四线) */
        } hpm_sfdp_read_para_t;

  - serial_nor spi NOR Flash编程参数结构体，用于配置闪存页编程操作的参数，支持不同模式的编程命令。

    .. code-block:: c

        typedef struct {
            bool has_4b_addressing_inst_table; /* 是否支持4字节地址编程指令 (针对容量>128Mb的闪存) */
            bool support_1_4_4_page_program;  /* 是否支持1-4-4模式编程 (1线CMD + 4线ADDR + 4线DATA) */
            bool support_1_1_4_page_program;  /* 是否支持1-1-4模式编程 (1线CMD + 1线ADDR + 4线DATA) */
            uint8_t page_program_cmd;         /* 页编程命令码 (如0x02: Standard, 0x32: Quad Input) */
        } hpm_sfdp_program_para_t;

  - serial_nor主机参数容器结构体,组合了主机功能标志、硬件参数和用户数据，于初始化serial_no主机实例。

    .. code-block:: c

        typedef struct {
            uint32_t flags;              /* 功能标志位 (组合使用SERIAL_NOR_HOST_SUPPORT_* 系列宏) */
            hpm_nor_host_param_t param;  /* 主机硬件参数 (引脚配置、DMA参数、时钟等) */
            void *user_data;             /* 用户自定义数据指针 (用于回调函数上下文传递) */
        } hpm_serial_nor_host_param_t;

  - serial_nor传输序列描述结构体，封装了完整的NOR Flash操作时序，包含四个可配置阶段：命令阶段 (必需)，地址阶段 (可选)，空周期阶段 (可选)，数据阶段 (可选)。 常用于构建读/写/擦除等操作时序。

    .. code-block:: c

        typedef struct {
            /* DMA使用选择 (即使host支持DMA，单次传输仍可手动禁用) */
            uint8_t use_dma;  /* 0: 禁用DMA 1: 启用DMA */

            /* 命令阶段配置 */
            struct {
                uint8_t cmd;  /* 操作命令码 (如0x03: 读取, 0x02: 页编程, 0x20: 扇区擦除) */
            } cmd_phase;

            /* 地址阶段配置 */
            struct {
                bool enable;  /* 是否启用地址阶段 */
                hpm_serial_nor_seq_addr_bit_t addr_bit;     /* 地址长度模式 */
                hpm_serial_nor_seq_io_mode_t addr_io_mode;  /* 地址传输模式 */
                uint32_t addr;               /* 目标地址值 */
            } addr_phase;

            /* 空周期阶段配置 */
            struct {
                uint8_t dummy_count;  /* 需要插入的时钟周期数，单位是设置的数据长度。 (根据闪存规格书设置) */
            } dummy_phase;

            /* 数据阶段配置 */
            struct {
                hpm_serial_nor_seq_direction_t direction;  /* 数据传输方向 */
                hpm_serial_nor_seq_io_mode_t data_io_mode;  /* 数据传输模式 */
                uint32_t len;    /* 数据长度 (字节) */
                uint8_t *buf;    /* 数据缓冲区指针 (读操作时为接收缓冲，写操作时为发送缓冲) */
            } data_phase;
        } hpm_serial_nor_transfer_seq_t;

  - serial_nor主机操作函数集，封装了主机控制器的底层操作接口，需要根据具体硬件平台实现这些函数。

    .. code-block:: c

        typedef struct {
            /**
            * @brief 主机控制器初始化函数
            * @param host 主机控制器实例指针 (如SPI/QSPI控制器)
            * @return hpm_stat_t 初始化状态 (status_success表示成功)
            */
            hpm_stat_t (*init)(void *host);

            /**
            * @brief 传输执行函数（核心方法）
            * @param host 主机控制器实例指针
            * @param command_seq 传输序列描述结构体
            * @return hpm_stat_t 传输执行结果
            */
            hpm_stat_t (*transfer)(void *host, hpm_serial_nor_transfer_seq_t *command_seq);

            /**
            * @brief 片选信号控制回调
            * @param cs_pin 片选引脚编号或索引
            * @param state 控制状态 (0: 拉低选中，1: 拉高取消选中)
            */
            void (*set_cs)(uint32_t cs_pin, uint8_t state);

            /**
            * @brief 频率设置回调
            * @param host 主机控制器实例指针
            * @param freq 目标频率值 (单位：Hz)
            */
            void (*set_frequency)(void *host, uint32_t freq);

            /**
            * @brief 用户自定义数据指针
            * @note 可用于保存设备上下文或配置信息
            */
            void *user_data;
        } serial_nor_host_ops_t;

  - serial_nor主机设备结构体, 组合了主机参数配置和操作函数集合，用于完整描述一个NOR Flash主机实例。

    .. code-block:: c

        typedef struct {
            hpm_serial_nor_host_param_t host_param;  /* 主机参数配置 (功能标志/硬件参数) */
            serial_nor_host_ops_t host_ops;          /* 主机操作函数集合 (初始化/传输/控制接口) */
            void *user_data;                        /* 用户自定义数据指针 (用于扩展功能) */
        } hpm_serial_nor_host_t;

  - serial_nor主机设备综合描述结构体, 整合了主机驱动、闪存参数和芯片信息，构成完整的NOR Flash设备描述。

    .. code-block:: c

        typedef struct {
            hpm_serial_nor_host_t host;          /* 主机驱动实例 (包含硬件接口配置) */
            hpm_sfdp_read_para_t nor_read_para;  /* 读取操作时序参数配置 */
            hpm_sfdp_program_para_t nor_program_para; /* 编程操作时序参数配置 */
            hpm_serial_nor_info_t flash_info;    /* 闪存芯片物理特性信息 */
        } hpm_serial_nor_t;

API调用流程介绍
---------------------

定义serial_nor初始化变量
~~~~~~~~~~~~~~~~~~~~~~~~

- 定义一个 ``hpm_serial_nor_t``  结构体的变量。例如，``nor_flash_dev`` 变量。

  .. code-block:: c

        hpm_serial_nor_t nor_flash_dev;

serial_nor初始化变量
~~~~~~~~~~~~~~~~~~~~~~~~

- 主要初始化nor_flash_dev的host成员变量，其他成员不需要设置，均由组件内获取。初始化主机控制的参数配置，比如SPI基地址、片选引脚、DMA配置等。

- **举例** :

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
            /* 主机功能标志配置 */
            host->host_param.flags =  PORT_SPI_IO_MODE |            // SPI接口模式
                                    SERIAL_NOR_HOST_SUPPORT_DMA | // 启用DMA支持
                                    SERIAL_NOR_HOST_SUPPORT_SPI_INTERFACE; // 使用SPI协议
            /* 主机硬件参数配置 */
            host->host_param.param.set_cs = board_write_spi_cs; /* 片选信号控制回调 */
            host->host_param.param.set_frequency = set_spi_clk_frequency; /* 频率设置回调 */
            host->host_param.param.clock_name = PORT_SPI_CLK_NAME; /* SPI时钟源 */
            host->host_param.param.pin_or_cs_index = BOARD_SPI_CS_PIN; /* 片选引脚/索引 */
            host->host_param.param.host_base = PORT_SPI_BASE; /* SPI控制器基地址 */
        #if (SERIAL_NOR_USE_DMA_MGR == 0)
            /* 手动DMA资源配置 */
            host->host_param.param.dma_control.dma_base = PORT_SPI_NOR_DMA; /* DMA控制器基地址 */
            host->host_param.param.dma_control.dmamux_base    = PORT_SPI_NOR_DMAMUX; /* DMAMUX控制器基地址 */
            host->host_param.param.dma_control.rx_dma_ch  = PORT_SPI_RX_DMA_CH; /* RX DMA通道 */
            host->host_param.param.dma_control.tx_dma_ch  = PORT_SPI_TX_DMA_CH; /* TX DMA通道 */
        #endif
            host->host_param.param.dma_control.rx_dma_req = PORT_SPI_RX_DMA_REQ; /* RX DMA请求线 */
            host->host_param.param.dma_control.tx_dma_req = PORT_SPI_TX_DMA_REQ; /* TX DMA请求线 */
            host->host_param.param.frequency = PORT_SPI_CLK_FREQUENCY; /* 初始时钟频率 */
            host->host_param.param.transfer_max_size = SPI_SOC_TRANSFER_COUNT_MAX; /* SPI最大传输长度 */
            host->host_param.param.user_data = NULL; /* 用户自定义数据指针 */

            return status_success;
        }

        serial_nor_get_board_host(&nor_flash_dev.host);

    - 当使能了dma管理器组件，则不需要手动配置DMA通道，只需要配置DMA请求线即可。
    - 需要开启SPI时钟源以及初始化SPI相关引脚。

serial_nor初始化
~~~~~~~~~~~~~~~~

- 调用 ``hpm_serial_nor_init``  API 来初始化 serial_nor。在此过程中， ``nor_flash_dev``  变量的参数会被赋值到API的 ``flash`` 并生效。并返回 ``hpm_serial_nor_info_t`` 对应的nor flash器件信息。

    - ``hpm_serial_nor_init``  API原型：

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_init(hpm_serial_nor_t *flash, hpm_serial_nor_info_t *info);

    - 参数说明

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 要初始化的NOR Flash设备实例指针
            * - info
              - ``hpm_serial_nor_info_t *``
              - [out] 输出参数，用于获取闪存芯片物理特性信息

    - 返回值

      - ``status_success``: 初始化成功
      - ``status_invalid_argument``: 非法参数
      - ``status_spi_nor_flash_not_identified``: 闪存芯片未识别

    - 该API主要完成以下核心功能：

        - 硬件接口初始化：
            - 绑定SPI主机操作函数集
            - 初始化SPI控制器硬件
            - 配置初始通信频率(10MHz用于SFDP读取)

        - 闪存参数识别
            - 读取SFDP参数表(JEDEC标准)
            - 解析物理参数：页大小/扇区大小/块大小
            - 获取容量信息(24bit或32bit地址模式)

        - 工作模式配置
            - 自动尝试四线模式使能(QE bit设置)
            - 配置最佳读写时序参数
            - 设置DMA传输模式(如果使能)

        - 信息同步
            - 将解析后的闪存参数通过info参数输出
            - 返回初始化状态(成功/失败原因)


- **举例** : 实例化serial_nor，并且初始化，获取nor flash信息。

    .. code-block:: c

            hpm_stat_t stat;
            hpm_serial_nor_info_t flash_info;
            stat = hpm_serial_nor_init(&nor_flash_dev, &flash_info);
            if (stat != status_success) {
                printf("spi nor flash init error\n");
            } else {
                printf("spi nor flash init ok\n");
                if (hpm_serial_nor_get_info(&nor_flash_dev, &flash_info) == status_success) {
                    printf("the flash sfdp version:%d\n", flash_info.sfdp_version);
                    printf("the flash size:%d KB\n", flash_info.size_in_kbytes);
                    printf("the flash page_size:%d Byte\n", flash_info.page_size);
                    printf("the flash sector_size:%d KB\n", flash_info.sector_size_kbytes);
                    printf("the flash block_size:%d KB\n", flash_info.block_size_kbytes);
                    printf("the flash sector_erase_cmd:0x%02x\n", flash_info.sector_erase_cmd);
                    printf("the flash block_erase_cmd:0x%02x\n", flash_info.block_erase_cmd);
            }

serial_nor检测nor flash设备忙状态
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- 在操作nor flash的时候，比如非阻塞下擦除或者编程操作，需要检测nor flash是否处于忙状态。

- 调用 ``hpm_serial_nor_is_busy``  API 来检测 NOR Flash设备是否忙。

    - ``hpm_serial_nor_is_busy``  API原型：

      .. code-block:: c

            hpm_stat_t hpm_serial_nor_is_busy(hpm_serial_nor_t *flash);

    - 参数说明

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 已初始化的NOR Flash设备实例指针

    - 返回值

      - ``status_spi_nor_flash_is_busy``: 设备处于忙状态（擦除/编程操作进行中）
      - ``status_success``: 设备就绪
      - ``status_invalid_argument``: 参数错误（空指针）

- **举例** :

    .. code-block:: c

            /* 等待擦除操作完成 */
            while(hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
                hpm_spi_nor_udelay(100); /* 延时100us */
            }
            printf("闪存操作完成\n");

- **注意事项**

    - 该函数不会自动处理重试机制，需要调用者实现轮询逻辑


serial_nor擦除操作
~~~~~~~~~~~~~~~~~~~~~~~~~

- 包括了阻塞和非阻塞接口，其中非阻塞擦除接口包括扇区和块。而阻塞擦除接口可以按扇区，块，也可以指定地址指定长度进行擦除, 还有整片擦除。

非阻塞擦除接口
^^^^^^^^^^^^^^^^^^

扇区擦除
""""""""""""""
- 调用 ``hpm_serial_nor_erase_sector_nonblocking``  API 来进行非阻塞扇区擦除操作。

    - ``hpm_serial_nor_erase_sector_nonblocking``  API原型：

        .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_sector_nonblocking(hpm_serial_nor_t *flash, uint32_t sector_addr);


    - 参数说明

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] NOR Flash设备实例指针，需通过:hpm_serial_nor_init:初始化
            * - sector_addr
              - ``uint32_t``
              - [in] 目标扇区地址（需扇区对齐）

    - 返回值

        - ``status_success``: 擦除指令发送成功
        - ``status_invalid_argument``: 参数错误（空指针/地址未对齐）
        - ``status_spi_nor_flash_is_busy``: 闪存设备忙状态

- **举例** :

    .. code-block:: c

           /* 擦除0x8000开始的扇区 */
            hpm_stat_t status = hpm_serial_nor_erase_sector_nonblocking(&nor_flash_dev, 0x8000);

            if (status == status_success) {
                /*  可以在此执行其他任务 */
                while(hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
                    /* 等待擦除完成 */
                }
                printf("扇区擦除完成\n");
            } else {
                printf("擦除失败: 0x%08X\n", status);
            }

- **注意事项**

    - 必须确保sector_addr是扇区大小（flash_info.sector_size_kbytes）的整数倍
    - 调用前需通过hpm_serial_nor_is_busy()确认闪存就绪

块擦除
""""""""""""""

- 调用 ``hpm_serial_nor_erase_block_nonblocking``  API 来进行非阻塞块擦除操作。

    - ``hpm_serial_nor_erase_block_nonblocking``  API原型：

       .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_block_nonblocking(hpm_serial_nor_t *flash, uint32_t block_addr);

    - 参数说明

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 已初始化的NOR Flash设备实例指针
            * - block_addr
              - ``uint32_t``
              - [in] 目标块地址（需块对齐，通过flash_info.block_size_kbytes获取块大小）

    - 返回值
        - ``status_success``: 擦除指令成功发送
        - ``status_invalid_argument``: 参数错误（空指针/地址未对齐）
        - ``status_spi_nor_flash_is_busy``: 闪存设备忙状态

    - **举例** :

      .. code-block:: c

            /* 擦除0x10000开始的块 */
            hpm_stat_t status = hpm_serial_nor_erase_block_nonblocking(&nor_flash_dev, 0x10000);
            if (status == status_success) {
                /* 可以在此执行其他任务 */
                while(hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
                    /* 等待擦除完成 */
                }
                printf("块擦除完成\n");
            } else {
                printf("擦除失败: 0x%08X\n", status);
            }

- **注意事项**

    - 必须确保block_addr是块大小（flash_info.block_size_kbytes）的整数倍
    - 调用前需通过hpm_serial_nor_is_busy()确认闪存就绪

阻塞擦除接口
^^^^^^^^^^^^^^^^^^

整片擦除
""""""""""""""

- 调用 ``hpm_serial_nor_erase_chip``  API 来进行整片擦除操作。

    - ``hpm_serial_nor_erase_chip``  API原型：

       .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_chip(hpm_serial_nor_t *flash);

    - 参数说明

       .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] NOR Flash设备实例指针，需通过:hpm_serial_nor_init:初始化

    - 返回值

        - ``status_success``: 擦除操作完成
        - ``status_invalid_argument``: 非法参数（空指针）
        - ``status_spi_nor_flash_is_busy``: 设备处于忙状态

- **举例** :

    .. code-block:: c

        /* 执行整片擦除 */
        hpm_stat_t status = hpm_serial_nor_erase_chip(&nor_flash_dev);
        if (status == status_success) {
            printf("整片擦除成功\n");
        } else {
            printf("擦除失败: 0x%08X\n", status);
        }

- **注意事项**
    - 该操作会清除nor flash芯片所有数据，慎用！
    - 典型擦除时间参考芯片手册（MX25L25635F约150秒）


扇区擦除
""""""""""""""
- 调用 ``hpm_serial_nor_erase_sector_blocking``  API 来进行扇区擦除操作。

    - ``hpm_serial_nor_erase_sector_blocking``  API原型：

      .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_sector_blocking(hpm_serial_nor_t *flash, uint32_t sector_addr);

    - 参数说明

        .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 已初始化的NOR Flash设备实例指针
            * - sector_addr
              - ``uint32_t``
              - [in] 目标扇区地址（需扇区对齐，通过flash_info.sector_size_kbytes获取扇区大小）

    - 返回值

        - ``status_success``: 扇区擦除操作完成
        - ``status_invalid_argument``: 参数错误（空指针/地址未对齐）
        - ``status_spi_nor_flash_is_busy``: 闪存设备忙状态超时

    - **举例** :

     .. code-block:: c

        /* 擦除0x8000开始的扇区 */
        hpm_stat_t status = hpm_serial_nor_erase_sector_blocking(&nor_flash_dev, 0x8000);
        if (status == status_success) {
            printf("扇区擦除成功\n");
        }

    - **注意事项**

        - 必须确保sector_addr是扇区大小（flash_info.sector_size_kbytes）的整数倍


块擦除
""""""""""""""
- 调用 ``hpm_serial_nor_erase_block_blocking``  API 来进行块擦除操作。

    - ``hpm_serial_nor_erase_block_blocking``  API原型：

     .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_block_blocking(hpm_serial_nor_t *flash, uint32_t block_addr);

    - 参数说明

       .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 已初始化的NOR Flash设备实例指针
            * - block_addr
              - ``uint32_t``
              - [in] 目标块地址（需块对齐，通过flash_info.block_size_kbytes获取块大小）

    - 返回值

        - ``status_success``: 块擦除操作完成
        - ``status_invalid_argument``: 参数错误（空指针/地址未对齐）
        - ``status_spi_nor_flash_is_busy``: 闪存设备忙状态超时

    - **举例** :

    .. code-block:: c

        /* 擦除0x10000开始的块 */
            hpm_stat_t status = hpm_serial_nor_erase_block_blocking(&nor_flash_dev, 0x10000);
            if (status == status_success) {
                printf("块擦除成功\n");
            }

- **注意事项**

    - 必须确保block_addr是块大小（flash_info.block_size_kbytes）的整数倍
    - 该操作会清除指定块内所有数据，慎用！

指定区域擦除
""""""""""""""

- 调用 ``hpm_serial_nor_erase_blocking``  API 来进行指定区域擦除操作。

    - ``hpm_serial_nor_erase_blocking``  API原型：

    .. code-block:: c

            hpm_stat_t hpm_serial_nor_erase_blocking(hpm_serial_nor_t *flash, uint32_t start, uint32_t length);

    - 参数说明

      .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 已初始化的NOR Flash设备实例指针
            * - start
              - ``uint32_t``
              - [in] 起始地址
            * - length
              - ``uint32_t``
              - [in] 擦除长度（字节）

    - 返回值

        - ``status_success``: 擦除操作完成
        - ``status_invalid_argument``: 参数错误（空指针/地址未对齐）
        - ``status_spi_nor_flash_is_busy``: 闪存设备忙状态超时

    - **举例** :

   .. code-block:: c

        /* 擦除0x20000开始的1024字节 */
        hpm_stat_t status = hpm_serial_nor_erase_blocking(&nor_flash_dev, 0x20000, 1024);
        if (status == status_success) {
            printf("指定区域擦除成功\n");
        } else {
            printf("擦除失败: 0x%08X\n", status);
        }

- **注意事项**

    - 擦除粒度自动适配：块擦除（64KB）→ 扇区擦除（4KB）
    - 实际擦除范围会扩展到扇区/块边界

serial_nor写编程操作
~~~~~~~~~~~~~~~~~~~~~~~~~

- 包括了阻塞和非阻塞接口，其中非阻塞擦除接口只支持页编程。阻塞编程接口可以按页，也可以指定地址指定长度进行编程。

非阻塞编程接口
^^^^^^^^^^^^^^^^^^

- 调用 ``hpm_serial_nor_page_program_nonblocking``  API 来进行页编程。

    - ``hpm_serial_nor_page_program_nonblocking``  API原型：

      .. code-block:: c

            hpm_stat_t hpm_serial_nor_page_program_nonblocking(hpm_serial_nor_t *flash, uint8_t *buf, uint32_t data_len, uint32_t address);

    - 参数说明

       .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 已初始化的NOR Flash设备实例指针
            * - buf
              - ``uint8_t *``
              - [in] 待写入数据的缓冲区
            * - data_len
              - ``uint32_t``
              - [in] 要写入的数据长度（字节）
            * - address
              - ``uint32_t``
              - [in] 目标写入地址

    - 返回值

        - ``status_success``: 编程指令发送成功
        - ``status_invalid_argument``: 参数错误（空指针/地址未对齐/长度超限）
        - ``status_spi_nor_flash_is_busy``: 设备忙状态

    - **举例** :

   .. code-block:: c

         uint8_t write_buf[256];
         /* 填充数据... */

        /* 非阻塞写入0x1000地址 */
        hpm_stat_t status = hpm_serial_nor_page_program_nonblocking(&nor_flash_dev, write_buf, 256, 0x1000);
        if (status == status_success) {
            while(hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
                 /* 执行其他任务 */
            }
            printf("页编程完成\n");

- **注意事项**

    - 必须确保address是页大小（flash_info.page_size）的整数倍
    - 调用前需通过hpm_serial_nor_is_busy()确认闪存就绪
    - 写入数据长度不能超过页大小
    - 需要在编程前预先擦除扇区

阻塞编程接口
^^^^^^^^^^^^^^^^^^

- 调用 ``hpm_serial_nor_program_blocking``  API 来进行编程, 可随意编程指定长度的数据到指定地址。

    - ``hpm_serial_nor_program_blocking``  API原型：

     .. code-block:: c

            hpm_stat_t hpm_serial_nor_program_blocking(hpm_serial_nor_t *flash, uint8_t *buf, uint32_t data_len, uint32_t address);

    - 参数说明

      .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 已初始化的NOR Flash设备实例指针
            * - buf
              - ``uint8_t *``
              - [in] 待写入数据的缓冲区
            * - data_len
              - ``uint32_t``
              - [in] 要写入的数据长度（字节）
            * - address
              - ``uint32_t``
              - [in] 目标写入地址

    - 返回值

        - ``status_success``: 数据写入完成
        - ``status_invalid_argument``: 参数错误（空指针/地址越界）
        - ``status_spi_nor_flash_is_busy``: 闪存设备忙状态超时

    - **举例** :

   .. code-block:: c

        uint8_t write_buf[256] = {0x01, 0x23, 0x45, 0x67};
        hpm_stat_t status = hpm_serial_nor_program_blocking(&nor_flash_dev, write_buf, sizeof(write_buf), 0x8000);
        if (status == status_success) {
            printf("数据写入成功\n");
        }

serial_nor读操作
~~~~~~~~~~~~~~~~~~~~~~~~~

- 调用 ``hpm_serial_nor_read``  API 来进行读取操作。

    - ``hpm_serial_nor_read``  API原型：

     .. code-block:: c

            hpm_stat_t hpm_serial_nor_read(hpm_serial_nor_t *flash, uint8_t *buf, uint16_t data_len, uint32_t address);

    - 参数说明

     .. list-table::
            :widths: 20 30 50
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - flash
              - ``hpm_serial_nor_t *``
              - [in] 已初始化的NOR Flash设备实例指针
            * - buf
              - ``uint8_t *``
              - [in] 读取数据的缓冲区
            * - data_len
              - ``uint16_t``
              - [in] 要读取的数据长度（字节）
            * - address
              - ``uint32_t``
              - [in] 读取起始地址

    - 返回值

        - ``status_success``: 数据读取成功
        - ``status_invalid_argument``: 参数错误（空指针/地址越界）
        - ``status_spi_nor_flash_is_busy``: 闪存设备忙状态超时

    - **举例** :

  .. code-block:: c

        uint8_t read_buf[256];
        hpm_stat_t status = hpm_serial_nor_read(&nor_flash_dev, read_buf, sizeof(read_buf), 0x8000);
        if (status == status_success) {
            printf("数据读取成功\n");
        }

- **注意事项**

    - 实际读取速度取决于SPI时钟配置
    - 建议data_len按4字节对齐以获得最佳性能
    - 支持跨页连续读取，无长度限制（除缓冲区大小外）
    - 读取前无需擦除操作

.. note::

    - 如果使能了DMA管理器组件，那么serial_nor内部的DMA的通道等配置由DMA管理器分配，在使用DMA时分配的DMA通道避免与SPI组件使用的DMA通道冲突。

    - serial_nor组件使用的发送DMA通道可以调用 ``hpm_serial_nor_get_tx_dma_mgr_resource`` API 获取SPI发送的DMA通道资源。

      - ``hpm_serial_nor_get_tx_dma_mgr_resource``  API函数原型:


        .. code-block:: c

                dma_resource_t *hpm_serial_nor_get_tx_dma_mgr_resource(hpm_serial_nor_t *flash);


    - serial_nor组件使用的接收DMA通道可以调用 ``hpm_serial_nor_get_rx_dma_mgr_resource`` API 获取SPI发送的DMA通道资源。

      - ``hpm_serial_nor_get_rx_dma_mgr_resource``  API函数原型:


        .. code-block:: c

               dma_resource_t *hpm_serial_nor_get_rx_dma_mgr_resource(hpm_serial_nor_t *flash);

    - **举例** : 如何使用 hpm_serial_nor_get_tx_dma_mgr_resource 和 hpm_serial_nor_get_rx_dma_mgr_resource 函数获取DMA通道资源以及获取到DMA通道资源后如何使用DMA通道资源
       .. code-block:: c

            /* 初始化SPI... 不做列举 */
            /* 获取发送DMA通道资源 */
            dma_resource_t *tx_dma_resource = hpm_serial_nor_get_tx_dma_mgr_resource(&nor_flash_dev);
            if (tx_dma_resource != NULL) {
                /* 成功获取发送DMA通道资源 */
                printf("TX DMA channel resource obtained successfully.\n");
                /* 打印获取到的发送DMA通道资源占用的DMA实例以及DMA通道 */
                printf("TX DMA instance: %d, TX DMA channel: %d\n", tx_dma_resource->dma_instance, tx_dma_resource->dma_channel);
                /* 改变TX DMA资源的中断优先级为1*/
                dma_mgr_enable_dma_irq_with_priority(tx_dma_resource, 1);
            }
            /* 获取接收DMA通道资源 */
            dma_resource_t *rx_dma_resource = hpm_serial_nor_get_rx_dma_mgr_resource(&nor_flash_dev);
            if (rx_dma_resource != NULL) {
                /* 成功获取接收DMA通道资源 */
                printf("RX DMA channel resource obtained successfully.\n");
                /* 获取接收DMA通道资源占用的DMA实例以及DMA通道 */
                printf("RX DMA instance: %d, TX DMA channel: %d\n", rx_dma_resource->dma_instance, rx_dma_resource->dma_channel);
                /* 改变RX DMA资源的中断优先级为1*/
                dma_mgr_enable_dma_irq_with_priority(rx_dma_resource, 1);
            }



