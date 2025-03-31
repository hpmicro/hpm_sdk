.. _i2s_interrupt:

I2S Interrupt
==========================

概述
------

该示例工程展示了I2S使用中断方式将音频数据传输给板载音频解码芯片并播放的功能。

工程配置
------------

- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的audio codec类型，例如："set(CONFIG_CODEC "sgtl5000")"

硬件设置
------------

- 连接3.5mm耳机到音频编解码芯片的 :ref:`耳机 <board_resource>` 接口

已知问题
------------

- 在部分开发板上使用codec的耳机接口播放音频时可能出现串扰， 比如通过耳机的左声道播发单声道音频时， 在右声道耳机上能听到微弱的声音。

运行现象
------------

当工程正确运行后，耳机会播放音频，同时串口终端会输出如下信息：

.. code-block:: console

   I2S DMA example
   I2C bus is ready
   i2s dma play finished

