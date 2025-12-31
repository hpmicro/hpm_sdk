.. _usb_hid_generic_inout:

USB HID Generic Inout
==========================================

概述
------

本示例工程展示USB HID通用传输回显

- PC 通过USB 发送USB HID Report，MCU接收后回发至PC

硬件设置
------------

* 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

* 使用USB Type-C线缆连接PC USB端口和开发板USB0端口或者USB1端口

运行现象
------------

当工程正确运行后

* 进入相对路径:“samples\tinyusb\device\hid_generic_inout”

* 运行脚本: python ./hid_test.py

  * 注意： 确保已安装hid包，步骤如下：

    * 安装 `hid` 包 (https://pypi.org/project/hid/)，指令如下：

      - $ pip install hid

    * `Hid` 包还需依赖 `hidapi` (https://github.com/libusb/hidapi)。Linux系统已经有了hidapi，可直接使用；但Windows还需要从网页下载hidapi，下载后将hidapi.dll（x64的还是x86的根据电脑系统选择）复制到python的安装根目录下。

* 在终端里面输入数值，hid设备将会回复收到的数据，不足64字节的会补零。

  .. image:: doc/hid_test.png
     :alt: hid_test.png
