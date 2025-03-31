.. _lvgl_coremark_demo:

LVGL Coremark 示例
================================

概述
------

双核 LVGL Coremark示例用于执行两核Coremark并显示在图形界面上

在本工程中:
 - 点击 “Start”按键执行双核CoreMark

 - 点击“频率切换”热键执行CPU主频切换

硬件设置
------------

  BOOT_PIN 应该设置为：0-OFF, 1-OFF

生成、编译和调试多核工程
------------------------------------

请参见 :ref:`Multicore 通用说明 <multicore_general_description>`

运行现象
------------

- 点击 “Start”按键执行双核CoreMark， 当Coremark程序完成后，会在相应的位置显示Coremark分数

- 点击“频率切换”热键执行CPU主频切换
