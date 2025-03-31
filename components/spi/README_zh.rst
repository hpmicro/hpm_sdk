.. _spi_component:

SPI component
==========================

概述
------

- SPI 组件提供了一套相比驱动层更高级的API，用于配置、初始化和操作SPI总线。支持多种SPI设备，并提供了阻塞和非阻塞模式下的数据传输功能。以下是该组件的主要特征：

  - 多实例支持

    - 支持多个SPI实例（如SPI0, SPI1等），每个实例都有独立的配置参数。

  - 阻塞与非阻塞读写操作接口

    - 支持主从机。

    - 提供了非阻塞式的读写函数。

    - 提供了阻塞式的读写函数，支持超时机制，确保在长时间未响应的情况下能够及时返回错误状态。

相关枚举和结构体介绍
------------------------------

- 枚举介绍

  - SPI工作模式枚举类型，这个枚举定义了两种不同的SPI工作模式选项，分别是主机模式和从机模式。该枚举在 ``hpm_spi_drv.h`` 中定义。

    .. code-block:: c

       typedef enum {
       spi_master_mode = 0,
       spi_slave_mode
       } spi_mode_selection_t;

  - SPI数据传输格式枚举类型，支持三种数据格式：单线、双线、四线三种格式。该枚举在 ``hpm_spi_drv.h`` 中定义。

    .. code-block:: c

       typedef enum {
           spi_single_io_mode = 0,
           spi_dual_io_mode,
           spi_quad_io_mode,
       } spi_data_phase_format_t;

  - SPI CPOL枚举类型，这个枚举定义了SPI SCLK在空闲状态下的电平。分别空闲低电平和空闲高电平。该枚举在 ``hpm_spi_drv.h`` 中定义。

    .. code-block:: c

       typedef enum {
           spi_sclk_low_idle = 0,
           spi_sclk_high_idle
       } spi_sclk_idle_state_t;


  - SPI CPHA枚举类型，这个枚举用于指定SPI SCLK采样是在奇数边沿还是偶数边沿进行。分别奇数边沿和偶数边沿进行。该枚举在 ``hpm_spi_drv.h`` 中定义。

    .. code-block:: c

       typedef enum {
           spi_sclk_sampling_odd_clk_edges = 0,
           spi_sclk_sampling_even_clk_edges
       } spi_sclk_sampling_clk_edges_t;



  - SPI CPHA枚举类型，这个枚举用于指定SPI通信中的位顺序。 ``spi_msb_first`` 表示最高有效位优先，而 ``spi_lsb_first`` 表示最低有效位优先。。该枚举在 ``hpm_spi_drv.h`` 中定义。

    .. code-block:: c

       typedef enum {
           spi_msb_first = 0,
           spi_lsb_first,
       } spi_shift_direction_t;



- 结构体介绍

  - 初始化配置结构体，用于配置SPI的设置。它包括SPI操作模式、数据线模式、时钟极性、时钟相位、数据移位方向、数据长度（以位为单位）和数据合并模式等参数。


    .. code-block:: c

           typedef struct {
               spi_mode_selection_t mode;               /*!< SPI工作模式 */
               spi_data_phase_format_t io_mode;         /*!< SPI数据线模式 */
               spi_sclk_idle_state_t clk_polarity;      /*!< 时钟极性 (CPOL) */
               spi_sclk_sampling_clk_edges_t clk_phase; /*!< 时钟相位 (CPHA) */
               spi_shift_direction_t direction;         /*!< 数据移位方向 (MSB或LSB) */
               uint8_t data_len;                        /*!< 单位是位 (1~32位) */
               bool data_merge;                         /*!< 数据合并模式 仅支持 data_len'为 8 */
           } spi_initialize_config_t;



API调用流程介绍
---------------------

定义SPI初始化变量
~~~~~~~~~~~~~~~~~~~~~~~~

- 定义一个 ``spi_initialize_config_t``  结构体的变量。例如，在示例中使用的 ``init_config`` 变量。

