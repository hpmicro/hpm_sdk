.. _spi:

SPI
===

概述
------

- 主要介绍先楫的SPI外设的主要驱动接口说明和调用方法。更多内容请参考 `hpm_spi_drv.h` 的API说明以及相关用户手册。
- 通信方式：

  - 支持单线SPI，双线SPI，四线SPI
  - 支持主机模式和从机模式

- 主机SCLK时钟特性：

  - 支持SPI时钟极性和相位的配置(CPOL和CPHA)
  - CS 与 SCLK 接口时序可配置
  - SPI时钟源与SCLK频率比可配置

- 数据格式：

  - 支持MSB和LSB传输
  - 数据单位长度1~32bit
  - 主机模式下可配置命令阶段，地址阶段，dummy阶段，数据阶段。（每个阶段可使能或者禁能）
  - 从机模式下可配置纯数据模式和非纯数据模式，对于纯数据模式，从机在总线收发都是数据阶段；对于非纯数据模式，从机在总线收发都是命令(8bit)+dummy(8bit)+数据阶段.
  - 命令阶段长度固定为8bit
  - 地址阶段长度可配置8，16，24，32bit
  - 数据阶段数据长度可配置，单次传输长度可参考每个SOC的 `hpm_soc_feature.h` 的 `SPI_SOC_TRANSFER_COUNT_MAX` 宏定义。
  - 主机模式下最多支持4路硬件CS片选。此项支持需要看SOC的 `hpm_soc_ip_feature.h` 是否定义 `HPM_IP_FEATURE_SPI_CS_SELECT` 宏

- 数据缓冲：

  - SPI的TX和RX有各自独立的硬件FIFO缓冲，TX最大 8 字深， 32 位宽的 FIFO 缓冲，RX最大 8 字深， 32 位宽的 FIFO 缓冲。
  - 具体深度可参考 `hpm_soc_feature.h` 的 `SPI_SOC_FIFO_DEPTH` 宏定义或者使用 `hpm_spi_drv.h` 的 `spi_get_rx_fifo_size` 和  `spi_get_tx_fifo_size` API接口获取

- 中断与DMA：

  - 从机命令中断，从机模式下收到属命令时产生中断
  - SPI 输出完成时产生中断
  - TXFIFO 有效数据小于等于设置阈值时产生中断（支持DMA触发）
  - RXFIFO 有效数据大于等于设置阈值时产生中断（支持DMA触发）
  - TXFIFO 欠载时产生中断
  - RXFIFO 溢出时产生中断

SPI相关初始化
--------------

- 需要确保SPI的时钟源已经开启，并且初始化了相关SPI外设引脚。


SPI SCLK时钟配置
^^^^^^^^^^^^^^^^^^^

- 仅支持主机模式，从机模式不需要配置。
- 使用 `spi_master_timing_init` API接口设置主机SCLK时钟频率。
- 在设置主机SCLK时钟频率之前，建议使用 `spi_master_get_default_timing_config` API接口获取默认的SCLK时钟配置。

- 相关结构体介绍：

  - `spi_timing_config_t`：

    - 目前用于设置主机SCLK时钟频率的结构体。

    .. code-block:: c

        typedef struct {
            uint32_t clk_src_freq_in_hz; /*!< SPI时钟源频率 */
            uint32_t sclk_freq_in_hz;    /*!< SPI SCLK时钟频率 */
            uint8_t cs2sclk;             /*!< CS 有效到 SCLK 边缘最短时间。 SCLK_周期 *（CS2SCLK+1）/2 */
            uint8_t csht;                /*< CS 有效到 SCLK 有效时间。 SCLK_周期 *（CSHT+1）/2 */
        } spi_master_timing_config_t

        typedef struct {
            spi_master_timing_config_t master_config; /*!< SPI主机模式配置 */
        } spi_timing_config_t;

    - `clk_src_freq_in_hz` ：SPI时钟源频率，单位Hz。SPI的时钟源频率可以通过 `hpm_clock_drv.h` 的 `clock_get_frequency` API获取， SPI时钟源默认80Mhz。
    - `sclk_freq_in_hz` ：SPI SCLK时钟频率，单位Hz。用户根据实际情况设置。设置的条件如下：

      - 时钟源频率 `sclk_freq_in_hz` 必须能被目标SCLK频率 `clk_src_freq_in_hz` 整除。
      - 分频系数必须为偶数，即 时钟源频率 `clk_src_freq_in_hz` / 目标SCLK频率  `sclk_freq_in_hz` 必须为偶数。并且分频系数不可超过510。
      - 目标SCLK时钟频率大于等于SPI时钟源时，目标SCLK频率强制等于SPI时钟源。

      - 成功设置：

        - 时钟源频率80Mhz，目标SCLK频率10Mhz，分频系数为8，即 80Mhz / 10Mhz = 8。
        - 时钟源频率60Mhz，目标SCLK频率16Mhz，分频系数为6，即 60Mhz / 10Mhz = 6。

      - 失败设置：

        - 时钟源80MHz，目标频率33MHz → 不满足整除条件
        - 时钟源80MHz，目标频率16MHz → 分频系数5（不满足分频系数为偶数）
        - 时钟源80MHz，目标频率1KHz → 分频系数800（超过510上限）

    - 如果 `spi_master_timing_init` API 无法满足设置的目标SCLK频率 `sclk_freq_in_hz`，可以调整SPI时钟源频率来适配以上的设置条件。

    - `cs2sclk` ：CS 有效到 SCLK 边缘最短时间。 SCLK_周期 * (CS2SCLK+1) / 2。

      - 例如 `cs2sclk` 设置为1，SCLK_周期为10ns，即 CS 有效到 SCLK 边缘最短时间为10ns。

    - `csht` ：CS 高电平的最短时间。 SCLK_周期 * (CSHT + 1) / 2。

      - 例如 `csht` 设置为1，SCLK_周期为10ns，即 CS 高电平保持最短时间为10ns。


