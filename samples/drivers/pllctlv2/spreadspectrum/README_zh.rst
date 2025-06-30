.. _pllctlv2_spreadspectrum:

PLLCTLV2 频谱扩展
======================

概述
--------

本示例展示了如何配置PLL扩频。

**频谱扩展功能**

展示如何配置频谱扩展调制以降低电磁干扰（EMI）。

- **扩频范围**
    - 默认值：**0.5%**
    - 通过 **SS_RANGE** 宏配置

- **调制频率**
    - 默认值：**30 kHz**
    - 通过 **MODULATION_FREQ** 宏自定义

当频谱扩展功能启用时，在示波器上观察到的FFT结果如下图所示：

.. image:: ./doc/pllctlv2_ss.png

对比没有开启扩频时，在示波器上观察到的FFT的结果如下图所示：

.. image:: ./doc/pllctlv2_no_ss.png

硬件设置
------------

- 配置的PLL频率可以通过示波器观察到clock_ref引脚，请参考 :ref:`Pin Description <board_resource>` 获取具体板子的引脚信息。

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   PLLCTLV2 spread spectrum example
   PLL2CLK0 @ 80000000Mhz spread spectrum has been enabled with range 0.5% @ 30000Hz, which can be observed on "J20[7]" pin by the oscilloscope

   PLLCTLV2 spread spectrum example finished

