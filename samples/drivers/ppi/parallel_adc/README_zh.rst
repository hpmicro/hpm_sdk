.. _ppi_parallel_adc_example:

PPI Parallel ADC example
================================================

概述
------

本示例工程展示了使用PPI外设读取外部并口ADC，使用异步sram接口方式去访问。

PPI外设的地址空间为：0xF8000000~0xFFFFFFFF，基地址需1MB对齐。

硬件设置
------------

需外接并口ADC模块，本sample测试的并口ADC型号为：MS9280。

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: text

   ppi parallel adc example
   adc data: 0xc3
   adc data: 0xc2
   adc data: 0xc3
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2
   adc data: 0xc2

