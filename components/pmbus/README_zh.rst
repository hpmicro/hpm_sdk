.. _pmbus:

PMBus
=======

概述
-----

- PMBus是一种总线协议，用于在计算机系统中管理和监控各种物理设备，如处理器、内存、存储设备等。PMBus协议定义了一组标准的命令和寄存器，用于读取和写入设备的状态信息，以及执行各种管理操作。PMBus总线通常与SMBus总线兼容，因此可以在许多不同的计算机系统中使用。

- PMBus协议的主要目标是提供一种统一的接口，使得不同的物理设备可以通过相同的总线进行通信和管理。它支持多种设备类型，如处理器、内存、存储设备、网络接口等。PMBus协议还支持设备的热插拔和热插拔事件的检测，以及设备的故障诊断和故障恢复。

- PMBus协议的实现通常由硬件和软件组成。硬件部分包括物理总线和设备，软件部分包括驱动和库函数。

  - 驱动负责将PMBus协议转换为SMBus协议，并将SMBus协议转换为物理总线的信号。（依赖smbus组件）
  - 本组件则提供了一组API，用于读取和写入设备的状态信息，以及执行各种管理操作。

- 本组件完整支持PMBus 1.2规范定义的200+个标准命令码（如电压/电流/温度监控、故障响应等）

- 提供命令参数配置表（pmbus_cmd_param_table），定义每个命令的：

  .. code-block:: c

    typedef struct {
        uint8_t command_code;        /* PMBus命令码 */
        uint32_t data_length;        /* 数据长度（0表示无数据） */
        pmbus_transaction_type_t read_transaction_type;  /* 读操作类型（字节/字/块） */
        pmbus_transaction_type_t write_transaction_type; /* 写操作类型 */
    } hpm_pmbus_cmd_param_t;

- 系统集成功能

  - 支持主机/从机模式（通过底层I2C/SMBus驱动）
  - 制造商扩展命令支持（MFR_SPECIFIC_00-45）
  - 从机模式使用中断回调方式响应命令并处理数据。

相关枚举和结构体介绍
------------------------------

- 枚举介绍

  - PMBUS通信中的事务类型，要用在PMBUS协议通信中，用于区分不同的数据传输类型。每个枚举值对应PMBUS规范中定义的标准事务类型，包括：字节/字/块级的读写操作、特殊单字节传输、以及厂商自定义和扩展命令的处理。

        .. code-block:: c

            typedef enum {
                none            = 0,    /**< 无操作 */
                write_byte      = 1,    /**< 写入一个字节 */
                read_byte       = 2,    /**< 读取一个字节 */
                write_word      = 3,    /**< 写入一个字（两个字节） */
                read_word       = 4,    /**< 读取一个字（两个字节） */
                write_block     = 5,    /**< 写入数据块 */
                read_block      = 6,    /**< 读取数据块 */
                send_byte       = 7,    /**< 发送单字节 */
                mfr_defined     = 8,    /**< 厂商自定义命令 */
                extended_command = 9,   /**< 扩展命令 */
            } hpm_pmbus_transaction_type;

  - PMBUS状态码，定义了PMBUS组件的错误状态码，主要用于标识电源管理总线通信中的特定错误类型。

        .. code-block:: c

            enum {
                status_pmbus_not_support_cmd = MAKE_STATUS(status_group_pmbus, 1),        /**< 不支持的PMBus命令 */
                status_pmbus_not_transaction_type = MAKE_STATUS(status_group_pmbus, 2),  /**< 不支持的事务类型 */
            };

  - 定义回调函数类型 hpm_pmbus_complete_cb_t，此回调函数会在 PMbus 操作完成后被调用，允许用户实现特定的完成处理逻辑。

        .. code-block:: c

            typedef void (*hpm_pmbus_complete_cb_t)(I2C_Type *base, hpm_pmbus_complete_cb_cfg_t *cfg);

        - 参数说明

            .. list-table::
                :widths: 20 30 50
                :header-rows: 1

                * - 参数名
                  - 类型
                  - 描述
                * - base
                  - ``I2C_Type *``
                  - 指向 I2C 控制器的基地址，用于标识正在进行操作的 I2C 控制器。
                * - cfg
                  - ``hpm_pmbus_complete_cb_cfg_t *``
                  - 指向 PMBUS 命令完成回调配置结构体的指针，用于提供操作完成的详细信息，包括命令码、数据长度、操作方向、PEC 校验状态以及数据缓冲区指针。


