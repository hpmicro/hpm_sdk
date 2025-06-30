.. _gptmr:

GPTMR
=====


概述
----


- 主要特征功能

  - 主要介绍先楫的gptmr外设的主要驱动接口说明和调用方法。更多内容请参考 `hpm_gptmr_drv.h` 的API说明以及相关用户手册。
  - 每个定时器拥有4个独立通道，每个通道配置了一个32位分辨率的计数器（仅支持向上模式）、可配置的32位重装载寄存器。
  - 定时器的每个通道的计数器支持同步。 可参考 :ref:`gptmr_sync_counter <gptmr_sync_counter>` 例子
  - 定时器每个通道可同时支持输入捕获和输出比较，可参考 :ref:`gptmr_input_captrue <gptmr_input_captrue>` 例子， :ref:`gptmr_output_compare <gptmr_output_compare>` 例子
  - 定时器每个通道支持两个输出比较器，生成边沿PWM和中心对齐PWM。 可参考 :ref:`gptmr_generate_pwm <gptmr_generate_pwm>` 例子
  - 定时器每个通道支持上升沿/下降沿捕获，支持硬件测量PWM周期和占空比。可参考 :ref:`gptmr_measure_pwm <gptmr_measure_pwm>` 例子
  - 支持外部信号计数。 可参考 :ref:`gptmr_external_counter_mode <gptmr_external_counter_mode>` 例子
  - 支持生成DMA请求。可参考 :ref:`dma_use_gptmr_event_transfer <dma_use_gptmr_event_transfer>` 例子
  - 定时器支持单次模式、burst模式、循环模式。可参考 :ref:`gptmr_one_shot_counter_mode <gptmr_one_shot_counter_mode>` 例子
  - 支持中断请求。可参考 :ref:`timer_basic <timer_basic>` 例子

GPTMR通道配置
---------------


- 需要确保GPTMR的时钟源已经开启，并且初始化了相关GPTMR外设引脚。

  - 可使用`clock_add_to_group` 函数用于将GPTMR时钟源添加到时钟组中，从而确保GPTMR时钟源已经开启。

- GPTMR每个通道支持独立配置，可配置不同功能，比如输入捕获、输出比较、PWM等。

