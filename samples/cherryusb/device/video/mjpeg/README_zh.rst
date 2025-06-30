.. _video_mjpeg:

Video MJPEG
====================

概述
------

本示例工程展示了USB UVC设备，使用CAM DVP接口获取摄像头设备采集的图片信息，然后通过 `components/jpeg` 组件将其编码为JPEG图像格式，最后通过USB将JPEG图片信息发送给PC主机。

硬件设置
------------

1. 连接OV7725摄像头设备到开发板的CAM接口，若使用OV5640，则在CMakeLists.txt文件将CONFIG_CAMERA设置为OV5640，指令为 `set(CONFIG_CAMERA "ov5640")` 。
2. 将USB0口接PC主机

运行现象
------------

当工程正确运行后，PC将枚举出一个摄像头设备，可通过`腾讯会议-设置-视频`选项进行测试，`摄像头`选择`HPMicro UVC MJPEG`。
