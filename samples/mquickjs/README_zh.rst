.. _mquickjs:

mquickjs
======================

概述
------

**mquickjs** 示例展示了如何在 HPMicro MCU 上运行 MicroQuickJS（轻量级 JavaScript 引擎）。

该示例包含以下内容：

- 初始化 JavaScript 运行时（本示例固定堆内存为 32 KB）
- 执行内嵌 JavaScript 演示代码
- 通过 JavaScript ``HPM_Pin`` API 控制板载 LED
- 进入简易 REPL，可通过串口终端输入并执行 JavaScript 命令

提示
------

- 工程在 ``CMakeLists.txt`` 中通过 ``CONFIG_MQUICKJS=1`` 使能 mquickjs。
- mquickjs 支持 JavaScript 的子集（偏 ES5 严格模式）：

  - 不支持箭头函数语法（``=>``）
  - 不支持 ``class`` 语法
  - 不支持 ``let``/``const``（请使用 ``var``）

硬件设置
------------

- 使用调试串口将开发板连接到主机。
- 确保板载用户 LED 可用（示例使用 ``BOARD_LED_GPIO_NAME``）。

运行现象
------------

工程正确运行后，串口终端会先输出启动信息并执行内嵌 JS 示例，随后进入 REPL。

输出类似如下：

.. code-block:: console

   ========================================
       HPM SDK + mquickjs Example
       MicroQuickJS for Embedded Systems
   ========================================

   Initializing JavaScript engine...
   Memory size: 32768 bytes
   JavaScript engine ready!

   --- Running embedded JavaScript example ---
   === HPM SDK + mquickjs Demo ===
   ...
   Blinking complete!

   mquickjs REPL (type 'exit()' to quit)
   js>

REPL 使用说明
----------------

- 在提示符下输入 JavaScript 表达式或语句，按 Enter 执行。
- 对于返回值非 ``undefined`` 的表达式，终端会自动打印结果。
- 输入 ``exit()`` 或 ``quit`` 可退出 REPL。