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
   MT:0xa5a, ST:0xfa5, sample_tm1:268206022, sample_tm2:0, sample_interval:1341030 us
   MT:0xa5a, ST:0xfa6, sample_tm1:308206752, sample_tm2:268206022, sample_interval:200003 us
   MT:0xa5a, ST:0xfa7, sample_tm1:348207481, sample_tm2:308206752, sample_interval:200003 us
   MT:0xa5a, ST:0xfa8, sample_tm1:388208211, sample_tm2:348207481, sample_interval:200003 us
   MT:0xa5a, ST:0xfa9, sample_tm1:428208940, sample_tm2:388208211, sample_interval:200003 us
   MT:0xa5a, ST:0xfaa, sample_tm1:468209669, sample_tm2:428208940, sample_interval:200003 us
   MT:0xa5a, ST:0xfab, sample_tm1:508210398, sample_tm2:468209669, sample_interval:200003 us
   MT:0xa5a, ST:0xfac, sample_tm1:548211128, sample_tm2:508210398, sample_interval:200003 us
   MT:0xa5a, ST:0xfad, sample_tm1:588211857, sample_tm2:548211128, sample_interval:200003 us
   MT:0xa5a, ST:0xfae, sample_tm1:628212585, sample_tm2:588211857, sample_interval:200003 us
   MT:0xa5a, ST:0xfaf, sample_tm1:668213314, sample_tm2:628212585, sample_interval:200003 us
   MT:0xa5a, ST:0xfb0, sample_tm1:708214043, sample_tm2:668213314, sample_interval:200003 us
   MT:0xa5a, ST:0xfb1, sample_tm1:748214771, sample_tm2:708214043, sample_interval:200003 us
   MT:0xa5a, ST:0xfb2, sample_tm1:788215500, sample_tm2:748214771, sample_interval:200003 us
   MT:0xa5a, ST:0xfb3, sample_tm1:828216228, sample_tm2:788215500, sample_interval:200003 us
   MT:0xa5a, ST:0xfb4, sample_tm1:868216957, sample_tm2:828216228, sample_interval:200003 us
   MT:0xa5a, ST:0xfb5, sample_tm1:908217685, sample_tm2:868216957, sample_interval:200003 us
   MT:0xa5a, ST:0xfb6, sample_tm1:948218414, sample_tm2:908217685, sample_interval:200003 us
   MT:0xa5a, ST:0xfb7, sample_tm1:988219142, sample_tm2:948218414, sample_interval:200003 us
   MT:0xa5a, ST:0xfb8, sample_tm1:1028219871, sample_tm2:988219142, sample_interval:200003 us
   MT:0xa5a, ST:0xfb9, sample_tm1:1068220599, sample_tm2:1028219871, sample_interval:200003 us
   MT:0xa5a, ST:0xfba, sample_tm1:1108221327, sample_tm2:1068220599, sample_interval:200003 us
   MT:0xa5a, ST:0xfbb, sample_tm1:1148222055, sample_tm2:1108221327, sample_interval:200003 us
   MT:0xa5a, ST:0xfbc, sample_tm1:1188222783, sample_tm2:1148222055, sample_interval:200003 us
   MT:0xa5a, ST:0xfbd, sample_tm1:1228223512, sample_tm2:1188222783, sample_interval:200003 us
   MT:0xa5a, ST:0xfbe, sample_tm1:1268224240, sample_tm2:1228223512, sample_interval:200003 us
   MT:0xa5a, ST:0xfbf, sample_tm1:1308224968, sample_tm2:1268224240, sample_interval:200003 us
   MT:0xa5a, ST:0xfc0, sample_tm1:1348225695, sample_tm2:1308224968, sample_interval:200003 us
   MT:0xa5a, ST:0xfc1, sample_tm1:1388226423, sample_tm2:1348225695, sample_interval:200003 us
   MT:0xa5a, ST:0xfc2, sample_tm1:1428227151, sample_tm2:1388226423, sample_interval:200003 us


