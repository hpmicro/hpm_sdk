# I2S Interrupt
## 概述

音频编解码示例工程展示了如何使用板载的音频编解码器播放音频信息。
MCU会通过I2S接口将收到的编码数据发送回音频编解码器，音频编解码器会解码收到的数据并播放。

## 工程配置

- 在文件[CMakeLists.txt](./CmakeLists.txt)中,  根据开发板原理图，设置匹配的audio codec类型，例如："set(CONFIG_CODEC "sgtl5000")"

## 硬件设置

- 连接3.5mm耳机到音频编解码芯片的[耳机](lab_board_app_headphone)接口

## 运行现象

当工程正确运行后，耳机会播放频率为1KHZ的正弦波的声音，通过示波器测量左声道或者右声道可以得到1KHZ的正弦波，而串口终端会输出如下信息：
```

> I2S Interrupt example
> Test Codec playback

```