.. _lvgl_indev_usb_keyboard_mouse:

lvgl_indev_usb_keyboard_mouse
====================================

概述
------

本示例演示了在嵌入式系统上使用LVGL图形库结合USB HOST HID协议实现鼠标和键盘输入功能，主要特性包括：

- 支持USB HID鼠标的坐标移动和点击事件
- 支持USB HID键盘输入
- 适配LVGL v9版本和LVGL v8版本显示和输入设备框架，自适应LVGL版本。
- 集成FreeRTOS实时操作系统

注意
------

- USB协议栈初始化会设置键盘和鼠标设备运行在引导协议（boot protocol）
- 在引导协议中，对于键盘设备，在引导协议下仅支持标准的键盘扫描码集，不支持修饰键；对于鼠标设备，则只支持基本的指针移动和按钮点击功能。

硬件要求
------------

- RGB/MIPI接口
- USB鼠标/键盘外设

硬件配置
------------

- 连接开发板支持的屏幕，默认使用RGB屏。如需连接其他类型的屏，需要同步修改`board/xxx/CMakeLists.txt`,选择对应的屏幕
- 连接键鼠设备到USB0端口上。

编译类型
------------

- flash_sdram_xip

- flash_sdram_xip_release

运行
------

- 一旦此程序正确运行，在屏幕上显示鼠标图标以及待编辑的输入框
- 鼠标移动：使用鼠标在屏幕上移动光标
- 键盘输入：输入文本并显示在屏幕上

- 如果键鼠设备插上，console会输出如下信息：

.. code-block:: console

    [I/USB] EHCI HCIVERSION:0x0100
    [I/USB] EHCI HCSPARAMS:0x010011
    [I/USB] EHCI HCCPARAMS:0x0006
    [I/USB] EHCI ppc:1, n_ports:1, n_cc:0, n_pcc:0
    [I/USB] EHCI uses tt for ls/fs device
    [I/usbh_hub] New full-speed device on Bus 0, Hub 1, Port 1 connected
    [I/usbh_core] New device found,idVendor:1ea7,idProduct:0066,bcdDevice:0200
    [I/usbh_core] The device has 1 bNumConfigurations
    [I/usbh_core] The device has 2 interfaces
    [I/usbh_core] Enumeration success, start loading class driver
    [I/usbh_core] Loading hid class driver
    [I/usbh_hid] Ep=81 Attr=03 Mps=8 Interval=04 Mult=00
    [I/usbh_hid] Register HID Class:/dev/input0
    [I/usbh_core] Loading hid class driver
    [I/usbh_hid] Ep=82 Attr=03 Mps=8 Interval=02 Mult=00
    [I/usbh_hid] Register HID Class:/dev/input1
    [I/usbh_hid] Unregister HID Class:/dev/input0
    [I/usbh_hid] Unregister HID Class:/dev/input1
    [I/usbh_hub] Device on Bus 0, Hub 1, Port 1 disconnected
    [I/usbh_hub] New full-speed device on Bus 0, Hub 1, Port 1 connected
    [I/usbh_core] New device found,idVendor:1ea7,idProduct:0066,bcdDevice:0200
    [I/usbh_core] The device has 1 bNumConfigurations
    [I/usbh_core] The device has 2 interfaces
    [I/usbh_core] Enumeration success, start loading class driver
    [I/usbh_core] Loading hid class driver
    [I/usbh_hid] Ep=81 Attr=03 Mps=8 Interval=04 Mult=00
    [I/usbh_hid] Register HID Class:/dev/input0
    [I/usbh_core] Loading hid class driver
    [I/usbh_hid] Ep=82 Attr=03 Mps=8 Interval=02 Mult=00
    [I/usbh_hid] Register HID Class:/dev/input1
