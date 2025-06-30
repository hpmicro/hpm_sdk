.. _lin:

LIN
===

概述
----

- 主要介绍LIN外设的主要驱动接口说明和调用方法，更多内容请参考 `hpm_lin_drv.h` 的API说明以及相关用户手册。
- 支持LIN规范2.2A，兼容LIN 1.3
- 支持主从模式：

  - 主模式：可工作在1Kbps～20Kbps数据速率下
  - 从模式：支持数据速率的动态自动探测

- 支持LIN帧格式：

  - Header部分（由主机发送）：

    - Break Field：超过11bit的低电平，标识帧起始
    - Sync Field：固定为0x55，用于从机同步波特率
    - Protected Identifier：6bit ID值和2bit校验位

  - Response部分（主机或从机发送）：

    - 数据字段：0~8字节数据
    - 校验和字段：1字节校验和

- 支持多种功能：

  - 支持bit error的检测与告警
  - 支持checksum的生成与检测（经典和增强模式）
  - 支持sleep模式的进入与退出
  - 支持timeout error的探测与告警
  - 支持数据缓存高达8字节

LIN初始化
--------------

- 需要确保LIN的时钟源已经开启，并且初始化了相关LIN外设引脚。

  - 可使用 `clock_add_to_group` 函数将LIN时钟源添加到时钟组中。

- 相关结构体介绍：

  - `lin_timing_t` 结构体用于配置LIN的时序参数

    .. code-block:: c

        typedef struct {
            uint32_t src_freq_in_hz;        /* 源时钟频率（Hz） */
            uint32_t baudrate;              /* 波特率 */
        } lin_timing_t;

  - `lin_trans_config_t` 结构体用于配置LIN的传输参数

    .. code-block:: c

        typedef struct {
            uint8_t id;                     /* ID */
            uint8_t *data_buff;             /* 数据缓冲区 */
            bool data_length_from_id;       /* 数据长度是否从ID解码 */
            uint8_t data_length;            /* 数据长度（当data_length_from_id为false时使用） */
            bool enhanced_checksum;         /* true：增强校验和；false：经典校验和 */
            bool transmit;                  /* true：发送操作；false：接收操作 */
        } lin_trans_config_t;

主机模式
^^^^^^^^^^^^^^^^^^^

- 初始化函数API：

  .. code-block:: c

        hpm_stat_t lin_master_configure_timing(LIN_Type *ptr, lin_timing_t *timing);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - LIN_Type*
              - 指向LIN控制器基地址的指针
            * - timing
              - lin_timing_t*
              - 指向LIN时序配置结构体的指针

- 传输函数API：

  - 非阻塞传输API：

    .. code-block:: c

        void lin_master_transfer(LIN_Type *ptr, lin_trans_config_t *config);

    - 参数说明：

      .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - LIN_Type*
                - 指向LIN控制器基地址的指针
              * - config
                - lin_trans_config_t*
                - 指向LIN传输配置结构体的指针

    .. note::
        这是一个非阻塞API，调用后立即返回。需要通过中断来查询传输完成状态。

  - 阻塞传输API：

    .. code-block:: c

        hpm_stat_t lin_master_sent(LIN_Type *ptr, lin_trans_config_t *config);
        hpm_stat_t lin_master_receive(LIN_Type *ptr, lin_trans_config_t *config);

    这两个API是阻塞式的，会等待传输完成后才返回：

    - `lin_master_sent`：主机发送数据，直到发送完成或出错才返回
    - `lin_master_receive`：主机接收数据，直到接收完成或出错才返回

从机模式
^^^^^^^^^^^^^^^^^^^

- 初始化函数API：

  .. code-block:: c

        hpm_stat_t lin_slave_configure_timing(LIN_Type *ptr, uint32_t src_freq_in_hz);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - LIN_Type*
              - 指向LIN控制器基地址的指针
            * - src_freq_in_hz
              - uint32_t
              - 源时钟频率（Hz）

- 传输函数API：

  - 非阻塞传输API：

    .. code-block:: c

        void lin_slave_transfer(LIN_Type *ptr, lin_trans_config_t *config);

    - 参数说明：

      .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - LIN_Type*
                - 指向LIN控制器基地址的指针
              * - config
                - lin_trans_config_t*
                - 指向LIN传输配置结构体的指针

    .. note::
        这是一个非阻塞API，调用后立即返回。需要通过中断来查询传输完成状态。

  - 阻塞传输API：

    .. code-block:: c

        hpm_stat_t lin_slave_sent(LIN_Type *ptr, lin_trans_config_t *config);
        hpm_stat_t lin_slave_receive(LIN_Type *ptr, lin_trans_config_t *config);

    这两个API是阻塞式的，会等待传输完成后才返回：

    - `lin_slave_sent`：从机发送数据，直到发送完成或出错才返回
    - `lin_slave_receive`：从机接收数据，直到接收完成或出错才返回

休眠和唤醒
^^^^^^^^^^^^^^^^^^^

- 休眠模式：

  - 主机可以发送休眠请求帧使从机进入休眠模式
  - 从机在总线空闲4s~10s后自动进入休眠模式（可配置）
  - 通过 `lin_sleep` 函数使LIN控制器进入休眠模式

- 唤醒功能：

  - 任何LIN节点都可以通过发送唤醒信号唤醒总线
  - 使用 `lin_wakeup` 函数发送唤醒信号
  - 从机唤醒后等待主机帧头超时时间可配置（180ms~240ms）

示例
--------------

- 主机模式示例：

  .. code-block:: c

        /* LIN主机初始化 */
        lin_timing_t timing;
        lin_trans_config_t config;

        timing.src_freq_in_hz = clock_get_frequency(TEST_LIN_CLOCK_NAME);
        timing.baudrate = TEST_LIN_BAUDRATE;
        lin_master_configure_timing(TEST_LIN, &timing);

        /* 发送数据帧 */
        config.id = TEST_LIN_TRANSMIT_ID;
        config.transmit = true;
        config.data_length = 8;
        config.enhanced_checksum = true;
        config.data_buff = sent_buff;
        lin_master_transfer(TEST_LIN, &config);

- 从机模式示例：

  .. code-block:: c

        /* LIN从机初始化 */
        uint32_t freq = clock_get_frequency(TEST_LIN_CLOCK_NAME);
        lin_slave_configure_timing(TEST_LIN, freq);

        /* 中断中响应主机请求 */
        if (status & LIN_STATE_DATA_REQ_MASK) {
            id = lin_get_id(TEST_LIN);
            config.id = id;
            config.transmit = true;
            config.data_length_from_id = true;
            config.enhanced_checksum = true;
            config.data_buff = sent_buff;
            lin_slave_transfer(TEST_LIN, &config);
        }

.. toctree::
   :maxdepth: 3

   master/README_zh
   slave/README_zh

