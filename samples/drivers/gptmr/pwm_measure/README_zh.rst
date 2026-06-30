.. _gptmr_measure_pwm:

GPTMR测量PWM
====================

概述
------

pwm_measure示例工程演示了GPTMR测量PWM波的功能。

硬件设置
------------

请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
连接外部PWM至GPTMR输入捕获引脚。

运行现象
------------

运行工程，示例执行GPTMR的测量PWM波的功能，若有PWM波输入，串口终端输出如下信息，分别是PWM波的频率和占空比。

.. code-block:: console

   gptmr pwm measure test
   [table_0]measured frequency: 1000.049987
   [table_0]measured duty cycle: 50.00%
   [table_1]measured frequency: 1000.049987
   [table_1]measured duty cycle: 50.00%
   [table_2]measured frequency: 1000.049987
   [table_2]measured duty cycle: 50.00%
   [table_3]measured frequency: 1000.049987
   [table_3]measured duty cycle: 50.00%
   [table_4]measured frequency: 1000.049987
   [table_4]measured duty cycle: 50.00%
   [table_5]measured frequency: 1000.049987
   [table_5]measured duty cycle: 50.00%
   [table_6]measured frequency: 1000.049987
   [table_6]measured duty cycle: 50.00%
   [table_7]measured frequency: 1000.049987
   [table_7]measured duty cycle: 50.00%
   [table_8]measured frequency: 1000.049987
   [table_8]measured duty cycle: 50.00%
   [table_9]measured frequency: 1000.059997
   [table_9]measured duty cycle: 50.00%

GPTMR V2 注意事项
---------------------

GPTMR V2 相比 V1 增加了 ``prescaler`` 预分频器字段，可通过宏 ``APP_GPTMR_TARGET_FREQ`` 配置目标计数器时钟频率：

.. code-block:: c

   #ifdef HPMSOC_HAS_HPMSDK_GPTMRV2
   config.prescaler = gptmr_freq / APP_GPTMR_TARGET_FREQ;
   #endif

- 预分频器公式：**计数器时钟 = 源时钟 / prescaler**
- V2 的 reload/cmp 寄存器为 24-bit（最大 16,777,215），V1 为 32-bit

测量范围估算（假设源时钟 100MHz）：

.. list-table::
   :header-rows: 1

   * - APP_GPTMR_TARGET_FREQ
     - prescaler
     - 每tick时间
     - 最高可靠频率
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

   - 增大 ``APP_GPTMR_TARGET_FREQ`` 可提高测量精度和可测最高频率，但会牺牲低频测量能力
   - V2 中 prescaler 必须 ≥ 1（默认值 1），否则 ``gptmr_channel_config()`` 返回 ``status_invalid_argument``

