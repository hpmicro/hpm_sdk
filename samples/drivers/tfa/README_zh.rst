.. _tfa:

TFA
======

概述
------

示例工程展示了TFA计算的精度和性能，支持以下8种函数计算：

- INV(x)

- SQRT(x)

- SIN(x)

- COS(x)

- POW(2,x)

- log2(x)

- arctan(x)

- INV(SQRT(x))

硬件设置
------------

无需特殊的设置

运行程序
------------

默认开启FPU，以获得最佳计算性能；同时，提供了不开启FPU的方案。

当该例程成功运行后，会在串口终端打印如下菜单:


.. code-block:: console

   *********************************************************************************
   *                                                                               *
   *                         TFA Example Menu                                      *
   *                                                                               *
   * 0 - Run basic test, compare value and time with math lib                      *
   * 1 - Run interrupt test, max test 4 nest interrupt                              *
   *                                                                               *
   *********************************************************************************

开启FPU：

- 菜单选项 0 ~ 7 测试了 TFA 8种函数的基本使用方法, 计算结果和库函数计算结果的差值比较, 计算周期和库函数计算周期的比较, 运行结果如下：

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

- 菜单选项 8 测试了基于上述运算的一个复杂计算, 计算结果和库函数计算结果的差值比较, 计算周期和库函数计算周期的比较, 运行结果如下：

.. code-block:: console

   8
   tfa and math diff value:-0.000000, math calculation time:9088 ticks, tfa calculation time:290 ticks.

不开启FPU：

- 菜单选项 0 ~ 7 测试了 TFA 8种函数的基本使用方法, 计算结果和库函数计算结果的差值比较, 计算周期和库函数计算周期的比较, 运行结果如下：

.. code-block:: console

   0
   tfa and math diff value:-0.000000, math calculation time:67 ticks, tfa calculation time:248 ticks.
   1
   tfa and math diff value:0.000000, math calculation time:357 ticks, tfa calculation time:234 ticks.
   2
   tfa and math diff value:0.000000, math calculation time:1703 ticks, tfa calculation time:235 ticks.
   3
   tfa and math diff value:0.000000, math calculation time:1764 ticks, tfa calculation time:237 ticks.
   4
   tfa and math diff value:0.000000, math calculation time:2989 ticks, tfa calculation time:250 ticks.
   5
   tfa and math diff value:-0.000000, math calculation time:2863 ticks, tfa calculation time:227 ticks.
   6
   tfa and math diff value:-0.000000, math calculation time:2045 ticks, tfa calculation time:346 ticks.
   7
   tfa and math diff value:-0.000000, math calculation time:539 ticks, tfa calculation time:228 ticks.

- 菜单选项 8 测试了基于上述运算的一个复杂计算, 计算结果和库函数计算结果的差值比较, 计算周期和库函数计算周期的比较, 运行结果如下：

.. code-block:: console

   8
   tfa and math diff value:-0.000000, math calculation time:9178 ticks, tfa calculation time:1263 ticks.
