.. _sdcard_fatfs_demo:

SDXC FATFS示例
========================

概览
------

本示例提供如下功能:
- 用FAT32格式来格式化SD卡

- 创建hello.txt

- 从hello.txt读出一行文本

- 创建并删除文件夹

- 大文件测试

本示例支持在`中断`或`轮询`模式下工作，默认工作在`轮询`模式。若想切换到中断模式下，请参考`CMakeLists.txt`中的注释

板级设置
------------

无特殊要求

运行示例
------------

当示例成功运行后，会显示如下菜单：


.. code-block:: console

   SD FATFS demo
   -----------------------------------
   1 - Format the SD card with FATFS
   2 - Create hello.txt
   3 - Read 1st line from hello.txt
   4 - Directory related test
   s - Large file write test
   Others - Show menu

