.. _i2c:

I2C
===

概述
----

- 主要介绍先楫I2C外设的主要驱动接口说明和调用方法，更多内容请参考 `hpm_i2c_drv.h` 的API说明以及相关用户手册。
- 支持标准模式（100Kb/s）、快速模式（400Kb/s）、增强快速模式（1Mb/s）。
- 支持7位地址和10位地址。
- 支持主机模式和从机模式。
- 支持各类中断以及DMA传输。

  - 传输完成中断
  - 字节接收中断
  - 字节发送中断
  - 开始信号中断
  - 停止信号中断
  - 仲裁丢失中断
  - 地址命中中断
  - FIFO半满/半空中断
  - FIFO满中断
  - FIFO空中断

- 支持总线死锁恢复。此项支持需要看SOC的 `hpm_soc_ip_feature.h` 是否定义 `HPM_IP_FEATURE_I2C_SUPPORT_RESET` 宏
- 具有4字节硬件FIFO缓冲。具体深度可查看 `hpm_soc_feature.h` 的 `I2C_SOC_FIFO_SIZE` 宏定义或者使用 `hpm_i2c_drv.h` 的 `i2c_get_fifo_size` API接口获取
- 单次传输支持最大长度为4096字节。具体长度可查看 `hpm_soc_feature.h` 的 `I2C_SOC_TRANSFER_COUNT_MAX` 宏定义



I2C初始化
--------------

- 需要确保I2C的时钟源已经开启，并且初始化了相关I2C外设引脚。

  - 可使用`clock_add_to_group` 函数用于将I2C时钟源添加到时钟组中，从而确保I2C时钟源已经开启。

- 相关结构体介绍：

  - `i2c_config_t` 结构体用于配置I2C的相关参数

    .. code-block:: c

        /* I2C配置结构体 */
        typedef struct {
            bool is_10bit_addressing;  /* 是否使用10位地址 */
            uint8_t i2c_mode;          /* I2C速度模式 */
        } i2c_config_t;

- 相关枚举值介绍：

  - `i2c_mode_t` 枚举用于配置I2C的速度模式

   .. code-block:: c

        /* I2C速度模式 */
        typedef enum i2c_mode {
            i2c_mode_normal,      /* 标准模式 100Kb/s */
            i2c_mode_fast,        /* 快速模式 400Kb/s */
            i2c_mode_fast_plus,  /* 增强快速模式 1Mb/s */
        } i2c_mode_t;

主机模式初始化
^^^^^^^^^^^^^^^^^^^

- 初始化函数API：

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
              - I2C控制器的时钟源频率（Hz）
            * - config
              - i2c_config_t*
              - 指向I2C配置结构体的指针

  - 返回值：

    .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 初始化成功
            * - status_i2c_not_supported
              - I2C控制器不支持

  - I2C控制器的时钟源频率可使用 `clock_get_frequency` API获取。

    - 示例：

      .. code-block:: c

            uint32_t i2c_clk_freq = clock_get_frequency(clock_i2c0);

- 示例：

  - I2C0使用标准模式，100Kb/s，7位地址

  .. code-block:: c

        /* I20C时钟源开启，相关引脚初始化，不做举例... */
        /* I2C0初始化，作为主机。 */

        i2c_config_t i2c_config;
        uint32_t i2c_clk_freq = clock_get_frequency(clock_i2c0);
        i2c_config.is_10bit_addressing = false,
        i2c_config.i2c_mode = i2c_mode_normal,
        hpm_stat_t status = i2c_init_master(HPM_I2C0, i2c_clk_freq, &i2c_config);


从机模式初始化
^^^^^^^^^^^^^^^^^^^

- 初始化函数API：

  .. code-block:: c

        hpm_stat_t i2c_init_slave(I2C_Type *ptr, uint32_t src_clk_in_hz, i2c_config_t *config, const uint16_t slave_address);

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
              - I2C控制器的时钟源频率（Hz）
            * - config
              - i2c_config_t*
              - 指向I2C配置结构体的指针
            * - slave_addr
              - uint8_t
              - 从机地址

  - 返回值：

    .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 初始化成功
            * - status_i2c_not_supported
              - I2C控制器不支持

  - I2C控制器的时钟源频率可使用 `clock_get_frequency` API获取。

