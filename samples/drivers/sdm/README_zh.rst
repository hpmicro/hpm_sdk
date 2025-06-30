.. _sdm:

SDM
======

概述
---------

SDM示例工程演示了SDM外设的对Sigma-Delta调制器输出的数据流进行数据采样和幅值检测的功能。

主要特性
--------

- 信号输入接口：

  - 4组独立的CLK和DAT信号输入
  - 每组CLK/DAT可独立配置采样模式
  - 支持多种采样模式：

    - Mode 0: MCLK上升沿采样
    - Mode 1: MCLK上升沿和下降沿采样
    - Mode 2: Manchester编码模式
    - Mode 3: MCLK下降沿采样
    - Mode 4: 每2个MCLK上升沿采样
    - Mode 5: 每2个MCLK下降沿采样

- 数据滤波器(PCM_CIC)：

  - 支持4种滤波器类型：

    - Sinc1 (1阶)
    - Sinc2 (2阶)
    - Sinc3 (3阶)
    - SincFast (2阶快速)

  - 可配置过采样率(1-256)
  - 内置16深度32bit宽度FIFO
  - 支持同步采样
  - 支持掩码控制（指定版本）
  - 可选时间戳输出功能（指定版本）

- 幅值检测器(AMP_CIC)：

  - 同样支持4种滤波器类型
  - 独立的幅值监测通道
  - 支持高/低门限检测
  - 支持过零检测
  - 可配置过采样率(1-32)
  - 实时幅值输出

- 中断支持：

  - FIFO阈值中断
  - 数据溢出中断
  - 数据饱和中断
  - 幅值超限中断

- 同步功能：

  - 支持TRGMUX触发输入
  - 支持TRGMUX触发输出

SDM初始化
---------

- 相关结构体介绍：

  - `sdm_control_t` 结构体用于配置SDM的基本参数：

    .. code-block:: c

        typedef struct {
            uint8_t clk_signal_sync;    /* 时钟信号同步配置 */
            uint8_t data_signal_sync;   /* 数据信号同步配置 */
            bool interrupt_en;          /* 中断使能 */
        } sdm_control_t;

  - `sdm_channel_common_config_t` 结构体用于配置通道公共参数：

    .. code-block:: c

        typedef struct {
            uint8_t sampling_mode;      /* 采样模式 */
            bool enable_err_interrupt;  /* 错误中断使能 */
            bool enable_data_ready_interrupt; /* 数据就绪中断使能 */
        } sdm_channel_common_config_t;

  - `sdm_filter_config_t` 结构体用于配置数据滤波器参数：

    .. code-block:: c

        typedef struct {
            uint8_t fifo_threshold;     /* FIFO阈值 */
            uint8_t filter_type;        /* 滤波器类型 */
            uint16_t oversampling_rate; /* 过采样率 */
            bool output_32bit;          /* 32位输出使能 */
            uint8_t sync_source;        /* 同步源选择 */
            /* 其他参数省略 */
        } sdm_filter_config_t;

数据滤波器配置
--------------

- 初始化函数API：

  .. code-block:: c

    void sdm_init_module(SDM_Type *ptr, sdm_control_t *control);
    void sdm_config_channel_common_setting(SDM_Type *ptr, uint8_t ch_index, sdm_channel_common_config_t *config);
    void sdm_config_channel_filter(SDM_Type *ptr, uint8_t ch_index, sdm_filter_config_t *filter_config);

- 示例：

  .. code-block:: c

    /* SDM基本初始化 */
    sdm_control_t control = {
        .clk_signal_sync = 0xf,
        .data_signal_sync = 0xf,
        .interrupt_en = true
    };
    sdm_init_module(HPM_SDM, &control);

    /* 配置通道采样模式 */
    sdm_channel_common_config_t ch_config = {
        .sampling_mode = sdm_sampling_rising_clk_edge,
        .enable_err_interrupt = true,
        .enable_data_ready_interrupt = true
    };
    sdm_config_channel_common_setting(HPM_SDM, channel, &ch_config);

    /* 配置数据滤波器 */
    sdm_filter_config_t filter_config;
    sdm_get_channel_default_filter_config(HPM_SDM, &filter_config);
    filter_config.filter_type = sdm_filter_sinc3;
    filter_config.oversampling_rate = 256;
    sdm_config_channel_filter(HPM_SDM, channel, &filter_config);

