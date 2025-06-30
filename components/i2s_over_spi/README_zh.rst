.. _i2s_over_spi:

I2S emulation
=============

概述
-----

- i2s_over_spi 组件提供了一套SPI模拟I2S的操作API，用于配置、初始化和读写I2S总线。支持多种I2S设备，并提供了阻塞和非阻塞模式下的数据传输功能。以下是该组件的主要特征：

  - 多实例支持
    - 支持多个SPI模拟I2S实例（如I2S0, I2S1等），每个实例都有独立的配置参数。
    - 通过``hpm_i2s_over_spi_t``结构体管理每个I2S实例的配置信息，比如bclk位时钟,lrck声道时钟,mclk主时钟的模拟生成，I2S数据模拟生成等。
  - 阻塞与非阻塞读写操作接口

    - 仅支持主机。
    - 提供了非阻塞式的读写函数。
    - 提供了阻塞式的读写函数，支持超时机制，确保在长时间未响应的情况下能够及时返回错误状态。

相关结构体介绍
---------------------

- 结构体介绍

  - gptmr定时器配置结构体，这个结构体用于用于I2S时钟生成, 封装了生成I2S协议所需时钟信号的关键硬件资源,比如

    - bclk（位时钟）：控制数据位的采样频率
    - lrck（帧时钟）：决定左右声道的切换频率
    - mclk（主时钟）：提供工作基准时钟

    .. code-block:: c

        typedef struct {
            GPTMR_Type *ptr;          /**< GPTMR硬件寄存器指针 */
            clock_name_t clock_name;  /**< 时钟源选择（如clock_mclk）*/
            uint8_t channel;          /**< 定时器通道编号（0或1）*/
        } hpm_i2s_gptmr_context_t;

  - SPI从机上下文配置结构体，这个结构体用于I2S数据通道模拟，封装了通过SPI接口实现I2S数据传输所需的硬件资源

    .. code-block:: c

        typedef struct {
            SPI_Type *ptr;          /**< SPI硬件寄存器指针 */
            clock_name_t clock_name; /**< 时钟源选择 */
            uint16_t txdma_src;     /**< TX DMA请求源编号 */
            uint16_t rxdma_src;     /**< RX DMA请求源编号 */
            uint32_t cs_pin;        /**< 片选引脚编号 */
            void (*write_cs)(uint32_t cs_pin, uint8_t state); /**< 片选控制回调函数 */
        } hpm_i2s_spi_context_t;

  - DMA上下文配置结构体，这个结构体用于I2S数据传输管理，封装了DMA链式传输所需的硬件资源，支持多段式连续传输

    .. code-block:: c

        typedef struct {
            dma_resource_t *resource;           /**< DMA通道资源指针 */
            dma_linked_descriptor_t *descriptors; /**< 链式描述符数组指针 */
        } hpm_i2s_dma_context_t;

  - I2S-over-SPI上下文结构体，集成管理通过SPI模拟I2S协议所需的全部硬件资源和状态信息。

    .. code-block:: c

        typedef struct hpm_i2s_over_spi {
            /* 时钟生成模块 */
            hpm_i2s_gptmr_context_t bclk;   /**< 位时钟生成器（控制数据位速率）*/
            hpm_i2s_gptmr_context_t lrck;   /**< 声道时钟生成器（控制采样率）*/
            hpm_i2s_gptmr_context_t mclk;   /**< 主时钟生成器（驱动编解码器）*/

            /* 数据传输模块 */
            hpm_i2s_spi_context_t spi_slave;/**< SPI从机配置（承载I2S数据流）*/
            hpm_i2s_dma_context_t tx_dma;   /**< 发送DMA资源（管理音频输出）*/
            hpm_i2s_dma_context_t rx_dma;    /**< 接收DMA资源（管理音频输入）*/

            /* 状态控制模块 */
            bool i2s_rx;                    /**< 接收使能标志 */
            i2s_rx_data_tc rx_callback;      /**< 接收完成回调函数指针 */
            hpm_i2s_gptmr_context_t transfer_time; /**< 传输计时器（记录传输耗时）*/
            bool has_done;                   /**< 传输完成标志 */
            void (*transfer_complete)(struct hpm_i2s_over_spi *i2s); /**< 全局传输完成回调 */
        } hpm_i2s_over_spi_t;

