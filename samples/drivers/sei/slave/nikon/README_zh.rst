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
   EAX:0x4, CC:0, ST:0xfffa5, MT:0x8888, CRC:0x63, sample_interval:1147104 us
   EAX:0x4, CC:0, ST:0xfffa6, MT:0x8888, CRC:0x5e, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffa7, MT:0x8888, CRC:0x9a, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffa8, MT:0x8888, CRC:0x53, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffa9, MT:0x8888, CRC:0x97, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffaa, MT:0x8888, CRC:0xaa, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffab, MT:0x8888, CRC:0x6e, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffac, MT:0x8888, CRC:0xd0, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffad, MT:0x8888, CRC:0x14, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffae, MT:0x8888, CRC:0x29, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffaf, MT:0x8888, CRC:0xed, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb0, MT:0x8888, CRC:0xca, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb1, MT:0x8888, CRC:0xe, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb2, MT:0x8888, CRC:0x33, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb3, MT:0x8888, CRC:0xf7, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb4, MT:0x8888, CRC:0x49, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb5, MT:0x8888, CRC:0x8d, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb6, MT:0x8888, CRC:0xb0, sample_interval:19999 us
   EAX:0x4, CC:0, ST:0xfffb7, MT:0x8888, CRC:0x74, sample_interval:19999 us