- 相关枚举值介绍：

  - 定时器通道硬件输入同步触发的边沿

    .. code-block:: c

        typedef enum gptmr_synci_edge {

            /* 无触发 */
            gptmr_synci_edge_none = 0,

            /* 下降沿触发 */
            gptmr_synci_edge_falling = GPTMR_CHANNEL_CR_SYNCIFEN_MASK,

            /* 上升沿触发 */
            gptmr_synci_edge_rising = GPTMR_CHANNEL_CR_SYNCIREN_MASK,

            /* 上升沿和下降沿都触发 */
            gptmr_synci_edge_both = gptmr_synci_edge_falling | gptmr_synci_edge_rising,
        } gptmr_synci_edge_t;

    - 这个枚举是用于配置定时器通道的硬件输入同步触发的边沿。硬件SYNCI输入信号是通过电机系统中的TRGM信号连接到定时器通道的内部SYNCI输入上。
    - 可以使用 `void gptmr_channel_select_synci_valid_edge` API函数来选择定时器通道的硬件输入同步触发的边沿。
    - 对于没有电机系统的外设，比如HPM6800，硬件同步SYNCI不是通过 TRGM 来的，而是来自GPTMR7的CMP0~3，GPTMR7的每一个CMP通道分别控制两个GPTMR。

  - 定时器通道输入捕获工作模式

    .. code-block:: c

        typedef enum gptmr_work_mode {

            /* 无捕获 */
            gptmr_work_mode_no_capture = 0,

            /* 上升沿捕获 */
            gptmr_work_mode_capture_at_rising_edge = 1,

            /* 下降沿捕获 */
            gptmr_work_mode_capture_at_falling_edge = 2,

            /* 上升沿和下降沿都捕获 */
            gptmr_work_mode_capture_at_both_edge = 3,

            /* 测量PWM模式 */
            gptmr_work_mode_measure_width = 4,
        } gptmr_work_mode_t;

    - 这个枚举是用于配置定时器通道的输入捕获工作模式。
    - 可以使用 `gptmr_channel_set_capmode` API函数来选择定时器通道的输入捕获工作模式。
    - 可以使用 `gptmr_channel_get_capmode` API函数来获取定时器通道的输入捕获工作模式。
    - 在 `gptmr_work_mode_measure_width` 模式下，捕获到输入信号上升沿时，会产生对应的中断事件或者DMA请求。

  - 定时器通道DMA请求事件

    .. code-block:: c

        typedef enum gptmr_dma_request_event {

            /* CMP0 DMA请求： 当计数器到达CMP0的时，定时器通道会请求一次DMA */
            gptmr_dma_request_on_cmp0 = 0,

            /* CMP1 DMA请求：当计数器到达CMP1的时，定时器通道会请求一次DMA */
            gptmr_dma_request_on_cmp1 = 1,

            /* 输入信号翻转DMA请求：输入捕获信号发生翻转时，定时器通道会请求一次DMA*/
            gptmr_dma_request_on_input_signal_toggle = 2,

            /* 重载DMA请求：当计数器达到重装载reload时，定时器通道会请求一次DMA */
            gptmr_dma_request_on_reload = 3,

            /* 关闭DMA请求 */
            gptmr_dma_request_disabled = 0xFF,
        } gptmr_dma_request_event_t;

    - 这个枚举是用于配置定时器通道的DMA请求事件。

  - 定时器获取计数器类型

    .. code-block:: c

        typedef enum gptmr_counter_type {

            /* 获取上升沿捕获的计数器值 */
            gptmr_counter_type_rising_edge = 0,

            /* 获取下降沿捕获的计数器值 */
            gptmr_counter_type_falling_edge,

            /* 获取PWM模式下的PWM宽度 */
            gptmr_counter_type_measured_period,

            /* 获取PWM模式下的PWM占空比 */
            gptmr_counter_type_measured_duty_cycle,

            /* 获取正常模式下的计数器值 */
            gptmr_counter_type_normal,
        } gptmr_counter_type_t;

    - 可以使用 `gptmr_channel_get_counter` API函数来获取定时器通道的各类类型的计数器值。


  - 定时器计数器模式

    .. code-block:: c

        typedef enum gptmr_counter_mode {

            /* 内部计数器模式 */
            gptmr_counter_mode_internal = 0,

            /* 外部计数器模式 */
            gptmr_counter_mode_external,
        } gptmr_counter_mode_t;

    - 仅适用于 `hpm_soc_ip_feature.h` 有 `HPM_IP_FEATURE_GPTMR_CNT_MODE` 宏定义的SOC

  - 定时器通道监控信号类型

    .. code-block:: c

        typedef enum gptmr_channel_monitor_type {

            /* 监控输入信号的周期 */
            monitor_signal_period = 0,

            /* 监控输入信号的高电平时间 */
            monitor_signal_high_level_time,
        } gptmr_channel_monitor_type_t;

    - 仅适用于 `hpm_soc_ip_feature.h` 有 `HPM_IP_FEATURE_GPTMR_MONITOR` 宏定义的SOC