API调用流程介绍
----------------

定义I2S-over-SPI实例变量
^^^^^^^^^^^^^^^^^^^^^^^^^^^

- 定义一个 ``hpm_i2s_over_spi_t`` 结构体的实例。例如，在samples/i2s_emulation示例中使用的 ``i2s_device`` 实例变量。


初始化I2S-over-SPI实例变量
^^^^^^^^^^^^^^^^^^^^^^^^^^

- 将所需的硬件资源，比如定时器，SPI，DMA等，复制到 ``hpm_i2s_over_spi_t`` 结构体的成员变量中,使之进行绑定，建立I2S-over-SPI硬件模拟系统。

- **举例**: 在samples/i2s_emulation示例中， ``i2s_emulation_config`` API就是对 ``i2s_device`` 进行硬件绑定操作, 绑定具体硬件资源到 ``i2s_device`` 实例变量中

    .. code-block:: c

        static void i2s_emulation_config(hpm_i2s_over_spi_t *i2s)
        {
            /*------------------------ 时钟系统配置 ------------------------*/
            /* 主时钟生成器配置 */
            i2s->mclk.ptr = BOARD_GPTMR_I2S_MCLK;          // 使用GPTMR定时器生成MCLK
            i2s->mclk.channel = BOARD_GPTMR_I2S_MCLK_CHANNEL; // 使用通道0/1
            i2s->mclk.clock_name = BOARD_GPTMR_I2S_MCLK_CLK_NAME; // 时钟源选择

            /* 声道时钟配置（采样率控制）*/
            i2s->lrck.ptr = BOARD_GPTMR_I2S_LRCK;          // 使用GPTMR定时器生成LRCK
            i2s->lrck.channel = BOARD_GPTMR_I2S_LRCK_CHANNEL; // 通道配置
            i2s->lrck.clock_name = BOARD_GPTMR_I2S_LRCK_CLK_NAME;

            /* 位时钟配置（数据位速率）*/
            i2s->bclk.ptr = BOARD_GPTMR_I2S_BCLK;          // 使用GPTMR定时器生成BCLK
            i2s->bclk.channel = BOARD_GPTMR_I2S_BLCK_CHANNEL;
            i2s->bclk.clock_name = BOARD_GPTMR_I2S_BLCK_CLK_NAME;

            /*------------------------ 传输计时器配置 ----------------------*/
            i2s->transfer_time.ptr = BOARD_GPTMR_I2S_FINSH; // 使用GPTMR定时器作为传输计时器
            i2s->transfer_time.channel = BOARD_GPTMR_I2S_FINSH_CHANNEL;
            i2s->transfer_time.clock_name = BOARD_GPTMR_I2S_FINSH_CLK_NAME;
            /* 使能传输完成中断，优先级设为5 */
            intc_m_enable_irq_with_priority(BOARD_GPTMR_I2S_FINSH_IRQ, 5);

            /*------------------------ SPI数据通道配置 ---------------------*/
            i2s->spi_slave.ptr = BOARD_APP_SPI_BASE;        // 使用SPI作为数据通道
            i2s->spi_slave.clock_name = BOARD_APP_SPI_CLK_NAME;
            i2s->spi_slave.cs_pin = I2S_OVER_SPI_CS_CONTROLLER; // 片选引脚编号
            i2s->spi_slave.write_cs = board_write_spi_cs;  // 片选控制函数指针
            /* DMA请求源配置 */
            i2s->spi_slave.rxdma_src = BOARD_APP_SPI_RX_DMA; // 接收DMA请求源
            i2s->spi_slave.txdma_src = BOARD_APP_SPI_TX_DMA; // 发送DMA请求源

            /*------------------------ DMA资源配置 ------------------------*/
            i2s->rx_callback = rx_callback;                // 注册接收完成回调
            /* 接收DMA描述符数组（内存需8字节对齐）*/
            i2s->rx_dma.descriptors = rx_descriptors;       // 预分配描述符内存空间，需要链式传输
            i2s->rx_dma.resource = &dma_resource_pools[0]; // 使用DMA通道0资源
            /* 发送DMA配置*/
            i2s->tx_dma.descriptors = NULL;                // 发送描述符暂未使用，不需要链式传输
            i2s->tx_dma.resource = &dma_resource_pools[1]; // 预留DMA通道1资源

            /*------------------------ 状态初始化 -------------------------*/
            i2s->has_done = false;                        // 传输完成标志复位
            i2s->i2s_rx = false;                          // 初始化为发送模式
        }

