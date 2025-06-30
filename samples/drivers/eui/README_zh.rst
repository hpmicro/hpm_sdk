.. _eui:

EUI
======

概述
------

EUI 示例工程展示5个8位的LED数码管和2x3的按键矩阵功能。

运行现象
------------

- LED数码管将闪烁循环显示3组数据，每组闪烁10次。

- 按键中断在KEY按下去时触发一次，释放时又会触发一次。

- 串口调试终端会先打印时钟配置信息，然后打印按键按下的状态信息（按键释放时不打印状态信息），打印信息示例如下所示：

.. code-block:: console

  eui example

  clko_tm_us:10, slot_tm_us:320, hold_tm_us:0, disp_tm_us:2560, filter_tm_us:51200

   esc: 1, up: 0, enter: 0
  left: 0, dn: 0, right: 0

   esc: 0, up: 1, enter: 0
  left: 0, dn: 0, right: 0

   esc: 0, up: 0, enter: 1
  left: 0, dn: 0, right: 0

   esc: 0, up: 0, enter: 0
  left: 0, dn: 0, right: 1

   esc: 0, up: 0, enter: 0
  left: 0, dn: 1, right: 0

   esc: 0, up: 0, enter: 0
  left: 1, dn: 0, right: 0

   esc: 0, up: 1, enter: 0
  left: 0, dn: 0, right: 0

   esc: 1, up: 0, enter: 0
  left: 0, dn: 0, right: 0

   esc: 0, up: 0, enter: 1
  left: 0, dn: 0, right: 0

其他说明
------------

数码管更高级的闪烁控制，可使用 `components/segment_led` 组件，示例可参见 `samples/segment_led_key` 。