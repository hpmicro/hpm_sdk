.. _sei_slave_simulate_ssi_encoder:

SEI Slave: Simulate SSI Encoder
==============================================================

概述
------

该工程主要演示通过SEI接口模拟SSI编码器，产生编码器位置数据等，并通过串口将信息打印出来。

配置
------

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。

- Master的sample的路径为：samples/drivers/sei/master/ssi，Slave即为本示例。

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
   SEI slave SSI sample
   Started sei engine!
   MT:0xa5a, ST:0x5a5, sample_tm1:209155491, sample_tm2:0, sample_interval:1161974 us
   MT:0xa5a, ST:0x5a6, sample_tm1:245155712, sample_tm2:209155491, sample_interval:200001 us
   MT:0xa5a, ST:0x5a7, sample_tm1:281155932, sample_tm2:245155712, sample_interval:200001 us
   MT:0xa5a, ST:0x5a8, sample_tm1:317156152, sample_tm2:281155932, sample_interval:200001 us
   MT:0xa5a, ST:0x5a9, sample_tm1:353156373, sample_tm2:317156152, sample_interval:200001 us
   MT:0xa5a, ST:0x5aa, sample_tm1:389156593, sample_tm2:353156373, sample_interval:200001 us
   MT:0xa5a, ST:0x5ab, sample_tm1:425156813, sample_tm2:389156593, sample_interval:200001 us
   MT:0xa5a, ST:0x5ac, sample_tm1:461157033, sample_tm2:425156813, sample_interval:200001 us
   MT:0xa5a, ST:0x5ad, sample_tm1:497157253, sample_tm2:461157033, sample_interval:200001 us
   MT:0xa5a, ST:0x5ae, sample_tm1:533157472, sample_tm2:497157253, sample_interval:200001 us
   MT:0xa5a, ST:0x5af, sample_tm1:569157692, sample_tm2:533157472, sample_interval:200001 us
   MT:0xa5a, ST:0x5b0, sample_tm1:605157912, sample_tm2:569157692, sample_interval:200001 us
   MT:0xa5a, ST:0x5b1, sample_tm1:641158132, sample_tm2:605157912, sample_interval:200001 us
   MT:0xa5a, ST:0x5b2, sample_tm1:677158352, sample_tm2:641158132, sample_interval:200001 us
   MT:0xa5a, ST:0x5b3, sample_tm1:713158573, sample_tm2:677158352, sample_interval:200001 us
   MT:0xa5a, ST:0x5b4, sample_tm1:749158793, sample_tm2:713158573, sample_interval:200001 us
   MT:0xa5a, ST:0x5b5, sample_tm1:785159013, sample_tm2:749158793, sample_interval:200001 us
   MT:0xa5a, ST:0x5b6, sample_tm1:821159233, sample_tm2:785159013, sample_interval:200001 us
   MT:0xa5a, ST:0x5b7, sample_tm1:857159453, sample_tm2:821159233, sample_interval:200001 us
   MT:0xa5a, ST:0x5b8, sample_tm1:893159673, sample_tm2:857159453, sample_interval:200001 us
   MT:0xa5a, ST:0x5b9, sample_tm1:929159893, sample_tm2:893159673, sample_interval:200001 us
   MT:0xa5a, ST:0x5ba, sample_tm1:965160112, sample_tm2:929159893, sample_interval:200001 us