I2S-over-SPI初始化
^^^^^^^^^^^^^^^^^^^^

- 调用 ``hpm_i2s_master_over_spi_init`` API 来初始化 I2S-over-SPI。在此过程中，``i2s_device`` 实例变量的参数会被赋值到API的``i2s``并生效。

- ``hpm_i2s_master_over_spi_init`` API原型：

    .. code-block:: c

        hpm_stat_t hpm_i2s_master_over_spi_init(hpm_i2s_over_spi_t *i2s)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - i2s
             - hpm_i2s_over_spi_t*
             - 指向I2S-over-SPI上下文结构体的指针，包含I2S配置信息和状态

    - 返回值：

      - ``status_success``: 成功
      - ``status_invalid_argument``: 无效参数
      - ``status_fail``: 其他错误

- **举例**: 如何使用 ``hpm_i2s_master_over_spi_init`` 函数来初始化 I2S-over-SPI

   .. code-block:: c

        hpm_i2s_master_over_spi_init(&i2s_device);

I2S-over-SPI接收操作
^^^^^^^^^^^^^^^^^^^^^^^^

接收配置
""""""""""""""

- 调用 ``hpm_i2s_master_over_spi_rx_config`` API 配置I2S接收参数。

  - 配置I2S接收参数，包括协议类型、采样率、音频位深度、接收缓冲区等。
  - 配置完成后，I2S-over-SPI会根据配置信息进行初始化和准备接收操作。
  - 该API支持单缓存区和双缓冲区模式。
  - 双缓冲模式下，I2S-over-SPI会使用两个缓冲区交替接收数据，确保数据的连续性和完整性。
  - 双缓冲模式下，在上述的rx_callback回调函数中，需要判断当前接收的是哪个缓冲区的数据，然后进行相应的处理。

- ``hpm_i2s_master_over_spi_rx_config`` API原型：

    .. code-block:: c

        hpm_stat_t hpm_i2s_master_over_spi_rx_config(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz,
                                                uint32_t audio_depth, uint8_t *buffer0, uint8_t *buffer1, uint32_t size);

    - 参数说明：

        .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - i2s
              - hpm_i2s_over_spi_t*
              - I2S-over-SPI上下文结构体指针
            * - protocol
              - uint8_t
              - 协议类型，当前不支持：``I2S_PROTOCOL_PCM`` 和 ``I2S_PROTOCOL_I2S_PHILIPS``
            * - lrck_hz
              - uint32_t
              - 声道时钟频率（采样率），单位Hz 比如 8000/48000
            * - audio_depth
              - uint32_t
              - 音频位深度，当前支持：(16bit)和(32bit)
            * - buffer0
              - uint8_t*
              - 接收缓冲区0指针
            * - buffer1
              - uint8_t*
              - 接收缓冲区1指针（双缓冲模式使用）
            * - size
              - uint32_t
              - 缓冲区大小（字节）

    - 返回值：

        .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 配置成功
            * - status_invalid_argument
              - 参数错误（包含以下情况）：缓冲区指针无效，协议类型不支持，缓冲区大小超限