- 相关API接口：

  - `spi_master_get_default_timing_config`：

    - 获取SPI主机模式默认的SCLK时钟配置。

        .. code-block:: c

            void spi_master_get_default_timing_config(spi_master_timing_config_t *config);

       - 参数说明：

        .. list-table::
             :header-rows: 1

             * - 参数名
               - 类型
               - 描述
             * - config
               - spi_master_timing_config_t*
               - 指向SPI主机模式SCLK时钟配置结构体的指针，包含SCLK时钟频率和相关配置信息

       - 返回值：

         - 无

  - `spi_master_timing_init`：

     - 设置SPI主机模式的SCLK时钟频率。

       .. code-block:: c

          hpm_stat_t spi_master_timing_init(SPI_Type *ptr, spi_master_timing_config_t *config);

       - 参数说明：

        .. list-table::
             :header-rows: 1

             * - 参数名
               - 类型
               - 描述
             * - ptr
               - SPI_Type*
               - 指向SPI控制器基地址的指针，用于指定要配置的SPI控制器。
             * - config
               - spi_master_timing_config_t*
               - 指向SPI主机模式SCLK时钟配置结构体的指针，包含SCLK时钟频率和相关配置信息

       - 返回值：

         - hpm_stat_t：

           - `status_success`：设置成功。
           - `status_invalid_argument`：参数无效。

- 举例：

  - 可参考 `spi示例` 的主机部分的例程。

    .. code-block:: c

        #include "hpm_clock_drv.h"
        #include "hpm_spi_drv.h"

        void spi_master_timing_init_example(void)
        {
                spi_master_timing_config_t config;
                uint32_t clock_freq_in_hz;
                /* 初始化SPI控制器引脚 ，不做举例 */

                /* 使能SPI时钟源 */
                clock_add_to_group(clock_spi1, 0); /* 默认80Mhz */

                /* 获取SPI时钟源频率 */
                clock_freq_in_hz = clock_get_frequency(clock_spi1);

                /* 获取SPI主机模式默认的SCLK时钟配置 */
                spi_master_get_default_timing_config(&config);

                /* 设置SPI主机模式的SCLK时钟频率 */
                config.clk_src_freq_in_hz = clock_freq_in_hz;
                config.sclk_freq_in_hz = 10000000; /* 设置SCLK时钟频率为10MHz */
                config.cs2sclk = 1;                /* 设置CS2SCLK为1 */
                config.csht = 1;                   /* 设置CSHT为1 */
                if (spi_master_timing_init(HPM_SPI1, &config) != status_success) {
                    /* 设置失败 */
                    printf("spi_master_timing_init failed\n");
                }
        }

SPI 格式配置
^^^^^^^^^^^^^^^^^^^

- 支持主机模式和从机模式。
- 主要配置SPI数据格式，包括数据位宽，数据传输顺序，命令位宽，地址位宽，dummy位宽，数据位宽。
- 在主机模式下，设置SPI格式配置之前，建议使用 `spi_master_get_default_format_config` API接口获取默认的SPI格式配置。
- 在从机模式下，设置SPI格式配置之前，建议使用 `spi_slave_get_default_format_config` API接口获取默认的SPI格式配置。

