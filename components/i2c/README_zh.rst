.. _i2c_component:

I2C component
=============

概述
-----

- I2C 组件提供了一套相比驱动层更高级的API，用于配置、初始化和操作I2C总线。支持多种I2C设备，并提供了阻塞和非阻塞模式下的数据传输功能。以下是该组件的主要特征：

  - 多实例支持
    - 支持多个I2C实例（如I2C0, I2C1等），每个实例都有独立的配置参数。
    - 通过 `hpm_i2c_context_t` 结构体管理每个I2C实例的配置信息，比如地址模式、速度模式、主从机模式
  - 阻塞与非阻塞读写操作接口
    - 支持主从机。
    - 提供了非阻塞式的读写函数。
    - 提供了阻塞式的读写函数，支持超时机制，确保在长时间未响应的情况下能够及时返回错误状态。

相关枚举和结构体介绍
---------------------

- 枚举介绍

  - I2C速度枚举类型，这个枚举定义了三种不同的I2C通信速度选项，分别是标准模式（100kHz）、快速模式（400kHz）和快速加模式（1MHz）

    .. code-block:: c

        typedef enum i2c_speed {
            i2c_speed_100khz,  /* 标准模式（100kHz） */
            i2c_speed_400khz,  /* 快速模式（400kHz） */
            i2c_speed_1Mhz,    /* 快速加模式（1MHz） */
        } hpm_i2c_speed_t;

  - I2C通信模式枚举类型，定义了两种通信模式：主模式和从模式。

    .. code-block:: c

        typedef enum i2c_communication_mode {
            i2c_master,
            i2c_slave,
        } hpm_i2c_communication_mode_t;

  - 地址字节序枚举类型，这个枚举定义了在主机模式下两种不同的寄存器地址字节序选项，分别是小端字节序（Little Endian）和大端字节序（Big Endian）。

    .. code-block:: c

        typedef enum i2c_master_addr_endianness {
            i2c_master_addr_little_endian = 0,
            i2c_master_addr_big_endian,
        } hpm_i2c_master_addr_endianness_t;

- 结构体介绍

  - 初始化配置结构体，包含了通信模式、速度、是否使用10位地址以及从设备地址等信息。

    .. code-block:: c

        typedef struct i2c_initialize_config {
            hpm_i2c_communication_mode_t communication_mode; /* I2C通信模式 */
            hpm_i2c_speed_t speed;                           /* I2C通信速度 */
            bool is_10bit_addressing;                        /* 是否使用10位地址 */
            uint8_t slave_address;                           /* 从设备地址，仅在从模式下支持 */
        } hpm_i2c_initialize_config_t;

  - I2C上下文结构体，包含了I2C实例的句柄、初始化配置等成员。在应用时需要定义该结构体，用于保存I2C实例的句柄和初始化配置。

    .. code-block:: c

        typedef struct i2c_context_t {
            I2C_Type *base;
            hpm_i2c_initialize_config_t init_config;
            hpm_i2c_master_addr_endianness_t addr_endianness;
        } hpm_i2c_context_t;

API调用流程介绍
----------------

定义I2C实例变量
^^^^^^^^^^^^^^^

- 定义一个 ``hpm_i2c_context_t`` 结构体的实例。例如，在示例中使用的 ``i2c_context`` 实例变量。

缺省I2C实例变量
^^^^^^^^^^^^^^^

- 使用 ``hpm_i2c_get_default_init_context`` API 对 ``i2c_context`` 实例变量赋值到API的``context``参数中，进行初始化。

  - ``hpm_i2c_get_default_init_context`` API原型：

    .. code-block:: c

        void hpm_i2c_get_default_init_context(hpm_i2c_context_t *context);

  - 此函数将各个成员设置为默认值:
    - 7-bit 地址模式
    - 主机模式
    - 100KHz 速度
    - 地址寄存器传输采用小端模式

I2C初始化
^^^^^^^^^

- 调用 ``hpm_i2c_initialize`` API 来初始化 I2C。在此过程中，``i2c_context`` 实例变量的参数会被赋值到API的``context``并生效。

- ``hpm_i2c_initialize`` API原型：

  .. code-block:: c

        void hpm_i2c_initialize(hpm_i2c_context_t *context);

