.. _winusb_2_0:

WinUSB 2.0
====================

概述
------

本示例工程展示WinUSB 2.0设备。

本示例中，USB设备有2个interface，每个interface有1个IN Endpoint和1个OUT Endpoint。OUT Endpoint收到PC下发的数据后，将通过IN Endponit将数据回发给PC。

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并安装驱动，设备管理器可查看得到2个通用串行总线设备。

    .. image:: ./doc/PC_device_manager.png
       :alt: PC_device_manager.png

- 通过LLCOM调试工具，在《小工具-WinUSB》菜单中，下拉可以看到2个usb interface。

    .. image:: ./doc/LLCOM_config.png
       :alt: LLCOM_config.png

- 选择HPMicro WinUSB0或HPMicro WinUSB1，点击《打开》按钮，然后输入发送数据，在左边窗口将显示发送和接收的数据。

    .. image:: ./doc/LLCOM_communication.png
       :alt: LLCOM_communication.png

备注
------

- 可在CMakeLists.txt中设置宏`DOUBLE_WINUSB=0`，使得USB设备只有1个interface。

- 当windows不识别该设备时，原因可能是设备的VID、PID以及设备版本号已经在Windows注册表中已登记了，不支持WCID。解决方法是删除注册表中相关项或是使用别的VID、PID、设备版本号。