- 相关结构体介绍：

  - `spi_master_format_config_t`：

    - 目前用于设置主机模式下的SPI格式配置的结构体。

      .. code-block:: c

         /*!< SPI格式配置结构体 */
        typedef struct {
            spi_master_format_config_t master_config;  /*!< SPI主机模式配置 */
            spi_common_format_config_t common_config;  /*!< SPI通用配置 */
        } spi_format_config_t;

        /*!< SPI主机模式格式配置结构体 */
        typedef struct {
            uint8_t addr_len_in_bytes;  /*!< 地址长度，单位字节 */
        } spi_master_format_config_t;

        /*!< SPI通用格式配置结构体 */
        typedef struct {
            uint8_t data_len_in_bits;  /*!< 数据长度，单位位 */
            bool data_merge;           /*!< 数据合并模式 */
            bool mosi_bidir;           /*!< MOSI双向模式 */
            bool lsb;                  /*!< LSB模式 */
            uint8_t mode;              /*!< SPI模式 */
            uint8_t cpol;              /*!< 时钟极性 */
            uint8_t cpha;              /*!< 时钟相位 */
        } spi_common_format_config_t;

    - `spi_format_config_t` 包括了 `spi_master_format_config_t` 和 `spi_common_format_config_t` 两个结构体。

    - `spi_master_format_config_t` 用于设置主机模式下的SPI格式配置，包括地址长度。

      - `addr_len_in_bytes` ：地址长度，单位字节。用于指定地址阶段的地址长度。如果地址阶段禁能，则不需要设置该参数。

    - `spi_common_format_config_t` 用于设置SPI通用格式配置，支持主从机模式，包括数据长度，数据合并模式，MOSI双向模式，LSB模式，SPI模式，时钟极性和时钟相位。

      - `data_len_in_bits` ：单位数据长度，单位位。用于指定数据阶段的数据单位长度，支持1到32bit。
      - `data_merge` ：数据合并模式。**仅支持单位数据长度为8bit时有效**，并且数据大小必须是4的整数倍。在接收和发送时将每次传输的8bit数据合并为一个32bit数据，提升接收和发送效率。
      - `mosi_bidir` ：MOSI双向模式。如果设置为true，则MOSI双向模式，否则为单向模式。**仅支持SPI单线模式**。开启双向模式后，MOSI引脚是双向信号。
      - `lsb` ：LSB模式。如果设置为true，则LSB模式，否则为MSB模式。
      - `mode` ：设置SPI主机模式或者从机模式，使用 `spi_master_mode` 或者 `spi_slave_mode` 枚举类型。
      - `cpol` ：时钟极性。用于指定SPI时钟极性，使用 `spi_sclk_low_idle` 或者 `spi_sclk_high_idle` 枚举类型。
      - `cpha` ：时钟相位。用于指定SPI时钟相位，使用 `spi_sclk_sampling_odd_clk_edges` 或者 `spi_sclk_sampling_even_clk_edges` 枚举类型。

- 相关API接口：

  - `spi_master_get_default_format_config`：

    - 获取SPI主机模式默认的SPI格式配置。

       .. code-block:: c

            void spi_master_get_default_format_config(spi_format_config_t *config);

      - 参数说明：

        .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - config
              - spi_format_config_t*
              - 指向SPI主机模式格式配置结构体的指针，包含SPI格式配置信息

      - 返回值：

        - 无

  - `spi_slave_get_default_format_config`：

    - 获取SPI从机模式默认的SPI格式配置。

      .. code-block:: c

            void spi_slave_get_default_format_config(spi_format_config_t *config);

      - 参数说明：

        .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - config
              - spi_format_config_t*
              - 指向SPI从机模式格式配置结构体的指针，包含SPI格式配置信息

      - 返回值：

        - 无

  - `spi_format_init`：

    - 设置SPI的格式配置。

      .. code-block:: c

            void spi_format_init(SPI_Type *ptr, spi_format_config_t *config);

      - 参数说明：

        .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - SPI_Type*
              - 指向SPI控制器基地址的指针，用于指定要配置的SPI控制器。
            * - config
              - spi_format_config_t*
              - 指向SPI格式配置结构体的指针，包含SPI格式配置信息

      - 返回值：

        - 无