- **举例**: 实例化I2C0，设置I2C0为主机模式，1Mhz速度，7bit地址模式，地址寄存器传输采用大端模式。

  .. code-block:: c

        i2c_context.init_config.speed = i2c_speed_1Mhz;
        i2c_context.init_config.speed = i2c_master;
        i2c_context.init_config.is_10bit_addressing = false;
        i2c_context.base = HPM_I2C0;
        i2c_context.addr_endianness = i2c_master_addr_big_endian;
        hpm_i2c_initialize(&i2c_context);

- **举例:** 实例化I2C0，设置I2C0为从机模式，1Mhz速度，10bit地址模式，从机设备地址为0x16。

  .. code-block:: c

        i2c_context.init_config.speed = i2c_speed_1Mhz;
        i2c_context.init_config.speed = i2c_slave;
        i2c_context.init_config.is_10bit_addressing = true;
        i2c_context.init_config.slave_address = 0x16;
        i2c_context.base = HPM_I2C0;
        hpm_i2c_initialize(&i2c_context);

DMA配置
^^^^^^^

- 若使用非阻塞读写接口，需要进行这一步配置
- 通过 ``hpm_i2c_dma_mgr_install_callback`` API 进行 DMA 初始化并注册回调函数。在使用非阻塞（non-blocking）API 进行数据传输时，必须调用此函数。如果不需要回调函数，可以传递 NULL 作为回调参数。
- 该函数的主要功能是将用户提供的DMA传输完成回调函数安装到给定的I2C上下文中。这样做可以确保在DMA传输完成后，系统能够自动调用用户的回调函数进行后续处理。

- ``hpm_i2c_dma_mgr_install_callback`` API原型：

  .. code-block:: c

        hpm_stat_t hpm_i2c_dma_mgr_install_callback(hpm_i2c_context_t *context, hpm_i2c_dma_complete_cb complete)

  - 参数说明

    .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - complete
             - hpm_i2c_dma_complete_cb
             - DMA传输完成后的回调函数指针。当DMA传输结束时，这个回调函数会被调用。

  - 返回值：

    - ``status_success``: 成功

    - ``status_invalid_argument``: 无效参数

    - ``status_fail``: 其他错误

- **举例**: 如何使用 hpm_i2c_dma_mgr_install_callback 函数来注册回调函数

  .. code-block:: c

         /* 定义回调函数 */
        void my_dma_complete_callback(uint32_t channel) {
            printf("DMA transfer on channel %d completed.\n", channel);
        }
        int main(void) {
            hpm_i2c_context_t i2c_context;
            /* 初始化I2C上下文... 不做列举 */

            /* 注册DMA传输完成回调 */
            hpm_stat_t result = hpm_i2c_dma_install_callback(&i2c_context, my_dma_complete_callback);

            if (result == status_success) {
                printf("Callback installed successfully.\n");
                /* 继续进行DMA传输设置和启动传输... */
            } else {
                printf("Failed to install callback. Error code: %d\n", result);
            }

            /* TODO */
        }

读写操作
^^^^^^^^

阻塞式读写操作
^^^^^^^^^^^^^^

