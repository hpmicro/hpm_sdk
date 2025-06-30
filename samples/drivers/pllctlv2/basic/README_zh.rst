.. _pllctlv2_basic:

PLLCTLV2基础
================

概述
--------

本示例展示了如何配置和控制锁相环（PLL）控制器第2版，通过调整PLL参数来操控系统时钟特性。

**调整频率范围**

- 设置频率从**400 MHz 至 1 GHz**
- 示例程序会显示实际输出频率与目标频率的对比

硬件设置
------------

- 无

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   PLLCTLV2 basic example
   Now configure PLL at integer mode
   Update PLL2 to 400000000Hz
   Actual PLL frequency is 400000000Hz
   Update PLL2 to 401000000Hz
   Actual PLL frequency is 401000000Hz
   Update PLL2 to 402000000Hz
   Actual PLL frequency is 402000000Hz
   Update PLL2 to 403000000Hz
   Actual PLL frequency is 403000000Hz
   Update PLL2 to 404000000Hz
   Actual PLL frequency is 404000000Hz
   Update PLL2 to 405000000Hz
   Actual PLL frequency is 405000000Hz
   Update PLL2 to 406000000Hz
   Actual PLL frequency is 406000000Hz
   Update PLL2 to 407000000Hz
   ...
   Update PLL2 to 998000000Hz
   Actual PLL frequency is 998000000Hz
   Update PLL2 to 999000000Hz
   Actual PLL frequency is 999000000Hz
   Update PLL2 to 1000000000Hz
   Actual PLL frequency is 1000000000Hz

   PLLCTLV2 basic example finished

