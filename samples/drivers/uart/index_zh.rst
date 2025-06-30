.. _uart:

UART
====

概述
----

- 主要特征功能

  - 主要介绍先楫的uart外设的主要驱动接口说明和调用方法。更多内容请参考 `hpm_uart_drv.h` 的API说明以及相关用户手册。

  - 支持5到9位数据位，支持地址匹配。可参考 :ref:`uart_9bit <uart_9bit>` 例子。 9bit数据位具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_9BIT_MODE` 宏定义

  - 支持DMA数据传输。可参考 :ref:`uart_dma <use_dma_to_receive_and_send_uart_data>` 例子

  - 支持硬件RX空闲以及TX空间检测。可参考 :ref:`uart_hardware_rx_idle <uart_hardware_rx_idle_detection>` 例子。具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` 宏定义

  - 支持各类中断。可参考 :ref:`uart_irq <uart_interrupt>` 例子

  - 支持接收各类错误检测，比如奇偶校验错误， FIFO溢出等。可参考 :ref:`uart_rx_line_status <uart_rxline_status>` 例子

  - 可配置停止位：1位，1.5位，2位。

  - 可配置奇偶校验位：无，奇校验，偶校验。

  - 可配置波特率。

  - 支持硬件流控

  - 接收和发送内置最大32字节的硬件FIFO缓存，具体深度可参考每个SOC的 `hpm_soc_feature.h` 的 `UART_SOC_FIFO_SIZE` 宏定义 或者使用 `hpm_uart_drv.h` 中的 `uart_intr_tx_slot_avail` API接口获取。

uart初始化
------------

- 需要确保UART的时钟源已经开启，并且初始化了相关UART外设引脚。

  - 可使用`clock_add_to_group` 函数用于将UART时钟源添加到时钟组中，从而确保UART时钟源已经开启。

