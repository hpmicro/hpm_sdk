# CAM
## 概述

CAM示例工程展示了使用CAM接口获取摄像头设备采集的图片信息并通过LCD显示的功能。

## 硬件设置

1. 连接摄像头设备到开发板的CAM接口
2. 连接LCD到开发板的LCD接口

## 工程设置

对于这个工程，用户需要在CMakeList.txt设置正在使用的摄像头型号
1. 如果使用ov7725摄像头， 在CMakeList.txt配置`set(CONFIG_CAMERA "ov7725")`
2. 如果使用ov5640摄像头， 在CMakeList.txt配置`set(CONFIG_CAMERA "ov5640")`

## 注意事项

当使用ov5640摄像头时， 由于其`framebuffer`太大了，不能支持ram和flash_xip方式， 请使用flash_sdram_xip方式。

## 运行现象

当工程正确运行后，LCD会显示摄像头采集的数据，而串口终端会输出如下信息：
```
> cam example
```

