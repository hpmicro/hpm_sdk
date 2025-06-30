.. _i2s:

I2S
===

概述
----

- 主要介绍先楫I2S外设的主要驱动接口说明和调用方法，更多内容请参考 `hpm_i2s_drv.h` 的API说明以及相关用户手册。
- 支持多种音频传输协议：

  - I2S飞利浦标准协议
  - MSB对齐/左对齐协议
  - LSB对齐/右对齐协议
  - PCM协议

- 支持主从模式：

  - 主模式：生成MCLK、BCLK和FCLK时钟信号
  - 从模式：使用外部时钟源

- 支持多种音频格式：

  - 采样深度（audio_depth）：

    - 支持16位、24位、32位
    - 表示实际有效的音频数据位宽

  - 通道长度（channel_length）：

    - 支持16位、32位
    - 必须大于等于采样深度
    - 表示每个通道的总位宽

  - 通道模式：

    - 标准模式：支持单声道和立体声
    - TDM模式：最多支持8个通道（4对立体声）

- 支持多种采样频率：

  - 根据系统音频主时钟MCLK进行分频
  - 支持常用采样频率：192kHz、96kHz、48kHz、44.1kHz、32kHz、22.05kHz、16kHz、8kHz
  - 可通过配置时钟分频实现其他采样频率

- 支持DMA传输
- 支持多种中断：

  - FIFO阈值中断
  - FIFO满/空中断

I2S初始化
--------------

- 需要确保I2S的时钟源已经开启，并且初始化了相关I2S外设引脚。

  - 可使用 `clock_add_to_group` 函数用于将I2S时钟源添加到时钟组中，从而确保I2S时钟源已经开启。

- 相关结构体介绍：

  - `i2s_config_t` 结构体用于配置I2S的基本参数

    .. code-block:: c

        typedef struct i2s_config {
            bool invert_mclk_out;           /* MCLK输出反相使能 */
            bool invert_mclk_in;            /* MCLK输入反相使能 */
            bool use_external_mclk;         /* 使用外部MCLK */
            bool invert_bclk_out;           /* BCLK输出反相使能 */
            bool invert_bclk_in;            /* BCLK输入反相使能 */
            bool use_external_bclk;         /* 使用外部BCLK */
            bool invert_fclk_out;           /* FCLK输出反相使能 */
            bool invert_fclk_in;            /* FCLK输入反相使能 */
            bool use_external_fclk;         /* 使用外部FCLK */
            bool enable_mclk_out;           /* MCLK输出使能 */
            bool frame_start_at_rising_edge; /* 帧起始边沿选择 */
            uint16_t tx_fifo_threshold;     /* 发送FIFO阈值 */
            uint16_t rx_fifo_threshold;     /* 接收FIFO阈值 */
        } i2s_config_t;

  - `i2s_transfer_config_t` 结构体用于配置I2S的传输参数

    .. code-block:: c

        typedef struct i2s_transfer_config {
            uint32_t sample_rate;           /* 采样率 */
            bool enable_tdm_mode;           /* TDM模式使能 */
            uint8_t channel_num_per_frame;  /* 每帧通道数 */
            uint8_t channel_length;         /* 通道长度：16位或32位 */
            uint8_t audio_depth;            /* 音频深度：16位、24位、32位 */
            bool master_mode;               /* 主从模式选择 */
            uint8_t protocol;               /* 协议选择 */
            i2s_line_num_t data_line;       /* 数据线选择 */
            uint32_t channel_slot_mask;     /* 通道掩码 */
        } i2s_transfer_config_t;

    - `channel_num_per_frame` 参数说明：

      用于配置每帧包含的通道数：

      - 参数值必须为偶数
      - 非TDM模式下固定为2
      - TDM模式下可配置为2、4、6、8等
      - 具体哪个通道被使能需要通过 `channel_slot_mask` 参数配置

    - `channel_slot_mask` 参数说明：

      用于配置I2S通道槽位使能的掩码值，每个比特位代表一个通道槽位：

      - 非TDM模式下（channel_num_per_frame = 2）：

        - 单声道：设置为 0x1（只启用一个通道）
        - 立体声：设置为 0x3（启用两个通道）

      - TDM模式下（如： channel_num_per_frame == 8）：

        - 掩码的每个位对应一个通道
        - 示例：
          - 4通道：设置为 0xF（二进制1111）
          - 8通道：设置为 0xFF（二进制11111111）

