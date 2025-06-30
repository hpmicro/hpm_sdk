.. _i2s_emulation:

SPI模拟I2S
================

概述
------

- 该示例工程展示了SPI模拟I2S主机接收音频解码模块录音数据并且播放录音数据的功能。

- 评估板需要接入音频解码模块，本例程使用wm8978

提示
------

- 组件详情文档请参见 :ref:`i2s_over_spi <i2s_over_spi>`

注意
------

- 由于该示例所用的SPI需要大长度传输，目前仅支持hpm5300系列。

需要使用到的组件
------------------------

- dma_mgr

- spi

- i2s_over_spi

- codec(wm8978)

硬件设置
------------

- 通过音频模块板载mic进行录音

- 连接3.5mm耳机到音频模块的输出耳机接口

- 需要用到GPTMR输出以及SPI引脚，引脚(请参考 :ref:`引脚描述 <board_resource>` 部分)

- 5V          <--> 5V(WM8978)

- 3.3V        <--> 3.3V(WM8978)

- GND         <--> GND(WM8978)

- MCLK(GPTMR) <--> MCLK(WM8978)

- LRCK(GPTMR) <--> (FSA)LRC(WM8978)

- BLCK(GPTMR) <--> SCLK(SPI) <--> SCK(WM8978)

- MISO(SPI)   <--> (DAT)DACDATA(WM8978)

- MOSI(SPI)   <--> (SDB)ADCDATA(WM8978)

- (CS)GPIO    <--> CS(SPI)

- I2C_SDA     <--> I2C_SDA(WM8978)

- I2C_SCL     <--> I2C_SCL(WM8978)

运行步骤
------------

- 当工程正确运行后，串口终端会输出如下信息。


.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                   i2s_emulation demo                        *
   *                                                             *
   *        1 - start play                                       *
   *        2 - stop play                                        *
   *        3 - start record                                     *
   *        4 - stop record                                      *
   *-------------------------------------------------------------*


- 终端输入'3'字符，开启录音，如果录音时长大于指定时长会自行退出保存。


.. code-block:: console

   record start enter....
   recording size overflow, stopping and exiting now...


- 终端输入'1'字符，开启录音播放


.. code-block:: console

   play start enter....
   play end, now exit....


- 终端输入'4'字符，停止录音


.. code-block:: console

   record_stop finish....


- 终端输入'2'字符，停止播放


.. code-block:: console

   play stop finish....