缺省SPI初始化变量
~~~~~~~~~~~~~~~~~~~~~~~~

- 使用 ``hpm_spi_get_default_init_config``  API 对 ``init_config``  变量赋值到API的 ``spi_initialize_config_t`` 参数中，进行初始化。

  - ``hpm_spi_get_default_init_config``  API原型：


    .. code-block:: c

       void hpm_spi_get_default_init_config(spi_initialize_config_t *config);


  - 此函数将各个成员设置为默认值:

    - 主机工作模式

    - 单线数据线模式

    - 时钟极性为低电平空闲

    - 时钟相位为在奇数边沿采样

    - 数据长度为8位

    - 不进行数据合并

    - 数据传输方向为MSB优先

SPI初始化
~~~~~~~~~~~~

- 调用 ``hpm_spi_initialize``  API 来初始化 SPI。在此过程中， ``init_config``  变量的参数会被赋值到API的 ``config`` 并生效。

- ``hpm_spi_initialize``  API原型：

    .. code-block:: c

        hpm_stat_t hpm_spi_initialize(SPI_Type *ptr, spi_initialize_config_t *config);


- **举例** : 实例化SPI0，设置SPI0为主机模式，四线数据线，CPOL为高电平空闲，CPHA为在奇数边沿采样，数据传输方向为MSB优先,数据单位长度为16位。

    .. code-block:: c

           init_config.mode = spi_master_mode;
           init_config.io_mode = spi_quad_io_mode;
           init_config.clk_polarity = spi_sclk_high_idle;
           init_config.clk_phase = spi_sclk_sampling_odd_clk_edges;
           init_config.data_len = 16;
           init_config.direction = spi_msb_first;
           hpm_spi_initialize(HPM_SPI0, &init_config);


- **举例:**  实例化SPI1，设置SPI1为从机模式，单线数据线，CPOL为低电平空闲，CPHA为在偶数边沿采样，数据传输方向为LSB优先,数据单位长度为8位。

    .. code-block:: c

           init_config.mode = spi_slave_mode;
           init_config.io_mode = spi_dual_io_mode;
           init_config.clk_polarity = spi_sclk_low_idle;
           init_config.clk_phase = spi_sclk_sampling_even_clk_edges;
           init_config.data_len = 8;
           init_config.direction = spi_lsb_first;
           hpm_spi_initialize(HPM_SPI1, &init_config);



SPI主机设置SCLK频率
~~~~~~~~~~~~~~~~~~~~~~~~~

- 从机模式下不需要此操作

- 调用 ``hpm_spi_set_sclk_frequency``  API 来设置SPI的SCLK频率。

- ``hpm_spi_set_sclk_frequency``  API原型：

    .. code-block:: c

        hpm_stat_t hpm_spi_set_sclk_frequency(SPI_Type *ptr, uint32_t freq)


  - 该函数用于设置指定SPI的SCLK频率。该函数通过选择合适的时钟源和分频系数来确保SPI时钟频率尽可能接近用户请求的频率。

    - 参数说明
        .. list-table::
           :widths: 20 30 50
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - ptr
             - ``SPI_Type *``
             - 指向SPI模块寄存器结构体的指针
           * - freq
             - ``uint32_t``
             - 请求的目标SPI时钟频率（单位：Hz）

    - 返回值

      - status_success: 成功设置SPI时钟频率。

      - status_invalid_argument: 提供的参数无效（例如，配置对象为NULL）

- **举例** : 如何使用 hpm_spi_set_sclk_frequency 函数来设置SPI1主机 SCLK频率为1Mhz

    .. code-block:: c

       uint32_t desired_freq = 1000000; /* 目标频率为1MHz */
       hpm_stat_t result = hpm_spi_set_sclk_frequency(HPM_SPI1, desired_freq);
       if (result == status_success) {
           printf("SPI SCLK frequency set successfully.\n");
       } else {
           printf("Failed to set SPI SCLK frequency.\n");
       }



DMA配置
~~~~~~~~~

- 若使用非阻塞读写接口，需要进行这一步配置

