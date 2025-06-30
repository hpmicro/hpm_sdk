.. _smbus:

SMBus
=======

概述
-----

- SMBus（System Management Bus）组件基于I2C总线协议扩展，提供系统管理总线规范兼容的通信接口。主要特性包括：

  - 协议兼容性

    - 支持SMBus 2.0规范定义的标准协议格式
    - 自动生成/校验数据包CRC（PEC）
    - 支持7位/10位设备地址格式

  - 传输模式支持

    - 主机模式和从机模式下的单字节/双字节读写操作
    - 主机模式带命令码的字节/字/块传输（Block Write/Read）
    - 支持正常模式（100Khz），快速模式（400kHz）和高速模式（1MHz）

  - 数据完整性保障

    - 硬件自动计算PEC校验码
    - 提供端到端的数据保护机制
    - 支持多字节传输的CRC校验

关键API功能
---------------------

- 基础传输接口

  - ``hpm_smbus_master_write_byte``: 主机单字节写入（无命令码）
  - ``hpm_smbus_master_read_byte``: 主机单字节读取（无命令码）
  - ``hpm_smbus_master_write``: 主机多字节写入（无命令码）
  - ``hpm_smbus_master_read``: 主机多字节读取（无命令码）
  - ``hpm_smbus_slave_write``: 从机多字节写入（无命令码）
  - ``hpm_smbus_slave_read``: 从机多字节读取（无命令码）


- 增强型传输接口

  - ``hpm_smbus_master_write_byte_in_command``: 主机带命令码的字节写入
  - ``hpm_smbus_master_read_byte_in_command``: 主机带命令码的字节读取
  - ``hpm_smbus_master_write_word_in_command``: 主机带命令码的双字节写入
  - ``hpm_smbus_master_read_word_in_command``: 主机带命令码的双字节读取
  - ``hpm_smbus_master_write_block_in_command``: 主机带命令码的块写入操作
  - ``hpm_smbus_master_read_block_in_command``: 主机带命令码的块读取操作

- 底层协议支持

  - ``hpm_smbus_pec_crc8``: PEC校验码生成算法（CRC-8）

API调用流程介绍
----------------

I2C初始化
^^^^^^^^^

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

            #define TEST_SMBUS                HPM_I2C0
            #define TEST_SMBUS_CLOCK_NAME     clock_i2c0

            i2c_config_t config;    /* 定义I2C配置结构体 */
            board_init_i2c_clock(TEST_SMBUS);
            init_i2c_pins(TEST_SMBUS);

            config.i2c_mode = i2c_mode_normal;
            config.is_10bit_addressing = false;
            freq = clock_get_frequency(TEST_SMBUS_CLOCK_NAME);
            stat = i2c_init_master(TEST_SMBUS, freq, &config);
            if (stat != status_success) {
                return stat;
            }

从机模式
""""""""""""""

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

- **举例**: 实例化I2C0，设置I2C0为从机模式，100Khz速度，7bit地址模式，从机设备地址为0x16。

   .. code-block:: c

        #define TEST_SMBUS                HPM_I2C0
        #define TEST_SMBUS_CLOCK_NAME     clock_i2c0

        i2c_config_t config;    /* 定义I2C配置结构体 */
        board_init_i2c_clock(TEST_SMBUS);
        init_i2c_pins(TEST_SMBUS);
        config.i2c_mode = i2c_mode_slave;
        config.is_10bit_addressing = false;
        config.slave_address = 0x16;
        freq = clock_get_frequency(TEST_SMBUS_CLOCK_NAME);
        stat = i2c_init_slave(TEST_SMBUS, freq, &config);
        if (stat!= status_success) {
            return stat;
        }

读写操作
^^^^^^^^

基础传输接口
""""""""""""""