- 相关枚举值介绍：

  - `parity_setting_t` 枚举用于配置UART奇偶校验类型，具体内容如下：

    .. code-block:: c

        typedef enum parity {

            /**< 无校验 */
            parity_none = 0,

            /**< 奇校验 */
            parity_odd,

            /**< 偶校验 */
            parity_even,

            /**< 校验位固定为1 */
            parity_always_1,

            /**< 校验位固定为0 */
            parity_always_0,

        } parity_setting_t;

  - `num_of_stop_bits_t` 枚举用于配置停止位长度，具体内容如下：

    .. code-block:: c

        typedef enum num_of_stop_bits {

            /**< 1位停止位 */
            stop_bits_1 = 0,

            /**< 1.5位停止位 */
            stop_bits_1_5,

            /**< 2位停止位 */
            stop_bits_2,

        } num_of_stop_bits_t;

  - `word_length_t` 枚举用于配置数据位长度，具体内容如下：

    .. code-block:: c

        typedef enum word_length {

            /**< 5位数据位 */
            word_length_5_bits = 0,

            /**< 6位数据位 */
            word_length_6_bits,

            /**< 7位数据位 */
            word_length_7_bits,

            /**< 8位数据位 */
            word_length_8_bits,

        } word_length_t;

  - `uart_rxline_idle_cond_t` 枚举用于配置UART RX线以及TX线空闲检测条件，具体内容如下：

    - 仅支持在当前SOC支持RX Idle检测的情况下使用，具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` 宏定义

    .. code-block:: c

        typedef enum hpm_uart_rxline_idle_cond {

            /**< 当RX线保持高电平的持续时间超过阈值时视为空闲 */
            uart_rxline_idle_cond_rxline_logic_one = 0,

            /**< 当接收状态机处于空闲状态的持续时间超过阈值时视为空闲 */
            uart_rxline_idle_cond_state_machine_idle = 1

        } uart_rxline_idle_cond_t;

- 相关结构体介绍：

  - `uart_modem_config_t` 结构体用于配置uart的硬件流控相关参数, 具体内容如下：

    .. code-block:: c

            typedef struct uart_modem_config {

                /**< 自动流控制使能标志 */
                bool auto_flow_ctrl_en;

                /**< 回环模式使能标志 */
                bool loop_back_en;

                /**< 设置 RTS 信号电平为高标志 */
                bool set_rts_high;
            } uart_modem_config_t;

  - `uart_rxline_idle_config_t` 结构体用于配置UART RX和TX线空闲检测参数，具体内容如下：

    - 仅支持在当前SOC支持RX Idle检测的情况下使用，具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` 宏定义

    .. code-block:: c

        typedef struct hpm_uart_rxline_idle_detect_config {

            /**< RX或者TX线空闲检测使能标志 */
            bool detect_enable;

            /**< RX线或者TX线空闲检测中断使能标志 */
            bool detect_irq_enable;

            /**< RX线或者TX线空闲检测条件 */
            uart_rxline_idle_cond_t idle_cond;

            /**< RX线或者TX空闲检测阈值（以bit为单位） */
            uint8_t threshold;

        } uart_rxline_idle_config_t;

  - `uart_config_t` 结构体用于配置uart的相关参数, 具体内容如下：

    .. code-block:: c

            typedef struct hpm_uart_config {
                /**< uart时钟源频率(Hz) */
                uint32_t src_freq_in_hz;

                /**< 波特率 */
                uint32_t baudrate;

                /**< 停止位 */
                uint8_t num_of_stop_bits;

                /**< 数据位 */
                uint8_t word_length;

                /**< 奇偶校验 */
                uint8_t parity;/**< 奇偶校验 */

                /**< 发送FIFO触发中断或者DMA阈值 */
                uint8_t tx_fifo_level;

                /**< 接收FIFO触发中断或者DMA阈值 */
                uint8_t rx_fifo_level;

                /**< DMA使能标志 */
                bool dma_enable;/**< DMA使能标志 */

                /**< FIFO使能标志 */
                bool fifo_enable;

                /**< 硬件流控配置 */
                uart_modem_config_t modem_config;

                /**< 如果当前SOC支持RX Idle检测 */
            #if defined(HPM_IP_FEATURE_UART_RX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_RX_IDLE_DETECT == 1)
                /**< 接收空闲配置 */
                uart_rxline_idle_config_t  rxidle_config;
            #endif

              /**< 如果当前SOC支持TX Idle检测 */
            #if defined(HPM_IP_FEATURE_UART_TX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_TX_IDLE_DETECT == 1)
                /**< 发送空闲配置 */
                uart_rxline_idle_config_t  txidle_config;
            #endif

            /**< 如果当前SOC支持RX使能 */
            #if defined(HPM_IP_FEATURE_UART_RX_EN) && (HPM_IP_FEATURE_UART_RX_EN == 1)
                /**< 此位是用来防止RX引脚跳动导致的错误接收。在上电启动的时候后默认会把RX信号强制为高电平，在初始化的时候需要把该位置位把RX输入为正常状态 */
                bool rx_enable;
            #endif
            } uart_config_t;

    - `fifo_enable`： 用于配置uart是否使用FIFO。如果为true，则使用FIFO模式，这时候RBR/THR寄存器作为RX/TX FIFO使用；如果为flase，则为缓冲模式，RBR/THR寄存器作为单字节缓冲区使用。开启有效数据 `uart_intr_id_rx_data_avail` 中断时，FIFO模式下，当FIFO中数据达到阈值时会触发中断，缓冲模式下，当RBR/THR寄存器中有数据时会触发中断。

    - `dma_enable`： 用于配置uart是否使用DMA。如果为true，则使能uart的DMA模式，当FIFO中的数据达到阈值时会触发请求DMA。

    - `tx_fifo_level`： 用于配置uart发送FIFO触发中断或者DMA阈值。阈值设置可参考 `uart_fifo_trg_lvl_t` 枚举。有两个bit宽度方式的FIFO阈值

      - 2bit位宽的FIFO阈值，范围为0-3，当FIFO小于阈值时，会触发中断或者DMA请求。表示的FIFO阈值是：

        - 发送FIFO阈值

          .. list-table::
                :header-rows: 1

                * - 枚举定义
                  - 16字节的FIFO深度
                  - 32字节的FIFO深度
                * - uart_tx_fifo_trg_not_full
                  - 不为满
                  - 不为满
                * - uart_tx_fifo_trg_lt_three_quarters
                  - 小于12字节
                  - 小于24字节
                * - uart_tx_fifo_trg_lt_half
                  - 小于8字节
                  - 小于16字节
                * - uart_tx_fifo_trg_lt_one_quarter
                  - 小于4字节
                  - 小于8字节
        - 举例:

          - tx_fifo_level = uart_tx_fifo_trg_lt_half， 在16字节FIFO深度的SOC下，发送FIFO深度小于8字节时，会触发一次DMA请求。

      - 4bit位宽的FIFO阈值，范围为0-31，表示的FIFO阈值是：1到32字节的FIFO阈值, 当FIFO小于等于阈值时，会触发中断或者DMA请求。该部分定义只能支持的SOC是有在 `hpm_soc_ip_feature.h` 定义 `HPM_IP_FEATURE_UART_FINE_FIFO_THRLD` 宏定义。

        - 举例:

          - tx_fifo_level = 15， 在32字节FIFO深度的SOC下，发送FIFO深度小于等于15字节时，会触发一次DMA请求。

    - `rx_fifo_level`： 用于配置uart接收FIFO触发中断或者DMA阈值。阈值设置可参考 `uart_fifo_trg_lvl_t` 枚举。

      - 2bit位宽的FIFO阈值，范围为0-3，当FIFO大于阈值时，会触发中断或者DMA请求。表示的FIFO阈值是：

        - 接收FIFO阈值

          .. list-table::
                :header-rows: 1

                * - 枚举定义
                  - 16字节的FIFO深度
                  - 32字节的FIFO深度
                * - uart_rx_fifo_trg_not_empty
                  - 不为空
                  - 不为空
                * - uart_rx_fifo_trg_gt_one_quarter
                  - 大于3字节
                  - 大于7字节
                * - uart_rx_fifo_trg_gt_half
                  - 大于7字节
                  - 大于15字节
                * - uart_rx_fifo_trg_gt_three_quarters
                  - 大于13字节
                  - 大于27字节

        - 举例:

          - rx_fifo_level = uart_rx_fifo_trg_gt_one_quarter， 在16字节FIFO深度的SOC下，接收FIFO深度大于3字节时，会触发一次DMA请求。

      - 4bit位宽的FIFO阈值，范围为0-31，表示的FIFO阈值是：1到32字节的FIFO阈值，当FIFO大于等于阈值时，会触发中断或者DMA请求。该部分定义只能支持的SOC是有在 `hpm_soc_ip_feature.h` 定义 `HPM_IP_FEATURE_UART_FINE_FIFO_THRLD` 宏定义。

        - 举例:

          - rx_fifo_level = 27， 在32字节FIFO深度的SOC下，接收FIFO深度大于等于27字节时，会触发一次DMA请求。

    - `rxidle_config`： 用于配置uart接收空闲检测参数。该参数仅在当前SOC支持RX Idle检测的情况下使用，具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` 宏定义。

      - `detect_enable`： 用于配置uart接收空闲检测使能标志。如果为true，则使能uart的接收空闲检测。

      - `detect_irq_enable`： 用于配置uart接收空闲检测中断使能标志。如果为true，则使能uart的接收空闲检测中断。

      - `idle_cond`： 用于配置uart接收空闲检测条件。可以选择是当RX线保持高电平的持续时间超过阈值时视为空闲，或者当接收状态机处于空闲状态的持续时间超过阈值时视为空闲。

      - `threshold`： 用于配置uart接收空闲检测阈值。按bit单位计算。

    - `txidle_config`： 用于配置uart发送空闲检测参数。该参数仅在当前SOC支持TX Idle检测的情况下使用，具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_TX_IDLE_DETECT` 宏定义。参数与 `rxidle_config` 相同。


