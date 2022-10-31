# USB MSC HOST

## 概述

本示例工程展示USB MSC HOST

## 硬件设置

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口
- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

## 运行现象

- 将程序下载至开发板运行，使用串口调试助手查看输出log；
- 首先，会在U盘里面创建一个"cherryusb_msc_test.txt"文件，往文件里面写入字符串"cherryusb fatfs demo..."，总共100行，总长度为2500字节；然后将该文件的数据读取出来，总长度为2500。
- 其次，会扫描U盘根目录下的目录和文件，将目录名和文件名打印出来。

## 输出Log示例
- 开发板USB0端口接入U盘时，串口调试助手会显示如下数据：
```
FATFS cherryusb mount succeeded!
test fatfs write
write success, write len：2500
test fatfs read
read success, read len：2500

Direcotry Path: /
Folder Name: LOST.DIR
Folder Name: DCIM
Folder Name: MIUI
Folder Name: Android
Folder Name: tencent
File   Name: USBHost.txt
File   Name: cherryusb_msc_test.txt
```