- 示例：

  - I2C0使用标准模式，100Kb/s，7位地址，从机地址为0x10

    .. code-block:: c

        /* I20C时钟源开启，相关引脚初始化，不做举例... */
        /* I2C0初始化，作为从机。 */
        i2c_config_t i2c_config;
        uint32_t i2c_clk_freq = clock_get_frequency(clock_i2c0);
        i2c_config.is_10bit_addressing = false,
        i2c_config.i2c_mode = i2c_mode_normal,
        hpm_stat_t status = i2c_init_slave(HPM_I2C0, i2c_clk_freq, &i2c_config, 0x10);


I2C数据传输
--------------

polling传输
^^^^^^^^^^^^^^^^^^^

主机模式
""""""""""""

- 提供了不带地址操作的读写API，带地址操作的读写API，以及自定义序列传输的API。

  - 不带地址操作的读写API：

    .. code-block:: c

        hpm_stat_t i2c_master_write(I2C_Type *ptr, const uint16_t device_address, uint8_t *buf, const uint32_t size);
        hpm_stat_t i2c_master_read(I2C_Type *ptr, const uint16_t device_address, uint8_t *buf, const uint32_t size);

    - 参数说明：读写API的参数一致，具体如下：

      .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - I2C_Type*
                - 指向I2C控制器基地址的指针
              * - device_address
                - uint16_t
                - 设备地址
              * - buf
                - uint8_t*
                - 数据缓冲区指针
              * - size
                - uint32_t
                - 数据长度

    - 返回值：

      .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 传输成功
              * - status_invalid_argument
                - 无效参数
              * - status_timeout
                - 传输超时
              * - status_i2c_transmit_not_completed
                - 传输未完成
              * - status_i2c_no_addr_hit
                - 未命中地址（总线上并无此从机设备地址）
              * - status_fail
                - 传输失败

    - 提示：

      - 此API可用于扫描是否存在指定的从机设备地址。 `buf` 参数可设置为NULL， `size` 参数可设置为0。

        - 示例：

          .. code-block:: c

            /* 扫描I2C0是否存在从机地址为0x10的设备 */
            uint8_t buf[1];
            hpm_stat_t status = i2c_master_read(HPM_I2C0, 0x10, NULL, 0);
            if (status == status_success) {
                /* 存在从机地址为0x10的设备 */
                printf("HPM_I2C0 has device address 0x10.\n");
            }

    - 示例：

      - 主机向从机地址为0x10的设备写入10个字节的数据

        .. code-block:: c

            uint8_t buf[10];
            hpm_stat_t status = i2c_master_write(HPM_I2C0, 0x10, buf, 10);
            if (status == status_success) {
                printf("HPM_I2C0 write 10 bytes to device address 0x10 success.\n");
            }

      - 主机从从机地址为0x10的设备读取10个字节的数据

        .. code-block:: c

            uint8_t buf[10];
            hpm_stat_t status = i2c_master_read(HPM_I2C0, 0x10, buf, 10);
            if (status == status_success) {
                printf("HPM_I2C0 read 10 bytes from device address 0x10 success.\n");
            }

  - 带地址操作的读写API：

    .. code-block:: c

        hpm_stat_t i2c_master_address_write(I2C_Type *ptr, const uint16_t device_address, uint8_t *addr, uint32_t addr_size_in_byte, uint8_t *buf, const uint32_t size_in_byte);
        hpm_stat_t i2c_master_address_read(I2C_Type *ptr, const uint16_t device_address, uint8_t *addr, uint32_t addr_size_in_byte, uint8_t *buf, const uint32_t size_in_byte);

    - 参数说明：读写API的参数一致，具体如下：

      .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - I2C_Type*
                - 指向I2C控制器基地址的指针
              * - device_address
                - uint16_t
                - 设备地址
              * - addr
                - uint8_t*
                - 地址缓冲区指针
              * - addr_size_in_byte
                - uint32_t
                - 地址长度
              * - buf
                - uint8_t*
                - 数据缓冲区指针
              * - size_in_byte
                - uint32_t
                - 数据长度
    - 返回值：

      .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 传输成功
              * - status_invalid_argument
                - 无效参数
              * - status_timeout
                - 传输超时
              * - status_i2c_transmit_not_completed
                - 传输未完成
              * - status_i2c_no_addr_hit
                - 未命中地址（总线上并无此从机设备地址）
              * - status_fail
                - 传输失败
    - 示例：

      - 主机向从机地址为0x10的设备的地址为0x01的寄存器写入10个字节的数据

        .. code-block:: c

            uint8_t addr[1] = {0x01};
            uint8_t buf[10];
            hpm_stat_t status = i2c_master_address_write(HPM_I2C0, 0x10, addr, 1, buf, 10);
            if (status == status_success) {
                printf("HPM_I2C0 write 10 bytes to device address 0x10 success.\n");
            }

      - 主机从从机地址为0x10的设备的地址为0x02的寄存器读取10个字节的数据

        .. code-block:: c

            uint8_t addr[1] = {0x02};
            uint8_t buf[10];
            hpm_stat_t status = i2c_master_address_read(HPM_I2C0, 0x10, addr, 1, buf, 10);
            if (status == status_success) {
                printf("HPM_I2C0 read 10 bytes from device address 0x10 success.\n");
            }

    - 注意：地址长度和数据长度不能超过 `I2C_SOC_TRANSFER_COUNT_MAX` 字节。

  - 自定义序列传输API：

    .. code-block:: c

        hpm_stat_t i2c_master_transfer(I2C_Type *ptr, const uint16_t device_address, uint8_t *buf, const uint32_t size,  uint16_t flags);

    - 参数说明：

      .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - I2C_Type*
                - 指向I2C控制器基地址的指针
              * - device_address
                - uint16_t
                - 设备地址
              * - buf
                - uint8_t*
                - 数据缓冲区指针
              * - size
                - uint32_t
                - 数据长度
              * - flags
                - uint16_t
                - 传输标志位

    - 返回值：

      .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 传输成功
              * - status_invalid_argument
                - 无效参数
              * - status_timeout
                - 传输超时
              * - status_i2c_transmit_not_completed
                - 传输未完成
              * - status_i2c_no_addr_hit
                - 未命中地址（总线上并无此从机设备地址）
              * - status_fail
                - 传输失败

    - flags 传输标志位：相关宏定义可在 `hpm_i2c_drv.h` 中查看

      .. list-table::
              :header-rows: 1

              * - 标志位
                - 描述
                - 备注
              * - I2C_WR
                - 写入数据
                - 此标志位不可与 I2C_RD 同时设置
              * - I2C_RD
                - 读取数据
                - 此标志位不可与 I2C_WR 同时设置
              * - I2C_ADDR_10BIT
                - 10位地址
                - 无
              * - I2C_NO_START
                - 无开始信号
                - 无
              * - I2C_NO_ADDRESS
                - 无地址
                - 无
              * - I2C_NO_READ_ACK
                - 无应答
                - 无
              * - I2C_NO_STOP
                - 无停止信号
                - 无
              * - I2C_WRITE_CHECK_ACK
                - 写入数据时检查应答
                - 无

    - 示例：

      - 在一些传感器中的repeated Start操作中，需要先写入一个命令，然后再读取数据，此时可以使用自定义序列传输API。

        .. code-block:: c

            /* 写入命令 */
            uint8_t cmd = 0x01;
            /* 写入数据, 不发stop信号，保持总线占用 */
            hpm_stat_t status = i2c_master_transfer(HPM_I2C0, 0x10, &cmd, 1, I2C_WR | I2C_NO_STOP);
            if (status == status_success) {
                printf("HPM_I2C0 write 1 byte to device address 0x10 success.\n");
            }
            /* 读取数据 */
            uint8_t buf[10];
            status = i2c_master_transfer(HPM_I2C0, 0x10, buf, 10, I2C_RD);
            if (status == status_success) {
                printf("HPM_I2C0 read 10 bytes from device address 0x10 success.\n");
            }