主机模式，分为直接读写操作和带地址寄存器读写操作
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **直接读写操作**：用于I2C主模式下的阻塞读写操作。它会一直等待直到数据传输完成或超时
- 分为 `hpm_i2c_master_write_blocking` 和 ``hpm_i2c_master_read_blocking`` API

  - ``hpm_i2c_master_write_blocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_master_write_blocking(hpm_i2c_context_t *context, uint16_t device_address,  uint8_t *buf, uint32_t size, uint32_t timeout)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - device_address
             - uint16_t
             - 要写入数据的目标从设备的地址。根据初始化配置中的is_10bit_addressing标志，这个值可以是7位或10位
           * - buf
             - uint8_t*
             - 指向要发送的数据缓冲区的指针
           * - size
             - uint32_t
             - 缓冲区中要发送的数据字节数
           * - timeout
             - uint32_t
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误

    - 返回值：

      - ``status_success``: 如果写操作成功完成。

      - ``status_invalid_argument``: 如果提供的size超过了硬件支持的最大传输计数值

      - ``status_timeout``: 如果在指定的timeout时间内未能完成操作

      - ``status_i2c_no_addr_hit``: 如果目标地址上没有检测到设备响应

      - ``status_i2c_transmit_not_completed``:  如果在传输结束前数据计数器与预期不符

    - **举例**: 一个I2C主设备，通过I2C总线向从设备的发送一些数据

    .. code-block:: c

        /* 定义I2C上下文和设备地址 */
        hpm_i2c_context_t i2c_context;
        const uint16_t device_address = 0x3C; /* 示例从设备地址 */
        /* 要发送的数据和其大小 */
        uint8_t data_to_send[] = {0x01, 0x02, 0x03}; /* 示例数据 */
        uint32_t size = sizeof(data_to_send); // 数据大小
        uint32_t timeout = 1000; /* 超时时间设置为1秒 */
        /* 初始化I2C上下文... 不做列举 */
        /* 使用hpm_i2c_master_write_blocking函数从设备读取数据 */
        hpm_stat_t result = hpm_i2c_master_write_blocking(
            &i2c_context,
            device_address,
            data_to_send,
            size,
            timeout
        );
        if (result == status_success) {
            /* 写入成功 */
            printf("Data successfully written to I2C slave.\n");
        } else {
            /* 写入失败，处理错误情况 */
            printf("Failed to write data to I2C slave. Error code: %d\n", result);
        }

  - ``hpm_i2c_master_read_blocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_master_read_blocking(hpm_i2c_context_t *context, uint16_t device_address,  uint8_t *buf, uint32_t size, uint32_t timeout)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - device_address
             - uint16_t
             - 要写入数据的目标从设备的地址。根据初始化配置中的is_10bit_addressing标志，这个值可以是7位或10位
           * - buf
             - uint8_t*
             - 指向接收数据的缓冲区的指针
           * - size
             - uint32_t
             - 缓冲区中预期接收的数据字节数
           * - timeout
             - uint32_t
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误

    - 返回值：

      - ``status_success``: 如果写操作成功完成。

      - ``status_invalid_argument``: 如果提供的size超过了硬件支持的最大传输计数值

      - ``status_timeout``: 如果在指定的timeout时间内未能完成操作

      - ``status_i2c_no_addr_hit``: 如果目标地址上没有检测到设备响应

      - ``status_i2c_transmit_not_completed``:  如果在传输结束前数据计数器与预期不符

    - **举例**: 一个I2C主设备，通过I2C总线向从设备的读取一些数据

    .. code-block:: c

        /* 定义I2C上下文和设备地址 */
        hpm_i2c_context_t i2c_context;
        const uint16_t device_address = 0x3C; /* 示例从设备地址 */
        /* 准备接收数据的缓冲区和其大小 */
        uint8_t received_data[10]; /* 接收数据的缓冲区 */
        uint32_t size = sizeof(received_data); /* 缓冲区大小 */
        uint32_t timeout = 1000; /* 超时时间设置为1秒 */
        /* 初始化I2C上下文... 不做列举 */
        /* 使用hpm_i2c_master_read_blocking函数从设备读取数据 */
        hpm_stat_t result = hpm_i2c_master_read_blocking(
            &i2c_context,
            device_address,
            received_data,
            size,
            timeout
        );

        if (result == status_success) {
            /* 读取成功 */
            printf("Data successfully read from I2C slave.\n");
            /* 这里可以对received_data进行处理 */
        } else {
            /* 读取失败，处理错误情况 */
            printf("Failed to read data from I2C slave. Error code: %d\n", result);
        }