- **举例**: 如何使用 ``hpm_i2s_master_over_spi_init`` 函数来初始化 I2S-over-SPI

.. code-block:: c

    #define RX_SIZE_MAX             (4096U)
    /* 双缓冲配置 */
    ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) uint8_t rx_buffer0[RX_SIZE_MAX];
    ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(8) uint8_t rx_buffer1[RX_SIZE_MAX];

    hpm_i2s_master_over_spi_rx_config(&i2s_device,
                                    I2S_PROTOCOL_MSB_JUSTIFIED,
                                    48000,
                                    16,
                                    rx_buffer0,
                                    rx_buffer1,
                                    RX_SIZE_MAX);

启动接收
""""""""""""""

- 调用 ``hpm_i2s_master_over_spi_rx_start`` API 启动配置好的I2S接收流程

  - 启动I2S接收流程，开始接收数据。
  - 该API会根据配置信息启动I2S-over-SPI的接收操作，开始接收数据。
  - 接收完成后，会触发相应的回调函数（如 ``rx_callback``）进行数据处理。

- ``hpm_i2s_master_over_spi_rx_start`` API原型：

    .. code-block:: c

        hpm_stat_t hpm_i2s_master_over_spi_rx_start(hpm_i2s_over_spi_t *i2s, i2s_rx_data_tc callback);

    - 参数说明：

        .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - i2s
              - hpm_i2s_over_spi_t*
              - I2S-over-SPI上下文结构体指针（需已通过rx_config配置）
            * - callback
              - i2s_rx_data_tc
              - 接收完成回调函数指针，当DMA完成一个缓冲区传输时触发

    - 返回值：

        .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 接收成功启动
            * - status_invalid_argument
              - 参数错误（包含以下情况）：上下文指针为空， 回调函数指针为空

- **举例**: 如何使用 ``hpm_i2s_master_over_spi_rx_start`` 函数来启动 I2S-over-SPI 接收

.. code-block:: c

    /* 定义接收完成回调 */
    void rx_done_callback(uint32_t buf_index) {
        printf("Buffer %d received\n", buf_index);
        /* 处理接收完成的数据缓冲区 buf_index为缓存索引，比如双缓存模式下，0代表缓存0，1代表缓存1 */
    }

    int main(void) {
        /* 配置接收参数... */
        hpm_i2s_master_over_spi_rx_config(&i2s_device, ...);

        /* 启动接收并注册回调 */
        hpm_stat_t result = hpm_i2s_master_over_spi_rx_start(&i2s_device, rx_done_callback);
        if (result != status_success) {
            printf("RX start failed: 0x%x\n", result);
        }
        /* TODO */
    }

停止接收
""""""""""""""
- 调用 ``hpm_i2s_master_over_spi_rx_stop`` API 停止正在进行的I2S接收流程。
  - 该API会停止I2S-over-SPI的接收操作，停止接收数据。

- ``hpm_i2s_master_over_spi_rx_start`` API原型：

    .. code-block:: c

        hpm_stat_t hpm_i2s_master_over_spi_rx_stop(hpm_i2s_over_spi_t *i2s);

    - 参数说明：

        .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - i2s
              - hpm_i2s_over_spi_t*
              - I2S-over-SPI上下文结构体指针

    - 返回值：

        .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 接收成功停止
            * - status_invalid_argument
              - 参数错误（上下文指针为空）

- **举例**: 如何使用 ``hpm_i2s_master_over_spi_rx_stop`` 函数来停止 I2S-over-SPI 接收

.. code-block:: c

    /* 停止接收流程 */
    hpm_stat_t result = hpm_i2s_master_over_spi_rx_stop(&i2s_device);
    if (result != status_success) {
        printf("Failed to stop RX: 0x%x\n", result);
    }

I2S-over-SPI发送操作
^^^^^^^^^^^^^^^^^^^^^^^^

启动发送，分为阻塞和非阻塞接口
""""""""""""""""""""""""""""""""

