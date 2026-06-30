.. _gptmr_input_captrue:

GPTMR输入捕获例子
======================

概述
------

input_captrue示例工程演示了GPTMR输入捕获的功能.

硬件设置
------------

请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
连接外部PWM至GPTMR输入捕获引脚。

运行现象
------------

运行工程，示例执行GPTMR的输入捕获的功能，串口终端输出如下信息，打印出每次捕获到的边沿间隔时间,也就是一个周期的时间。

.. code-block:: console

   gptmr input capture test
   capture the pulse width : 100001 tick
   capture the pulse width : 100000 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100000 tick
   capture the pulse width : 100000 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100000 tick
   capture the pulse width : 100001 tick
   capture the pulse width : 100001 tick

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
