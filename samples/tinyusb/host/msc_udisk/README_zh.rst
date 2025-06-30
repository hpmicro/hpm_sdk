.. _usb_msc_u_disk:

USB MSC U Disk
============================

概述
------

本示例工程展示U盘（注意：U盘格式化的 `分配单元大小` 不能超过32KB，建议格式化参数为FAT32/32KB）访问操作

  .. image:: ./doc/UDisk_Format.png
     :alt: UDisk_Format

- MCU读取并列出U盘根目录文件及目录，并打印测试文件写入结果

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C转USB-A线缆连接U 盘和开发板USB0端口或者USB1端口

软件配置
------------

- 在CMakeLists.txt文件中，若未开启sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1)，则FATFS支持Cacheable Buffer的数据传输；若开启sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1)，则FATFS仅支持Noncacheable Buffer的数据传输。
- 当未开启sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1)时，建议传输数据的Buffer地址CacheLine对齐，否则会影响传输性能。

工程配置
------------

- 文件`CMakeLists.txt`中设置BOARD_TUH_RHPORT为0或1，0表示USB0, 1表示USB1

- USB host 属性可通过tusb_config.h或CMakeListx.txt配置

运行现象
------------

当工程正确运行后

* 观察U盘信息和文件写入结果

  .. image:: doc/1646400244250.png
     :alt: 1646400244250.png