- 结构体介绍

  - PMBUS命令完成回调配置结构体，用于在PMBUS命令传输完成后，向回调函数传递完整的操作上下文信息。

    .. code-block:: c

        typedef struct {
            uint8_t command; /**< 当前处理的PMBus命令码（如VOUT_COMMAND=0x21） */
            uint32_t len;    /**< 本次传输的有效数据长度（单位：字节） */
            bool read;       /**< 操作方向标志：true=读操作，false=写操作 */
            bool pec_check;  /**< 是否启用了数据包错误校验(PEC) */
            uint8_t *data;   /**< 数据缓冲区指针：
                                - 写操作：指向已发送的数据
                                - 读操作：指向接收数据存储位置 */
        } hpm_pmbus_complete_cb_cfg_t;

  - PMBus命令参数结构体,用于存储PMBus命令的相关参数，方便在PMBus通信中使用。

    .. code-block:: c

        typedef struct {
            /**< 命令码，用于标识特定的PMBus命令 */
            uint8_t command_code;
            /**< 写事务类型，指示写操作的具体类型 */
            hpm_pmbus_transaction_type write_transaction_type;
            /**< 读事务类型，指示读操作的具体类型 */
            hpm_pmbus_transaction_type read_transaction_type;
            /**< 数据长度，指示命令关联的数据长度，单位为字节，0表示无数据 */
            uint32_t data_length;
        } hpm_pmbus_cmd_param_t;

  - PMBus从机配置结构体，用于存储PMBus从机设备的运行时配置。

    .. code-block:: c

        typedef struct {
            bool is_valid;        /**< 从机配置有效性标志 */
            uint8_t *wdata;      /**< 指向待写入数据的指针（主机→从机） */
            uint8_t *rdata;      /**< 指向读取数据缓冲区的指针（从机→主机） */
            uint32_t data_length; /**< 数据传输长度（单位：字节） */
            hpm_pmbus_complete_cb_t callback; /**< 数据传输完成回调函数指针 */
        } hpm_pmbus_slave_cfg_t;

API调用流程介绍
---------------------

PMbus初始化
^^^^^^^^^^^^^^

主机模式
""""""""""""""

- 调用 ``i2c_init_master`` API 来初始化 I2C为主机模式。该API在 ``hpm_i2c_drv`` 驱动中定义。

    - ``i2c_init_master`` API原型：

        .. code-block:: c

                hpm_stat_t i2c_init_master(I2C_Type *ptr, uint32_t src_clk_in_hz, i2c_config_t *config);

        - 参数说明：

            .. list-table::
                :header-rows: 1

                * - 参数名
                  - 类型
                  - 描述
                * - ptr
                  - I2C_Type*
                  - 指向I2C控制器基地址的指针
                * - src_clk_in_hz
                  - uint32_t
                  - I2C时钟源频率
                * - config
                  - i2c_config_t*
                  - 指向I2C配置结构体的指针

        - 返回值：

            - ``status_success``: 成功
            - ``status_invalid_argument``: 无效参数


- **举例**: 实例化I2C0，设置I2C0为主机模式，100Khz速度，7bit地址模式。

    .. code-block:: c

            #define TEST_PMBUS                HPM_I2C0
            #define TEST_PMBUS_CLOCK_NAME     clock_i2c0

            i2c_config_t config;    /* 定义I2C配置结构体 */
            board_init_i2c_clock(TEST_PMBUS);
            init_i2c_pins(TEST_PMBUS);

            config.i2c_mode = i2c_mode_normal;
            config.is_10bit_addressing = false;
            freq = clock_get_frequency(TEST_PMBUS_CLOCK_NAME);
            stat = i2c_init_master(TEST_PMBUS, freq, &config);
            if (stat != status_success) {
                return stat;
            }

