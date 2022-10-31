# JPEG 解码
## 概述
***
jpeg 解码示例工程展示了如何使用JPEG模块或者软件库解码JPG图片数据

## 硬件设置
***
- **USB盘**连接到开发板``USB0``，或**SD内存卡**插入到开发板``sd card``。
- **LCD屏**连接到开发板

## 工程配置
***
- 文件[CMakeLists.txt](./CMakeLists.txt) :
  - ``set (JPEG_USE_SDCARD 1)`` 从SD卡中读取jpeg文件(.jpg); ``set (JPEG_USE_UDISK 1)`` 从u盘中读取jpeg文件(.jpg)。这两个为排他选项
  - ``set (JPEG_HW_MODE 1)`` 通过JPEG模块解码; ``set (JPEG_HW_MODE 0)``通过软件库解码

## 支持的图片格式
***
- 支持标准的JPEG图片：
  JPEG图片格式组成部分：SOI（文件头）+APP0（图像识别信息）+ DQT（定义量化表）+ SOF0（图像基本信息）+ DHT（定义Huffman表） + DRI（定义重新开始间隔）+ SOS（扫描行开始）+ EOI（文件尾），其中SOI是由两个16进制字节FFD8组成。
- JPEG图片支持的采样因子为：YUV420、YUV422H、YUV422V、YUV444、YUV400，如一张800*400 YUV420格式的JPG图片，通过图虫EXIF查看器[图虫EXIF查看器官网](https://exif.tuchong.com//)可获取：
    FileType	        JPEG
    FileTypeExtension	jpg
    MIMEType	        image/jpeg
    ImageWidth	        800
    ImageHeight	        480
    EncodingProcess	    Baseline DCT, Huffman coding
    BitsPerSample	    8
    ColorComponents	    3
    YCbCrSubSampling  **YCbCr4:2:0 (2 2)**

##本例程支持的图片大小及分辨率
***
- jpeg文件不大于500KB
- 图像分辨率（宽*高）不大于 1024 * 768

## 运行现象
***
- 按需要更改CMakeLists.txt中关于目标存储/解码方式的配置
- 将jpeg文件复制到目标存储中
- 连接LCD屏和存储介质到开发板
- 将开发板上电
- 下载编译后的elf文件到开发板
- 如果一切正常，存储介质中的jpeg文件将被一张张显示在LCD屏上
