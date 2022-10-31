# JPEG编码
## 概述
***
JPEG编码示例工程展示了如何将摄像头采集数据通过PEG模块或者软件库编码为JPG图像数据并保存在存储介质中。

## 硬件设置
***
- **USB盘**连接到开发板``USB0``，或**SD内存卡**插入到开发板``sd card``
- **CAM摄像头** 和 **LCD屏**连接到开发板

## 工程配置
***
- 文件[CMakeLists.txt](./CMakeLists.txt) :
  - ``set (JPEG_USE_SDCARD 1)`` 将编码后的jpeg文件(.jpg)存到SD卡中; ``set (JPEG_USE_UDISK 1)`` 将文件存到u盘中。这两个为排他选项
  - ``set (JPEG_HW_MODE 1)`` 通过JPEG模块编码; ``set (JPEG_HW_MODE 0)``通过软件库编码

## 存储图片大小
***
- 本例程中存储的图片格式为JPG，分辨率大小为640*480

## 运行现象
***
- 按需要更改CMakeLists.txt中关于目标存储/编码方式的配置
- 连接摄像头、LCD屏和存储介质到开发板
- 将开发板上电
- 下载编译后的elf文件到开发板
- 如果一切正常:
  - 摄像头预览图像会被显示在LCD屏上
  - 按下GPIO按键(请确认具体开发板 {ref}`按键 <lab_board_resource>` 部分描述)将把当前预览帧编码为jpeg文件存储到介质中
  - 同时编码后的图像会被再次解码并显示到LCD屏，直到GPIO按键再次被按下，将切换回预览模式
