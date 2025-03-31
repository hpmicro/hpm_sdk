.. _sei_slave_simulate_endat_encoder:

SEI Slave: Simulate ENDAT Encoder
==================================================================

概述
------

该工程主要演示通过SEI接口模拟ENDAT编码器，产生编码器位置数据等，并通过串口将信息打印出来。

配置
------

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。

- Master的sample的路径为：samples/drivers/sei/master/endat，Slave即为本示例。

- 将Master的SEI_CLK跨针跨至Master侧，将Slave的SEI_CLK跨针跨至Slave侧。

- 将Master的SEI接口信号DATA_P/DATA_N与Slave的SEI接口信号DATA_P/DATA_N相连接。

- 将Master的SEI接口信号CLKO_P/CLKO_N与Slave的SEI接口信号CLKI_P/CLKI_N相连接。

- 将Master的GND与Slave的GND相连接。

运行现象
------------

- 通过串口终端查看到的log如下，ST数值每次加1，两次采样间隔时间为200ms。


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
   SEI slave EnDat sample
   Started sei engine!
   CMD:0x7, E:0x0, ST:0xa5a5, CRC:0x5, sample_tm1:20525173, sample_tm2:0, sample_interval:128282 us
   CMD:0x7, E:0x0, ST:0xa5a6, CRC:0xf, sample_tm1:52524852, sample_tm2:20525173, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5a7, CRC:0x1a, sample_tm1:84524531, sample_tm2:52524852, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5a8, CRC:0x7, sample_tm1:116524210, sample_tm2:84524531, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5a9, CRC:0x12, sample_tm1:148523889, sample_tm2:116524210, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5aa, CRC:0x18, sample_tm1:180523568, sample_tm2:148523889, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5ab, CRC:0xd, sample_tm1:212523247, sample_tm2:180523568, sample_interval:199997 us
   CMD:0x7, E:0x0, ST:0xa5ac, CRC:0x1d, sample_tm1:244522927, sample_tm2:212523247, sample_interval:199998 us