从机模式
""""""""""""

- 读写API

  .. code-block:: c

        hpm_stat_t i2c_slave_write(I2C_Type *ptr, uint8_t *buf, const uint32_t size);
        hpm_stat_t i2c_slave_read(I2C_Type *ptr, uint8_t *buf, const uint32_t size);

  - 参数说明：读写API的参数一致，具体如下：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - I2C_Type*
                - 指向I2C控制器基地址的指针
              * - buf
                - uint8_t*
                - 数据缓冲区指针
              * - size
                - uint32_t
                - 数据长度

  - 返回值：

    .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 传输成功
              * - status_invalid_argument
                - 无效参数
              * - status_timeout
                - 传输超时
              * - status_i2c_transmit_not_completed
                - 传输未完成
              * - status_fail
                - 传输失败

  - 示例：

    - 从机向主机写入10个字节的数据

      .. code-block:: c

            uint8_t buf[10];

            /* 等待主机写入数据 */
            do {
                stat = i2c_slave_write(TEST_I2C, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
            } while (stat == status_fail);

    - 从机从主机读取10个字节的数据

      .. code-block:: c

            uint8_t buf[10];

            /* 等待主机读取数据 */
            do {
                stat = i2c_slave_read(TEST_I2C, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
            } while (stat == status_fail);


启动DMA传输
^^^^^^^^^^^^^^^^^^^

- 此部分不负责DMA传输的相关参数（DMA通道，DMA源地址和设备地址等等设置），只负责I2C DMA传输的启动相关设置。
- 具体可参考 **I2C示例** 中的DMA示例。
- 每次传输完成后，需要清除 `CMPL` 标志位，否则会影响下一次DMA传输。可使用 `i2c_clear_status` API清除 `CMPL` 标志位。
- 需要DMA收发的API，可以查看I2C组件文档 :ref:`i2c_components <i2c_component>`


主机模式
""""""""""""

- 读写DMA启动API：

  .. code-block:: c

        hpm_stat_t i2c_master_start_dma_write(I2C_Type *i2c_ptr, const uint16_t device_address, uint32_t size);
        hpm_stat_t i2c_master_start_dma_read(I2C_Type *i2c_ptr, const uint16_t device_address, uint32_t size);

  - 参数说明：读写API的参数一致，具体如下：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - i2c_ptr
                - I2C_Type*
                - 指向I2C控制器基地址的指针
              * - device_address
                - uint16_t
                - 设备地址
              * - size
                - uint32_t
                - 数据长度
  - 返回值：

    .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 设置成功
              * - status_fail
                - 设置失败
              * - status_invalid_argument
                - 无效参数
              * - status_timeout
                - 设置超时

  - 注意：每次DMA传输前，需要先调用此API设置DMA传输参数。具体可参考 **I2C示例** 中的DMA示例。

  - 示例：

    - 主机向从机地址为0x10的设备写入10个字节的数据

      .. code-block:: c

            uint8_t buf[10];
            hpm_stat_t status = i2c_master_start_dma_write(HPM_I2C0, 0x10, 10);
            if (status == status_success) {
                printf("HPM_I2C0 dma write config success.\n");
            } else {
                assert(0);
            }
            /* 配置DMA的通道，DMA源地址和设备地址等等，启动DMA传输，不做举例，参考 `I2C示例`的dma相关 */

    - 主机从从机地址为0x10的设备读取10个字节的数据

      .. code-block:: c

            uint8_t buf[10];
            hpm_stat_t status = i2c_master_start_dma_read(HPM_I2C0, 0x10, 10);
            if (status == status_success) {
                printf("HPM_I2C0 dma read config success.\n");
            } else {
                assert(0);
            }
            /* 配置DMA的通道，DMA源地址和设备地址等等，启动DMA传输，不做举例，参考 `I2C示例`的dma相关 */

从机模式
""""""""""""

- 启动DMA传输API：

  .. code-block:: c

        hpm_stat_t i2c_slave_dma_transfer(I2C_Type *i2c_ptr, uint32_t size);

  - 参数说明：

     .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - i2c_ptr
                - I2C_Type*
                - 指向I2C控制器基地址的指针
              * - size
                - uint32_t
                - 数据长度

  - 返回值：

    .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 设置成功
              * - status_fail
                - 设置失败
              * - status_invalid_argument
                - 无效参数
              * - status_timeout
                - 设置超时

  - 注意：

    - 配置DMA参数后，需要先调用此API设置DMA传输参数。具体可参考 **I2C示例** 中的DMA示例。
    - 等待地址命中，再读取传输方向，若是读取方向，则DMA配置比如源地址为I2C数据寄存器地址等，若是写入方向，则DMA配置比如源地址为I2C数据寄存器地址等。
    - 等待地址命中可以使用地址命中中断，也可以使用轮询方式等待地址命中。
    - 轮询方式可以使用 `i2c_get_status` API获取地址命中状态。
    - 读取传输方向可以使用 `i2c_is_writing` 和 `i2c_is_reading` API获取传输方向。

  - 示例：

    - 从机轮询向主机写入10个字节的数据

     .. code-block:: c

            uint8_t buf[10];
            /* 等待地址命中 */
            while (!(i2c_get_status(HPM_I2C0) & I2C_STATUS_ADDRHIT_MASK)) {
            }
            /* 读取方向 */
            if (i2c_is_writing(HPM_I2C0)) {
                /* 在从机模式，为发送写入方向 */
                /* 配置DMA的通道，DMA源地址和设备地址等等，启动DMA传输，不做举例，参考 `I2C示例`的dma相关 */
            } else {
                assert(0);
            }
            hpm_stat_t status = i2c_slave_dma_transfer(HPM_I2C0, 10);
            if (status == status_success) {
                printf("i2c slave dma transfer success.\n");
            } else {
                printf("i2c slave dma transfer failed.\n");
                assert(0);
            }
            /* 等待I2C传输完成 */
            do {
                status = i2c_get_status(ptr);
            } while (!(status & I2C_STATUS_CMPL_MASK));
            /* 清除CMPL标志位 避免影响下一次传输 */
            i2c_clear_status(ptr, status);

    - 从机轮询方式从主机读取10个字节的数据

    .. code-block:: c

            uint8_t buf[10];
            /* 等待地址命中 */
            while (!(i2c_get_status(HPM_I2C0) & I2C_STATUS_ADDRHIT_MASK)) {
            }
            /* 读取方向 */
            if (i2c_is_reading(HPM_I2C0)) {
                /* 在从机模式，为接收读取方向 */
                /* 配置DMA的通道，DMA源地址和设备地址等等，启动DMA传输，不做举例，参考 `I2C示例`的dma相关 */
            } else {
                assert(0);
            }
            hpm_stat_t status = i2c_slave_dma_transfer(HPM_I2C0, 10);
            if (status == status_success) {
                printf("i2c slave dma transfer success.\n");
            } else {
                printf("i2c slave dma transfer failed.\n");
                assert(0);
            }
            /* 等待I2C传输完成 */
            do {
                status = i2c_get_status(ptr);
            } while (!(status & I2C_STATUS_CMPL_MASK));
            /* 清除CMPL标志位 避免影响下一次传输 */
            i2c_clear_status(ptr, status);


I2C中断
--------------

- 参考 **I2C示例** 中的interrupt示例。
- 需要打开plic控制器对应的I2C中断，使用 `intc_m_enable_irq` API接口打开I2C中断。
- 需要在plic控制器中配置I2C中断的优先级，使用 `intc_m_enable_irq_with_priority` API接口配置I2C中断的优先级。
- 读取I2C中断使能，可以使用 `i2c_get_irq_setting` API接口读取I2C中断使能。
- 每次对应的中断事件发生，需要在中断处理函数中调用 `i2c_clear_status` API接口清除中断标志。
- 每次对应的中断事件发生，可以在中断处理函数中调用 `i2c_get_status` API接口获取中断标志。
- I2C外设支持以下中断，可以从 `hpm_i2c_drv.h` 宏定义中查看。

  .. list-table::
        :header-rows: 1

        * - 中断
          - 描述
        * - I2C_EVENT_TRANSACTION_COMPLETE
          - 传输完成
        * - I2C_EVENT_BYTE_RECEIVED
          - 接收字节
        * - I2C_EVENT_BYTE_TRANSMIT
          - 发送字节
        * - I2C_EVENT_START_CONDITION
          - 起始条件
        * - I2C_EVENT_STOP_CONDITION
          - 停止条件
        * - I2C_EVENT_LOSS_ARBITRATION
          - 仲裁丢失
        * - I2C_EVENT_ADDRESS_HIT
          - 地址命中
        * - I2C_EVENT_FIFO_HALF
          - FIFO半满
        * - I2C_EVENT_FIFO_FULL
          - FIFO满
        * - I2C_EVENT_FIFO_EMPTY
          - FIFO空

- 相关API接口：

  - 使能I2C中断：

    .. code-block:: c

        hpm_stat_t i2c_enable_irq(I2C_Type *ptr, uint32_t mask);

    - 参数说明：

      .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - I2C_Type*
                - 指向I2C控制器基地址的指针
              * - mask
                - uint32_t
                - 中断使能掩码，对应的中断掩码可以从 上述中断表格查看。

  - 禁能I2C中断：

    .. code-block:: c

        hpm_stat_t i2c_disable_irq(I2C_Type *ptr, uint32_t mask);

    - 参数说明：

      .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - I2C_Type*
                - 指向I2C控制器基地址的指针
              * - mask
                - uint32_t
                - 中断使能掩码，对应的中断掩码可以从 上述中断表格查看。


I2C示例
--------------

.. toctree::
   :maxdepth: 3

   dma/master/README_zh
   dma/slave/README_zh
   interrupt/master/README_zh
   interrupt/slave/README_zh
   polling/master/README_zh
   polling/slave/README_zh

