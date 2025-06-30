.. _qeov2_sei2abz:

QEOV2_SEI2ABZ
==========================

概述
------

QEOV2_SEI2ABZ示例工程展示了QEOv2 (Quadrature Encoder Output Version 2) 外设通过SEI接口获取传感器位置信息(本示例使用多摩川传感器TS5700N8401)，根据SEI获取的位置信息，动态地调整QEO输出ABZ信号的宽度，实现均匀输出ABZ信号。

工作流程
-----------

本示例演示了SEI位置信息到QEO ABZ信号的转换过程：

1. SEI位置采集：

   - 配置SEI接口采样参数
   - 使能SEI采样中断
   - 实时获取传感器位置信息

2. QEO ABZ信号生成：

   - 配置ABZ信号参数：
     * 设置编码器分辨率（4096线/圈）
     * 设置最大输出频率（250kHz）
     * 配置位置同步功能

   - 动态宽度调整：
     * 在SEI采样中断中计算位置变化
     * 根据位置变化调整ABZ信号宽度
     * 确保ABZ信号输出均匀性


硬件设置
------------

1. 传感器连接：
   - 将多摩川传感器TS5700N8401的DATA_P/DATA_N信号连接至开发板对应引脚
   - 为传感器提供5V电源供电

2. 信号观测：
   - 连接开发板ABZ输出引脚（QEO_A/QEO_B/QEO_Z）至逻辑分析仪
   - 用于抓取输出波形观察
   (请参考 :ref:`引脚描述 <board_resource>` 部分)

运行现象
------------

当工程正确运行后，可以观察到以下现象：

1. 串口终端输出信息：

.. code-block:: console

   QEO SEI2ABZ example
   QEO generate ABZ signal with hardware(SEI) provide postion
   device init finish

2. ABZ信号输出：

转动传感器时，可以通过逻辑分析仪观察到：
- A/B信号：90度相位差的正交方波
- Z信号：每转一圈输出一个基准脉冲
- 信号特点：与转动速度相匹配的均匀ABZ波形