- **带地址寄存器读写操作**：用于I2C主模式下向从设备写入地址和数据的阻塞操作。它会等待直到地址和数据传输完成或超时
- 分为 ``hpm_i2c_master_addr_write_blocking`` 和 ``hpm_i2c_master_addr_read_blocking`` API

  - ``hpm_i2c_master_addr_write_blocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_master_addr_write_blocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size,
                                   uint8_t *buf, uint32_t buf_size, uint32_t timeout)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - device_address
             - uint16_t
             - 要写入数据的目标从设备的地址。根据初始化配置中的is_10bit_addressing标志，这个值可以是7位或10位
           * - addr
             - uint8_t
             - 从设备内部的寄存器或内存地址，将在此基础上进行写操作
           * - addr_size
             - uint32_t
             - 地址addr占用的字节数，通常为1到4字节
           * - buf
             - uint8_t*
             - 指向要发送的数据缓冲区的指针
           * - buf_size
             - uint32_t
             - 缓冲区中要发送的数据字节数
           * - timeout
             - uint32_t
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误

    - 返回值：

      - ``status_success``: 如果写操作成功完成。

      - ``status_invalid_argument``: 如果提供的addr_size或buf_size不符合要求，或者总数据长度超过了硬件支持的最大传输计数值

      - ``status_timeout``: 如果在指定的timeout时间内未能完成操作

      - ``status_i2c_no_addr_hit``: 如果目标地址上没有检测到设备响应

    - **举例**: 一个I2C主设备，通过I2C总线向从设备的某个寄存器写入一些数据

    .. code-block:: c

        const uint16_t device_address = 0x3C; /* 示例从设备地址 */
        /* 要发送的数据和其大小 */
        uint8_t data_to_send[] = {0x01, 0x02, 0x03}; /* 示例数据 */
        uint32_t addr = 0x00; /* 目标寄存器地址 */
        uint8_t addr_size = 1; /* 寄存器地址大小为1字节 */
        uint32_t size = sizeof(data_to_send); // 数据大小
        uint32_t timeout = 1000; /* 超时时间设置为1秒 */
        /* 初始化I2C上下文... 不做列举 */
        /* 使用hpm_i2c_master_write_blocking函数向设备写入数据 */
        hpm_stat_t result = hpm_i2c_master_addr_write_blocking(
            &i2c_context,
            device_address,
            addr,
            addr_size,
            data_to_send,
            size,
            timeout
        );
        if (result == status_success) {
            // 写入成功
            printf("Data successfully written to I2C slave.\n");
        } else {
            // 写入失败，处理错误情况
            printf("Failed to write data to I2C slave. Error code: %d\n", result);
        }

  - ``hpm_i2c_master_addr_read_blocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_master_addr_read_blocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size, uint8_t *buf, uint32_t buf_size, uint32_t timeout)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - device_address
             - uint16_t
             - 要写入数据的目标从设备的地址。根据初始化配置中的is_10bit_addressing标志，这个值可以是7位或10位
           * - addr
             - uint8_t
             - 从设备内部的寄存器或内存地址，将在此基础上进行读操作
           * - addr_size
             - uint32_t
             - 地址addr占用的字节数，通常为1到4字节
           * - buf
             - uint8_t*
             - 指向接收数据的缓冲区的指针
           * - buf_size
             - uint32_t
             - 缓冲区中预期接收的数据字节数
           * - timeout
             - uint32_t
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误

    - 返回值：

      - ``status_success``: 如果写操作成功完成。

      - ``status_invalid_argument``: 如果提供的addr_size或buf_size不符合要求，或者总数据长度超过了硬件支持的最大传输计数值

      - ``status_timeout``: 如果在指定的timeout时间内未能完成操作

      - ``status_i2c_no_addr_hit``: 如果目标地址上没有检测到设备响应

    - **举例**: 一个I2C主设备，通过I2C总线向从设备的某个寄存器读取一些数据

    .. code-block:: c

        /* 定义I2C上下文和设备地址 */
        hpm_i2c_context_t i2c_context;
        const uint16_t device_address = 0x3C; /* 示例从设备地址，根据实际设备修改 */

        /* 要读取的数据及其相关信息 */
        uint8_t received_data[4]; /* 接收数据的缓冲区大小，根据需要调整 */
        uint32_t addr = 0x01; /* 目标寄存器地址，根据实际需求修改 */
        uint8_t addr_size = 1; /* 寄存器地址大小为1字节 */
        uint32_t size = sizeof(received_data); /* 数据大小 */
        uint32_t timeout = 1000; /* 超时时间设置为1秒 */

        /* 初始化I2C上下文... 不做列举 */

        /* 使用假设存在的hpm_i2c_master_addr_read_blocking函数从设备读取数据 */
        /* 注意：这里假设存在一个名为hpm_i2c_master_addr_read_blocking的函数，它允许指定寄存器地址 */
        hpm_stat_t result = hpm_i2c_master_addr_read_blocking(
            &i2c_context,
            device_address,
            addr,
            addr_size,
            received_data,
            size,
            timeout
        );

        if (result == status_success) {
            /* 读取成功 */
            printf("Data successfully read from I2C slave.\n");
            for (int i = 0; i < size; i++) {
                printf("Received byte %d: 0x%02X\n", i, received_data[i]);
            }
        } else {
            /* 读取失败，处理错误情况 */
            printf("Failed to read data from I2C slave. Error code: %d\n", result);
        }

