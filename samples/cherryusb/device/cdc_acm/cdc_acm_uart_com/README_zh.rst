.. _cdc_acm_uart_com:

CDC ACM Uart COM
================================

概述
------

本示例工程展示USB CDC设备串口

- PC 通过设备管理器查看得到一个串口

- USB CDC ACM对接了MCU的UART串口，引脚可查看板级文件的readme，名称为：USB_CDC_ACM_UART。

- UART接收数据采用DMA循环接收方式。启动定时器周期性查询数据，若有数据则通过USB IN事务发送出去。

- UART发送数据采用DMA普通模式，当USB OUT事务收到数据时，启动UART DMA发送数据。

- 应用程序内部是5ms读取一次uart rx buffer，buffer大小512，故设计的最大波特率为921600。若用户希望提高支持的最大波特率，可以缩短定时器周期或者增大uart rx buffer。

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

- 使用USB转TTL电平的串口调试模块，USB侧接电脑，UART侧接USB_CDC_ACM_UART引脚

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并安装串口驱动，并枚举出一个COM口设备，称为COM_A。

- USB转TTL电平的串口调试模块接电脑，电脑识别出一个COM口设备，称为COM_B。

- 打开2个串口调试助手窗口，二者通讯设置一样（波特率、校验位等），一个打开COM_A，一个打开COM_B。

- 在COM_A的串口调试助手中输入字符，点击发送，COM_B的串口调试助手会收到COM_A发送的字符；在COM_B的串口调试助手中输入字符，点击发送，COM_A的串口调试助手会收到COM_B发送的字符。