- 初始化API：

  - `uart_default_config` 函数用于配置uart的默认缺省参数，建议在uart进行初始化之前调用。具体内容如下：

    .. code-block:: c

        void uart_default_config(UART_Type *ptr, uart_config_t *config);

    - 参数说明：

      .. list-table::
        :header-rows: 1

        * - 参数名
          - 类型
          - 描述
        * - ptr
          - UART_Type *
          - 指向UART控制器基地址的指针
        * - config
          - uart_config_t *
          - 指向UART配置结构体的指针，包含波特率/数据位/停止位等参数

    - 默认参数值是：波特率为115200，数据位为8位，停止位为1位，无奇偶校验，使能FIFO，关闭DMA, 对于支持接收和发送空闲的soc， 使能接收空闲检测，关闭发送空闲检测。


  .. code-block:: c

        hpm_stat_t uart_init(UART_Type *ptr, uart_config_t *config);

  - 参数说明：

    .. list-table::
        :header-rows: 1

        * - 参数名
          - 类型
          - 描述
        * - ptr
          - UART_Type *
          - 指向UART控制器基地址的指针
        * - config
          - uart_config_t *
          - 指向UART配置结构体的指针，包含波特率/数据位/停止位等参数

  - 返回值：

    .. list-table::
        :header-rows: 1

        * - 返回值
          - 描述
        * - status_success
          - 初始化成功
        * - status_uart_no_suitable_baudrate_parameter_found
          - 未找到合适的波特率参数
        * - status_invalid_argument
          - 无效的参数

  - **注意**：

    1. 调用前需确保时钟源已正确配置
    2. config参数中的波特率/数据位等参数需在有效范围内
    3. 需根据具体SOC特性启用相关功能宏

  - 示例：

    - 使用默认配置初始化UART0，波特率为11520，0数据位为8位，停止位为1位，无奇偶校验。

        .. code-block:: c

            void uart_init_sample(void)
            {
                uart_config_t config;
                uart_default_config(HPM_UART0, &config);
                config.src_freq_in_hz = clock_get_frequency(clock_uart0);
                config.baudrate = 115200;
                hpm_stat_t status = uart_init(HPM_UART0, &config);
                if (status != status_success) {
                    printf("UART初始化失败\n");
                }
            }


