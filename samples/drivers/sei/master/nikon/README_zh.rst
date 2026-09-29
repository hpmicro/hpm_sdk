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
   ST:0xfffa5, MT:0x8888, CRC:0x63, TimeDelta:307*0.1us
   ST:0xfffa6, MT:0x8888, CRC:0x5e, TimeDelta:307*0.1us
   ST:0xfffa7, MT:0x8888, CRC:0x9a, TimeDelta:307*0.1us
   ST:0xfffa8, MT:0x8888, CRC:0x53, TimeDelta:307*0.1us
   ST:0xfffa9, MT:0x8888, CRC:0x97, TimeDelta:307*0.1us
   ST:0xfffaa, MT:0x8888, CRC:0xaa, TimeDelta:307*0.1us
   ST:0xfffab, MT:0x8888, CRC:0x6e, TimeDelta:307*0.1us
   ST:0xfffac, MT:0x8888, CRC:0xd0, TimeDelta:307*0.1us
   ST:0xfffad, MT:0x8888, CRC:0x14, TimeDelta:307*0.1us
   ST:0xfffae, MT:0x8888, CRC:0x29, TimeDelta:307*0.1us
   ST:0xfffaf, MT:0x8888, CRC:0xed, TimeDelta:307*0.1us
   ST:0xfffb0, MT:0x8888, CRC:0xca, TimeDelta:307*0.1us
   ST:0xfffb1, MT:0x8888, CRC:0xe, TimeDelta:307*0.1us
   ST:0xfffb2, MT:0x8888, CRC:0x33, TimeDelta:307*0.1us
   ST:0xfffb3, MT:0x8888, CRC:0xf7, TimeDelta:307*0.1us
   ST:0xfffb4, MT:0x8888, CRC:0x49, TimeDelta:307*0.1us