非阻塞发送
~~~~~~~~~~~~

- 调用 ``hpm_i2s_master_over_spi_tx_buffer_nonblocking`` API 启动非阻塞式发送。
  - 启动I2S发送流程，开始发送数据。
  - 该API会根据配置信息启动I2S-over-SPI的发送操作，开始发送数据。

    - ``hpm_i2s_master_over_spi_tx_buffer_nonblocking`` API原型：

        .. code-block:: c

            hpm_stat_t hpm_i2s_master_over_spi_tx_buffer_nonblocking(hpm_i2s_over_spi_t *i2s,  uint8_t protocol, uint32_t lrck_hz,
                                                            uint8_t audio_depth, uint8_t *data, uint32_t size);

    - 参数说明：

        .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - i2s
              - hpm_i2s_over_spi_t*
              - I2S-over-SPI上下文结构体指针（需已初始化）
            * - protocol
              - uint8_t
              - 协议类型，当前不支持：``I2S_PROTOCOL_PCM`` 和 ``I2S_PROTOCOL_I2S_PHILIPS``
            * - lrck_hz
              - uint32_t
              - 声道时钟频率（采样率），单位Hz 比如8000/16000
            * - audio_depth
              - uint8_t
              - 音频位深度，当前支持：(16bit)和(32bit)
            * - data
              - uint8_t*
              - 发送数据缓冲区指针
            * - size
              - uint32_t
              - 数据长度（字节）

    - 返回值：

        .. list-table::
            :header-rows: 1

            * - 返回值
              - 描述
            * - status_success
              - 发送成功启动
            * - status_invalid_argument
              - 参数错误（包含以下情况）：缓冲区指针无效，协议类型不支持，数据长度超限

    - **举例**: 如何使用 ``hpm_i2s_master_over_spi_tx_buffer_nonblocking`` 函数来进行非阻塞发送

        .. code-block:: c

            /* 非阻塞发送16bit音频数据 */
            ATTR_PLACE_AT_NONCACHEABLE uint8_t tx_buffer[TX_SIZE_MAX];

            hpm_stat_t result = hpm_i2s_master_over_spi_tx_buffer_nonblocking(&i2s_device,
                                                                            I2S_PROTOCOL_MSB_JUSTIFIED,
                                                                            16000,
                                                                            16,
                                                                            tx_buffer,
                                                                            TX_SIZE_MAX);
            if (result != status_success) {
                printf("非阻塞发送启动失败: 0x%x\n", result);
            }

- 调用 ``hpm_i2s_master_over_spi_tx_is_busy`` API 检查I2S发送状态，当``hpm_i2s_master_over_spi_tx_buffer_nonblocking`` 发送完成后会返回true。

    - ``hpm_i2s_master_over_spi_tx_is_busy`` API原型：

        .. code-block:: c

            bool hpm_i2s_master_over_spi_tx_is_busy(hpm_i2s_over_spi_t *i2s);

        - 参数说明：

            .. list-table::
                :header-rows: 1

                * - 参数名
                  - 类型
                  - 描述
                * - i2s
                  - hpm_i2s_over_spi_t*
                  - I2S-over-SPI上下文结构体指针

        - 返回值：

            .. list-table::
                :header-rows: 1

                * - 返回值
                  - 描述
                * - true
                  - 发送正在进行中
                * - false
                  - 发送已完成或未启动

        - **举例**: 如何使用 ``hpm_i2s_master_over_spi_tx_is_busy`` 函数来检查I2S发送状态

           .. code-block:: c

                /* 在非阻塞发送中轮询状态 */
                while (hpm_i2s_master_over_spi_tx_is_busy(&i2s_device)) {
                }


阻塞发送
~~~~~~~~~~~~

- 调用 ``hpm_i2s_master_over_spi_tx_buffer_blocking`` API 执行阻塞式发送

    - 启动I2S发送流程，开始发送数据，会阻塞等待发送完成。

