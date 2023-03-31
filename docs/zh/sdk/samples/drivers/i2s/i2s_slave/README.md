# I2S Slave

## 概述

I2S slave示例工程展示了I2S控制器作为slave搭配DMA与master进行板间通信。
I2S slave循环发送音频数据到I2S master。

## 硬件设置

- 将两个板子的[I2S引脚](lab_board_app_i2s_pin)相连

  连接 Master FCLK 和 Slave FCLK

  连接 Master BCLK 和 Slave BCLK

  连接 Master RXD 和 Slave TXD

  板间共地

## 运行现象

无