从机模式
""""""""""""""
- 除了需要 ``i2c_init_slave`` API初始化I2C外设基本的配置之外，还需要使用 ``hpm_pmbus_slave_init`` API初始化PMBus从机设备开启相关中断，为PMBus从机设备准备好通信基础环境。中断函数中使用 ``hpm_pmbus_isr_handler`` 来处理I2C从机地址匹配和传输完成事件。

    - 调用 ``i2c_init_slave`` API 来初始化 I2C为从机模式。该API在 ``hpm_i2c_drv`` 驱动中定义。

        - ``i2c_init_slave`` API原型：

            .. code-block:: c

                hpm_stat_t i2c_init_slave(I2C_Type *ptr, uint32_t src_clk_in_hz, i2c_config_t *config);

          - 参数说明：

              .. list-table::
                :header-rows: 1

                * - 参数名
                  - 类型
                  - 描述
                * - ptr
                  - I2C_Type*
                  - 指向I2C控制器基地址的指针
                * - src_clk_in_hz
                  - uint32_t
                  - I2C时钟源频率
                * - config
                  - i2c_config_t*
                  - 指向I2C配置结构体的指针

          - 返回值：

            - ``status_success``: 成功
            - ``status_invalid_argument``: 无效参数

    - 调用 ``hpm_pmbus_slave_init`` API 来初始化 PMBus从机设备。该API在 ``hpm_pmbus`` 中定义。主要的作用是启用I2C的中断事件：地址命中和传输完成，这样当从机地址匹配或传输完成时，会触发中断，确保从机能正确响应主机的请求，接着在中断回调处理请求相关业务。

        - ``hpm_pmbus_slave_init`` API原型：

           .. code-block:: c

                hpm_stat_t hpm_pmbus_slave_init(I2C_Type *ptr, uint8_t slave_addr);

          - 参数说明：

             .. list-table::
                :header-rows: 1

                * - 参数名
                  - 类型
                  - 描述
                * - ptr
                  - I2C_Type*
                  - 指向I2C控制器基地址的指针
                * - slave_addr
                  - uint8_t
                  - 从机设备地址

          - 返回值：

            - ``status_success``: 成功
            - ``status_invalid_argument``: 无效参数

    - 中断回调函数原型：

        .. code-block:: c

            void hpm_pmbus_isr_handler(I2C_Type *ptr);

        - 参数说明：

           .. list-table::
                :header-rows: 1

                * - 参数名
                  - 类型
                  - 描述
                * - ptr
                  - I2C_Type*
                  - 指向I2C控制器基地址的指针


- **举例**: 实例化I2C0，设置I2C0为从机模式，100Khz速度，7bit地址模式，从机设备地址为0x16。

   .. code-block:: c

        #define TEST_PMBUS                HPM_I2C0
        #define TEST_PMBUS_CLOCK_NAME     clock_i2c0
        #define TEST_I2C_IRQ              IRQn_I2C0
        #define TEST_PMBUS_SLAVE_ADDRESS  (0x16U)

        SDK_DECLARE_EXT_ISR_M(TEST_I2C_IRQ, i2c_isr)
        void i2c_isr(void)
        {
            hpm_pmbus_isr_handler(TEST_PMBUS);
        }

        i2c_config_t config;    /* 定义I2C配置结构体 */
        board_init_i2c_clock(TEST_PMBUS);
        init_i2c_pins(TEST_PMBUS);
        config.i2c_mode = i2c_mode_slave;
        config.is_10bit_addressing = false;
        config.slave_address = 0x16;
        freq = clock_get_frequency(TEST_PMBUS_CLOCK_NAME);
        stat = i2c_init_slave(TEST_PMBUS, freq, &config);
        if (stat!= status_success) {
            return stat;
        }
        intc_m_enable_irq_with_priority(TEST_I2C_IRQ, 1);
        hpm_pmbus_slave_init(TEST_PMBUS, TEST_PMBUS_SLAVE_ADDRESS);


读写操作，支持主从机模式
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

主机模式
""""""""""""""

写操作
~~~~~~~~~~~~~~