uart发送
------------

- 支持FIFO发送以及DMA发送

发送
^^^^^^^^^^^^^^^^^^^

- 可以使用轮询方式进行FIFO发送，也可以使用中断方式进行FIFO发送。
- 使用中断方式需要打开plic控制器对应的UART中断，使用 `intc_m_enable_irq` API接口打开UART中断。
- 使用中断方式如果设置中断的优先级，使用 `intc_m_enable_irq_with_priority` API接口配置UART中断的优先级。

轮询阻塞方式发送
""""""""""""""""

- 单字节发送API，

  .. code-block:: c

        hpm_stat_t uart_send_byte(UART_Type *ptr, uint8_t c);

  - 参数说明：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - UART_Type *
                - 指向UART控制器基地址的指针
              * - data
                - uint8_t
                - 要发送的数据

  - 返回值：

    .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 发送成功
              * - status_timeout
                - 发送超时

  - **注意**：该API接口会阻塞等待FIFO发送完成。

  - 示例：

    .. code-block:: c

        void send_data_sample(void)
        {
            uint8_t data = 0x12;
            hpm_stat_t status = uart_send_byte(uart0, data);
            if (status == status_success) {
                printf("send data success\n");
            } else {
                printf("send data failed\n");
            }
        }

- 多字节发送API，

  .. code-block:: c

        hpm_stat_t uart_send_data(UART_Type *ptr, uint8_t *source, uint32_t size_in_byte);

  - 参数说明：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - UART_Type *
                - 指向UART控制器基地址的指针
              * - source
                - uint8_t *
                - 要发送的数据
              * - size_in_byte
                - uint32_t
                - 要发送的数据长度
  - 返回值：

    .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 发送成功
              * - status_timeout
                - 发送超时
  - **注意**：该API接口会阻塞等待FIFO发送完成。

  - 示例：

   .. code-block:: c

        void send_data_sample(void)
        {
            uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
            hpm_stat_t status = uart_send_data(uart0, data, sizeof(data));
            if (status == status_success) {
                printf("send data success\n");
            } else {
                printf("send data failed\n");
            }
        }

中断方式发送
""""""""""""""""

- 先楫uart具有高达32字节的发送FIFO硬件缓存，可以开启发送空闲中断，在一开始发送数据的时候，数据塞入一部分到FIFO，然后开启发送空闲中断，当FIFO发送完成之后，会触发发送空闲中断，此时可以继续往FIFO塞入数据，直到FIFO满。

- 单字节发送API，此API是将数据写入FIFO，不会等待发送完成立即返回。

  .. code-block:: c

        void uart_write_byte(UART_Type *ptr, uint8_t c)

  - 参数说明：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - UART_Type *
                - 指向UART控制器基地址的指针
              * - data
                - uint8_t
                - 要发送的数据

  - 示例：

   .. code-block:: c

        #define UART_TX_SEND_SIZE  100
        uint8_t tx_buffer[UART_TX_SEND_SIZE];
        uint32_t tx_buffer_index = 0;
        uint8_t tx_fifo_size = 0;

        /* 中断处理函数 */
        void uart0_isr(void)
        {
            uint8_t irq_id = uart_get_irq_id(TEST_UART);
            if (irq_id & uart_intr_id_tx_slot_avail) {

                /* 关闭发送FIFO空中断 */
                uart_disable_irq(HPM_UART0, uart_intr_tx_slot_avail);
                /* 继续往FIFO塞入数据 */
                send_size = (tx_fifo_size <= (UART_TX_SEND_SIZE - tx_buffer_index)) ? tx_fifo_size : (UART_TX_SEND_SIZE - tx_buffer_index);
                while (send_size--) {
                    uart_write_byte(HPM_UART0, tx_buffer[tx_buffer_index]);
                    tx_buffer_index++;
                }
                if (tx_buffer_index < UART_TX_SEND_SIZE) {
                    /* 还有数据需要发送，继续开启发送FIFO空中断 */
                    uart_enable_irq(HPM_UART0, uart_intr_tx_slot_avail);
                }
            }
        }

        void send_data_sample(void)
        {
            uint8_t send_size = 0;
            /* 关闭发送FIFO空中断 */
            uart_disable_irq(HPM_UART0, uart_intr_tx_slot_avail);
            /* 获取FIFO大小 */
            tx_fifo_size = uart_get_fifo_size(HPM_UART0, uart_intr_tx_slot_avail);

            send_size = (tx_fifo_size <= UART_TX_SEND_SIZE) ? tx_fifo_size : UART_TX_SEND_SIZE;

            /* 往FIFO塞入数据 */
            while (tx_buffer_index < send_size) {
                uart_write_byte(HPM_UART0, tx_buffer[tx_buffer_index]);
                tx_buffer_index++;
            }

            /* 开启发送FIFO空中断 */
            uart_enable_irq(HPM_UART0, uart_intr_tx_slot_avail);
        }

  - **注意**： `uart_intr_tx_slot_avail` 中断指的是发送FIFO缓存为空中断，并不代表数据已经移位到发送移位寄存器。如果需要知道实际数据已经移位发送了，可以使用 `uart_enable_rxline_idle_detection` API开启使用发送空闲中断。

