.. _audio_v2_microphone_and_speaker_rt_thread:

Audio V2 Microphone and Speaker (RT-Thread)
======================================================================================

概述
------

本示例工程展示USB Audio V2喇叭和麦克风设备。

- PC可以看到一个CherryUSB喇叭设备和一个CherryUSB麦克风设备。

工程配置
--------

- USB High Speed 和 Full Speed 的服务周期分别由 ``EP_INTERVAL_HS`` 和 ``EP_INTERVAL_FS`` 配置。对应的 packets/s 以及喇叭和麦克风 USB 最大包长会根据 interval 自动计算。

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