- 使用 ``hpm_pmbus_master_write`` API执行写操作

    - 该API关键功能说明：

      - 通过pmbus_cmd_param_table[command]获取预定义的命令参数配置，包含：

        - 事务类型（字节/字/块）
        - 预期数据长度
        - 其他协议参数

      - 多事务类型支持

        - 支持5种写操作模式：

          - send_byte: 仅发送命令字节（无数据），用于简单控制命令
          - write_byte: 命令+1字节数据（如设置使能位）
          - write_word: 命令+2字节数据（如设置电压值0x12 0x34）
          - write_block: 命令+长度字节+数据块（用于大数据传输）

        - 当处理块写入时：

          - 0xFFFFFFFF表示动态长度模式，使用调用者传入的len参数
          - 其他值表示使用预定义的固定数据长度
          - 自动处理PMBus协议要求的长度字节前缀

        - 返回标准化的错误代码：

          - status_pmbus_not_support_cmd: 遇到未实现的命令类型
          - status_pmbus_not_transaction_type: 无效的事务类型配置

    - API原型：

        .. code-block:: c

            hpm_stat_t hpm_pmbus_master_write(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t len);

    - 参数说明：

        .. list-table::
            :widths: 15 15 70
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - I2C_Type*
              - 指向I2C控制器基地址
            * - slave_address
              - uint8_t
              - 从机设备地址（7位）
            * - command
              - uint8_t
              - PMBus命令码
            * - data
              - uint8_t*
              - 待写入数据指针（根据命令类型选择字节/字/块数据）
            * - len
              - uint32_t
              - 数据长度（仅块写入时生效）

    - 返回值：
        - ``status_success``: 操作成功
        - ``status_pmbus_not_support_cmd``: 不支持的PMBus命令
        - ``status_pmbus_not_transaction_type``: 无效的事务类型

    - 示例：设置输出电压为3.3V（写入字数据0x0D 0x00）

        .. code-block:: c

            /* PMbus初始化，不做举例 */
            uint16_t voltage = 0x0D00;
            hpm_stat_t status = hpm_pmbus_master_write(I2C0, 0x5A, PMBUS_CODE_VOUT_COMMAND, (uint8_t*)&voltage, 2);

读操作
~~~~~~~~~~~~~~

- 使用 ``hpm_pmbus_master_read`` API执行读操作

    - 该API关键功能说明：

      - 多模式支持，处理三种标准读取操作：

        - 字节读取：用于读取状态寄存器等单字节数据
        - 字读取：用于读取电压/电流等16位测量值
        - 块读取：用于获取日志信息等大数据块

      - 长度处理策略

        - 字节/字读取：固定返回1/2字节
        - 块读取：
          - 0xFFFFFFFF表示动态长度模式，使用调用者传入的*len作为缓冲区容量
          - 其他值表示使用预定义长度，自动校验数据完整性

        - 返回标准化的错误代码：

          - status_pmbus_not_support_cmd: 遇到未实现的命令类型
          - status_pmbus_not_transaction_type: 无效的事务类型配置

    - API原型：

        .. code-block:: c

            hpm_stat_t hpm_pmbus_master_read(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t *len);

    - 参数说明：

        .. list-table::
            :widths: 15 15 70
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - I2C_Type*
              - 指向I2C控制器基地址
            * - slave_address
              - uint8_t
              - 从机设备地址（7位）
            * - command
              - uint8_t
              - PMBus命令码
            * - data
              - uint8_t*
              - 数据接收缓冲区指针
            * - len
              - uint32_t*
              - 输入：缓冲区最大容量 / 输出：实际读取长度

    - 返回值：
        - ``status_success``: 操作成功
        - ``status_pmbus_not_support_cmd``: 不支持的PMBus命令
        - ``status_pmbus_not_transaction_type``: 无效的事务类型

    - 示例：读取输出电压值

        .. code-block:: c

            /* PMbus初始化，不做举例 */
            uint8_t vout_data[2];
            uint32_t read_len = 2;
            hpm_stat_t status = hpm_pmbus_master_read(I2C0, 0x5A, PMBUS_CODE_VOUT_COMMAND, vout_data, &read_len);
            if (status == status_success) {
                float voltage = (vout_data[0] | (vout_data[1] << 8)) * 0.001; /* 转换为电压值 */
            }

从机模式
""""""""""""""

从机命令事务管理
~~~~~~~~~~~~~~~~~~

命令事务安装
++++++++++++++++++++++++++

