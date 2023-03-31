# I2S Master

## 概述

I2S master示例工程展示了I2S控制器作为master搭配DMA与slave进行板间通信。

I2S master从I2S Slave获取audio数据，并通过DAO播放。

## 硬件设置

- 将两个板子的[I2S引脚](lab_board_app_i2s_pin)相连

  连接 Master FCLK 和 Slave FCLK

  连接 Master BCLK 和 Slave BCLK

  连接 Master RXD 和 Slave TXD

  板间共地

  连接一个喇叭到开发板的DAO接口

## 运行现象

- 当工程正确运行后，可以听到一段关于先楫半导体简介的音频