DMA方式发送
""""""""""""""""
  - 详情请参考 :ref:`uart_dma <use_dma_to_receive_and_send_uart_data>` 例子


uart接收
------------

- 支持FIFO接收以及DMA接收

接收
^^^^^^^^^^^^^^^^^^^

- 可以使用轮询方式进行FIFO接收，也可以使用中断方式进行FIFO接收。
- 使用中断方式需要打开plic控制器对应的UART中断，使用 `intc_m_enable_irq` API接口打开UART中断。
- 使用中断方式如果设置中断的优先级，使用 `intc_m_enable_irq_with_priority` API接口配置UART中断的优先级。

轮询阻塞方式接收
""""""""""""""""

- 单字节接收API，

  .. code-block:: c

        hpm_stat_t uart_receive_byte(UART_Type *ptr, uint8_t *byte);

  - 参数说明：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - UART_Type *
                - 指向UART控制器基地址的指针
              * - byte
                - uint8_t *
                - 接收的数据指针

  - 返回值：

    .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 接收成功
              * - status_timeout
                - 接收超时

  - **注意**：该API接口会阻塞等待FIFO接收完成。

  - 示例：

   .. code-block:: c

        void receive_data_sample(void)
        {
            uint8_t data = 0;
            hpm_stat_t status = uart_receive_byte(uart0, &data);
            if (status == status_success) {
                printf("receive data success, data = 0x%x\n", data);
            } else {
                printf("receive data failed\n");
            }
        }

- 多字节接收API，

  .. code-block:: c

        hpm_stat_t uart_receive_data(UART_Type *ptr, uint8_t *source, uint32_t size_in_byte);

  - 参数说明：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - UART_Type *
                - 指向UART控制器基地址的指针
              * - source
                - uint8_t *
                - 接收的数据指针
              * - size_in_byte
                - uint32_t
                - 要接收的数据长度

  - 返回值：

    .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - status_success
                - 接收成功
              * - status_fail
                - 接收失败

  - **注意**：该API接口会阻塞等待FIFO接收完成。

  - 示例：

  .. code-block:: c

        void receive_data_sample(void)
        {
            uint8_t data[100];
            hpm_stat_t status = uart_receive_data(uart0, data, sizeof(data));
            if (status == status_success) {
                printf("receive data success\n");
            } else {
                printf("receive data failed\n");
            }
        }

中断方式接收
""""""""""""""""

- 先楫uart具有高达32字节的接收FIFO硬件缓存，可以开启接收有效数据和超时中断，设置接收FIFO阈值，当FIFO中有数据时，会触发接收有效数据中断，当没有字符从接收 FIFO 中被移出或移入，并且在最后四个字符时间内接收 FIFO 中至少有一个字符这时候会触发FIFO超时中断。

- 单字节接收API：此API是从FIFO中读取数据，不会等待接收完成立即返回。

  .. code-block:: c

        uint8_t uart_read_byte(UART_Type *ptr);

  - 参数说明：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - UART_Type *
                - 指向UART控制器基地址的指针

  - 返回值：

    .. list-table::
              :header-rows: 1

              * - 返回值
                - 描述
              * - 接收到的数据
                - 接收到的数据


  - 示例： 可参考 :ref:`uart_rx_timeout <uart_rx_timeout>` 例子

DMA方式接收
""""""""""""""""
  - 详情请参考 :ref:`uart_dma <use_dma_to_receive_and_send_uart_data>` 例子