- 相关结构体介绍：

  - 定时器通道监控信号配置

    .. code-block:: c

        typedef struct gptmr_channel_monitor_config {

            /* 监控信号类型 */
            gptmr_channel_monitor_type_t monitor_type;

            /* 监控信号的最大和最小值 */
            uint32_t max_value;
            uint32_t min_value;
        } gptmr_channel_monitor_config_t;

  - 定时器通道配置

    .. code-block:: c

        typedef struct gptmr_channel_config {

            /* 输入捕获的工作模式 */
            gptmr_work_mode_t mode;

            /* DMA请求事件 */
            gptmr_dma_request_event_t dma_request_event;

            /* 输入捕获的同步触发边沿 */
            gptmr_synci_edge_t synci_edge;

            /* 两个输出比较的比较值 */
            uint32_t cmp[GPTMR_CH_CMP_COUNT];

            /* 重装载值 */
            uint32_t reload;

            /* 输出比较的极性 */
            bool cmp_initial_polarity_high;

            /* 是否使能输出比较 */
            bool enable_cmp_output;

            /* 是否使能该通道计数器会随着上一通道计数器的一起重载。 该参数不适用于通道0 */
            bool enable_sync_follow_previous_channel;

            /* 是否使能软件同步 */
            bool enable_software_sync;

            /* 是否使能调试模式 */
            bool debug_mode;

            /* 当前SOC是否支持监控信号 */
        #if defined(HPM_IP_FEATURE_GPTMR_MONITOR) && (HPM_IP_FEATURE_GPTMR_MONITOR == 1)

            /* 是否使能监控信号 */
            bool enable_monitor;
            /* 监控信号配置 */
            gptmr_channel_monitor_config_t monitor_config;
        #endif

            /* 当前SOC是否支持外部计数器模式 */
        #if defined(HPM_IP_FEATURE_GPTMR_CNT_MODE) && (HPM_IP_FEATURE_GPTMR_CNT_MODE == 1)

            /* 计数器模式 */
            gptmr_counter_mode_t counter_mode;
        #endif

            /* 当前SOC是否支持OPMODE 循环模式或者单次模式 */
        #if defined(HPM_IP_FEATURE_GPTMR_OP_MODE) && (HPM_IP_FEATURE_GPTMR_OP_MODE == 1)
            bool enable_opmode;
        #endif
        } gptmr_channel_config_t;

    - 单次模式，计时器到达重载值后会停止计时，软件需要清除CEN 后重新置位 CEN，开始下一次计数。下一次开启可以直接使用 `gptmr_start_counter` API函数来开启，内部已做了清除步骤。
    - 单次模式下，若需要使用重载中断，则需要在重载中断中关闭计数器使能、复位计数器、清除重载中断状态，方可清除重载中断。推荐使用CMP比较中断。可参考 :ref:`gptmr_one_shot_counter_mode <gptmr_one_shot_counter_mode>` 例子

- 缺省配置API:

  - 建议在配置gptmr通道前，先使用 `gptmr_channel_get_default_config` API函数获取默认配置，然后根据需要修改配置。

  .. code-block:: c

        void gptmr_channel_get_default_config(GPTMR_Type *ptr, gptmr_channel_config_t *config);

  - 默认配置如下：

    - 输入捕获工作模式： `gptmr_work_mode_no_capture`
    - DMA请求事件： `gptmr_dma_request_disabled`
    - 输入捕获的同步触发边沿： `gptmr_synci_edge_none`
    - 两个输出比较的比较值： `0xFFFFFFFEUL`
    - 重装载值： `0xFFFFFFFEUL`
    - 输出比较的极性： `high`
    - 是否使能输出比较： `true`
    - 是否使能该通道计数器会随着上一通道计数器的一起重载。 该参数不适用于通道0： `false`
    - 是否使能软件同步： `true`
    - 是否使能调试模式： `true`
    - 是否使能监控信号： `false`
    - 计数器模式： `gptmr_counter_mode_internal`
    - 是否使能OPMODE 循环模式或者单次模式： `false`

  - **注意** ：

    - 输出比较支持快速输出逻辑 0 和逻辑 1，即把 CMP0 配置为 32’hFFFFFFFF 时，通道输出始终为逻辑 0。把CMP1 配置为 32’hFFFFFFFF，且 CMP0 不为全 1 时，通道输出始终为逻辑 1。
    - 默认使能输出比较，如果需要恒定输出高电平或者低电平，则需要配置对应的输出比较的比较值为0xFFFFFFFFUL。

