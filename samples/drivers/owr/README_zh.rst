.. _1_wire:

1-Wire
============

概述
------

本示例展示了通过1-Wire总线访问数字温度传感器DS18B20。

HPM 1-Wire特性如下：

- 1-Wire Master

- Reset and Rresence Pulses(RPP) 输出及1-Wire Slave检测

- 单字节或多字节读写

- DMA模式

- SRA模式

- 中断

硬件设置
------------

无

背景
------

1-Wire(单总线) 是DALLAS公司推出的外围串行扩展总线技术总线，顾名思义，它是采用一根信号线进行通信，既传输时钟信号又传输数据，而且能够进行双向通信，具有节省I/O口线、资源结构简单、成本低廉、便于总线扩展和维护等诸多优点。常用到单总线的器件，一般是稳定传感器、EEPROM、唯一序列号芯片等，如DS18B20、DS2431等。

运行现象
------------

- 串口终端显示如下信息


  .. code-block:: console

   This is a onewire sample: DS18B20
   OWR Slave Detected!
   Family Code: 28
   Serial Num:67 e2 79 00 00 00
   CRC: 3a
   Thermometer resolution: 12 bit
   Current Temp: 26.94 degree centigrade
   Current Temp: 26.94 degree centigrade
   Current Temp: 26.87 degree centigrade
   Current Temp: 26.94 degree centigrade
   Current Temp: 26.87 degree centigrade
   Current Temp: 26.87 degree centigrade
   Current Temp: 26.94 degree centigrade
   Current Temp: 27.25 degree centigrade
   Current Temp: 27.69 degree centigrade
   Current Temp: 28.06 degree centigrade
   Current Temp: 28.31 degree centigrade
   Current Temp: 28.50 degree centigrade
   Current Temp: 28.62 degree centigrade
   Current Temp: 28.44 degree centigrade
   Current Temp: 28.31 degree centigrade
   Current Temp: 28.19 degree centigrade
   Current Temp: 28.06 degree centigrade

