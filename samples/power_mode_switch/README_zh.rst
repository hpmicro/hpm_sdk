.. _power_mode_switch:

Power Mode Switch
==================================

概述
------

power_mode_switch示例工程展示了如何切换不同的工作模式

硬件设置
------------

- 有PUART的开发板

  - 使用PUART作为Console

  - 使用PUART作为唤醒源

  - 将串口(PUART, 具体引脚信息请参考 :ref:`PUART引脚 <board_resource>` )连接到PC.

- 没有PUART的开发板

  - 使用正常的UART Console

  - 使用WKUP引脚作为唤醒源

注意
------------

- 当用调试器下载完固件之后, 需要将开发板重新上电(从flash启动)以运行该示例程序

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：


.. code-block:: console

   Power mode switch demo:
     1 - Enter wait mode
     2 - Enter stop mode
     3 - Enter standby mode
     4 - Enter shutdown mode


在串口终端输入对应的选项进入不同的工作模式
