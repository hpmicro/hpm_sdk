.. _mipi_csi:

MIPI_CSI
================

概述
------

示例工程展示了使用`MIPI_CSI`接口获取摄像头的视频数据，并通过`CAM`接收视频数据到缓冲区，然后通过屏幕进行显示。

硬件设置
------------

1. 连接摄像头和屏幕到video扩展板
2. 连接video扩展板到开发板

工程设置
------------

- 该例子由于板载RGB和mipi csi引脚硬件设计冲突，不支持RGB LCD屏，需要修改`board/xxx/CMakeLists.txt`,选择对应的屏幕(lvds或者mipi)

注意事项
------------

- 摄像头支持: OV5640

- 屏支持: mipi或者lvds接口

运行现象
------------

当工程正确运行后，屏幕会显示摄像头采集的数据，而串口终端会输出如下信息：


.. code-block:: console

   mipi_csi example
   I2C bus is ready
   name: cc10128007, lcdc_clk: 74250khz
   mipi csi clk freq: 13813953 Hz