主机模式初始化
^^^^^^^^^^^^^^^^^^^

- 初始化函数API：

  .. code-block:: c

        void i2s_init(I2S_Type *ptr, i2s_config_t *config);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - I2S_Type*
              - 指向I2S控制器基地址的指针
            * - config
              - i2s_config_t*
              - 指向I2S配置结构体的指针

- 传输配置函数API：

  .. code-block:: c

        hpm_stat_t i2s_config_tx(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config);
        hpm_stat_t i2s_config_rx(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config);
        hpm_stat_t i2s_config_transfer(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_transfer_config_t *config);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - I2S_Type*
              - 指向I2S控制器基地址的指针
            * - mclk_in_hz
              - uint32_t
              - MCLK时钟频率（Hz）
            * - config
              - i2s_transfer_config_t*
              - 指向I2S传输配置结构体的指针

  - 返回值：

    .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 配置成功
            * - status_invalid_argument
              - 无效参数

- 示例：

  .. code-block:: c

        /* I2S初始化，配置为主机模式 */
        i2s_config_t i2s_config;
        i2s_transfer_config_t transfer;
        uint32_t mclk_freq;

        /* 获取默认配置 */
        i2s_get_default_config(HPM_I2S0, &i2s_config);
        i2s_config.enable_mclk_out = true;
        i2s_init(HPM_I2S0, &i2s_config);

        /* 传输参数赋值 */
        i2s_get_default_transfer_config(&transfer);
        transfer.sample_rate = 48000;
        transfer.audio_depth = i2s_audio_depth_16_bits;
        transfer.channel_num_per_frame = 2;
        transfer.channel_length = i2s_channel_length_32_bits;
        transfer.master_mode = true;
        transfer.protocol = I2S_PROTOCOL_I2S_PHILIPS;
        transfer.data_line = I2S_DATA_LINE_0;
        transfer.channel_slot_mask = 0x3;

        /* 获取MCLK频率 */
        mclk_freq = clock_get_frequency(clock_i2s0);

        /* 配置传输参数，包括发送数据帧格式和分频等 */
        hpm_stat_t status = i2s_config_tx(HPM_I2S0, mclk_freq, &transfer);
        if (status != status_success) {
            printf("I2S TX config failed\n");
        }

从机模式初始化
^^^^^^^^^^^^^^^^^^^

- 传输配置函数API：

  .. code-block:: c

        hpm_stat_t i2s_config_tx_slave(I2S_Type *ptr, i2s_transfer_config_t *config);
        hpm_stat_t i2s_config_rx_slave(I2S_Type *ptr, i2s_transfer_config_t *config);
        hpm_stat_t i2s_config_transfer_slave(I2S_Type *ptr, i2s_transfer_config_t *config);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - I2S_Type*
              - 指向I2S控制器基地址的指针
            * - config
              - i2s_transfer_config_t*
              - 指向I2S传输配置结构体的指针

  - 返回值：

    .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 配置成功
            * - status_invalid_argument
              - 无效参数

- 示例：

  .. code-block:: c

        /* I2S初始化，配置为从机模式 */
        i2s_config_t i2s_config;
        i2s_transfer_config_t transfer;

        /* 获取默认配置 */
        i2s_get_default_config(HPM_I2S0, &i2s_config);
        i2s_config.use_external_mclk = true;
        i2s_config.use_external_bclk = true;
        i2s_config.use_external_fclk = true;
        i2s_init(HPM_I2S0, &i2s_config);

        /* 传输参数赋值 */
        i2s_get_default_transfer_config(&transfer);
        transfer.sample_rate = 48000;
        transfer.audio_depth = i2s_audio_depth_16_bits;
        transfer.channel_num_per_frame = 2;
        transfer.channel_length = i2s_channel_length_32_bits;
        transfer.master_mode = false;
        transfer.protocol = I2S_PROTOCOL_I2S_PHILIPS;
        transfer.data_line = I2S_DATA_LINE_0;
        transfer.channel_slot_mask = 0x3;

        /* 配置传输参数，包括发送数据帧格式等 */
        hpm_stat_t status = i2s_config_tx_slave(HPM_I2S0, &transfer);
        if (status != status_success) {
            printf("I2S TX slave config failed\n");
        }

