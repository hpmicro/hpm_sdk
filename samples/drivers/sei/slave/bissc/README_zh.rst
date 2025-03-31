.. _sei_slave_simulate_bissc_encoder:

SEI Slave: Simulate BISSC Encoder
==================================================================

概述
------

该工程主要演示通过SEI接口模拟BISSC编码器，产生编码器位置数据等，并通过串口将信息打印出来。

配置
------

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。

- Master的sample的路径为：samples/drivers/sei/master/bissc，Slave即为本示例。

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
   SEI slave BiSS_C sample
   Started sei engine!
   MT:0xa5a, ST:0x5a5, EW:0x3, CRC:0x7991, sample_tm1:217221230, sample_tm2:0, sample_interval:1206784 us
   MT:0xa5a, ST:0x5a6, EW:0x3, CRC:0x6, sample_tm1:253221299, sample_tm2:217221230, sample_interval:200000 us
   MT:0xa5a, ST:0x5a7, EW:0x3, CRC:0x799174a, sample_tm1:289221368, sample_tm2:253221299, sample_interval:200000 us
   MT:0xa5a, ST:0x5a8, EW:0x3, CRC:0x7991746, sample_tm1:325221437, sample_tm2:289221368, sample_interval:200000 us
   MT:0xa5a, ST:0x5a9, EW:0x3, CRC:0x7991701, sample_tm1:361221506, sample_tm2:325221437, sample_interval:200000 us
   MT:0xa5a, ST:0x5aa, EW:0x3, CRC:0x799170d, sample_tm1:397221575, sample_tm2:361221506, sample_interval:200000 us
   MT:0xa5a, ST:0x5ab, EW:0x3, CRC:0x7991719, sample_tm1:433221645, sample_tm2:397221575, sample_interval:200000 us
   MT:0xa5a, ST:0x5ac, EW:0x3, CRC:0x7991715, sample_tm1:469221714, sample_tm2:433221645, sample_interval:200000 us
   MT:0xa5a, ST:0x5ad, EW:0x3, CRC:0x7991731, sample_tm1:505221783, sample_tm2:469221714, sample_interval:200000 us
   MT:0xa5a, ST:0x5ae, EW:0x3, CRC:0x799173d, sample_tm1:541221852, sample_tm2:505221783, sample_interval:200000 us
   MT:0xa5a, ST:0x5af, EW:0x3, CRC:0x7991729, sample_tm1:577221921, sample_tm2:541221852, sample_interval:200000 us
   MT:0xa5a, ST:0x5b0, EW:0x3, CRC:0x7991725, sample_tm1:613221990, sample_tm2:577221921, sample_interval:200000 us
   MT:0xa5a, ST:0x5b1, EW:0x3, CRC:0x79917a7, sample_tm1:649222059, sample_tm2:613221990, sample_interval:200000 us
   MT:0xa5a, ST:0x5b2, EW:0x3, CRC:0x79917ab, sample_tm1:685222128, sample_tm2:649222059, sample_interval:200000 us
   MT:0xa5a, ST:0x5b3, EW:0x3, CRC:0x79917bf, sample_tm1:721222197, sample_tm2:685222128, sample_interval:200000 us
   MT:0xa5a, ST:0x5b4, EW:0x3, CRC:0x79917b3, sample_tm1:757222267, sample_tm2:721222197, sample_interval:200000 us
   MT:0xa5a, ST:0x5b5, EW:0x3, CRC:0x7991797, sample_tm1:793222336, sample_tm2:757222267, sample_interval:200000 us
   MT:0xa5a, ST:0x5b6, EW:0x3, CRC:0x799179b, sample_tm1:829222405, sample_tm2:793222336, sample_interval:200000 us
   MT:0xa5a, ST:0x5b7, EW:0x3, CRC:0x799178f, sample_tm1:865222474, sample_tm2:829222405, sample_interval:200000 us

