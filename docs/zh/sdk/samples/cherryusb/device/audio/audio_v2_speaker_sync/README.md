# USB AUDIO V2 SPEAKER SYNC

## 概述

本示例工程展示USB AUDIO V2喇叭设备，它使用反馈端点实现同步。

- PC可以看到一个CHERRY USB喇叭设备。

## 硬件设置

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口
- 将喇叭连接至DAO接口

## 运行现象

- 将程序下载至开发板运行，电脑可自动识别并安装喇叭驱动，枚举出一个喇叭设备
- 选择该喇叭设备作为默认播放器，PC将通过DAO接口播放音频