- 使用 ``hpm_pmbus_slave_command_transaction_install`` 配置从机命令响应

    - **关键功能**：

        - 预绑定PMBus命令与数据缓冲区
        - 注册事务完成回调函数
        - 支持标准命令码（0x00-0xFF）
        - 自动处理PEC校验字节空间分配

    - API原型：

        .. code-block:: c

            hpm_stat_t hpm_pmbus_slave_command_transaction_install(
                I2C_Type *ptr,
                uint8_t command,
                uint8_t *wdata,
                uint8_t *rdata,
                uint32_t len,
                hpm_pmbus_complete_cb_t callback
            );

    - 参数说明：

        .. list-table::
            :widths: 20 25 55
            :header-rows: 1

            * - 参数
              - 类型
              - 描述
            * - ptr
              - I2C_Type*
              - 绑定的I2C控制器实例（如HPM_I2C0）
            * - command
              - uint8_t
              - PMBus标准命令码
            * - wdata
              - uint8_t*
              - 主机写操作数据缓冲区（从机接收用），需预留PEC字节空间
            * - rdata
              - uint8_t*
              - 主机读操作数据缓冲区（从机发送用），需预留PEC字节空间
            * - len
              - uint32_t
              - 缓冲区总长度（包含数据+PEC校验字节）
            * - callback
              - hpm_pmbus_complete_cb_t
              - 事务完成回调函数指针

    - 返回值：
        - ``status_success``: 配置成功
        - ``status_invalid_argument``: 空指针/缓冲区长度不足
        - ``status_pmbus_not_support_cmd``: 不支持的命令类型


命令事务卸载
~~~~~~~~~~~~~~

- 使用 ``hpm_pmbus_slave_command_transaction_uninstall`` 移除命令配置

    - **关键功能**：

        - 释放命令槽位资源
        - 清除关联的回调函数
        - 禁用指定命令的响应

    - API原型：

        .. code-block:: c

            hpm_stat_t hpm_pmbus_slave_command_transaction_uninstall(
                I2C_Type *ptr,
                uint8_t command
            );

    - 参数说明：

        .. list-table::
            :widths: 25 25 50
            :header-rows: 1

            * - 参数
              - 类型
              - 描述
            * - ptr
              - I2C_Type*
              - I2C控制器实例（需与安装时一致）
            * - command
              - uint8_t
              - 需要移除的PMBus命令码

    - 返回值：
        - ``status_success``: 卸载成功
        - ``status_invalid_argument``: 非法控制器指针

应用示例
~~~~~~~~~~~~~~

- 从机电压监控示例（响应READ_VOUT命令）

.. code-block:: c

    /* 定义命令参数 */
    #define VOUT_CMD      PMBUS_CODE_VOUT_COMMAND  /* 0x8B
    #define DATA_BUF_LEN  3                   /* 2字节数据 + 1字节PEC */

    uint8_t vout_data[DATA_BUF_LEN] = {0};    /* 电压数据存储（小端格式）*/

    /* 电压读取完成回调 */
    void vout_callback(I2C_Type *base, hpm_pmbus_complete_cb_cfg_t *cfg)
    {
        if (cfg->read) {
            printf("主机读取了%d字节电压数据\n", cfg->len);
        } else {
            printf("主机写入了%d字节配置\n", cfg->len);
        }
    }

    /* 系统初始化 */
    void pmbus_slave_init(void)
    {
        /* PMbus初始化，不做举例 */

        /* 安装电压读取命令事务 */
        hpm_pmbus_slave_command_transaction_install(
            TEST_PMBUS,
            VOUT_CMD,
            NULL,       /* 主机写操作不处理 */
            vout_data,  /* 读数据缓冲区 */
            DATA_BUF_LEN,
            vout_callback
        );

        /*填充初始电压值（11.5V = 0x47E）
        vout_data[0] = 0xE0 | (0x47 & 0x1F);  /* 指数部分 */
        vout_data[1] = (0x47 >> 5);           /*  尾数部分 */
    }

    /* 停止监控时卸载 */
    void pmbus_slave_deinit(void)
    {
        hpm_pmbus_slave_command_transaction_uninstall(TEST_PMBUS, VOUT_CMD);
    }