从机模式
^^^^^^^^

- 用于I2C从模式下的阻塞读写操作。它会一直等待直到数据传输完成或超时
- 分为 ``hpm_i2c_slave_write_blocking`` 和 ``hpm_i2c_slave_read_blocking`` API

  - ``hpm_i2c_slave_write_blocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_slave_write_blocking(hpm_i2c_context_t *context,  uint8_t *buf, uint32_t size, uint32_t timeout)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - buf
             - uint8_t*
             - 指向要发送的数据缓冲区的指针
           * - size
             - uint32_t
             - 缓冲区中要发送的数据字节数
           * - timeout
             - uint32_t
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误

    - 返回值：

      - ``status_success``: 如果写操作成功完成。

      - ``status_invalid_argument``: 如果提供的size超过了硬件支持的最大传输计数值

      - ``status_timeout``: 如果在指定的timeout时间内未能完成操作

      - ``status_i2c_transmit_not_completed``:  如果在传输结束前数据计数器与预期不符


    - **举例**: 一个I2C从设备，想要响应来自主设备的写请求并发送一些数据:

    .. code-block:: c

        hpm_i2c_context_t context;
        /* 初始化I2C上下文... */
        uint8_t data_to_send[] = {0x01, 0x02, 0x03};
        hpm_stat_t result = hpm_i2c_slave_write_blocking(&context, data_to_send, sizeof(data_to_send), 1000);
        if (result == status_success) {
             /* 成功处理写请求 */
        } else {
            /* 处理错误情况 */
        }

  - ``hpm_i2c_slave_read_blocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_slave_read_blocking(hpm_i2c_context_t *context,  uint8_t *buf, uint32_t size, uint32_t timeout)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - buf
             - uint8_t*
             - 指向接收数据的缓冲区的指针
           * - size
             - uint32_t
             - 缓冲区中预期接收的数据字节数
           * - timeout
             - uint32_t
             - 超时时间（单位为毫秒）。如果在这个时间内操作没有完成，函数将返回一个超时错误

    - 返回值：

      - ``status_success``: 如果写操作成功完成

      - ``status_invalid_argument``: 如果提供的size超过了硬件支持的最大传输计数值

      - ``status_timeout``: 如果在指定的timeout时间内未能完成操作

      - ``status_i2c_transmit_not_completed``:  如果在传输结束前数据计数器与预期不符

    - **举例**: 一个I2C从设备，想要响应来自主设备的写请求并发送一些数据:

    .. code-block:: c

        hpm_i2c_context_t context;
        /* 初始化I2C上下文... */
        uint8_t received_data[10];
        hpm_stat_t result = hpm_i2c_slave_read_blocking(&context, received_data, sizeof(received_data), 1000);
        if (result == status_success) {
             /* 成功处理写请求 */
        } else {
            /* 处理错误情况 */
        }

非阻塞式读写操作
^^^^^^^^^^^^^^^^^

- 用于I2C主从模式下的非阻塞读写操作。它允许在操作完成之前继续执行其他任务，而不需要等待操作完成
- 在使用非阻塞读写接口之前，需要进行上述的**DMA配置**流程