- 仅支持主机模式单字节读写操作（无命令码）

  - 主机模式

    - 主机单字节写入

      - 调用 ``hpm_smbus_master_write_byte`` API 来实现主机模式下的单字节写入操作。

        - ``hpm_smbus_master_write_byte`` API原型：

            .. code-block:: c

                hpm_stat_t hpm_smbus_master_write_byte(I2C_Type *ptr, uint8_t slave_address, uint8_t data);

            - 参数说明：

                .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - data
                      - uint8_t
                      - 要写入的数据

            - 返回值：

              - ``status_success``: 成功
              - ``status_invalid_argument``: 无效参数
              - ``status_timeout``: 超时
              - ``status_fail``: 失败

        - **举例**: 主机模式下，通过I2C总线向从设备0x16写入数据0x55。

            .. code-block:: c

                /* 初始化I2C，不做举例... */
                hpm_stat_t stat;
                stat = hpm_smbus_master_write_byte(TEST_SMBUS, 0x16, 0x55);
                if (stat!= status_success) {
                    printf("hpm_smbus_master_write_byte failed.\n");
                }

    - 主机单字节读取

      - 调用 ``hpm_smbus_master_read_byte`` API 来实现主机模式下的单字节读取操作。

        - ``hpm_smbus_master_read_byte`` API原型：

            .. code-block:: c

                hpm_stat_t hpm_smbus_master_read_byte(I2C_Type *ptr, uint8_t slave_address, uint8_t *data);

            - 参数说明：

                .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - data
                      - uint8_t*
                      - 读取到的数据

            - 返回值：

              - ``status_success``: 成功
              - ``status_invalid_argument``: 无效参数

        - **举例**: 主机模式下，通过I2C总线从从设备0x16读取数据。

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data;
                /* 初始化I2C，不做举例... */
                stat = hpm_smbus_master_read_byte(TEST_SMBUS, 0x16, &data);
                if (stat!= status_success) {
                    printf("hpm_smbus_master_read_byte failed.\n");
                }

