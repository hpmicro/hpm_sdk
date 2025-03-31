.. _segment_led_key:

SEGMENT LED KEY
=======================

概述
-----------

`SEGMENT LED KEY` 示例工程展示5个8位的LED数码管和2x3的按键矩阵功能，使用了 `components/segment_led` 组件，可方便控制每个数码管任意段的闪烁。

运行现象
-----------

- LED数码管小数点闪烁表示数码管处于选中状态，可通过左右按键进行数码管选择；可通过上下按键来操作选中的数码管数值。

- LED数码管DIG0为闪烁模式，数值分别代表：

  - 1 - DIG2、DIG3、DIG4都不闪；

  - 2 - DIG2闪烁，DIG3、DIG4不闪；

  - 3 - DIG3闪烁，DIG2、DIG4不闪；

  - 4 - DIG4闪烁，DIG2、DIG3不闪；

  - 5 - DIG2、DIG3、DIG4都闪烁；

  - 6 - DIG2、DIG3、DIG4部分段闪烁。

- LED数码管DIG1为空白，不显示数据。

- LED数码管DIG2~DIG5显示数值，范围为0x00~0x0F。

- 程序运行时，串口调试终端会先输出时钟配置信息；当按键按下时，串口调试终端会打印按键状态。如下所示：

.. code-block:: console

  segment led key example

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

  ```
