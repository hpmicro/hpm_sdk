.. _tinyusb_host_cdc_msc_hid_freertos:

TINYUSB HOST CDC MSC HID FREERTOS
==================================

概述
------

本示例工程展示USB CDC MSC HID HOST

硬件设置
------------

- 使用USB Type-C线缆线连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C转Type-A线缆线连接开发板USB0端口，可接入CDC设备、MSC设备或HID设备（如鼠标和键盘）

运行现象
------------

- 将程序下载至开发板运行，使用串口调试助手查看输出log

接键盘
~~~~~~~~~

- 开发板USB0端口接入键盘时，串口调试助手会显示如下数据：


.. code-block:: text

   TinyUSB Host CDC MSC HID Example
   HID device address = 1, instance = 0 is mounted
   HID Interface Protocol = Keyboard
   HID device address = 1, instance = 1 is mounted
   HID Interface Protocol = None
   HID has 2 reports

- 当按压键盘时，将显示键盘发送的数据，例如输出hpmicro，回显如下：


.. code-block:: console

   hpmicro


接鼠标
~~~~~~~~~

- 开发板USB0端口接入鼠标时，串口调试助手会显示如下数据：


.. code-block:: text

   TinyUSB Host CDC MSC HID Example
   HID device address = 1, instance = 0 is mounted
   HID Interface Protocol = Mouse

- 当移动或点击鼠标时，将显示鼠标发送的数据：


.. code-block:: console

   (-25 -68 0)
   (-18 -44 0)
   (-12 -27 0)
   (-7 -16 0)
   (-1 -1 0)
   (1 -2 0)
   (5 -5 0)

接U盘
~~~~~~~~~

- 开发板USB0端口接入U盘时，串口调试助手会显示U盘的信息数据：


.. code-block:: text

   TinyUSB Host CDC MSC HID Example
   A MassStorage device is mounted
   A device with address 1 is mounted
   IS917    innostor rev 1.00
   Disk Size: 7500 MB
   Block Count = 15360000, Block Size: 512

接CDC设备
~~~~~~~~~

- 开发板USB0端口接入CDC-ACM设备时，串口调试助手会显示如下数据：


.. code-block:: text

   TinyUSB Host CDC MSC HID Example
   CDC Interface is mounted: address = 1, itf_num = 0
     Baudrate: 115200, Stop Bits : 0
     Parity  : 0, Data Width: 8


- 在console中输入任意字符，CDC设备会转发给Host，Host的console会进行回显。