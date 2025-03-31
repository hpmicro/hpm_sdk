.. _png_encode:

PNG编码
==========

概述
------

PNG编码示例工程展示了如何将摄像头采集数据通过LodePNG软件库编码为PNG图像数据并保存在存储介质中。

硬件设置
------------

- **USB盘** 连接到开发板 ``USB0`` （注意：U盘格式化的 `分配单元大小` 不能超过32KB，建议格式化参数为FAT32/32KB），或 **SD内存卡** 插入到开发板 ``sd card``

  .. image:: ../common/doc/UDisk_Format.png
     :alt: UDisk_Format

- **CAM摄像头**  和 **LCD屏** 连接到开发板

工程配置
------------

- 文件 `CMakeLists.txt` :

  - `set (PNG_USE_SDCARD 1)` 将编码后的png文件(.png)存到SD卡中; `set (PNG_USE_UDISK 1)` 将文件存到u盘中。这两个为排他选项

  - 如果使用ov7725摄像头， 在CMakeList.txt配置 `set(CONFIG_CAMERA "ov7725")`

  - 如果使用ov5640摄像头， 在CMakeList.txt配置 `set(CONFIG_CAMERA "ov5640")`

存储图片大小
------------------

- 本例程中存储的图片格式为PNG，分辨率大小为640*480

运行现象
------------

- 按需要更改CMakeLists.txt中关于目标存储/编码方式的配置，目标存储默认为SD卡。

- 连接摄像头、LCD屏和存储介质到开发板

- 将开发板上电

- 下载编译后的elf文件到开发板

- 如果一切正常:

  - 摄像头预览图像会被显示在LCD屏上

  - 按下GPIO按键(请确认具体开发板 {ref}`按键 <board_resource>` 部分描述)将把当前预览帧编码为png文件存储到介质中

  - 同时编码后的图像会被再次解码并显示到LCD屏，直到GPIO按键再次被按下，将切换回预览模式
