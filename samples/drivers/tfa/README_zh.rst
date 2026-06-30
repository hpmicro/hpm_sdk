.. _tfa:

TFA
======

概述
------

TFA 是一种硬件三角函数加速器，用于提升数学运算的性能。

支持的函数
------------
- INV(x)
- SQRT(x)
- SIN(x)
- COS(x)
- POW(2,x)
- LOG2(x)
- ATAN(x)
- INVSQRT(x)
- ATAN2(y,x)
- CORDIC(x,y)

.. note::
   ATAN2和CORDIC函数仅在部分 SoC 上支持。
   在其他 SoC 上，选择这些函数将打印"not supported"提示信息。

硬件设置
------------

无需特殊硬件设置。

工具链要求
------------

本示例需要支持DSP扩展的工具链：

- **IDE**: Segger Embedded Studio（或其他支持DSP工具链的IDE）
- **GCC工具链**: Andes RISC-V工具链（带DSP扩展）或 ZCC工具链（带DSP扩展）

.. note::
   不支持DSP扩展的标准GCC工具链无法正确编译和运行本示例。

运行程序
------------

程序默认开启FPU以获得最佳性能，同时提供关闭FPU的编译选项。

运行示例（开启FPU）：

.. code-block:: console

   *********************************************************************************
   *                                                                               *
   *                         TFA Example Menu                                      *
   *                                                                               *
   * 0 - INV TEST                                                                  *
   * 1 - SQRT TEST                                                                 *
   * 2 - SIN TEST                                                                  *
   * 3 - COS TEST                                                                  *
   * 4 - POW2 TEST                                                                 *
   * 5 - LOG2 TEST                                                                 *
   * 6 - ATAN TEST                                                                 *
   * 7 - INVSQRT TEST                                                              *
   * 8 - ANANPU2 TEST                                                              *
   * 9 - CORDIC32 OPERATION                                                        *
   * A - MIXED OPERATION                                                           *
   *                                                                               *
   *********************************************************************************

菜单 0~9 测试 TFA 的 10 种基本函数，与库函数对比结果误差和计算周期，结果如下：

.. code-block:: console

   0
   tfa and math diff value:-0.000000, math calculation time:33 ticks, tfa calculation time:62 ticks.
   1
   tfa and math diff value:0.000000, math calculation time:363 ticks, tfa calculation time:63 ticks.
   2
   tfa and math diff value:0.000000, math calculation time:1755 ticks, tfa calculation time:63 ticks.
   3
   tfa and math diff value:0.000000, math calculation time:1826 ticks, tfa calculation time:63 ticks.
   4
   tfa and math diff value:0.000000, math calculation time:3031 ticks, tfa calculation time:63 ticks.
   5
   tfa and math diff value:-0.000000, math calculation time:2875 ticks, tfa calculation time:63 ticks.
   6
   tfa and math diff value:-0.000000, math calculation time:1959 ticks, tfa calculation time:180 ticks.
   7
   tfa and math diff value:-0.000000, math calculation time:552 ticks, tfa calculation time:63 ticks.
   8
   tfa and math diff value:0.000000, math calculation time:2108 ticks, tfa calculation time:150 ticks.
   9
   math angle value:6.981058, tfa angle value:6.980525, tfa and math angle diff value:-0.000533, math mode value:98.731964, tfa mode value:98.000000, tfa and math mode diff value:-0.731960, math calculation time:2901 ticks, tfa calculation time:93 ticks.

菜单 A 测试基于前 7 种运算的复杂计算，对比库函数的结果误差与周期，结果如下：

.. code-block:: console

   A
   tfa and math diff value:-0.000000, math calculation time:9088 ticks, tfa calculation time:290 ticks.

