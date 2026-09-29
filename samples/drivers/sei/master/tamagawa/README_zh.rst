.. _sei_master_connect_tamagawa_encoder:

SEI Master: Connect TAMAGAWA Encoder
========================================================================

概述
------

该工程主要演示通过SEI接口获取多摩川编码器位置数据，自动计算采样/更新延时，并通过串口将信息打印出来。

配置
------

- 多摩川编码器的型号为TS5700N8401。

- 将SEI接口信号DATA_P/DATA_N与编码器的数据信号相连接，编码器通过开发板+5V供电。

运行现象
------------

- 通过串口终端查看到的log如下，旋转编码器，相应的数值会变化。


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
   SEI master tamagawa sample
   Started sei engine!
   MT:0x8888, ST:0xa5a5, ALMC:0, CRC:0xd, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5a6, ALMC:0, CRC:0xe, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5a7, ALMC:0, CRC:0xf, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5a8, ALMC:0, CRC:0, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5a9, ALMC:0, CRC:0x1, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5aa, ALMC:0, CRC:0x2, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5ab, ALMC:0, CRC:0x3, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5ac, ALMC:0, CRC:0x4, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5ad, ALMC:0, CRC:0x5, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5ae, ALMC:0, CRC:0x6, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5af, ALMC:0, CRC:0x7, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5b0, ALMC:0, CRC:0x18, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5b1, ALMC:0, CRC:0x19, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5b2, ALMC:0, CRC:0x1a, TimeDelta:462*0.1us
   MT:0x8888, ST:0xa5b3, ALMC:0, CRC:0x1b, TimeDelta:462*0.1us