幅值检测器配置
--------------

- 配置函数API：

  .. code-block:: c

    void sdm_config_channel_comparator(SDM_Type *ptr, uint8_t ch_index, sdm_comparator_config_t *cmp_config);

- 示例：

  .. code-block:: c

    /* 配置幅值检测器 */
    sdm_comparator_config_t cmp_config;
    sdm_get_channel_default_comparator_config(HPM_SDM, &cmp_config);
    cmp_config.high_threshold = threshold;
    cmp_config.en_high_threshold_int = true;
    cmp_config.filter_type = sdm_filter_sinc3;
    sdm_config_channel_comparator(HPM_SDM, channel, &cmp_config);

数据传输
--------

- 数据读取API：

  .. code-block:: c

    int32_t sdm_get_channel_fifo_data(SDM_Type *ptr, uint8_t ch);
    int16_t sdm_get_channel_fifo_16bit_data(SDM_Type *ptr, uint8_t ch);
    uint16_t sdm_get_channel_comparator_data(SDM_Type *ptr, uint8_t ch);

- 示例：

  .. code-block:: c

    /* 读取32位滤波数据 */
    int32_t data = sdm_get_channel_fifo_data(HPM_SDM, channel);

    /* 读取16位滤波数据 */
    int16_t data = sdm_get_channel_fifo_16bit_data(HPM_SDM, channel);

    /* 读取幅值检测结果 */
    uint16_t amplitude = sdm_get_channel_comparator_data(HPM_SDM, channel);

同步采样
--------

- 同步采样配置：

  1. 选择同步信号源
  2. 配置TRGM模块连接
  3. 使能同步功能

- 示例：

  .. code-block:: c

    /* 配置同步采样 */
    filter_config.sync_source = sync_src;
    filter_config.fifo_clean_on_sync = 1;
    filter_config.wtsyncen = 1;
    filter_config.wtsynaclr = 1;
    sdm_config_channel_filter(HPM_SDM, channel, &filter_config);

    /* 配置TRGM连接 */
    trgm_output_update_source(TRGM, output_index, input_src);

示例工作流程
------------

本示例提供了四种工作模式演示：

1. 轮询模式采样
   配置步骤：

   - 初始化SDM模块，配置时钟和数据同步
   - 配置通道采样模式为上升沿采样
   - 配置数据滤波器：

     * 选择Sinc3滤波器类型
     * 设置过采样率为256
     * 配置32位数据输出

   - 使能SDM通道
   - 轮询方式读取数据：

     * 等待数据就绪标志
     * 从FIFO读取采样数据
     * 将数据转换为电压值显示

2. 幅值检测模式

   SDM使用幅值监测通道监测电压超限事件。设置幅值检测的高电压阈值为SDM_AMPLITUDE_HIGH_THRESHOLD，当输入模拟电压由小于阈值的情况增大到超出阈值时，触发错误中断。

   配置步骤：

   - 初始化SDM模块，配置时钟和数据同步和使能总中断功能
   - 配置通道采样模式为上升沿采样和使能错误中断(赋值超限)
   - 配置幅值检测器：

     * 选择Sinc3滤波器类型
     * 设置过采样率
     * 配置高电压阈值（240mV）
     * 使能高电压阈值中断

   - 使能SDM通道
   - 等待幅值超限中断：

     * 中断处理中读取幅值
     * 显示超限电压值
     * 停止采样并清除中断标志

3. 中断模式采样
   配置步骤：

   - 初始化SDM模块，配置时钟和数据同步和使能总中断功能
   - 配置通道：

     * 采样模式为上升沿采样
     * 使能错误中断和数据就绪中断

   - 配置数据滤波器：

     * 选择Sinc3滤波器类型
     * 设置过采样率为256
     * 使能FIFO功能和阈值

   - 使能SDM通道
   - 中断处理流程：

     * FIFO达到阈值触发中断
     * 读取FIFO中的数据
     * 达到指定数据量后停止采样