- 通过 ``hpm_spi_dma_mgr_install_callback``  API 进行 DMA 初始化并注册回调函数。在使用非阻塞（non-blocking）API 进行数据传输时，必须调用此函数。如果不需要回调函数，可以传递 NULL 作为回调参数。

- 用于为SPI模块的DMA传输配置回调函数。该函数允许用户为发送（TX）和接收（RX）DMA通道分别设置完成回调函数，以便在DMA传输完成后执行特定的操作。

- ``hpm_spi_dma_mgr_install_callback``  API原型：


    .. code-block:: c

           hpm_stat_t hpm_spi_dma_mgr_install_callback(SPI_Type *ptr, spi_dma_complete_cb tx_complete, spi_dma_complete_cb rx_complete)

    - 参数说明

        .. list-table::
           :widths: 20 20 60
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - ptr
             - ``SPI_Type*``
             - 指向SPI模块寄存器结构体的指针
           * - tx_complete
             - ``spi_dma_complete_cb``
             - 用于TX DMA传输完成的回调函数
           * - rx_complete
             - ``spi_dma_complete_cb``
             - 用于RX DMA传输完成的回调函数

    - 返回值：

      - ``status_success`` : 成功

      - ``status_invalid_argument`` : 无效参数


- **举例** : 如何使用 hpm_spi_dma_mgr_install_callback 函数来注册回调函数


    .. code-block:: c

           /**
           * TX DMA传输完成回调函数示例。
           */
           void tx_dma_complete_callback(void *context) {
               printf("TX DMA transfer completed.\n");
           }

           /**
           * RX DMA传输完成回调函数示例。
           */
           void rx_dma_complete_callback(void *context) {
               printf("RX DMA transfer completed.\n");
           }
           int main(void) {
               /* 初始化SPI... 不做列举 */

               /* 注册DMA传输完成回调 */
               hpm_stat_t result = hpm_spi_dma_mgr_install_callback(spi_ptr, tx_dma_complete_callback, rx_dma_complete_callback);

               if (result == status_success) {
                   /* 成功安装DMA传输完成回调函数 */
                   printf("DMA callback functions installed successfully.\n");
               } else {
                   /* 安装DMA传输完成回调函数失败 */
                   printf("Failed to install DMA callback functions.\n");
               }

               /* TODO */
           }



读写操作，支持主从机模式
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

阻塞式读写操作，分为全双工读写操作和半双工读写操作
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **全双工读写操作** ：用于阻塞同时读写操作。它会一直等待直到数据传输完成或超时，为 ``hpm_spi_transmit_receive_blocking`` API

- ``hpm_spi_transmit_receive_blocking`` API

  - ``hpm_spi_transmit_receive_blocking``  API函数原型:

    .. code-block:: c

           hpm_stat_t hpm_spi_transmit_receive_blocking(SPI_Type *ptr, uint8_t *wbuff, uint8_t *rbuff, uint32_t size, uint32_t timeout)


    - 参数说明
        .. list-table::
           :widths: 20 20 60
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - ptr
             - ``SPI_Type*``
             - 指向SPI模块寄存器结构体的指针
           * - wbuff
             - ``uint8_t*``
             - 指向要发送的数据缓冲区的指针
           * - rbuff
             - ``uint8_t*``
             - 指向接收数据的缓冲区的指针
           * - size
             - ``uint32_t``
             - 要传输的数据大小（以字节为单位）
           * - timeout
             - ``uint32_t``
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误

    - 返回值：

      - ``status_success`` : 成功完成数据传输
      - ``status_invalid_argument`` : 提供的参数无效
      - ``status_timeout`` : 如果在指定的timeout时间内未能完成操作

    - **举例** : 一个SPI1主设备或者从设备，通过SPI总线同时收发一些数据


        .. code-block:: c

               uint8_t tx_buffer[4] = {0x01, 0x02, 0x03, 0x04};  /* 发送缓冲区 */
               uint8_t rx_buffer[4];  /* 接收缓冲区 */
               uint32_t transfer_size = 4;  /* 要传输的数据大小 */
               uint32_t timeout_ms = 1000;  /* 超时时间为1秒 */

               /* 初始化SPI... 不做列举 */

               hpm_stat_t result = hpm_spi_transmit_receive_blocking(HPM_SPI1, tx_buffer, rx_buffer, transfer_size, timeout_ms);
               if (result == status_success) {
                   /* 成功完成数据传输 */
                   printf("Data transfer completed successfully.\n");
                   printf("Received data: ");
                   for (uint32_t i = 0; i < transfer_size; i++) {
                       printf("%02X ", rx_buffer[i]);
                   }
                   printf("\n");
               } else {
                   /* 数据传输失败 */
                   printf("Failed to complete data transfer.\n");
               }