多数据线传输配置
^^^^^^^^^^^^^^^^^^^

- 多数据线传输配置结构体：

  .. code-block:: c

        typedef struct i2s_multiline_transfer_config {
            uint32_t sample_rate;           /* 采样率 */
            bool enable_tdm_mode;           /* TDM模式使能 */
            uint8_t channel_num_per_frame;  /* 每帧通道数 */
            uint8_t channel_length;         /* 通道长度：16位或32位 */
            uint8_t audio_depth;            /* 音频深度：16位、24位、32位 */
            bool master_mode;               /* 主从模式选择 */
            uint8_t protocol;               /* 协议选择 */
            bool tx_data_line_en[4];        /* 发送数据线使能 */
            bool rx_data_line_en[4];        /* 接收数据线使能 */
            uint32_t tx_channel_slot_mask[4];/* 数据线发送通道槽位掩码 */
            uint32_t rx_channel_slot_mask[4];/* 数据线接收通道槽位掩码 */
        } i2s_multiline_transfer_config_t;

- 多数据线传输配置函数API：

  .. code-block:: c

        hpm_stat_t i2s_config_multiline_transfer(I2S_Type *ptr, uint32_t mclk_in_hz, i2s_multiline_transfer_config_t *config);

  - 参数说明：

    .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - I2S_Type*
              - 指向I2S控制器基地址的指针
            * - mclk_in_hz
              - uint32_t
              - MCLK时钟频率（Hz）
            * - config
              - i2s_multiline_transfer_config_t*
              - 指向I2S多通道传输配置结构体的指针

  - 返回值：

    .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 配置成功
            * - status_invalid_argument
              - 无效参数

- 示例：

  .. code-block:: c

        /* I2S多通道传输配置 */
        i2s_config_t i2s_config;
        i2s_multiline_transfer_config_t transfer;
        uint32_t mclk_freq;

        /* 获取默认配置 */
        i2s_get_default_config(HPM_I2S0, &i2s_config);
        i2s_config.enable_mclk_out = true;
        i2s_init(HPM_I2S0, &i2s_config);

        /* 配置多通道传输参数 */
        i2s_get_default_multiline_transfer_config(&transfer);
        transfer.sample_rate = audio_data.sample_rate;         /* 设置采样率 */
        transfer.channel_num_per_frame = 2;                    /* 每帧通道数 */
        transfer.audio_depth = audio_data.audio_depth;         /* 设置位深 */
        transfer.channel_length = i2s_channel_length_32_bits;  /* 通道长度 */
        transfer.master_mode = true;                           /* 主模式 */
        transfer.protocol = I2S_PROTOCOL_MSB_JUSTIFIED;        /* MSB对齐协议 */

        /* 配置数据线0-3的发送功能 */
        transfer.tx_data_line_en[0] = true;
        transfer.tx_data_line_en[1] = true;
        transfer.tx_data_line_en[2] = true;
        transfer.tx_data_line_en[3] = true;
        transfer.tx_channel_slot_mask[0] = 0x3;  /* 数据线0使用通道0-1 */
        transfer.tx_channel_slot_mask[1] = 0x3;  /* 数据线1使用通道0-1 */
        transfer.tx_channel_slot_mask[2] = 0x3;  /* 数据线2使用通道0-1 */
        transfer.tx_channel_slot_mask[3] = 0x3;  /* 数据线3使用通道0-1 */

        /* 获取MCLK频率 */
        mclk_freq = clock_get_frequency(clock_i2s0);

        /* 配置传输参数 */
        hpm_stat_t status = i2s_config_multiline_transfer(HPM_I2S0, mclk_freq, &transfer);
        if (status != status_success) {
            printf("I2S multiline transfer config failed\n");
        }

