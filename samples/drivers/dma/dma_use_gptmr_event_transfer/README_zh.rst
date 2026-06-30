.. _dma_use_gptmr_event_transfer:

定时器触发请求DMA传输
========================

概述
------

- 本示例工程展示了使用定时器事件触发请求DMA传输。

- DMA传输的设备地址是板载的LED的GPIO口的TOGGLE寄存器。当发生对应定时器事件，对应的LED会翻转电平进行闪烁。

硬件设置
------------

请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
- 使用串口调试助手进行输入定时器请求配置。

- 请求配置为输入捕获翻转时，需要连接外部PWM至GPTMR输入捕获引脚。

- 串口波特率设置为 ``115200bps`` ，``1个停止位`` ，``无奇偶校验位``

注意
------

- 当使用input_signal_toggle事件触发时，所使用的PWM输入建议在30Hz以内，以便可观察LED闪烁情况，若高于30Hz输入请使用示波器查看对应的LED GPIO引脚变化

GPTMR V2 注意事项
---------------------

GPTMR V2 相比 V1 增加了 ``prescaler`` 预分频器字段，可通过宏 ``APP_GPTMR_TARGET_FREQ`` 配置目标计数器时钟频率：

.. code-block:: c

   #ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
   config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
   reload_ticks = ((uint64_t)APP_BOARD_RELOAD_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
   cmp_ticks = ((uint64_t)APP_BOARD_CMP_MS * gptmr_freq) / ((uint64_t)config.prescaler * 1000);
   #endif

- 预分频器公式：**计数器时钟 = 源时钟 / prescaler**
- V2 的 reload/cmp 寄存器为 24-bit（最大 16,777,215），V1 为 32-bit
- ``APP_BOARD_RELOAD_MS`` 与 ``APP_BOARD_CMP_MS`` 分别控制定时器重载周期和比较周期，默认值分别为 500ms 和 100ms

input_signal_toggle 输入频率范围
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

当选择 ``gptmr_dma_request_on_input_signal_toggle``（菜单项2）时，GPTMR 工作于 ``gptmr_work_mode_capture_at_both_edge`` 双边沿捕获模式，
输入 PWM 频率的可测范围受计数器位宽和 prescaler 共同影响：

- GPTMR V2 计数器为 24-bit，最大计数值 16,777,215；V1 为 32-bit
- 输入信号周期 = 相邻同向边沿之间的 tick 差值 × 每tick时间

输入频率范围估算（假设源时钟 100MHz）：

.. list-table::
   :header-rows: 1

   * - APP_GPTMR_TARGET_FREQ
     - prescaler
     - 每tick时间
     - 最高可测频率
     - 最低可测频率
   * - 100KHz (默认)
     - 1000
     - 10 µs
     - ~50KHz
     - ~0.006 Hz
   * - 1MHz
     - 100
     - 1 µs
     - ~500KHz
     - ~0.06 Hz
   * - 10MHz
     - 10
     - 100 ns
     - ~5MHz
     - ~0.6 Hz
   * - 100MHz (prescaler=1)
     - 1
     - 10 ns
     - ~50MHz
     - ~6 Hz

.. note::

   - 为便于肉眼观察 LED 闪烁，输入 PWM 建议在 30Hz 以内；高于 30Hz 请使用示波器查看 GPIO 引脚变化
   - ``APP_GPTMR_TARGET_FREQ`` 影响输入捕获分辨率和可测频率范围，请根据实际输入信号频率选择合适的值

运行现象
------------

- 使用串口调试助手，敲击enter按键会打印提示，根据提示选择对应的事件触发，板载的LED会进行不同频率的闪烁。

- demo菜单中0、1、3如果若需要改变LED的闪烁频率，可以改变代码中的缺省宏APP_BOARD_RELOAD_MS和APP_BOARD_CMP_MS，缺省下分别为500ms和100ms。2的LED闪烁效果由GPTMR的输入捕获引脚的输入PWM决定。

- 信息如下：


.. code-block:: console

   -----------------------------------------------------------------------------------
   *                                                                                 *
   *                   trigger dma transfer using gptmr event demo                   *
   *                                                                                 *
   *        0. use gptmr cmp0 event                                                  *
   *        1. use gptmr cmp1 event                                                  *
   *        2. use gptmr input_signal_toggle event                                   *
   *        3. use gptmr reload event                                                *
   *                                                                                 *
   *---------------------------------------------------------------------------------*

