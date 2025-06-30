.. _i2s_master:

I2S Master
====================

概述
------

I2S master示例工程展示了I2S控制器作为master搭配DMA与slave进行板间通信。
I2S master从I2S Slave获取audio数据，并通过DAO播放。

工作流程
--------

1. 配置系统音频时钟：

2. 配置I2S Master：

   - 使能MCLK输出
   - 设置FIFO阈值
   - 配置DMA请求
   - 配置采样率和位宽

3. 配置I2S_DAO和DAO

   - 配置采样率和位宽

3. 配置DMA传输：

   - 设置源地址为I2S_MASTER接收FIFO
   - 设置目标地址为I2S_DAO发送FIFO
   - 配置固定地址模式
   - 使能DMA中断

5. 启动传输：

   - 使能I2S Master
   - 使能I2S_DAO和DAO
   - 启动DMA传输

已知问题
------------

使用DAO播放某些音频时会产生噪声。

硬件设置
------------

- 将两个板子的 :ref:`I2S引脚 <board_resource>` 相连：

  - 连接 Master FCLK 和 Slave FCLK
  - 连接 Master BCLK 和 Slave BCLK
  - 连接 Master RXD 和 Slave TXD
  - 板间共地

- 连接一个喇叭到开发板的DAO接口


运行现象
------------

当工程正确运行后：

1. 串口终端输出信息：

.. code-block:: console

   I2S Master example

2. 可以听到一段关于先楫半导体简介的音频

调试建议
------------

1. 检查I2S连线是否正确
2. 确认DMA传输是否正常
