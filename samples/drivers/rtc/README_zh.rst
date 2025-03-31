.. _rtc:

RTC
======

概述
------

RTC 例程提供如下功能：
- 设置时间

- 读取时间

- 设置闹钟

- 使能电池域的寄存器保持功能

- 设置闹钟中断

硬件设置
------------

无特殊设置

运行现象
------------

当该例程成功运行后，程序会在串口终端上打印如下菜单:

.. code-block:: console

   ---------------------------------------------------------------------------
   *                                                                         *
   *                                   RTC Demo                              *
   *                                                                         *
   *       1. Show RTC time                                                  *
   *       2. Demonstrate 3 types of RTC alarm                               *
   *       3. Demonstrate the retention mode of Battery Domain               *
   *       4. Set RTC Time                                                   *
   *       5. Demonstrate RTC interrupt                                      *
   *                                                                         *
   ---------------------------------------------------------------------------

在串口终端中按以上菜单中的任一数字来执行相应的示例。

注意：
---------

示例#3 会触发系统复位，建议只在`flash_xip`或`flash_sdram_xip`相关的构建中执行该示例