I2S数据传输
--------------

polling传输
^^^^^^^^^^^^^^^^^^^

- 发送数据API：

  .. code-block:: c

        void i2s_send_data(I2S_Type *ptr, i2s_line_num_t tx_line_index, uint32_t data);
        uint32_t i2s_send_buff(I2S_Type *ptr, i2s_line_num_t tx_line_index, uint8_t samplebits, uint8_t *src, uint32_t size);

- 接收数据API：

  .. code-block:: c

        void i2s_receive_data(I2S_Type *ptr, i2s_line_num_t rx_line_index, uint32_t *data);
        uint32_t i2s_receive_buff(I2S_Type *ptr, i2s_line_num_t rx_line_index, uint8_t samplebits, uint8_t *dst, uint32_t size);

- 示例：

  .. code-block:: c

        /* 发送单个数据 */
        uint32_t data = 0x12345678;
        i2s_send_data(HPM_I2S0, I2S_DATA_LINE_0, data);

        /* 发送缓冲区数据 */
        uint8_t tx_buff[32];
        uint32_t sent = i2s_send_buff(HPM_I2S0, I2S_DATA_LINE_0, i2s_audio_depth_16_bits, tx_buff, sizeof(tx_buff));
        if (sent != sizeof(tx_buff)) {
            printf("I2S TX failed\n");
        }

        /* 接收单个数据 */
        uint32_t rx_data;
        i2s_receive_data(HPM_I2S0, I2S_DATA_LINE_0, &rx_data);

        /* 接收缓冲区数据 */
        uint8_t rx_buff[32];
        uint32_t received = i2s_receive_buff(HPM_I2S0, I2S_DATA_LINE_0, i2s_audio_depth_16_bits, rx_buff, sizeof(rx_buff));
        if (received != sizeof(rx_buff)) {
            printf("I2S RX failed\n");
        }

DMA传输
^^^^^^^^^^^^^^^^^^^

- I2S支持DMA传输，可以通过以下步骤配置：

  1. 使能I2S的DMA请求
  2. 配置DMAMUX，选择I2S作为DMA请求源
  3. 配置DMA通道参数
  4. 启动DMA传输

- 相关API：

  .. code-block:: c

        static inline void i2s_enable_tx_dma_request(I2S_Type *ptr);
        static inline void i2s_enable_rx_dma_request(I2S_Type *ptr);
        static inline void i2s_disable_tx_dma_request(I2S_Type *ptr);
        static inline void i2s_disable_rx_dma_request(I2S_Type *ptr);