- ``hpm_i2s_master_over_spi_tx_buffer_blocking`` API原型：

        .. code-block:: c

            hpm_stat_t hpm_i2s_master_over_spi_tx_buffer_blocking(hpm_i2s_over_spi_t *i2s, uint8_t protocol, uint32_t lrck_hz,
                                                        uint8_t audio_depth, uint8_t *data, uint32_t size, uint32_t timeout);

        - 参数说明：

            .. list-table::
                :header-rows: 1

                * - 参数名
                  - 类型
                  - 描述
                * - i2s
                  - hpm_i2s_over_spi_t*
                  - I2S-over-SPI上下文结构体指针（需已初始化）
                * - protocol
                  - uint8_t
                  - 协议类型，当前不支持：``I2S_PROTOCOL_PCM`` 和 ``I2S_PROTOCOL_I2S_PHILIPS``
                * - lrck_hz
                  - uint32_t
                  - 声道时钟频率（采样率），单位Hz 比如8000/16000
                * - audio_depth
                  - uint8_t
                  - 音频位深度，当前支持：(16bit)和(32bit)
                * - data
                  - uint8_t*
                  - 发送数据缓冲区指针
                * - size
                  - uint32_t
                  - 数据长度（字节)
                * - timeout
                  - uint32_t
                  - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误

        - 返回值：

            .. list-table::
                :header-rows: 1

                * - 返回值
                  - 描述
                * - status_success
                  - 发送成功完成
                * - status_invalid_argument
                  - 参数错误（包含以下情况）：缓冲区指针无效，协议类型不支持，数据长度超限

- **举例**: 如何使用 ``hpm_i2s_master_over_spi_tx_buffer_blocking`` 函数来阻塞发送数据。

           .. code-block:: c

                /* 阻塞发送16bit音频数据 */
                uint8_t tx_buffer[TX_SIZE_MAX];

                hpm_stat_t result = hpm_i2s_master_over_spi_tx_buffer_blocking(&i2s_device,
                                                                            I2S_PROTOCOL_MSB_JUSTIFIED,
                                                                            16000,
                                                                            16,
                                                                            tx_buffer,
                                                                            TX_SIZE_MAX,
                                                                            1000000);
                if (result != status_success) {
                    printf("阻塞发送失败: 0x%x\n", result);
                }

注意
----

- 由于I2S_over_SPI组件使用了DMA管理器组件，DMA的通道等配置由DMA管理器分配，在使用DMA时分配的DMA通道避免与I2S_over_SPI组件使用的DMA通道冲突。

    - I2S_over_SPI组件使用的TX DMA通道可以在 ``i2s_device`` 实例变量的成员变量中找到，比如 ``i2s_device.tx_dma.resource`` 。
    - I2S_over_SPI组件使用的RX DMA通道可以在 ``i2s_device`` 实例变量的成员变量中找到，比如 ``i2s_device.rx_dma.resource`` 。

- **举例** : 如何使用DMA通道资源

    .. code-block:: c

        hpm_i2s_over_spi_t i2s_device;
        /* 初始I2S over SPI实例... 不做列举 */
        /* 打印发送DMA通道资源占用的DMA实例以及DMA通道 */
        printf("TX DMA instance: %d, TX DMA channel: %d\n", i2s_device.rx_dma.resource->dma_instance, i2s_device.rx_dma.resource->dma_channel);
        /* 改变TX DMA资源的中断优先级为1*/
        dma_mgr_enable_dma_irq_with_priority(i2s_device.rx_dma.resource, 1);
        /* 获取接收DMA通道资源占用的DMA实例以及DMA通道 */
        printf("RX DMA instance: %d, TX DMA channel: %d\n", i2s_device.rx_dma.resource->dma_instance, i2s_device.rx_dma.resource->dma_channel);
        /* 改变RX DMA资源的中断优先级为1*/
        dma_mgr_enable_dma_irq_with_priority(i2s_device.rx_dma.resource, 1);


