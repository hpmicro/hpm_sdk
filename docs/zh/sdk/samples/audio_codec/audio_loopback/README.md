# Audio Codec Loopback

## 概述

音频编解码示例工程展示了如何使用板载的音频编解码器记录和播放音频信息。
在这个工程中，音频编解码器采集音频信息，编码后通过I2S接口传送给MCU,
MCU会通过I2S接口将收到的编码数据发送回音频编解码器，音频编解码器会解码收到的数据并播放。

## 工程配置

- 在文件`CMakeLists.txt`中,  根据开发板原理图，设置匹配的audio codec类型，例如："set(CONFIG_CODEC "sgtl5000")"

## 硬件设置

1. 根据开发板原理图选择下列方式中的一种音频录入方式：
    - 使用3.5mm音频连接线将音乐播放器与板上codec的LINE_IN接口相连, 录入音频数据
    - 使用板上codec的mic录入音频数据
2. 连接3.5mm耳机到音频编解码芯片的[耳机](lab_board_app_headphone)接口， 播放音频数据


## 运行现象

当工程正确运行后，耳机会播放音频播放器正在放的声音，而串口终端会输出如下信息：
```console
Audio Codec example
Test Codec playback and record
```