- 支持主机模式和从机模式下的多字节读写操作（无命令码）

  - 主机模式

    - 主机多字节写入

      - 调用 ``hpm_smbus_master_write`` API 来实现主机模式下的多字节写入操作。

        - ``hpm_smbus_master_write`` API原型：

            .. code-block:: c

                hpm_stat_t hpm_smbus_master_write(I2C_Type *ptr, uint8_t slave_address, uint8_t *data, uint32_t size);

            - 参数说明：

               .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - data
                      - uint8_t*
                      - 要写入的数据
                    * - size
                      - uint32_t
                      - 数据长度（字节）

            - 返回值：

              - ``status_success``: 成功
              - ``status_invalid_argument``: 无效参数
              - ``status_timeout``: 超时
              - ``status_fail``: 失败

        - **举例**: 主机模式下，通过I2C总线向从设备0x16写入多个数据。

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data[3] = {0x11, 0x22, 0x33};
                /* 初始化I2C，不做举例... */
                stat = hpm_smbus_master_write(TEST_SMBUS, 0x16, data, 3);
                if (stat!= status_success) {
                    printf("hpm_smbus_master_write failed.\n");
                }

    - 主机多字节读取

      - 调用 ``hpm_smbus_master_read`` API 来实现主机模式下的多字节读取操作。

        - ``hpm_smbus_master_read`` API原型：

            .. code-block:: c

                hpm_stat_t hpm_smbus_master_read(I2C_Type *ptr, uint8_t slave_address, uint8_t *data, uint32_t size);

            - 参数说明：

              .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - data
                      - uint8_t*
                      - 读取到的数据
                    * - size
                      - uint32_t
                      - 数据长度（字节）

            - 返回值：

              - ``status_success``: 成功
              - ``status_invalid_argument``: 无效参数
              - ``status_timeout``: 超时
              - ``status_fail``: 失败

        - **举例**: 主机模式下，通过I2C总线从从设备0x16读取多个数据。

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data[3];
                /* 初始化I2C，不做举例... */
                stat = hpm_smbus_master_read(TEST_SMBUS, 0x16, data, 3);
                if (stat!= status_success) {
                    printf("hpm_smbus_master_read failed.\n");
                }

  - 从机模式

    - 从机多字节写入

      - 调用 ``hpm_smbus_slave_write`` API 来实现从机模式下的单字节写入操作。

        - ``hpm_smbus_slave_write`` API原型：

            .. code-block:: c

                hpm_stat_t hpm_smbus_slave_write(I2C_Type *ptr, uint8_t *data, uint32_t size);

            - 参数说明：

              .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - data
                      - uint8_t
                      - 要写入的数据
                    * - size
                      - uint32_t
                      - 数据长度（字节）

            - 返回值：

              - ``status_success``: 成功
              - ``status_invalid_argument``: 无效参数
              - ``status_timeout``: 超时
              - ``status_fail``: 失败

        - **举例**: 从机模式下，如果总线主机下发的地址设备是该从机地址，写入数据。

            .. code-block:: c

                hpm_stat_t stat;
                uint8_t data[3];
                /* 初始化I2C，不做举例... */
                /* 等待地址命中 */
                do {
                    stat = hpm_smbus_slave_write(TEST_SMBUS, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
                } while (stat == status_fail);

    - 从机多字节读取

      - 调用 ``hpm_smbus_slave_read`` API 来实现从机模式下的单字节读取操作。

        - ``hpm_smbus_slave_read`` API原型：

            .. code-block:: c

                hpm_stat_t hpm_smbus_slave_read(I2C_Type *ptr, uint8_t *data, uint32_t size);

            - 参数说明：

              .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - data
                      - uint8_t*
                      - 读取到的数据
                    * - size
                      - uint32_t
                      - 数据长度（字节）

            - 返回值：

              - ``status_success``: 成功
              - ``status_invalid_argument``: 无效参数
              - ``status_timeout``: 超时
              - ``status_fail``: 失败

        - **举例**: 从机模式下，如果总线主机下发的地址设备是该从机地址，读取数据。

            .. code-block:: c

                    hpm_stat_t stat;
                    uint8_t data[3];
                    /* 初始化I2C，不做举例... */
                    /* 等待地址命中 */
                    do {
                        stat = hpm_smbus_slave_read(TEST_SMBUS, data_buff, TEST_TRANSFER_DATA_IN_BYTE);
                    } while (stat == status_fail);


增强型传输接口
""""""""""""""

- 仅支持主机模式带命令码的字节/字（16bit）/块传输（Block Write/Read）

  - 主机带命令码的字节写入

    - 调用 ``hpm_smbus_master_write_byte_in_command`` API 来实现主机模式下的带命令码的字节写入操作。

      - ``hpm_smbus_master_write_byte_in_command`` API原型：

        .. code-block:: c

              hpm_stat_t hpm_smbus_master_write_byte_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t data);

        - 参数说明：

            .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - command
                      - uint8_t
                      - 命令码
                    * - data
                      - uint8_t
                      - 要写入的数据

        - 返回值：

          - ``status_success``: 成功
          - ``status_invalid_argument``: 无效参数
          - ``status_timeout``: 超时
          - ``status_fail``: 失败

      - **举例**: 主机模式下，通过I2C总线向从设备0x16写入命令0x01和数据0x55。

         .. code-block:: c

              hpm_stat_t stat;
              /* 初始化I2C，不做举例... */
              stat = hpm_smbus_master_write_byte_in_command(TEST_SMBUS, 0x16, 0x01, 0x55);
              if (stat!= status_success) {
                  printf("hpm_smbus_master_write_byte_in_command failed.\n");
              }

  - 主机带命令码的字节读取

    - 调用 ``hpm_smbus_master_read_byte_in_command`` API 来实现主机模式下的带命令码的字节读取操作。

      - ``hpm_smbus_master_read_byte_in_command`` API原型：

       .. code-block:: c

              hpm_stat_t hpm_smbus_master_read_byte_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data);

       - 参数说明：

           .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - command
                      - uint8_t
                      - 命令码
                    * - data
                      - uint8_t*
                      - 读取到的数据

        - 返回值：

          - ``status_success``: 成功
          - ``status_invalid_argument``: 无效参数
          - ``status_timeout``: 超时
          - ``status_fail``: 失败

      - **举例**: 主机模式下，通过I2C总线从从设备0x16读取命令0x01的数据。

        .. code-block:: c

              hpm_stat_t stat;
              uint8_t data;
              /* 初始化I2C，不做举例... */
              stat = hpm_smbus_master_read_byte_in_command(TEST_SMBUS, 0x16, 0x01, &data);
              if (stat!= status_success) {
                  printf("hpm_smbus_master_read_byte_in_command failed.\n");
              }

  - 主机带命令码的字写入

    - 调用 ``hpm_smbus_master_write_word_in_command`` API 来实现主机模式下的带命令码的字写入操作。

      - ``hpm_smbus_master_write_word_in_command`` API原型：

       .. code-block:: c

              hpm_stat_t hpm_smbus_master_write_word_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint16_t data);

       - 参数说明：

          .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - command
                      - uint8_t
                      - 命令码
                    * - data
                      - uint16_t
                      - 要写入的数据

        - 返回值：

          - ``status_success``: 成功
          - ``status_invalid_argument``: 无效参数
          - ``status_timeout``: 超时
          - ``status_fail``: 失败

      - **举例**: 主机模式下，通过I2C总线向从设备0x16写入命令0x01和数据0x5555。

       .. code-block:: c

              hpm_stat_t stat;
              /* 初始化I2C，不做举例... */
              stat = hpm_smbus_master_write_word_in_command(TEST_SMBUS, 0x16, 0x01, 0x5555);
              if (stat!= status_success) {
                  printf("hpm_smbus_master_write_word_in_command failed.\n");
              }

  - 主机带命令码的字读取

    - 调用 ``hpm_smbus_master_read_word_in_command`` API 来实现主机模式下的带命令码的字读取操作。

      - ``hpm_smbus_master_read_word_in_command`` API原型：

        .. code-block:: c

              hpm_stat_t hpm_smbus_master_read_word_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint16_t *data);

        - 参数说明：

            .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - command
                      - uint8_t
                      - 命令码
                    * - data
                      - uint16_t*
                      - 读取到的数据

        - 返回值：

          - ``status_success``: 成功
          - ``status_invalid_argument``: 无效参数
          - ``status_timeout``: 超时
          - ``status_fail``: 失败

      - **举例**: 主机模式下，通过I2C总线从从设备0x16读取命令0x01的数据。

       .. code-block:: c

              hpm_stat_t stat;
              uint16_t data;
              /* 初始化I2C，不做举例... */
              stat = hpm_smbus_master_read_word_in_command(TEST_SMBUS, 0x16, 0x01, &data);
              if (stat!= status_success) {
                  printf("hpm_smbus_master_read_word_in_command failed.\n");
              }

  - 主机带命令码的块写入

    - 调用 ``hpm_smbus_master_write_block_in_command`` API 来实现主机模式下的带命令码的块写入操作。

      - ``hpm_smbus_master_write_block_in_command`` API原型：

        .. code-block:: c

              hpm_stat_t hpm_smbus_master_write_block_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t size);

        - 参数说明：

            .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - command
                      - uint8_t
                      - 命令码
                    * - data
                      - uint8_t*
                      - 要写入的数据
                    * - size
                      - uint32_t
                      - 数据长度（字节）

        - 返回值：

          - ``status_success``: 成功
          - ``status_invalid_argument``: 无效参数
          - ``status_timeout``: 超时
          - ``status_fail``: 失败

      - **举例**: 主机模式下，通过I2C总线向从设备0x16写入命令0x01和多个数据。

      .. code-block:: c

              hpm_stat_t stat;

              uint8_t data[3] = {0x11, 0x22, 0x33};
              /* 初始化I2C，不做举例... */
              stat = hpm_smbus_master_write_block_in_command(TEST_SMBUS, 0x16, 0x01, data, 3);
              if (stat!= status_success) {
                  printf("hpm_smbus_master_write_block_in_command failed.\n");
              }


  - 主机带命令码的块读取

    - 调用 ``hpm_smbus_master_read_block_in_command`` API 来实现主机模式下的带命令码的块读取操作。

      - ``hpm_smbus_master_read_block_in_command`` API原型：

        .. code-block:: c

              hpm_stat_t hpm_smbus_master_read_block_in_command(I2C_Type *ptr, uint8_t slave_address, uint8_t command, uint8_t *data, uint32_t size);

        - 参数说明：

            .. list-table::
                    :header-rows: 1

                    * - 参数名
                      - 类型
                      - 描述
                    * - ptr
                      - I2C_Type*
                      - 指向I2C控制器基地址的指针
                    * - slave_address
                      - uint8_t
                      - 从设备地址
                    * - command
                      - uint8_t
                      - 命令码
                    * - data
                      - uint8_t*
                      - 读取到的数据
                    * - size
                      - uint32_t
                      - 数据长度（字节）

        - 返回值：

          - ``status_success``: 成功
          - ``status_invalid_argument``: 无效参数
          - ``status_timeout``: 超时
          - ``status_fail``: 失败

      - **举例**: 主机模式下，通过I2C总线从从设备0x16读取命令0x01的数据。

     .. code-block:: c

              hpm_stat_t stat;
              uint8_t data[3];
              /* 初始化I2C，不做举例... */
              stat = hpm_smbus_master_read_block_in_command(TEST_SMBUS, 0x16, 0x01, data, 3);
              if (stat!= status_success) {
                  printf("hpm_smbus_master_read_block_in_command failed.\n");
              }

