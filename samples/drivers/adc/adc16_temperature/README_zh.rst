.. _adc16_temperature:

ADC16 Temperature
==================================

概述
------

本示例展示通过ADC16读取SOC温度

硬件设置
------------

- 根据开发板设计，如有必要，请连接VREF管脚处的跳帽  (请参考  :ref:`引脚描述 <board_resource>`  部分)

运行现象
------------

- 串口终端显示如下信息


  .. code-block:: console

   This is an ADC16 temperature acquisition demo:
   ADC16 clock: input=200000000 Hz, div=4, conv=50000000 Hz
   ADC16 sample: cycle=511, convert=21, fs=93984 Hz
   Current Soc Temp: 26℃
   Current Soc Temp: 26℃
   Current Soc Temp: 26℃
   Current Soc Temp: 28℃
   Current Soc Temp: 27℃
   Current Soc Temp: 26℃
   Current Soc Temp: 27℃
   Current Soc Temp: 28℃
   Current Soc Temp: 29℃
   Current Soc Temp: 27℃
   Current Soc Temp: 30℃
   Current Soc Temp: 28℃
   Current Soc Temp: 28℃
   Current Soc Temp: 26℃
   Current Soc Temp: 28℃
   Current Soc Temp: 26℃
   Current Soc Temp: 28℃
   Current Soc Temp: 28℃

- 以上时钟行为 ``hpm6750evk2`` 默认 AHB 200 MHz 的打印。本示例需要 ADC16 温度传感器支持。