- 定时器通道配置API:

  .. code-block:: c

        hpm_stat_t gptmr_channel_config(GPTMR_Type *ptr, uint8_t ch_index,gptmr_channel_config_t *config, bool enable);

  - 参数说明：

    .. list-table::
        :header-rows: 1

        * - 参数名
          - 类型
          - 描述
        * - ptr
          - GPTMR_Type *
          - 指向GPTMR控制器的指针
        * - ch_index
          - uint8_t
          - 通道索引，取值范围为0-3
        * - config
          - gptmr_channel_config_t *
          - 指向通道配置结构体的指针
        * - enable
          - bool
          - 是否使能通道的计数器开启计数

  - 返回值：

    .. list-table::
        :header-rows: 1

        * - 返回值
          - 描述
        * - status_invalid_argument
          - 无效的参数
        * - status_success
          - 配置成功

  - 举例：

    - 配置定时器0通道0定时1ms

      .. code-block:: c

            gptmr_channel_config_t config;
            uint32_t gptmr_freq;
            gptmr_channel_get_default_config(HPM_GPTMR0, &config);
            gptmr_freq = clock_get_frequency(clock_gptmr0); /* 获取GPTMR0的时钟频率 */
            config.mode = gptmr_work_mode_no_capture;  /* 无捕获 */
            config.reload = gptmr_freq / (1000 * 1);  /* 1ms */
            config.enable_cmp_output = false;        /* 不使能输出比较 */
            config.enable_software_sync = false;      /* 不使能软件同步 */
            gptmr_channel_config(HPM_GPTMR0, 0, &config, true);

    - 配置定时器0通道1生成100KHz，50%占空比的边沿对齐PWM

      .. code-block:: c

            gptmr_channel_config_t config;
            uint32_t gptmr_freq;
            gptmr_channel_get_default_config(HPM_GPTMR0, &config);
            gptmr_freq = clock_get_frequency(clock_gptmr0); /* 获取GPTMR0的时钟频率 */
            config.mode = gptmr_work_mode_no_capture;  /* 无捕获 */
            config.reload = gptmr_freq / 100000;  /* 100KHz */
            config.cmp[0] = config.reload / 2;  /* 50%占空比 */
            config.enable_cmp_output = true;        /* 使能输出比较 */
            config.enable_software_sync = false;      /* 不使能软件同步 */
            gptmr_channel_config(HPM_GPTMR0, 1, &config, true);

Burst 模式
---------------

- burst 模式：计时器到达重载值达到指定的次数（burst_cfg）后停止计数。
- 仅适用于 `hpm_soc_ip_feature.h` 有 `HPM_IP_FEATURE_GPTMR_BURST_MODE` 宏定义的SOC

- 相关枚举值介绍：

  - Burst模式下的两种计数器启动模式

    .. code-block:: c

        typedef enum gptmr_burst_counter_mode {
            gptmr_burst_counter_restart = 0,   /* 重新开始计数 */
            gptmr_burst_counter_continue,      /* 继续计数 */
        } gptmr_burst_counter_mode_t;

    - `gptmr_burst_counter_restart`：重启模式。在此模式下，启动计数器，会重置当前计数（通常从 0 开始）， GPTMR的 `BURST_COUNT` 清零 并重新开始新的突发周期。
    - `gptmr_burst_counter_continue`：继续模式。在此模式下，启动计数器时，若当前突发计数未达到配置的最大次数（由 `BURST_CFG` 设置），则继续从当前值计数；若已达到最大次数，则重置后继续。

- **注意**：

  - 单次循环模式和Burst模式最多只能有一个使能

    - 可使用 `gptmr_channel_enable_burst_mode` API使能Burst模式，如果使能了单次循环模式，该API会返回参数错误。
    - 可使用 `gptmr_channel_disable_burst_mode` API禁能Burst模式。
    - 判断是否使能了Burst模式可使用 `gptmr_channel_is_burst_mode` API

  - 当 GPTMR的 `BURST_COUNT` 等于 `BURST_CFG` 时，`BURST_COUNT` 不会再自增, 计数器也不会再计数。如果需要开启下一次计数，需要清除 'CEN' 再置位 'CEN'，然后复位计数器 'CNTRST'。如果复位GPTMR的 `BURST_COUNT`，可间接操作复位计数器 'CNTRST'。

    - 开启Busrt模式计数，可使用 `gptmr_channel_burst_mode_start_counter` API。计数模式按照 `gptmr_burst_counter_mode_t` 配置。
    - 关闭Burst模式计数，可使用 `gptmr_channel_burst_mode_stop_counter` API。

    - 获取GPTMR的 `BURST_COUNT` 可使用 `gptmr_channel_get_current_burst_count` API。
    - 设置GPTMR的 `BURST_CFG` 可使用 `gptmr_channel_set_target_burst_count` API。
    - 获取GPTMR的 `BURST_CFG` 可使用 `gptmr_channel_get_target_burst_count` API。