- 举例：

  - 可参考 `spi示例` 的例程。

  - 主机模式下，设置SPI格式配置：LSB模式，数据长度为8bit，时钟极性为低，时钟相位为奇数（CPOL = 0, CPHA = 0）.

     .. code-block:: c

        #include "hpm_clock_drv.h"
        #include "hpm_spi_drv.h"
        void spi_format_init_example(void)
        {
            spi_format_config_t format_config;
            /* 初始化SPI控制器引脚，不做举例.. */
            /* 使能SPI时钟源，不做举例.. */
            /* 设置SPI主机模式的SCLK时钟频率，不做举例..*/

            /* 获取SPI主机模式默认的SPI格式配置 */
            spi_master_get_default_format_config(&format_config);
            format_config.common_config.data_len_in_bits = 8; /* 设置数据长度为8bit */
            format_config.common_config.lsb = true;           /* 设置LSB模式 */
            format_config.common_config.cpol = spi_sclk_low_idle; /* 设置时钟极性为低 */
            format_config.common_config.cpha = spi_sclk_sampling_odd_clk_edges; /* 设置时钟相位为奇数 */
            format_config.common_config.mode = spi_master_mode; /* 设置SPI主机模式 */
            /* 设置SPI格式配置 */
            spi_format_init(HPM_SPI1, &format_config);
        }

  - 主机模式下，设置SPI格式配置：MSB模式，地址阶段的地址长度为3字节（24bit地址），数据长度为16bit，时钟极性为高，时钟相位为偶数（CPOL = 1, CPHA = 1）.

      .. code-block:: c

            #include "hpm_clock_drv.h"
            #include "hpm_spi_drv.h"
            void spi_format_init_example(void)
            {
                spi_format_config_t format_config;
                /* 初始化SPI控制器引脚，不做举例.. */
                /* 使能SPI时钟源，不做举例.. */
                /* 设置SPI主机模式的SCLK时钟频率，不做举例..*/

                /* 获取SPI主机模式默认的SPI格式配置 */
                spi_master_get_default_format_config(&format_config);
                format_config.common_config.data_len_in_bits = 16; /* 设置数据长度为16bit */
                format_config.common_config.lsb = false;           /* 设置MSB模式 */
                format_config.common_config.cpol = spi_sclk_high_idle; /* 设置时钟极性为高 */
                format_config.common_config.cpha = spi_sclk_sampling_even_clk_edges; /* 设置时钟相位为偶数 */
                format_config.common_config.mode = spi_master_mode; /* 设置SPI主机模式 */
                format_config.master_config.addr_len_in_bytes = 3; /* 设置地址阶段的地址长度为3字节 */
                /* 设置SPI格式配置 */
                spi_format_init(HPM_SPI1, &format_config);
            }

  - 从机模式下，设置SPI格式配置：MSB模式，数据长度为8bit，时钟极性为低，时钟相位为奇数（CPOL = 0, CPHA = 0）.

     .. code-block:: c

            #include "hpm_clock_drv.h"
            #include "hpm_spi_drv.h"
            void spi_format_init_example(void)
            {
                spi_format_config_t format_config;
                /* 初始化SPI控制器引脚，不做举例.. */
                /* 使能SPI时钟源，不做举例.. */
                /* 设置SPI主机模式的SCLK时钟频率，不做举例..*/

                /* 获取SPI从机模式默认的SPI格式配置 */
                spi_slave_get_default_format_config(&format_config);
                format_config.common_config.data_len_in_bits = 8; /* 设置数据长度为8bit */
                format_config.common_config.lsb = false;           /* 设置MSB模式 */
                format_config.common_config.cpol = spi_sclk_low_idle; /* 设置时钟极性为低 */
                format_config.common_config.cpha = spi_sclk_sampling_odd_clk_edges; /* 设置时钟相位为奇数 */
                format_config.common_config.mode = spi_slave_mode; /* 设置SPI从机模式 */
                /* 设置SPI格式配置 */
                spi_format_init(HPM_SPI1, &format_config);
            }

SPI数据传输
--------------

- 相关结构体介绍：

  .. code-block:: c

        /*!< SPI传输配置结构体 */
        typedef struct {
            bool cmd_enable;         /* !< 命令阶段使能 */
            bool addr_enable;        /*!< 地址阶段使能 */
            uint8_t addr_phase_fmt;  /*!< 地址阶段格式 */
            bool token_enable;       /*!< 令牌阶段使能 */
            uint8_t token_value;    /*!< 令牌值 */
        } spi_master_control_config_t;

        /*!< SPI从机模式控制配置结构体 */
        typedef struct {
            bool slave_data_only;    /*!< 从机纯数据模式 */
        } spi_slave_control_config_t;

        /*!< SPI传输配置结构体 */
        typedef struct {
            bool tx_dma_enable;     /*!< 发送DMA使能 */
            bool rx_dma_enable;     /*!< 接收DMA使能 */
            uint8_t trans_mode;     /*!< 传输模式 */
            uint8_t data_phase_fmt; /*!< 数据阶段格式 */
            uint8_t dummy_cnt;      /*!< dummy计数 */
        #if defined(HPM_IP_FEATURE_SPI_CS_SELECT) && (HPM_IP_FEATURE_SPI_CS_SELECT == 1)  /* 当前SOC支持硬件CS */
            uint8_t cs_index;       /*!< CS索引 */
        #endif
        } spi_common_control_config_t;

        /*!< SPI控制配置结构体 */
        typedef struct {
            spi_master_control_config_t master_config;  /*!< SPI主机模式控制配置 */
            spi_slave_control_config_t  slave_config;   /*!< SPI从机模式控制配置 */
            spi_common_control_config_t common_config;  /*!< SPI通用控制配置 */
        } spi_control_config_t;

