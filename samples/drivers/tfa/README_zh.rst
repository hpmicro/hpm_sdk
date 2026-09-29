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
   TFA支持最高4层嵌套计算，超过4层嵌套时，最外层计算将丢失结果，会在回到最外层时重新计算。
   该工程默认打开中断嵌套保护。
   如用户确认当前环境已经屏蔽中断，不会发生中断嵌套，导致正在计算的数据被新的中断中可能发生的数据计算打断并破坏，可在 cmakelist 中使能宏 CONFIG_TFA_NOT_USE_IRQ_NEST_PROTECTION 关闭嵌套保护以获得更高性能。

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
   tfa and math diff value:0.000000, math calculation time:24 ticks, tfa calculation time:57 ticks.
   1
   tfa and math diff value:0.000000, math calculation time:25 ticks, tfa calculation time:58 ticks.
   2
   tfa and math diff value:0.000000, math calculation time:307 ticks, tfa calculation time:58 ticks.
   3
   tfa and math diff value:0.000000, math calculation time:309 ticks, tfa calculation time:58 ticks.
   4
   tfa and math diff value:0.000000, math calculation time:358 ticks, tfa calculation time:58 ticks.
   5
   tfa and math diff value:-0.000000, math calculation time:291 ticks, tfa calculation time:58 ticks.
   6
   tfa and math diff value:-0.000000, math calculation time:535 ticks, tfa calculation time:58 ticks.
   7
   tfa and math diff value:0.000000, math calculation time:43 ticks, tfa calculation time:58 ticks.
   8
   tfa and math diff value:0.000000, math calculation time:513 ticks, tfa calculation time:121 ticks.
   9
   math angle value:6.981057, tfa angle value:6.980525, tfa and math angle diff value:-0.000533,math mode value:98.731964, tfa mode value:98.000000, tfa and math mode diff value:-0.731964,math calculation time:401 ticks, tfa calculation time:80 ticks.

菜单 A 测试基于前 7 种运算的复杂计算，对比库函数的结果误差与周期，结果如下：

.. code-block:: console

   A
   tfa and math diff value:-0.000000, math calculation time:1269 ticks, tfa calculation time:288 ticks.

Note: 菜单 4 POW2 TEST 中用到的数学函数库计算函数 powf 在 zcc 4.1.5 工具链下，无法计算出正确值; 该问题已经在 zcc 4.1.8 fix，用户可以升级到 4.1.8 进行计算。




