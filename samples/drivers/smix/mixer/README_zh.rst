.. _smix:

SMIX
========

概述
------

SMIX示例工程演示音频码流的混音功能。示例工程中将两种不同采样率的音频转换成相同的采样率混合后进行播放。

工程配置
------------

- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的audio codec类型，例如："set(CONFIG_CODEC "wm8960")"

硬件设置
------------

- 连接3.5mm耳机到音频编解码芯片的 :ref:`耳机 <board_resource>` 接口， 播放音频数据. 请注意使用合适的耳机接口类型。

已知问题
------------

- 在部分开发板上使用codec的耳机接口播放音频时可能出现串扰， 比如通过耳机的左声道播发单声道音频时， 在右声道耳机上能听到微弱的声音。

运行现象
------------

当工程正确运行后，串口终端会输出如下信息，并从耳机中听到声音：

.. code-block:: console

   Sound mixer example
   mixer play one sound
   input sound sample rate: 8000Hz
   output sound sample rate: 48000Hz
   mixer play one sound
   input sound sample rate: 16000Hz
   output sound sample rate: 48000Hz
   mixer play two sound
   input sound0 sample rate: 8000Hz
   input sound1 sample rate: 16000Hz
   output sound sample rate: 48000Hz

