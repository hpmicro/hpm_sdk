.. _sei_slave_simulate_nikon_encoder:

SEI Slave: Simulate NIKON Encoder
==================================================================

概述
------

- 该工程主要演示通过SEI接口模拟NIKON编码器，产生编码器位置数据等，并通过串口将信息打印出来。

- 模拟的NIKON编码器的型号为MAR-MC42AHN00。

配置
------

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。

- Master的sample的路径为：samples/drivers/sei/master/nikon，Slave即为本示例。

- 将Master的SEI接口信号DATA_P/DATA_N与Slave的SEI接口信号DATA_P/DATA_N相连接。

- 将Master的GND与Slave的GND相连接。

运行现象
------------

- 将程序下载至开发板并运行，可通过串口终端查看开发板输出的log信息，ST数值每次加1。


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
   SEI slave nikon sample
   Started sei engine!
   EAX:0x4, CC:0x0, ST:0xfffa5, MT:0x8888, CRC:0xa9, sample_tm1:16614570, sample_tm2:0, sample_interval:103841 us
   EAX:0x4, CC:0x0, ST:0xfffa6, MT:0x8888, CRC:0x7a, sample_tm1:48614245, sample_tm2:16614570, sample_interval:199997 us
   EAX:0x4, CC:0x0, ST:0xfffa7, MT:0x8888, CRC:0x59, sample_tm1:80613920, sample_tm2:48614245, sample_interval:199997 us
   EAX:0x4, CC:0x0, ST:0xfffa8, MT:0x8888, CRC:0xca, sample_tm1:112613594, sample_tm2:80613920, sample_interval:199997 us
   EAX:0x4, CC:0x0, ST:0xfffa9, MT:0x8888, CRC:0xe9, sample_tm1:144613269, sample_tm2:112613594, sample_interval:199997 us
   EAX:0x4, CC:0x0, ST:0xfffaa, MT:0x8888, CRC:0x55, sample_tm1:176612944, sample_tm2:144613269, sample_interval:199997 us

