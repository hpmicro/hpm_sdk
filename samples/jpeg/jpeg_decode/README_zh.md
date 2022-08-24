# JPEG_DECODE解码
## 概述
***
**jpeg_decode**解码示例工程展示了如何将JPG图片数据转换为RGB数据。
- 存储于**USB**或**SD卡**内的JPG图片(默认扫描存储设备根目录),通过**代码库libjpeg_turbo**,或**硬件JPEG模块**，将图片数据转换为RGB565数据，
并通过**液晶屏**显示出解码后的图片数据。

## 硬件设置
***
- **USB盘**连接到开发板``USB0``，或**SD内存卡**插入到开发板``sd card``，存储介质中存放JPG图片,示例图片可使用jpeg路径下的**example.jpg**
- **LCD屏**连接到开发板``LCD1``

## 工程配置
***
- 文件[CMakeLists.txt](./CmakeLists.txt)中配置``set(CONFIG_SDFATFS_MODE 1)``，则默认从**SD卡**中读取JPG图片；配置
``set(CONFIG_TINYUSBHOST_MODE 1)``，则默认从**USB**中读取JPG图片
- 文件[CMakeLists.txt](./CmakeLists.txt)中配置``set(CONFIG_JPEG_TURBO_MODE 1)``，则默认解码方式为通过**软件libjpeg_turbo库**完成数据解码；配置
``set(CONFIG_JPEG_HARDWARE_MODE 1)``，则默认解码方式为通过**硬件jpeg模块**完成数据解码

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
- JPG图片不大于10M(hpm6750evk开发板)/4M(hpm6750evkmini开发板)
- JPG图片分辨率(宽*高)不大于8M(hpm6750evk开发板)/3.5M(hpm6750evkmini开发板)

## 运行现象
***
- SD卡方式(存储示例图片example.jpg) & 软件libjpeg_turbo解码方式：
当工程正确运行后，LCD屏正确显示存储设备中的图片信息，而串口终端会输出如下信息：
```
> Read picture data by SD card
> Libjpeg-turbo decode completed
```

- USB方式(存储示例图片example.jpg) & 软件libjpeg_turbo解码方式：
当工程正确运行后，LCD屏正确显示存储设备中的图片信息，而串口终端会输出如下信息：
```
> Read picture data by usb-mode
> Libjpeg-turbo decode completed
```

- SD卡方式(存储示例图片example.jpg) & 硬件jpeg模块解码方式：
当工程正确运行后，LCD屏正确显示存储设备中的图片信息，而串口终端会输出如下信息：
```
> Read picture data by SD card
> jpeg-hardware decode completed
```

- USB方式(存储示例图片example.jpg) & 硬件jpeg模块解码方式：
当工程正确运行后，LCD屏正确显示存储设备中的图片信息，而串口终端会输出如下信息：
```
> Read picture data by usb-mode
> jpeg-hardware decode completed
```