硬件自动流控
---------------

- 支持CTS和RTS硬件自动流控

- 结构体说明：

  .. code-block:: c

        /* UART modem配置结构体 */

        typedef struct uart_modem_config {

        /* 自动流控使能 */
        bool auto_flow_ctrl_en;

        /* 内部RX TX loopback使能 */
        bool loop_back_en;

        /* 使能RTS */
        bool set_rts_high;
    } uart_modem_config_t;

  - **注意**

    - 不需要loopback功能，可以把 `loop_back_en` 设置为false
    - 只要`auto_flow_ctrl_en` 设置为true，就会使能硬件CTS硬件流控
    - 如果只需要使能CTS，可以把 `set_rts_high` 设置为true，`auto_flow_ctrl_en` 设置为true
    - 如果需要同时使能CTS和RTS，可以把 `set_rts_high` 设置为false，`auto_flow_ctrl_en` 设置为true

- 配置API说明：

  - 使能硬件自动流控

    .. code-block:: c

        void uart_modem_config(UART_Type *ptr, uart_modem_config_t *config);

  - 参数说明：

    .. list-table::
              :header-rows: 1

              * - 参数名
                - 类型
                - 描述
              * - ptr
                - UART_Type *
                - 指向UART控制器基地址的指针
              * - config
                - uart_modem_config_t *
                - 指向uart modem结构体指针，包含自动流控使能，内部RX TX loopback使能，使能RTS等参数。

  - 举例：使能RTS和CTS硬件流控，关闭loopback功能

    .. code-block:: c

        void uart_modem_config_sample(void)
        {
            uart_modem_config_t config;
            config.auto_flow_ctrl_en = true;
            config.loop_back_en = false;
            config.set_rts_high = false;
            uart_modem_config(HPM_UART0, &config);
        }

CTS硬件流控
^^^^^^^^^^^^^^

- 如果需要在中断中判断CTS电平状态，可以开启uart modem状态中断，然后在中断中获取CTS电平状态。

  - **注意**：

    - 使用中断方式必须把硬件流控关闭，也就是 `uart_modem_config_t` 的 `auto_flow_ctrl_en` 成员变量设置为false。
    - 如果开启硬件流控，uart modem中断即使使能也不会被触发，可以使用 `uart_get_modem_status` API接口获取CTS电平状态。

  - 举例：使用中断方式判断CTS电平状态

    .. code-block:: c


            SDK_DECLARE_EXT_ISR_M(IRQn_UART0, uart_isr)

            void uart_isr(void)
            {
                uint32_t irq_id = uart_get_irq_id(HPM_UART0);
                uint8_t cts_sta;
                if (irq_id == uart_intr_id_modem_stat) {
                    /* 读取MSR清零中断标志 */
                    cts_sta = (uart_get_modem_status(HPM_UART0) & UART_MSR_DCTS_MASK) >> UART_MSR_DCTS_SHIFT;
                    printf("CTS 0x%02x\r\n", cts_sta);
                }
            }

            void uart_modem_config_sample(void)
            {
                uart_enable_irq(HPM_UART0, uart_intr_modem_stat);
                intc_m_enable_irq_with_priority(IRQn_UART0, 2);
                uart_modem_config_t config;
                config.auto_flow_ctrl_en = false;
                config.loop_back_en = false;
                uart_modem_config(HPM_UART0, &config);
            }

中断
^^^^^^^^^^^^^^

