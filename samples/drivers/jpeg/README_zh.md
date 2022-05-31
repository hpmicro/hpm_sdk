# JPEG
## 概述
***
**jpeg**编解码驱动示例工程展示了如何将**YUV图片数据**转换为**RGB数据**，以及将**RGB数据**转换为**YUV数据**。
- 解码功能：提取示例JPG图片数据源,将图片数据中的**huffmin**、**huffbase**、**huffsymb**、**huffenc**、**Decoder Q 值**表格信息，以及**YUV像素流**数据，经过硬件JPEG模块进行解码，将**YUV数据**解码为**RGB数据**，并通过**液晶屏**显示原图片信息
- 编码功能：将图片**RGB565像素流**数据，以及**huffmin**、**huffbase**、**huffsymb**、**huffenc**、**Encoder Q值**表格信息，经过硬件JPEG模块进行编码，将**RGB数据**编码为**YUV数据**，再通过解码流程还原为RGB数据，并通过**液晶屏**显示原图片信息

## 硬件设置
***
-  **LCD屏**连接到开发板``LCD1``

## 运行现象
***
当工程正确运行后，LCD屏正确显示绿竹图片信息，而串口终端会输出如下信息：
```
> jpeg example
> start decoding
> decoding done
> start encoding and decoding
> encoded 937
> encoding and decoding done
```