- 功能说明：
  - 从 **结构体介绍** 中可以概括以下功能：
  - 支持主机模式和从机模式。
  - 支持DMA传输和轮询传输。

  - 主机模式下，SPI数据传输顺序是命令阶段 + 地址阶段 + 令牌阶段 + dummy阶段 + 数据阶段。各个阶段可使能或者禁能。

    - 命令阶段：

      - 命令阶段使能：`cmd_enable` 设置为true，命令阶段使能。
      - 命令阶段禁能：`cmd_enable` 设置为false，命令阶段禁能。
      - 命令阶段长度固定为8bit，命令值可通过 `spi_write_command` API接口设置。
      - **无论是否使能和禁能命令阶段，开启传输都必须设置命令值，可通过 `spi_write_command` API接口设置**

    - 地址阶段：

      - 地址阶段使能：`addr_enable` 设置为true，地址阶段使能。
      - 地址阶段禁能：`addr_enable` 设置为false，地址阶段禁能。
      - 地址阶段的地址长度可配置8，16，24，32bit。
      - 地址阶段的地址长度每次传输都固定不变，可通过设置  `spi_format_init` 中的 `addr_len_in_bytes` 成员进行设置。
      - 地址阶段的地址长度值可通过 `spi_write_address` API接口设置。
      - 地址阶段的地址传输支持SPI，DSPI，QSPI三种格式，可 `addr_phase_fmt` 成员进行设置。
        - `addr_phase_fmt` ：

          - `spi_address_phase_format_single_io_mode` ：SPI地址格式。
          - `spi_address_phase_format_dualquad_io_mode` ：DSPI或者QSPI地址格式。与 `data_phase_fmt` 成员一致的设置。比如 `data_phase_fmt` 设置为 `spi_quad_io_mode` QSPI格式，那么地址阶段的地址格式也为QSPI格式 。

    - 令牌阶段：

      - 令牌阶段使能：可通过  `spi_master_enable_token_transfer` API设置。
      - 令牌阶段禁能：可通过  `spi_master_disable_token_transfer` API设置。
      - 令牌阶段的令牌值可通过 `spi_master_set_token_value` API接口设置，只支持0x00和0x69两个令牌值。

    - dummy阶段：

      - `dummy_cnt` ：设置dummy数，单位是 `spi_format_init` 中的 `data_len_in_bits` 。比如 `data_len_in_bits` 设置为8bit， `dummy_cnt` 设置为2，那么dummy阶段的长度为16bit。
      - 只在 `trans_mode` 设置为 `spi_trans_write_dummy_read`、`spi_trans_read_dummy_write`、`spi_trans_dummy_write`、`spi_trans_dummy_read` 时有效。

    - 数据阶段：

      - `data_phase_fmt` ：设置数据阶段的格式，支持十种数据传输格式。可在 `spi_trans_mode_t` 枚举类型中查看。

        - `spi_trans_write_read_together`：同时读写传输顺序，仅支持SPI模式。比如在SPI模式下，SPI控制器在发送数据时，也会同时接收数据。
        - `spi_trans_write_only`：仅发送数据。
        - `spi_trans_read_only`：仅接收数据。
        - `spi_trans_write_read`：先发送数据再接收数据。
        - `spi_trans_read_write`：先接收数据再发送数据。
        - `spi_trans_write_dummy_read`：先发送数据再发送dummy再接收数据。
        - `spi_trans_read_dummy_write`：先接收数据再接收dummy再发送数据。
        - `spi_trans_no_data`: 无数据传输。仅传输命令和地址。需要开启命令和地址阶段。
        - `spi_trans_dummy_write`：先发送dummy再发送数据
        - `spi_trans_dummy_read`：先发送dummy再接收数据

  - 从机模式下，SPI数据传输分为纯数据模式和非纯数据模式。

    - 纯数据模式：

      - 从机纯数据模式：`slave_data_only` 设置为true，从机纯数据模式。
      - 从机纯数据模式下，从机在总线收发都是数据阶段。仅支持 `spi_trans_write_read_together` 传输模式。

    - 非纯数据模式：

      - 从机非纯数据模式：`slave_data_only` 设置为false，从机非纯数据模式。
      - 从机非纯数据模式下，从机在总线收发都是命令(8bit)+dummy(8bit)+数据阶段。