- 需要打开plic控制器对应的UART中断，使用 `intc_m_enable_irq` API接口打开UART中断。
- 需要在plic控制器中配置UART中断的优先级，使用 `intc_m_enable_irq_with_priority` API接口配置UART中断的优先级。
- 每次对应的中断事件发生，可以在中断处理函数中调用 `uart_get_irq_id` API接口获取中断ID，比如相关中断使能为 `uart_intr_rx_data_avail_or_timeout` ，则调用 `uart_get_irq_id` 获取中断ID是否为 `uart_intr_id_rx_data_avail` 。
- UART外设支持以下中断，可以从 `uart_intr_enable` 枚举类型中查看。

  .. code-block:: c

        typedef enum uart_intr_enable {
            uart_intr_rx_data_avail_or_timeout = UART_IER_ERBI_MASK,   /* 接收到的有效数据达到FIFO阈值或FIFO超时中断 */
            uart_intr_tx_slot_avail = UART_IER_ETHEI_MASK,             /* 发送FIFO为空中断 */
            uart_intr_rx_line_stat = UART_IER_ELSI_MASK,               /* 接收线路状态中断 */
            uart_intr_modem_stat = UART_IER_EMSI_MASK,                 /* 流控状态中断 */

            /* 如果当前的SOC支持串口空闲中断 */
        #if defined(HPM_IP_FEATURE_UART_RX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_RX_IDLE_DETECT == 1)
            uart_intr_rx_line_idle = UART_IER_ERXIDLE_MASK,           /* 接收空闲中断 */
        #endif
            /* 如果当前的SOC支持串口空闲中断 */
        #if defined(HPM_IP_FEATURE_UART_TX_IDLE_DETECT) && (HPM_IP_FEATURE_UART_TX_IDLE_DETECT == 1)
            uart_intr_tx_line_idle = UART_IER_ETXIDLE_MASK,           /* 发送空闲中断 */
        #endif

            /* 如果当前SOC支持接收地址匹配功能 */
        #if defined(HPM_IP_FEATURE_UART_ADDR_MATCH) && (HPM_IP_FEATURE_UART_ADDR_MATCH == 1)
            uart_intr_addr_match = UART_IER_EADDRM_MASK,                   /*  接收地址匹配中断 */
            uart_intr_addr_match_and_rxidle = UART_IER_EADDRM_IDLE_MASK,   /*  接收地址匹配和接收空闲中断 */
            uart_intr_addr_datalost = UART_IER_EDATLOST_MASK,              /*  接收地址匹配数据丢失中断 */
        #endif

        /*  如果当前SOC支持接收状态检测 */
        #if defined(HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT) && (HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT == 1)
            uart_intr_errf = UART_IER_LSR_ERRF_IRQ_EN_MASK,               /*  接收状态错误中断 */
            uart_intr_break_err = UART_IER_LSR_BREAK_IRQ_EN_MASK,         /*  传输打断中断 */
            uart_intr_framing_err = UART_IER_LSR_FRAMING_IRQ_EN_MASK,     /*  帧格式错误中断 */
            uart_intr_parity_err = UART_IER_LSR_PARITY_IRQ_EN_MASK,       /*  奇偶校验错误中断 */
            uart_intr_overrun = UART_IER_LSR_OVERRUN_IRQ_EN_MASK,         /*  接收数据溢出中断 */
        #endif
        } uart_intr_enable_t;

