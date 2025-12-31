.. _spi_read_ssi_encoder:

SPI READ SSI ENCODER
==========================================================================

概述
------

使用SPI读取SSI encoder，并打印相关数据。

硬件设置
------------

本示例使用另外一块开发板运行 `hpm_sdk/samples/drivers/sei/slave/ssi` 示例模拟SSI Encoder编码器。

将板上的 :ref:`SPI引脚 <board_resource>` SCLK与MISO引脚通过TTL转RS485模块与SEI的CLOCK和DATA信号分别相连接。

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   SPI Master Read SSI Encoder Example
   SSI Encoder Data: [0xa5afa6]
   SSI Encoder Data: [0xa5afa7]
   SSI Encoder Data: [0xa5afa8]
   SSI Encoder Data: [0xa5afa9]
   SSI Encoder Data: [0xa5afaa]
   SSI Encoder Data: [0xa5afab]
   SSI Encoder Data: [0xa5afac]
   SSI Encoder Data: [0xa5afad]
   SSI Encoder Data: [0xa5afae]
   SSI Encoder Data: [0xa5afaf]
   SSI Encoder Data: [0xa5afb0]
   SSI Encoder Data: [0xa5afb1]
   SSI Encoder Data: [0xa5afb2]
   SSI Encoder Data: [0xa5afb3]
   SSI Encoder Data: [0xa5afb4]
   SSI Encoder Data: [0xa5afb5]
   SSI Encoder Data: [0xa5afb6]
   SSI Encoder Data: [0xa5afb7]
   SSI Encoder Data: [0xa5afb8]