主机模式，分为直接读写操作和带地址寄存器读写操作
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- **直接读写操作**：用于I2C主模式下的非阻塞读写操作
- 分为 ``hpm_i2c_master_write_nonblocking`` 和 ``hpm_i2c_master_read_nonblocking`` API

  - ``hpm_i2c_master_write_nonblocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_master_write_nonblocking(hpm_i2c_context_t *context, uint16_t device_address, uint8_t *buf, uint32_t size)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - device_address
             - uint16_t
             - 要写入数据的目标从设备的地址。根据初始化配置中的is_10bit_addressing标志，这个值可以是7位或10位
           * - buf
             - uint8_t*
             - 指向要发送的数据缓冲区的指针
           * - size
             - uint32_t
             - 缓冲区中要发送的数据字节数

    - 返回值：

      - ``status_success``: 如果写操作成功完成

      - ``status_invalid_argument``: 如果提供的size超过了硬件支持的最大传输计数值

      - ``status_i2c_no_addr_hit``: 如果目标地址上没有检测到设备响应

    - **举例**: 一个I2C主设备，通过I2C总线向从设备的发送一些数据

    .. code-block:: c

        /* 定义I2C上下文和设备地址 */
        hpm_i2c_context_t i2c_context;
        const uint16_t device_address = 0x3C; /* 示例从设备地址 */
        /* 要发送的数据和其大小 */
        uint8_t data_to_send[] = {0x01, 0x02, 0x03}; /* 示例数据 */
        uint32_t size = sizeof(data_to_send); // 数据大小
        /* 初始化I2C上下文... 不做列举 */
        /* DMA配置... 不做列举 */
        /* 使用hpm_i2c_master_read_blocking函数从设备读取数据 */
        hpm_stat_t result = hpm_i2c_master_write_nonblocking(
            &i2c_context,
            device_address,
            data_to_send,
            size
        );
        if (result == status_success) {
            printf("Data transmission started successfully\n");
        } else {
            printf("Failed to start data transmission. Error code: %d\n", result);
        }

        /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */

  - ``hpm_i2c_master_read_nonblocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_master_read_nonblocking(hpm_i2c_context_t *context, uint16_t device_address,  uint8_t *buf, uint32_t size)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - device_address
             - uint16_t
             - 要写入数据的目标从设备的地址。根据初始化配置中的is_10bit_addressing标志，这个值可以是7位或10位
           * - buf
             - uint8_t*
             - 指向接收数据的缓冲区的指针
           * - size
             - uint32_t
             - 缓冲区中预期接收的数据字节数

    - 返回值：

      - ``status_success``: 如果写操作成功完成

      - ``status_invalid_argument``: 如果提供的size超过了硬件支持的最大传输计数值

      - ``status_i2c_no_addr_hit``: 如果目标地址上没有检测到设备响应

    - **举例**: 一个I2C主设备，通过I2C总线向从设备的读取一些数据

    .. code-block:: c

        /* 定义I2C上下文和设备地址 */
        hpm_i2c_context_t i2c_context;
        const uint16_t device_address = 0x3C; /* 示例从设备地址 */
        /* 准备接收数据的缓冲区和其大小 */
        uint8_t received_data[10]; /* 接收数据的缓冲区 */
        uint32_t size = sizeof(received_data); /* 缓冲区大小 */
        /* 初始化I2C上下文... 不做列举 */
        /* DMA配置... 不做列举 */
        /* 使用hpm_i2c_master_read_blocking函数从设备读取数据 */
        hpm_stat_t result = hpm_i2c_master_read_nonblocking(
            &i2c_context,
            device_address,
            received_data,
            size
        );

        if (result == status_success) {
            printf("Data transmission started successfully\n");
        } else {
            printf("Failed to start data transmission. Error code: %d\n", result);
        }

        /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */

