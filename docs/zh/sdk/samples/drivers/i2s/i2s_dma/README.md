# I2S DMA

## 概述

该示例工程展示了I2S使用DMA将音频数据传输给板载音频解码芯片并播放的功能。

## 工程配置

- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的audio codec类型，例如："set(CONFIG_CODEC "sgtl5000")"

## 硬件设置

- 连接3.5mm耳机到音频编解码芯片的[耳机](lab_board_app_headphone)接口


## 运行现象

当工程正确运行后，耳机会播放音频，同时串口终端会输出如下信息：
```console
I2S DMA example
I2C bus is ready
i2s dma play finished
```