- 相关API接口：

    - 轮询传输数据API。

      .. code-block:: c

            hpm_stat_t spi_transfer(SPI_Type *ptr,
                        spi_control_config_t *config,
                        uint8_t *cmd, uint32_t *addr,
                        uint8_t *wbuff, uint32_t wcount, uint8_t *rbuff, uint32_t rcount);

      - 参数说明：

       .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - SPI_Type*
              - 指向SPI控制器基地址的指针，用于指定要配置的SPI控制器。
            * - config
              - spi_control_config_t*
              - 指向SPI控制配置结构体的指针，包含SPI控制配置信息
            * - cmd
              - uint8_t*
              - 指向命令值的指针，用于指定命令值。
            * - addr
              - uint32_t*
              - 指向地址值的指针，用于指定地址值。
            * - wbuff
              - uint8_t*
              - 指向发送缓冲区的指针，用于指定发送缓冲区。
            * - wcount
              - uint32_t
              - 发送数据的字节数。
            * - rbuff
              - uint8_t*
              - 指向接收缓冲区的指针，用于指定接收缓冲区。
            * - rcount
              - uint32_t
              - 接收数据的字节数。

      - 返回值：

        - `status_success`：传输成功。
        - `status_invalid_argument`：参数无效。
        - `status_timeout`：传输超时。
        - `status_fail`：传输错误。

     - 提示：

        - `cmd_enable` 设置为false, 则 `cmd` 可设置为NULL。
        - `addr_enable` 设置为false, 则 `addr` 可设置为NULL。
        - `data_phase_fmt` 设置为 `spi_trans_no_data` 时， `wcount` 和 `rcount` 可设置为0。

      举例：

        - 主机模式下，单线SPI，开启命令阶段，命令值为0x01，地址阶段使能（SPI格式），地址值为0x02，传输模式为 `spi_trans_write_read_together`，数据收发长度为5字节。

          .. code-block:: c

                #include "hpm_clock_drv.h"
                #include "hpm_spi_drv.h"
                void spi_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t cmd = 0x01;
                    uint32_t addr = 0x02;
                    uint8_t wbuff[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
                    uint8_t rbuff[5] = {0};
                    /* 初始化SPI控制器引脚，不做举例.. */
                    /* 使能SPI时钟源，不做举例.. */
                    /* 设置SPI主机模式的SCLK时钟频率，不做举例..*/
                    /* 设置SPI主机模式的SPI格式配置，不做举例..*/
                    spi_master_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_read_together; /* 设置传输模式为同时读写传输顺序 */
                    control_config.common_config.data_phase_fmt = spi_single_io_mode; /* 设置数据阶段的格式为同时读写传输顺序 */
                    control_config.master_config.cmd_enable = true; /* 开启命令阶段 */
                    control_config.master_config.addr_enable = true; /* 开启地址阶段 */
                    control_config.master_config.addr_phase_fmt = spi_address_phase_format_single_io_mode; /* 设置地址阶段的格式为SPI格式 */
                    if (spi_transfer(HPM_SPI1, &control_config, &cmd, &addr, wbuff, 5, rbuff, 5)!= status_success) {
                        /* 传输失败 */
                        printf("spi_transfer failed\n");
                    }
                }
        - 主机模式下，四线QSPI，禁能命令阶段以及地址阶段，传输模式为 `spi_trans_write_dummy_read`，数据发送长度为5字节，数据接收长度为10字节，dummy数为2。

         .. code-block:: c

                void spi_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t wbuff[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
                    uint8_t rbuff[10] = {0};
                    /* 初始化SPI控制器引脚，不做举例.. */
                    /* 使能SPI时钟源，不做举例.. */
                    /* 设置SPI主机模式的SCLK时钟频率，不做举例..*/
                    /* 设置SPI主机模式的SPI格式配置，不做举例..*/
                    spi_master_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_dummy_read; /* 设置传输模式为先发送dummy再接收数据 */
                    control_config.common_config.data_phase_fmt = spi_quad_io_mode; /* 设置数据阶段的格式为四线传输顺序 */
                    control_config.master_config.cmd_enable = false; /* 禁能命令阶段 */
                    control_config.master_config.addr_enable = false; /* 禁能地址阶段 */
                    control_config.common_config.dummy_cnt = spi_dummy_count_2; /* 设置dummy数为2 */
                    if (spi_transfer(HPM_SPI1, &control_config, NULL, NULL, wbuff, 5, rbuff, 10)!= status_success) {
                        /* 传输失败 */
                        printf("spi_transfer failed\n");
                    }
                }

        - 从机模式下，四线QSPI，从机纯数据模式，传输模式为 `spi_trans_write_read_together`，数据收发长度为5字节。

        .. code-block:: c

                void spi_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t wbuff[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
                    uint8_t rbuff[5] = {0};
                    /* 初始化SPI控制器引脚，不做举例.. */
                    /* 使能SPI时钟源，不做举例.. */
                    /* 设置SPI从机模式的SPI格式配置，不做举例..*/
                    spi_slave_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_read_together; /* 设置传输模式为同时读写传输顺序 */
                    control_config.common_config.data_phase_fmt = spi_quad_io_mode; /* 设置数据阶段的格式为同时读写传输顺序 */
                    control_config.slave_config.slave_data_only = true; /* 设置从机纯数据模式 */
                    if (spi_transfer(HPM_SPI1, &control_config, NULL, NULL, wbuff, 5, rbuff, 5)!= status_success) {
                        /* 传输失败 */
                        printf("spi_transfer failed\n");
                    }
                }

    - DMA传输初始化设置API

      - 用于设置SPI使用DMA传输的初始化设置，包括使能发送或者接收DMA，设置SPI控制器的传输长度等。
      - 如果每次传输的长度不变，则只需要初始化一次即可。主机模式下，下次传输可使用`spi_write_command` API接口设置。
      - TXFIFO 有效数据小于等于设置阈值时，会触发DMA传输。阈值设置可以通过 `spi_set_tx_fifo_threshold` API接口设置。
      - RXFIFO 有效数据大于等于设置阈值时，会触发DMA传输。阈值设置可以通过 `spi_set_rx_fifo_threshold` API接口设置。
      - 使用DMA收发API参考 SPI组件 :ref:`spi_component <spi_component>`

      .. code-block:: c

            hpm_stat_t spi_setup_dma_transfer(SPI_Type *ptr, spi_control_config_t *config,
                        uint8_t *cmd, uint32_t *addr, uint32_t wcount, uint32_t rcount);

      - 参数说明：

          .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - SPI_Type*
              - 指向SPI控制器基地址的指针，用于指定要配置的SPI控制器。
            * - config
              - spi_control_config_t*
              - 指向SPI控制配置结构体的指针，包含SPI控制配置信息
            * - cmd
              - uint8_t*
              - 指向命令值的指针，用于指定命令值。
            * - addr
              - uint32_t*
              - 指向地址值的指针，用于指定地址值。
            * - wcount
              - uint32_t
              - 发送数据的字节数。
            * - rcount
              - uint32_t
              - 接收数据的字节数。

      - 返回值：

          - `status_success`：DMA传输初始化设置成功。
          - `status_invalid_argument`：参数无效。
          - `status_fail`：DMA传输初始化设置失败。

      举例：

        - 主机模式下，双线DSPI，开启命令阶段，命令值为0x01，地址阶段使能（DSPI格式），地址值为0x02，传输模式为 `spi_trans_write_read`，数据收发长度为100字节。

          .. code-block:: c

                #include "hpm_clock_drv.h"
                #include "hpm_spi_drv.h"
                void spi_setup_dma_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t cmd = 0x01;
                    uint32_t addr = 0x02;
                    uint8_t wbuff[100] = {0};
                    uint8_t rbuff[100] = {0};
                    /* 初始化SPI控制器引脚，不做举例.. */
                    /* 使能SPI时钟源，不做举例.. */
                    /* 设置SPI主机模式的SCLK时钟频率，不做举例..*/
                    /* 设置SPI主机模式的SPI格式配置，不做举例..*/
                    spi_master_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_read; /* 设置传输模式为先发送数据再接收数据 */
                    control_config.common_config.data_phase_fmt = spi_dual_io_mode; /* 设置数据阶段的格式为双线传输顺序 */
                    control_config.master_config.cmd_enable = true; /* 开启命令阶段 */
                    control_config.master_config.addr_enable = true; /* 开启地址阶段 */
                    control_config.master_config.addr_phase_fmt = spi_address_phase_format_dualquad_io_mode; /* 设置地址阶段的格式为DSPI格式 */
                    if (spi_setup_dma_transfer(HPM_SPI1, &control_config, &cmd, &addr, 100, 100)!= status_success) {
                        /* DMA传输初始化设置失败 */
                        printf("spi_setup_dma_transfer failed\n");
                        return;
                    }
                    /* 配置DMA的通道，DMA源地址和设备地址等等，启动DMA传输，不做举例，参考 `spi示例`的dma相关 */
                }

        - 主机模式下，四线QSPI，禁能命令阶段和地址阶段，传输模式为 `spi_trans_write_only`，数据发送长度为100字节。

          .. code-block:: c

                void spi_setup_dma_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t wbuff[100] = {0};
                    /* 初始化SPI控制器引脚，不做举例.. */
                    /* 使能SPI时钟源，不做举例.. */
                    /* 设置SPI主机模式的SCLK时钟频率，不做举例..*/
                    /* 设置SPI主机模式的SPI格式配置，不做举例..*/
                    spi_master_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_only; /* 设置传输模式为先发送数据 */
                    control_config.common_config.data_phase_fmt = spi_quad_io_mode; /* 设置数据阶段的格式为四线传输顺序 */
                    control_config.master_config.cmd_enable = false; /* 禁能命令阶段 */
                    control_config.master_config.addr_enable = false; /* 禁能地址阶段 */
                    if (spi_setup_dma_transfer(HPM_SPI1, &control_config, NULL, NULL, 100, 0)!= status_success) {
                        /* DMA传输初始化设置失败 */
                        printf("spi_setup_dma_transfer failed\n");
                        return;
                    }
                    /* 配置DMA的通道，DMA源地址和设备地址等等，启动DMA传输，不做举例，参考 `spi示例`的dma相关 */
                }

        - 从机模式下，四线QSPI，从机纯数据模式，传输模式为 `spi_trans_write_read_together`，数据收发长度为100字节。

         .. code-block:: c

                void spi_setup_dma_transfer_example(void)
                {
                    spi_control_config_t control_config;
                    uint8_t wbuff[100] = {0};
                    uint8_t rbuff[100] = {0};
                    /* 初始化SPI控制器引脚，不做举例.. */
                    /* 使能SPI时钟源，不做举例.. */
                    /* 设置SPI从机模式的SPI格式配置，不做举例..*/
                    spi_slave_get_default_format_config(&control_config);
                    control_config.common_config.trans_mode = spi_trans_write_read_together; /* 设置传输模式为同时读写传输顺序 */
                    control_config.common_config.data_phase_fmt = spi_quad_io_mode; /* 设置数据阶段的格式为四线传输顺序 */
                    control_config.slave_config.slave_data_only = true; /* 设置从机纯数据模式 */
                    if (spi_setup_dma_transfer(HPM_SPI1, &control_config, NULL, NULL, 100, 100)!= status_success) {
                        /* DMA传输初始化设置失败 */
                        printf("spi_setup_dma_transfer failed\n");
                        return;
                    }
                    /* 配置DMA的通道，DMA源地址和设备地址等等，启动DMA传输，不做举例，参考 `spi示例`的dma相关 */
                }