- uart中断状态枚举定义

  .. code-block:: c

        typedef enum uart_intr_id {
            uart_intr_id_modem_stat = 0x0,      /*  MODEM状态中断 */
            uart_intr_id_tx_slot_avail = 0x2,   /*  发送FIFO为空中断 ，不代表发送移位寄存器移位完成。*/
            uart_intr_id_rx_data_avail = 0x4,   /*  接收FIFO数据可用中断 */
            uart_intr_id_rx_line_stat = 0x6,    /*  接收线路状态中断 */
            uart_intr_id_rx_timeout = 0xc,      /*  接收FIFO数据超时中断 */
        } uart_intr_id_t;



  - `uart_intr_rx_data_avail_or_timeout`

    - 中断触发条件: 如果使能FIFO，当接收到的数据达到FIFO阈值时就会触发，当没有字符从接收 FIFO 中被移出或移入，并且在最后四个字符时间内接收 FIFO 中至少有一个字节就会发生超时触发。
    - 中断标志清零：使用 `uart_read_byte` API 读取FIFO数据清零。 清零 `uart_intr_id_rx_data_avail` 状态，在FIFO模式下需要读取FIFO数据直到剩余数据小于FIFO阈值。

  - `uart_intr_tx_slot_avail`

    - 中断触发条件：发送FIFO为空时会触发。
    - 中断标志清零：使用 `uart_write_byte` API 写入FIFO数据。

  - `uart_intr_rx_line_stat`

    - 中断触发条件：接收到奇偶校验错误、帧错误、溢出错误、超时错误等。
    - 中断标志清零：使用 `uart_get_status` API 清零，该API也可以作为串口接收状态查询接口。

  - `uart_intr_modem_stat`

    - 参考 `CTS硬件流控` 章节

  - `uart_intr_rx_line_idle`

    - 中断触发条件：接收线路空闲时触发
    - 中断标志清零：使用 `uart_clear_rxline_idle_flag` API 清零。
    - **注意**  仅支持在当前SOC支持RX Idle检测的情况下使用，具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_IDLE_DETECT` 宏定义

  - `uart_intr_tx_line_idle`

    - 中断触发条件：发送线路空闲时触发
    - 中断标志清零：使用 `uart_clear_txline_idle_flag` API 清零。
    - **注意**  仅支持在当前SOC支持TX Idle检测的情况下使用，具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_TX_IDLE_DETECT` 宏定义

  - `uart_intr_addr_match`

    - 中断触发条件：当收到的第一个字节数据与配置的Address匹配时触发。在中断函数使用 `uart_is_addr_match` API判断是否触发。
    - 中断标志清零：使用 `uart_clear_addr_match_flag` API 清零。
    - **注意**  具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_ADDR_MATCH` 宏定义

  - `uart_intr_addr_match_and_rxidle`

    - 中断触发条件：当收到的第一个字节数据与配置的Address匹配后接收总线空闲时触发。在中断函数使用 `uart_is_addr_match_and_rxidle` API判断是否触发。
    - 中断标志清零：使用 `uart_clear_addr_match_and_rxidle_flag` API 清零。
    - **注意**  具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_ADDR_MATCH` 宏定义

  - `uart_intr_addr_datalost`

    - 中断触发条件：在地址匹配前的数据会抛弃，然后产生中断，在中断函数使用 `uart_is_data_lost` API判断是否触发。比如准备两组数据，数据组1(没带可匹配的地址)在数据组2(带匹配的地址)之前产生中断，数据组1数据会抛弃。
    - 中断标志清零：使用 `uart_clear_data_lost_flag` API 清零。
    - **注意**  具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_ADDR_MATCH` 宏定义

  - `uart_intr_errf`

    - 中断触发条件：接收 fifo 错误标志位，在 fifo 模式下，出现奇偶校验错误、帧错误或传输打断错误时触发，在中断函数使用 `uart_rx_is_fifo_error` API判断是否触发。
    - 中断标志清零：使用 `uart_clear_rx_fifo_error_flag` API 清零。
    - **注意**  具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` 宏定义

  - `uart_intr_framing_err`

    - 中断触发条件：在出现帧格式错误时触发，在中断函数使用 `uart_rx_is_framing_error` API判断是否触发。
    - 中断标志清零：使用 `uart_clear_rx_framing_error_flag` API 清零。
    - **注意**  具体支持在SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` 宏定义

  - `uart_intr_break_err`

    - 中断触发条件：在出现传输打断错误时产生时触发，在中断函数使用 `uart_rx_is_break` API判断是否触发。
    - 中断标志清零：使用 `uart_clear_rx_break_flag` API 清零。
    - **注意**  具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` 宏定义

  - `uart_intr_parity_err`

    - 中断触发条件：在出现奇偶校验错误时触发，在中断函数使用 `uart_rx_is_parity_error` API判断是否触发。
    - 中断标志清零：使用 `uart_clear_rx_parity_error_flag` API 清零。
    - **注意**  具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` 宏定义

  - `uart_intr_overrun`

    - 中断触发条件：接收数据过载时触发，在中断函数使用 `uart_rx_is_overrun` API判断是否触发。
    - 中断标志清零：使用 `uart_clear_rx_overrun_flag` API 清零。
    - **注意**

      - 具体支持参考每个SOC的 `hpm_soc_ip_feature.h` 的 `HPM_IP_FEATURE_UART_RX_LINE_ERROR_DETECT` 宏定义
      - 在具有32字节FIFO的SOC下，由于存在1字节异步FIFO，接收到33字节数据时，才会触发。

- 相关API接口：

  - 使能UART中断

    .. code-block:: c

            void uart_enable_irq(UART_Type *ptr, uint32_t irq_mask);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - UART_Type*
              - 指向UART控制器基地址的指针，用于指定要配置的UART控制器。
            * - irq_mask
              - uint32_t
              - 中断掩码，用于指定要使能的中断。对应的中断掩码可以从 `uart_intr_enable_t` 枚举类型中查看。

    - 返回值：

      - 无

  - 禁用UART中断

    .. code-block:: c

            void uart_disable_irq(UART_Type *ptr, uint32_t irq_mask)

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - UART_Type*
              - 指向UART控制器基地址的指针，用于指定要配置的UART控制器。
            * - irq_mask
              - uint32_t
              - 中断掩码，用于指定要使能的中断。对应的中断掩码可以从 `uart_intr_enable_t` 枚举类型中查看。

    - 返回值：

      - 无

uart示例
------------

.. toctree::
  :maxdepth: 3

  uart_9bit/README_zh
  uart_dma/README_zh
  uart_hardware_rx_idle/README_zh
  uart_irq/README_zh
  uart_lin/master/README_zh
  uart_lin/slave/README_zh
  uart_lin/slave_baudrate_adaptive/README_zh
  uart_rx_line_status/README_zh
  uart_rx_timeout/README_zh
  uart_software_rx_idle/README_zh
  uart_tamagawa/README_zh