- 示例：

  .. code-block:: c

        /* 步骤1：I2S初始化，配置为主机模式 */
        i2s_config_t i2s_config;
        i2s_transfer_config_t transfer;
        uint32_t mclk_freq;

        /* 步骤2：获取并设置默认配置 */
        i2s_get_default_config(HPM_I2S0, &i2s_config);
        i2s_config.enable_mclk_out = true;          /* 使能MCLK输出 */
        i2s_init(HPM_I2S0, &i2s_config);

        /* 步骤3：配置传输参数 */
        i2s_get_default_transfer_config(&transfer);
        transfer.sample_rate = 48000;               /* 设置采样率为48kHz */
        transfer.audio_depth = i2s_audio_depth_16_bits;    /* 16位音频数据 */
        transfer.channel_num_per_frame = 2;         /* 2个通道（立体声） */
        transfer.channel_length = i2s_channel_length_32_bits;  /* 32位通道长度 */
        transfer.master_mode = true;                /* 主机模式 */
        transfer.protocol = I2S_PROTOCOL_I2S_PHILIPS;  /* 标准I2S协议 */
        transfer.data_line = I2S_DATA_LINE_0;      /* 使用数据线0 */
        transfer.channel_slot_mask = 0x3;          /* 启用两个通道 */

        /* 步骤4：获取MCLK频率 */
        mclk_freq = clock_get_frequency(clock_i2s0);

        /* 步骤5：配置并启动传输 */
        hpm_stat_t status = i2s_config_tx(HPM_I2S0, mclk_freq, &transfer);
        if (status != status_success) {
            printf("I2S TX config failed\n");
        }

        /* 步骤1：使能I2S DMA请求 */
        i2s_enable_tx_dma_request(HPM_I2S0);

        /* 步骤2：配置DMAMUX，选择I2S作为DMA请求源 */
        dmamux_config(HPM_DMAMUX, DMA_MUX_CHANNEL, I2S_DMA_REQ, true);

        /* 步骤3：配置DMA通道参数 */
        dma_channel_config_t config = {
            .src_width = DMA_TRANSFER_WIDTH_WORD,    /* 源数据宽度：32位 */
            .dst_width = DMA_TRANSFER_WIDTH_WORD,    /* 目标数据宽度：32位 */
            .src_addr = (uint32_t)tx_buff,          /* 源数据缓冲区地址 */
            .dst_addr = (uint32_t)&HPM_I2S0->TXD[0], /* I2S发送寄存器地址 */
            .src_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T,  /* 源突发传输大小：1 */
            .dst_burst_size = DMA_NUM_TRANSFER_PER_BURST_1T,  /* 目标突发传输大小：1 */
            .size_in_byte = sizeof(tx_buff)         /* 传输总字节数 */
        };
        dma_setup_channel(HPM_DMA, DMA_CHANNEL, &config, true);

        /* 步骤4：启动DMA传输 */
        dma_start_channel(HPM_DMA, DMA_CHANNEL);

I2S中断
--------------

- I2S支持以下中断：

  .. list-table::
        :header-rows: 1

        * - 中断类型
          - 描述
        * - TX_FIFO_THRESHOLD
          - 发送FIFO低于阈值中断
        * - RX_FIFO_THRESHOLD
          - 接收FIFO高于阈值中断
        * - TX_FIFO_EMPTY
          - 发送FIFO空中断
        * - RX_FIFO_FULL
          - 接收FIFO满中断

- 相关API：

  .. code-block:: c

        void i2s_enable_irq(I2S_Type *ptr, uint32_t mask);
        void i2s_disable_irq(I2S_Type *ptr, uint32_t mask);
        void i2s_clear_irq_status(I2S_Type *ptr, uint32_t mask);

- 示例：

  .. code-block:: c

        /* 使能发送FIFO阈值中断 */
        i2s_enable_irq(HPM_I2S0, i2s_tx_fifo_threshold_irq_mask);

        /* 配置中断优先级并使能中断 */
        intc_m_enable_irq_with_priority(IRQn_I2S0, 1);

        /* 中断处理函数 */
        void isr_i2s(void)
        {
            uint32_t status = i2s_get_irq_status(HPM_I2S0);
            if (status & i2s_tx_fifo_threshold_irq_mask) {
                /* 处理发送FIFO阈值中断 */
                i2s_clear_irq_status(HPM_I2S0, i2s_tx_fifo_threshold_irq_mask);
            }
        }

注意事项
--------------

- I2S数据位宽对齐规则：

  - I2S在传输不同数据位宽的数据时，始终保持有效数据位于32bit接口寄存器的高位
  - 当数据格式为16bit时：

    - 发送时：需要将16bit数据左移16bit后写入发送FIFO寄存器
    - 接收时：需要从接收FIFO寄存器读取数据右移16bit后得到16bit数据

  - 数据移位要求：

    - 使用 `i2s_send_data` 和 `i2s_receive_data` 函数时需要进行数据移位操作
    - 使用DMA方式传输时需要进行数据移位操作
    - 移位位数 = 32 - 实际数据位宽

.. toctree::
  :maxdepth: 3

  i2s/README_zh
  i2s_dma/README_zh
  i2s_interrupt/README_zh
  i2s_master/README_zh
  i2s_slave/README_zh

