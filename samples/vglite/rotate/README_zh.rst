.. _rotate:

Rotate
============

概述
------

该示例工程演示了如何使用vglite接口绘制矢量图形，并展示在屏幕上。

硬件配置
------------

连接开发板支持的屏幕，默认使用RGB屏。如需连接其他类型的屏，需要同步修改`board/xxx/CMakeLists.txt`,选择对应的屏幕

编译类型
------------

- flash_sdram_xip

- flash_sdram_xip_release

运行
------

一旦此程序正确运行，在屏幕上显示旋转的虎头图像, 串口终端会输出如下信息:

.. code-block:: console

   gpu: name: GCNanoUltraV, id: 0x00000555, rev: 0x00001230, cid: 0x0000041A