- **半双工读写操作** ：用于阻塞半双工读写操作。它会一直等待直到数据传输完成或超时，为 ``hpm_spi_transmit_blocking`` API 和 ``hpm_spi_receive_blocking`` API

- ``hpm_spi_transmit_blocking`` API

  - 用于通过SPI接口进行数据的发送。该函数在阻塞模式下工作，即直到数据传输完成或超时才会返回。

  - ``hpm_spi_transmit_blocking``  API函数原型:


    .. code-block:: c

           hpm_stat_t hpm_spi_transmit_blocking(SPI_Type *ptr, uint8_t *buff, uint32_t size, uint32_t timeout)


    - 参数说明

        .. list-table::
           :widths: 20 20 60
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - ptr
             - ``SPI_Type*``
             - 指向SPI模块寄存器结构体的指针
           * - buff
             - ``uint8_t*``
             - 指向要发送的数据缓冲区的指针（注意：根据描述，类型应为指向 `uint8_t` 的指针，因此类型应该是 ``uint8_t*`` 而不是 ``uint8_t``）
           * - size
             - ``uint32_t``
             - 要传输的数据大小（以字节为单位）
           * - timeout
             - ``uint32_t``
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误
    - 返回值：

      - ``status_success`` : 成功完成数据传输

      - ``status_invalid_argument`` : 提供的参数无效

      - ``status_timeout`` : 如果在指定的timeout时间内未能完成操作

    - **举例** : 一个SPI1主设备或者从设备，通过SPI总线发送一些数据


    .. code-block:: c

           uint8_t tx_buffer[4] = {0x01, 0x02, 0x03, 0x04};  /* 发送缓冲区 */
           uint32_t transfer_size = 4;  /* 要传输的数据大小 */
           uint32_t timeout_ms = 1000;  /* 超时时间为1秒 */

           /* 初始化SPI... 不做列举 */

           hpm_stat_t result = hpm_spi_transmit_blocking(HPM_SPI1, tx_buffer, transfer_size, timeout_ms);
           if (result == status_success) {
           /* 成功完成数据传输 */
               printf("Data transmission completed successfully.\n");
           } else {
               /* 数据传输失败 */
               printf("Failed to complete data transmission.\n");
           }


- ``hpm_spi_receive_blocking`` API

  - 用于通过SPI接口进行数据的接收。该函数在阻塞模式下工作，即直到数据传输完成或超时才会返回。

  - ``hpm_spi_receive_blocking``  API函数原型:


    .. code-block:: c

           hpm_stat_t hpm_spi_receive_blocking(SPI_Type *ptr, uint8_t *buff, uint32_t size, uint32_t timeout)


    - 参数说明

        .. list-table::
           :widths: 20 20 60
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - ptr
             - ``SPI_Type*``
             - 指向SPI模块寄存器结构体的指针
           * - buff
             - ``uint8_t*``
             - 指向接收数据缓冲区的指针（注意：根据描述，类型应为指向 ``uint8_t`` 的指针）
           * - size
             - ``uint32_t``
             - 要接收的数据大小（以字节为单位）
           * - timeout
             - ``uint32_t``
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误
    - 返回值：

      - ``status_success`` : 成功完成数据接收

      - ``status_invalid_argument`` : 提供的参数无效

      - ``status_timeout`` : 如果在指定的timeout时间内未能完成操作

    - **举例** : 一个SPI1主设备或者从设备，通过SPI总线接收一些数据


    .. code-block:: c

           uint8_t rx_buffer[4];  /* 接收缓冲区 */
           uint32_t transfer_size = 4;  /* 要传输的数据大小 */
           uint32_t timeout_ms = 1000;  /* 超时时间为1秒 */

           /* 初始化SPI... 不做列举 */
           hpm_stat_t result = hpm_spi_receive_blocking(HPM_SPI1, rx_buffer, transfer_size, timeout_ms);
           if (result == status_success) {
               /* 成功完成数据接收 */
               printf("Data reception completed successfully.\n");
               printf("Received data: ");
               for (uint32_t i = 0; i < transfer_size; i++) {
                   printf("%02X ", rx_buffer[i]);
               }
               printf("\n");
           } else {
               /* 数据接收失败 */
               printf("Failed to complete data reception.\n");
           }



