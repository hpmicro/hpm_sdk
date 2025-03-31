.. _sei_master_connect_nikon_encoder:

SEI Master: Connect NIKON Encoder
==================================================================

概述
------

该工程主要演示通过SEI接口获取NIKON编码器（型号为MAR-MC42AHN00）位置数据，自动计算采样/更新延时，并通过串口将信息打印出来。

配置
------

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。

- Slave的sample的路径为：samples/drivers/sei/slave/nikon，master即为本示例。

- 将Master的SEI接口信号DATA_P/DATA_N与Slave的SEI接口信号DATA_P/DATA_N相连接。

- 将Master的GND与Slave的GND相连接。

运行现象
------------

- 通过串口终端查看到的log如下，ST数值每次加1。


.. code-block:: console

   ----------------------------------------------------------------------
   $$\   $$\ $$$$$$$\  $$\      $$\ $$\
   $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
   $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
   $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
   $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
   $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
   $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
   \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
   ----------------------------------------------------------------------
   SEI master nikon sample
   Started sei engine!
   ST:0xfffa5, MT:0x8888, CRC:0xa9, sample_tm:320369459, update_tm:320375597, TimeDelay:306*0.1us
   ST:0xfffa6, MT:0x8888, CRC:0x7a, sample_tm:360369459, update_tm:360375597, TimeDelay:306*0.1us
   ST:0xfffa7, MT:0x8888, CRC:0x59, sample_tm:400369459, update_tm:400375598, TimeDelay:306*0.1us
   ST:0xfffa8, MT:0x8888, CRC:0xca, sample_tm:440369459, update_tm:440375597, TimeDelay:306*0.1us
   ST:0xfffa9, MT:0x8888, CRC:0xe9, sample_tm:480369459, update_tm:480375597, TimeDelay:306*0.1us
   ST:0xfffaa, MT:0x8888, CRC:0x55, sample_tm:520369459, update_tm:520375597, TimeDelay:306*0.1us
   ST:0xfffab, MT:0x8888, CRC:0x76, sample_tm:560369459, update_tm:560375598, TimeDelay:306*0.1us
   ST:0xfffac, MT:0x8888, CRC:0xb, sample_tm:600369459, update_tm:600375598, TimeDelay:306*0.1us