4. 同步信号采样模式

   本例中使用GPTMR输出通过TRGMMUX配置连接到SDM的同步输入信号，当GPTMR的输出信号上升沿时触发SDM的同步事件。
   SDM会根据同步信号进行采样，当同步事件发生时，SDM开始采样数据进入FIFO；当FIFO中的数据达到预设的阈值时，SDM会触发数据有效中断去读取数据，并且硬件会清除同步事件标志，停止继续采样直到下一个同步事件发生。

   配置步骤：

   - 初始化SDM模块，配置时钟和数据同步和使能总中断功能
   - 配置SDM通道：

     * 采样模式为上升沿采样
     * 使能错误中断和数据就绪中断

   - 配置数据滤波器：

     * 选择Sinc3滤波器类型
     * 设置过采样率为256
     * 使能FIFO功能和阈值
     * 选择同步信号源
     * 使能同步采样功能
     * 使能FIFO自动清除
     * 使能数据就绪自动清除同步标志

   - 使能SDM通道
   - 配置GPTMR产生同步信号：

     * 设置4秒周期
     * 配置50%占空比

   - 配置TRGM模块：

     * 将GPTMR输出连接到SDM同步输入

   - 同步采样流程：

     * 等待同步事件触发
     * FIFO达到阈值后读取数据
     * 自动清除同步标志
     * 等待下一次同步事件

硬件设置
-----------

请参考具体开发板 :ref:`引脚描述 <board_resource>` 部分的描述。
根据开发板是否板载AD采样芯片，采用不同的接线方式：

- 无板载AD采样芯片：

  - 需外接采样芯片
  - 将采样芯片的CLK和DAT引脚连接到板上SDM的CLK和DAT引脚

- 有板载AD采样芯片：

  - 直接将模拟信号接入采样芯片输入接口即可

注意事项
-----------

1. 本示例中提供了适用于AD7400、NSI1306等Sigma-Delta调制器(Full-scale ±320 mV)的输入电压与输出数值的转换公式， 如使用不同类型Sigma-Delta调制器请检查转换公式。

2. 同步采样时需注意：

   - 同步事件后的前几个采样可能不准确，建议忽略
   - 不同滤波器类型需忽略的采样点数不同：

     * Sinc1: 0个
     * Sinc2: 1个
     * Sinc3: 2个
     * SincFast: 2个

3. 16位数据输出模式下需要注意：

   - 需要配置合适的移位值以避免数据溢出
   - 不同滤波器类型和过采样率下的输出范围不同

运行现象
-----------

运行工程后，通过串口可以看到菜单界面，输入相应数字可以执行不同的测试功能：

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

每种模式下会显示相应的测试结果，包括采样电压值或检测到的超限情况。

1. SDM sample data in polling mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SDM轮询数据有效标志并从FIFO中读出采样数据。

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

SDM使用幅值监测通道监测电压超限事件。设置幅值检测的高电压阈值为SDM_AMPLITUDE_HIGH_THRESHOLD，当输入模拟电压由小于阈值的情况增大到超出阈值时，触发错误中断。

.. code-block:: console

   Selected option: 2

   Set amplitude high threshold 240mV
   sdm channel comparator detected amplitude over the high threshold
   voltage: 253.75mV


3. SDM sample data in interrupt mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SDM使用FIFO的数据有效标志进行采样，当FIFO中的数据达到预设的阈值时，SDM会触发数据有效中断，中断中读取数据，当读取的数据数目达到TEST_DATA_COUNT时停止继续采样。

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

本例中使用GPTMR输出通过TRGMMUX配置连接到SDM的同步输入信号，当GPTMR的输出信号上升沿时触发SDM的同步事件。
SDM会根据同步信号进行采样，当同步事件发生时，SDM开始采样数据进入FIFO；当FIFO中的数据达到预设的阈值时，SDM会触发数据有效中断去读取数据，并且硬件会清除同步事件标志，停止继续采样直到下一个同步事件发生。

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