非阻塞式读写操作，分为全双工读写操作和半双工读写操作
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **全双工读写操作** ：用于非阻塞同时读写操作。它允许在操作完成之前继续执行其他任务，而不需要等待操作完成，为 ``hpm_spi_transmit_receive_nonblocking`` API

- ``hpm_spi_transmit_receive_nonblocking`` API

  - ``hpm_spi_transmit_receive_nonblocking``  API函数原型:


    .. code-block:: c

           hpm_stat_t hpm_spi_transmit_receive_nonblocking(SPI_Type *ptr, uint8_t *wbuff, uint8_t *rbuff, uint32_t size);


    - 参数说明

        .. list-table::
           :widths: 20 20 60
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - ptr
             - ``SPI_Type*``
             - 指向SPI模块寄存器结构体的指针
           * - wbuff
             - ``uint8_t*``
             - 指向要发送的数据缓冲区的指针（注意：根据描述，类型应为指向 ``uint8_t`` 的指针）
           * - rbuff
             - ``uint8_t*``
             - 指向接收数据的缓冲区的指针
           * - size
             - ``uint32_t``
             - 要传输的数据大小（以字节为单位）
    - 返回值：

      - ``status_success`` : 成功配置了非阻塞数据传输

      - ``status_invalid_argument`` : 提供的参数无效

    - **举例** : 一个SPI1主设备或者从设备，通过SPI总线同时收发一些数据


    .. code-block:: c

           uint8_t tx_buffer[4] = {0x01, 0x02, 0x03, 0x04};  /* 发送缓冲区 */
           uint8_t rx_buffer[4];  /* 接收缓冲区 */
           uint32_t transfer_size = 4;  /* 要传输的数据大小 */
           uint32_t timeout_ms = 1000;  /* 超时时间为1秒 */

           /* 初始化SPI... 不做列举 */

           hpm_stat_t result = hpm_spi_transmit_receive_nonblocking(spi_ptr, tx_buffer, rx_buffer, transfer_size);
           if (result == status_success) {
               /* 成功配置非阻塞传输/接收 */
               printf("Non-blocking data transmission and reception configured successfully.\n");
           } else {
               /* 配置非阻塞传输/接收失败 */
               printf("Failed to configure non-blocking data transmission and reception.\n");
           }

           /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */


- **半双工读写操作** ：用于非阻塞半双工读写操作，为 ``hpm_spi_transmit_nonblocking`` API 和 ``hpm_spi_receive_nonblocking`` API

