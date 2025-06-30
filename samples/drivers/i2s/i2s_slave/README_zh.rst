
.. _i2s_slave:

I2S Slave
==================

概述
------

I2S slave示例工程展示了I2S控制器作为slave搭配DMA与master进行板间通信。
I2S slave循环发送音频数据到I2S master。

工作流程
--------

1. 配置系统音频时钟：

   - 使用外部MCLK、BCLK和FCLK

2. 配置I2S Slave：

   - 设置FIFO阈值
   - 配置DMA请求
   - 配置采样率和位宽
   - 配置为从机模式

3. 配置DMA传输：

   - 设置源地址为音频数据缓冲区
   - 设置目标地址为I2S_SLAVE发送FIFO
   - 配置源地址递增模式
   - 配置目标地址固定模式
   - 使能DMA中断

4. 启动传输：

   - 使能I2S Slave
   - 启动DMA传输
   - 循环发送音频数据

已知问题
------------

无

硬件设置
------------

- 将两个板子的 :ref:`I2S引脚 <board_resource>` 相连：

  - 连接 Master FCLK 和 Slave FCLK
  - 连接 Master BCLK 和 Slave BCLK
  - 连接 Master RXD 和 Slave TXD
  - 板间共地

运行现象
------------

当工程正确运行后：

1. 串口终端输出信息：

.. code-block:: console

   I2S Slave Demo

2. I2S Slave将持续发送音频数据到I2S Master

调试建议
------------

1. 检查I2S连线是否正确
2. 确认DMA传输是否正常

