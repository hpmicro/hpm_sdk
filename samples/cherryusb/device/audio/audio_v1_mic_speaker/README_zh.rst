.. _audio_v1_microphone_and_speaker:

Audio V1 Microphone and Speaker
==============================================================

概述
------

本示例工程展示USB Audio V1喇叭和麦克风设备。

- PC可以看到一个CherryUSB喇叭设备和一个CherryUSB麦克风设备。

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

- 将喇叭连接至DAO接口

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并安装喇叭和麦克风驱动，枚举出一个喇叭设备和一个麦克风设备

- 选择该喇叭设备作为默认播放器，PC将通过DAO接口播放音频

- 选择该麦克风设备作为默认音频输入设备，PC将通过PDM接口输入音频。
