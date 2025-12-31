.. _tinyusb_dual_port:

TINYUSB DUAL PORT
========================

概述
------

- 本示例工程展示双USB口，其中USB0作为Device，为CDC-ACM Device；USB1作为Host，为HID Host。

- USB主机接收HID设备的报告，并通过USB设备CDC接口进行打印。

硬件设置
------------

- USB0口作为Device，接PC。

- USB1口作为HID Host，接键盘.

运行现象
------------

- 将程序下载至开发板运行，PC可枚举出一个CDC-ACM设备。

- 使用串口调试助手打开该串口，输入任意字符，串口调试助手会回显输入的字符。

- USB1口接入键盘，按压键盘按钮，将会在串口调试助手中显示键盘字符。