- **带地址寄存器读写操作**：用于I2C主模式下向从设备写入地址和数据的非阻塞操作
- 分为 ``hpm_i2c_master_addr_write_nonblocking`` 和 ``hpm_i2c_master_addr_read_nonblocking`` API

  - ``hpm_i2c_master_addr_write_nonblocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_master_addr_write_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size, uint8_t *buf, uint32_t buf_size)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - device_address
             - uint16_t
             - 要写入数据的目标从设备的地址。根据初始化配置中的is_10bit_addressing标志，这个值可以是7位或10位
           * - addr
             - uint8_t
             - 从设备内部的寄存器或内存地址，将在此基础上进行写操作
           * - addr_size
             - uint32_t
             - 地址addr占用的字节数，通常为1到4字节
           * - buf
             - uint8_t*
             - 指向要发送的数据缓冲区的指针
           * - buf_size
             - uint32_t
             - 缓冲区中要发送的数据字节数

    - 返回值：

      - ``status_success``: 如果写操作成功完成

      - ``status_invalid_argument``: 如果提供的addr_size或buf_size不符合要求，或者总数据长度超过了硬件支持的最大传输计数值

      - ``status_i2c_no_addr_hit``: 如果目标地址上没有检测到设备响应

    - **举例**: 一个I2C主设备，通过I2C总线向从设备的某个寄存器写入一些数据

    .. code-block:: c

        const uint16_t device_address = 0x3C; /* 示例从设备地址 */
        /* 要发送的数据和其大小 */
        uint8_t data_to_send[] = {0x01, 0x02, 0x03}; /* 示例数据 */
        uint32_t addr = 0x00; /* 目标寄存器地址 */
        uint8_t addr_size = 1; /* 寄存器地址大小为1字节 */
        uint32_t size = sizeof(data_to_send); // 数据大小
        /* 初始化I2C上下文... 不做列举 */
        /* DMA配置... 不做列举 */

        hpm_stat_t result = hpm_i2c_master_addr_write_nonblocking(
            &i2c_context,
            device_address,
            addr,
            addr_size,
            data_to_send,
            size
        );
        if (result == status_success) {
            printf("Data transmission started successfully\n");
        } else {
            printf("Failed to start data transmission. Error code: %d\n", result);
        }

        /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */

  - ``hpm_i2c_master_addr_read_nonblocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_master_addr_read_nonblocking(hpm_i2c_context_t *context, const uint16_t device_address, uint32_t addr, uint8_t addr_size, uint8_t *buf, uint32_t buf_size)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - device_address
             - uint16_t
             - 要写入数据的目标从设备的地址。根据初始化配置中的is_10bit_addressing标志，这个值可以是7位或10位
           * - addr
             - uint8_t
             - 从设备内部的寄存器或内存地址，将在此基础上进行读操作
           * - addr_size
             - uint32_t
             - 地址addr占用的字节数，通常为1到4字节
           * - buf
             - uint8_t*
             - 指向接收数据的缓冲区的指针
           * - buf_size
             - uint32_t
             - 缓冲区中预期接收的数据字节数

    - 返回值：

      - ``status_success``: 如果写操作成功完成

      - ``status_invalid_argument``: 如果提供的addr_size或buf_size不符合要求，或者总数据长度超过了硬件支持的最大传输计数值

      - ``status_timeout``: 如果在指定的timeout时间内未能完成操作

      - ``status_i2c_no_addr_hit``: 如果目标地址上没有检测到设备响应

    - **举例**: 一个I2C主设备，通过I2C总线向从设备的某个寄存器读取一些数据

    .. code-block:: c

        /* 定义I2C上下文和设备地址 */
        hpm_i2c_context_t i2c_context;
        const uint16_t device_address = 0x3C; /* 示例从设备地址，根据实际设备修改 */

        /* 要读取的数据及其相关信息 */
        uint8_t received_data[4]; /* 接收数据的缓冲区大小，根据需要调整 */
        uint32_t addr = 0x01; /* 目标寄存器地址，根据实际需求修改 */
        uint8_t addr_size = 1; /* 寄存器地址大小为1字节 */
        uint32_t size = sizeof(received_data); /* 数据大小 */
        uint32_t timeout = 1000; /* 超时时间设置为1秒 */

        /* 初始化I2C上下文... 不做列举 */

        /* 使用假设存在的hpm_i2c_master_addr_read_blocking函数从设备读取数据 */
        /* 注意：这里假设存在一个名为hpm_i2c_master_addr_read_blocking的函数，它允许指定寄存器地址 */
        hpm_stat_t result = hpm_i2c_master_addr_read_nonblocking(
            &i2c_context,
            device_address,
            addr,
            addr_size,
            received_data,
            size
        );

        if (result == status_success) {
            printf("Data transmission started successfully\n");
        } else {
            printf("Failed to start data transmission. Error code: %d\n", result);
        }

        /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */

从机模式
^^^^^^^^

