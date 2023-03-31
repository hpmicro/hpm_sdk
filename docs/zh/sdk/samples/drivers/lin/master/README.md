# lin_master
## 概述

Lin_master工程展示了lin在master模式接收和发送数据的功能。

## 硬件设置

需要一个LIN收发器和一个USB_LIN调试器
请参考具体开发板{ref}`引脚描述 <lab_board_resource>`部分的描述。
将开发板上的LIN TX和RX引脚与LIN收发器的MCU信号相连，将USB_LIN调试器与LIN收发器的LIN信号相连。

## 运行现象

运行此程序，需要配置usb_lin调试器
- 配置usb_lin调试器，选择正确的串口和波特率， 然后点击`设置到调试器`：
  ![lin_debugger_configuration](../../../../../../assets/sdk/samples/lin_debugger_configuration.png)
- 配置作为从机发送配置选项， 设置ID和数据以及校验方式，勾选使能选项， 点击`配置到调试器`：
  ![lin_debugger_slave_sent](../../../../../../assets/sdk/samples/lin_debugger_slave_sent_config.png)
- 当程序正确运行之后， 调试器窗口可以看到结果：
  ![lin_debugger_slave_result](../../../../../../assets/sdk/samples/lin_debugger_slave_result.png)

当工程正确运行后，串口终端会输出如下信息：
```console
LIN master example
LIN master sent wakeup signal
ID: 31, sent 8 bytes:
0 1 2 3 4 5 6 7
ID: 30, receive 8 bytes
7 6 5 4 3 2 1 0
```