中断
--------------

- 需要打开plic控制器对应的SPI中断，使用 `intc_m_enable_irq` API接口打开SPI中断。
- 需要在plic控制器中配置SPI中断的优先级，使用 `intc_m_enable_irq_with_priority` API接口配置SPI中断的优先级。
- 每次对应的中断事件发生，需要在中断处理函数中调用 `spi_clear_interrupt_status` API接口清除中断标志。
- 每次对应的中断事件发生，可以在中断处理函数中调用 `spi_get_interrupt_status` API接口获取中断标志。
- SPI外设支持以下中断，可以从 `spi_interrupt_t` 枚举类型中查看。

  .. code-block:: c

        typedef enum {
            spi_rx_fifo_overflow_int  = SPI_INTREN_RXFIFOORINTEN_MASK,   /* 接收FIFO溢出中断 */
            spi_tx_fifo_underflow_int = SPI_INTREN_TXFIFOURINTEN_MASK,   /* 发送FIFO下溢中断 */
            spi_rx_fifo_threshold_int = SPI_INTREN_RXFIFOINTEN_MASK,     /* 接收FIFO阈值中断 */
            spi_tx_fifo_threshold_int = SPI_INTREN_TXFIFOINTEN_MASK,     /* 发送FIFO阈值中断 */
            spi_end_int               = SPI_INTREN_ENDINTEN_MASK,        /* 传输结束中断 */
            spi_slave_cmd_int         = SPI_INTREN_SLVCMDEN_MASK,        /* 从机命令中断 */
        #if defined(HPM_IP_FEATURE_SPI_CS_EDGE_DETECT_FOR_SLAVE) && (HPM_IP_FEATURE_SPI_CS_EDGE_DETECT_FOR_SLAVE == 1)  /* 如果当前SOC支持硬件CS边沿检测 */
            spi_slave_cs_edge_falling_int = SPI_INTREN_CS_NEGEN_MASK,    /* 从机CS下降沿中断 */
            spi_slave_cs_edge_rising_int = SPI_INTREN_CS_POSEN_MASK,     /* 从机CS上升沿中断 */
        #endif
        } spi_interrupt_t;

  - 其中，`spi_rx_fifo_overflow_int` 、 `spi_tx_fifo_underflow_int` 、 `spi_slave_cmd_int` 、`spi_slave_cs_edge_falling_int` 、`spi_slave_cs_edge_rising_int` 仅支持从机模式。
  - `spi_rx_fifo_threshold_int` 当spi_set_rx_fifo_threshold设置的阈值大于等于接收FIFO有效数据时，会触发中断。比如SPI的单位宽度为8bit，spi_set_rx_fifo_threshold设置的阈值为5字节，当接收FIFO有效数据大于等于5字节，会触发中断。此时可以在中断处理函数中读取接收FIFO的数据。
  - `spi_tx_fifo_threshold_int` 当spi_set_tx_fifo_threshold设置的阈值小于等于发送FIFO剩余空间时，会触发中断。比如SPI的单位宽度为8bit，spi_set_tx_fifo_threshold设置的阈值为5字节，当发送FIFO剩余空间小于等于5字节，会触发中断。此时可以在中断处理函数中写入发送FIFO的数据。