- 用于I2C从模式下的阻塞读写操作。它会一直等待直到数据传输完成或超时
- 分为 ``hpm_i2c_slave_write_nonblocking`` 和 ``hpm_i2c_slave_read_nonblocking`` API

  - ``hpm_i2c_slave_write_nonblocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_slave_write_nonblocking(hpm_i2c_context_t *context,  uint8_t *buf, uint32_t size)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - buf
             - uint8_t*
             - 指向要发送的数据缓冲区的指针
           * - size
             - uint32_t
             - 缓冲区中要发送的数据字节数

    - 返回值：

      - ``status_success``: 如果写操作成功完成。

      - ``status_invalid_argument``: 如果提供的size超过了硬件支持的最大传输计数值

    - **举例**: 一个I2C从设备，想要响应来自主设备的写请求并发送一些数据:

    .. code-block:: c

        hpm_i2c_context_t context;
        /* 初始化I2C上下文... */
        uint8_t data_to_send[] = {0x01, 0x02, 0x03};
        hpm_stat_t result = hpm_i2c_slave_write_nonblocking(&context, data_to_send, sizeof(data_to_send), 1000);
        if (result == status_success) {
             /* 成功处理写请求 */
        } else {
            /* 处理错误情况 */
        }
        /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */

  - ``hpm_i2c_slave_read_nonblocking`` API函数原型:

    .. code-block:: c

        hpm_stat_t hpm_i2c_slave_read_nonblocking(hpm_i2c_context_t *context,  uint8_t *buf, uint32_t size)

    - 参数说明

      .. list-table::
           :header-rows: 1

           * - 参数名
             - 类型
             - 描述
           * - context
             - hpm_i2c_context_t*
             - 指向I2C上下文结构体的指针，包含I2C配置信息和状态
           * - buf
             - uint8_t*
             - 指向接收数据的缓冲区的指针
           * - size
             - uint32_t
             - 缓冲区中预期接收的数据字节数

    - 返回值：

      - ``status_success``: 如果写操作成功完成

      - ``status_invalid_argument``: 如果提供的size超过了硬件支持的最大传输计数值

    - **举例**: 一个I2C从设备，想要响应来自主设备的写请求并发送一些数据:

    .. code-block:: c

        hpm_i2c_context_t context;
        /* 初始化I2C上下文... */
        uint8_t received_data[10];
        hpm_stat_t result = hpm_i2c_slave_read_nonblocking(&context, received_data, sizeof(received_data), 1000);
        if (result == status_success) {
             /* 成功处理写请求 */
        } else {
            /* 处理错误情况 */
        }
        /* TODO 由于是非阻塞操作，函数会立即返回，允许程序继续执行其他任务。比如等待此次传输完成 */

注意
----

- 由于I2C组件使用了DMA管理器组件，DMA的通道等配置由DMA管理器分配，在使用DMA时分配的DMA通道避免与I2C组件使用的DMA通道冲突。
- I2C组件使用的DMA通道可以调用``hpm_i2c_get_dma_mgr_resource``API 获取。

  - ``hpm_i2c_get_dma_mgr_resource`` API函数原型:

    .. code-block:: c

        dma_resource_t *hpm_i2c_get_dma_mgr_resource(hpm_i2c_context_t *context)

- **举例** : 如何使用 hpm_i2c_get_dma_mgr_resource 函数获取DMA通道资源以及获取到DMA通道资源后如何使用DMA通道资源
    .. code-block:: c

            hpm_i2c_context_t context;
            /* 初始化I2C... 不做列举 */
            /* 获取DMA通道资源 */
            dma_resource_t *i2c_dma_resource = hpm_i2c_get_dma_mgr_resource(&context);
            if (i2c_dma_resource != NULL) {
                /* 成功获取DMA通道资源 */
                printf("I2C DMA channel resource obtained successfully.\n");
                /* 打印获取到的DMA通道资源占用的DMA实例以及DMA通道 */
                printf("i2c DMA instance: %d, i2c DMA channel: %d\n", i2c_dma_resource->dma_instance, i2c_dma_resource->dma_channel);
                /* 改变DMA资源的中断优先级为1*/
                dma_mgr_enable_dma_irq_with_priority(i2c_dma_resource, 1);
            }

