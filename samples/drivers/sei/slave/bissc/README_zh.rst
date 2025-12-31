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
   MT:0xa5a, ST:0xfa5, EW:0x3, CRC:0x19, sample_tm1:198765386, sample_tm2:0, sample_interval:993826 us
   MT:0xa5a, ST:0xfa6, EW:0x3, CRC:0xd, sample_tm1:238766086, sample_tm2:198765386, sample_interval:200003 us
   MT:0xa5a, ST:0xfa7, EW:0x3, CRC:0x1, sample_tm1:278766786, sample_tm2:238766086, sample_interval:200003 us
   MT:0xa5a, ST:0xfa8, EW:0x3, CRC:0x6, sample_tm1:318767486, sample_tm2:278766786, sample_interval:200003 us
   MT:0xa5a, ST:0xfa9, EW:0x3, CRC:0xa, sample_tm1:358768187, sample_tm2:318767486, sample_interval:200003 us
   MT:0xa5a, ST:0xfaa, EW:0x3, CRC:0x1e, sample_tm1:398768887, sample_tm2:358768187, sample_interval:200003 us
   MT:0xa5a, ST:0xfab, EW:0x3, CRC:0x12, sample_tm1:438769588, sample_tm2:398768887, sample_interval:200003 us
   MT:0xa5a, ST:0xfac, EW:0x3, CRC:0x36, sample_tm1:478770288, sample_tm2:438769588, sample_interval:200003 us
   MT:0xa5a, ST:0xfad, EW:0x3, CRC:0x3a, sample_tm1:518770989, sample_tm2:478770288, sample_interval:200003 us
   MT:0xa5a, ST:0xfae, EW:0x3, CRC:0x2e, sample_tm1:558771690, sample_tm2:518770989, sample_interval:200003 us
   MT:0xa5a, ST:0xfaf, EW:0x3, CRC:0x22, sample_tm1:598772391, sample_tm2:558771690, sample_interval:200003 us
   MT:0xa5a, ST:0xfb0, EW:0x3, CRC:0x20, sample_tm1:638773091, sample_tm2:598772391, sample_interval:200003 us
   MT:0xa5a, ST:0xfb1, EW:0x3, CRC:0x2c, sample_tm1:678773792, sample_tm2:638773091, sample_interval:200003 us
   MT:0xa5a, ST:0xfb2, EW:0x3, CRC:0x38, sample_tm1:718774493, sample_tm2:678773792, sample_interval:200003 us
   MT:0xa5a, ST:0xfb3, EW:0x3, CRC:0x34, sample_tm1:758775195, sample_tm2:718774493, sample_interval:200003 us
   MT:0xa5a, ST:0xfb4, EW:0x3, CRC:0x10, sample_tm1:798775896, sample_tm2:758775195, sample_interval:200003 us
   MT:0xa5a, ST:0xfb5, EW:0x3, CRC:0x1c, sample_tm1:838776597, sample_tm2:798775896, sample_interval:200003 us
   MT:0xa5a, ST:0xfb6, EW:0x3, CRC:0x8, sample_tm1:878777299, sample_tm2:838776597, sample_interval:200003 us
   MT:0xa5a, ST:0xfb7, EW:0x3, CRC:0x4, sample_tm1:918778001, sample_tm2:878777299, sample_interval:200003 us
   MT:0xa5a, ST:0xfb8, EW:0x3, CRC:0x3, sample_tm1:958778702, sample_tm2:918778001, sample_interval:200003 us
   MT:0xa5a, ST:0xfb9, EW:0x3, CRC:0xf, sample_tm1:998779403, sample_tm2:958778702, sample_interval:200003 us
   MT:0xa5a, ST:0xfba, EW:0x3, CRC:0x1b, sample_tm1:1038780104, sample_tm2:998779403, sample_interval:200003 us
   MT:0xa5a, ST:0xfbb, EW:0x3, CRC:0x17, sample_tm1:1078780806, sample_tm2:1038780104, sample_interval:200003 us
   MT:0xa5a, ST:0xfbc, EW:0x3, CRC:0x33, sample_tm1:1118781507, sample_tm2:1078780806, sample_interval:200003 us
   MT:0xa5a, ST:0xfbd, EW:0x3, CRC:0x3f, sample_tm1:1158782209, sample_tm2:1118781507, sample_interval:200003 us
   MT:0xa5a, ST:0xfbe, EW:0x3, CRC:0x2b, sample_tm1:1198782910, sample_tm2:1158782209, sample_interval:200003 us
   MT:0xa5a, ST:0xfbf, EW:0x3, CRC:0x27, sample_tm1:1238783612, sample_tm2:1198782910, sample_interval:200003 us
   MT:0xa5a, ST:0xfc0, EW:0x3, CRC:0x3b, sample_tm1:1278784313, sample_tm2:1238783612, sample_interval:200003 us
   MT:0xa5a, ST:0xfc1, EW:0x3, CRC:0x37, sample_tm1:1318785015, sample_tm2:1278784313, sample_interval:200003 us