- 相关API接口：

  - 使能SPI中断

    .. code-block:: c

            void spi_enable_interrupt(SPI_Type *ptr, uint32_t mask);

    - 参数说明：

      .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - SPI_Type*
              - 指向SPI控制器基地址的指针，用于指定要配置的SPI控制器。
            * - mask
              - uint32_t
              - 中断掩码，用于指定要使能的中断。对应的中断掩码可以从 `spi_interrupt_t` 枚举类型中查看。

    - 返回值：

        - 无

  - 禁用SPI中断

    .. code-block:: c

            void spi_disable_interrupt(SPI_Type *ptr, uint32_t mask);

    - 参数说明：

     .. list-table::
            :header-rows: 1

            * - 参数名
              - 类型
              - 描述
            * - ptr
              - SPI_Type*
              - 指向SPI控制器基地址的指针，用于指定要配置的SPI控制器。
            * - mask
              - uint32_t
              - 中断掩码，用于指定要禁用的中断。对应的中断掩码可以从 `spi_interrupt_t` 枚举类型中查看。

    - 返回值：

      - 无


SPI示例
--------

.. toctree::
   :maxdepth: 3

   dma/master/README_zh
   dma/slave/README_zh
   interrupt/master/README_zh
   interrupt/slave/README_zh
   master_trans_large_amount_of_data/README_zh
   polling/master/README_zh
   polling/slave/README_zh

