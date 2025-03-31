.. _gptmr_monitor:

定时器监视功能
==============

概述
------

该示例工程演示了监视输入的周期性信号，宽度超出范围(小于最小监视值，大于最大监视值)触发捕获中断。

硬件设置
------------

请参考具体开发板  :ref:`引脚描述 <board_resource>`  部分的描述。
连接外部PWM至GPTMR输入捕获引脚。

运行现象
------------

- 运行工程，示例执行定时器的监视功能，串口终端输出如下信息，若监视的周期性信号在范围内，则输出如下信息：


.. code-block:: console

   gptmr monitor test, sel: period
   gptmr freq: 100000000


- 若监视的周期性信号超出范围，则输出如下信息：


.. code-block:: console

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range

   monitor the input signal is not within range <2000, 1000000> ns
   Ensure PWM input and signal frequency range


注意
------

- 监视值单位是定时器时钟源的周期时间

- 第一次上电由于检测上升沿时间不确定，有可能出现检测错误

- 如果上电无输入信号时间超过最大监视值或者重载后，则会触发捕获中断，并且打印超出范围信息。

- 如果在周期性信号突然断开，不会立马触发捕获中断，需要等待重载时间。
