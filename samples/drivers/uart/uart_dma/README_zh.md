# DMA接收和发送数据UART
## 概述
***
该示例工程中，展示UART采用DMA的方式接收和发送数据的操作。

## 端口设置
***
-  串口波特率设置为``115200bps``，``1个停止位``，``无奇偶校验位``

## 运行现象
***
当工程正确运行后，串口终端会输出如下信息：
```
> ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWX
Channel 0 transfers done!

```
通过串口手动输入字符，如 'A'，则串口终端会收到如下信息：
```
>  Channel 1 transfers done!
A
```