- 举例：GPTMR0通道1 Burst模式配置，重载为1S，计数10次，计数模式为重启模式

  .. code-block:: c

        #include <stdio.h>
        #include "board.h"
        #include "hpm_sysctl_drv.h"
        #include "hpm_gptmr_drv.h"
        #include "hpm_debug_console.h"

        #define APP_BOARD_GPTMR               BOARD_GPTMR
        #define APP_BOARD_GPTMR_CH            BOARD_GPTMR_CHANNEL
        #define APP_BOARD_GPTMR_IRQ           BOARD_GPTMR_IRQ

        #define APP_TICK_MS                   (1000)

        static void timer_config(void);

        uint32_t target_burst_count, current_burst_count;

        SDK_DECLARE_EXT_ISR_M(APP_BOARD_GPTMR_IRQ, tick_ms_isr)
        void tick_ms_isr(void)
        {
            target_burst_count = gptmr_channel_get_target_burst_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            current_burst_count = gptmr_channel_get_current_burst_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            if (gptmr_check_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH))) {
                gptmr_clear_status(APP_BOARD_GPTMR, GPTMR_CH_RLD_STAT_MASK(APP_BOARD_GPTMR_CH));
                printf("timer tick %d ms\n", APP_TICK_MS);
                board_led_toggle();
                /* 若BURST_COUNT等于BURST_CFG，说明已经计数BURST_CFG次 */
                if (target_burst_count == current_burst_count) {
                    gptmr_channel_burst_mode_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, gptmr_burst_counter_restart);
                    printf("gptmr channel burst\n");
                }
            }
        }

        int main(void)
        {
            board_init();
            board_init_led_pins();
            init_gptmr_pins(APP_BOARD_GPTMR);
            printf("gptmr timer basic test\n");
            timer_config();
            while (1) {
            }
        }

        static void timer_config(void)
        {
            uint32_t gptmr_freq;
            gptmr_channel_config_t config;

            gptmr_freq = board_init_gptmr_clock(APP_BOARD_GPTMR);
            /* config 缺省关闭了单次模式-OP mode */
            gptmr_channel_get_default_config(APP_BOARD_GPTMR, &config);
            /* reload为1S */
            config.reload = gptmr_freq / 1000 * APP_TICK_MS;
            gptmr_channel_config(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, &config, false);

            /*设置GPTMR的 `BURST_CFG` 为10，计数10次 */
            gptmr_channel_set_target_burst_count(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH, 10);
            /* 使能Burst模式 */
            gptmr_channel_enable_burst_mode(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            /* 使能计数器 */
            gptmr_start_counter(APP_BOARD_GPTMR, APP_BOARD_GPTMR_CH);
            /* 使能 reload中断 */
            gptmr_enable_irq(APP_BOARD_GPTMR, GPTMR_CH_RLD_IRQ_MASK(APP_BOARD_GPTMR_CH));
            intc_m_enable_irq_with_priority(APP_BOARD_GPTMR_IRQ, 1);
        }




GPTMR例子
------------

.. toctree::
  :maxdepth: 3

  cntmode/README_zh
  counter_sync/README_zh
  input_capture/README_zh
  monitor/README_zh
  opmode/README_zh
  output_compare/README_zh
  pwm_generate/README_zh
  pwm_measure/README_zh
  sent_signal_decode/README_zh
  t_shape_accel_decel/README_zh
  timer_basic/README_zh
  qeimode/README_zh