- ``hpm_spi_transmit_nonblocking`` API

  - 用于通过SPI接口进行数据的非阻塞发送。该函数不会等待传输完成，而是立即返回，允许在后台处理数据传输。

  - ``hpm_spi_transmit_nonblocking``  API函数原型:


    .. code-block:: c

           hpm_stat_t hpm_spi_transmit_nonblocking(SPI_Type *ptr, uint8_t *buff, uint32_t size)


    - 参数说明

        .. list-table::
           :widths: 20 20 60
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - ptr
             - ``SPI_Type*``
             - 指向SPI模块寄存器结构体的指针
           * - buff
             - ``uint8_t*``
             - 指向要发送的数据缓冲区的指针（注意：类型应为指向 ``uint8_t`` 的指针）
           * - size
             - ``uint32_t``
             - 要传输的数据大小（以字节为单位）
    - 返回值：

      - ``status_success`` : 成功完成数据传输

      - ``status_invalid_argument`` : 提供的参数无效

    - **举例** : 一个SPI1主设备或者从设备，通过SPI总线发送一些数据


    .. code-block:: c

           uint8_t tx_buffer[4] = {0x01, 0x02, 0x03, 0x04};  /* 发送缓冲区 */
           uint32_t transfer_size = 4;  /* 要传输的数据大小 */
           uint32_t timeout_ms = 1000;  /* 超时时间为1秒 */

           /* 初始化SPI... 不做列举 */

           hpm_stat_t result = hpm_spi_transmit_nonblocking(HPM_SPI1, tx_buffer, transfer_size, timeout_ms);
           if (result == status_success) {
           /* 成功配置非阻塞传输 */
               printf("Non-blocking data transmission configured successfully.\n");
           } else {
               /* 配置非阻塞传输失败 */
               printf("Failed to configure non-blocking data transmission.\n");
           }
           /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */


- ``hpm_spi_receive_nonblocking`` API

  - 用于通过SPI接口进行数据的非阻塞接收。该函数不会等待接收完成，而是立即返回，允许在后台处理数据接收

  - ``hpm_spi_receive_nonblocking``  API函数原型:


    .. code-block:: c

           hpm_stat_t hpm_spi_receive_nonblocking(SPI_Type *ptr, uint8_t *buff, uint32_t size)


    - 参数说明

        .. list-table::
           :widths: 20 20 60
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - ptr
             - ``SPI_Type*``
             - 指向SPI模块寄存器结构体的指针
           * - buff
             - ``uint8_t*``
             - 指向接收数据缓冲区的指针（注意：类型应为指向 ``uint8_t`` 的指针）
           * - size
             - ``uint32_t``
             - 要接收的数据大小（以字节为单位）
    - 返回值：

      - ``status_success`` : 成功配置了非阻塞接收

      - ``status_invalid_argument`` : 提供的参数无效

    - **举例** : 一个SPI1主设备或者从设备，通过SPI总线接收一些数据


    .. code-block:: c

           uint8_t rx_buffer[4];  /* 接收缓冲区 */
           uint32_t transfer_size = 4;  /* 要传输的数据大小 */
           uint32_t timeout_ms = 1000;  /* 超时时间为1秒 */

           /* 初始化SPI... 不做列举 */
           hpm_stat_t result = hpm_spi_receive_nonblocking(spi_ptr, rx_buffer, transfer_size);
           if (result == status_success) {
               /* 成功配置非阻塞接收 */
               printf("Non-blocking data reception configured successfully.\n");
           } else {
               /* 配置非阻塞接收失败 */
               printf("Failed to configure non-blocking data reception.\n");
           }
           /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */



.. note::

    - 由于SPI组件使用了DMA管理器组件，DMA的通道等配置由DMA管理器分配，在使用DMA时分配的DMA通道避免与SPI组件使用的DMA通道冲突。

    - SPI组件使用的发送DMA通道可以调用 ``hpm_spi_get_tx_dma_resource`` API 获取SPI发送的DMA通道资源。

      - ``hpm_spi_get_tx_dma_resource``  API函数原型:


        .. code-block:: c

               dma_resource_t *hpm_spi_get_tx_dma_resource(SPI_Type *ptr)


    - SPI组件使用的接收DMA通道可以调用 ``hpm_spi_get_rx_dma_resource`` API 获取SPI发送的DMA通道资源。

      - ``hpm_spi_get_rx_dma_resource``  API函数原型:


        .. code-block:: c

               dma_resource_t *hpm_spi_get_rx_dma_resource(SPI_Type *ptr)

