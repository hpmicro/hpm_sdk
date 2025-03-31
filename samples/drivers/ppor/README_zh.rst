.. _ppor:

PPOR
========

概述
------

**PPOR** 驱动示例工程展示如何进行复位操作

硬件设置
------------

-  无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   ppor example
   reset_soc

如果Segger Embedded Studio工程构建类型采用flash_xip或flash_sdram_xip时，则会追加打印如下信息：

.. code-block:: text

   > Software reset has occurred

