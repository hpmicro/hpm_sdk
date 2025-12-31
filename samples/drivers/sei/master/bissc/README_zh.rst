.. _sei_master_connect_bissc_encoder:

SEI Master: Connect BISSC Encoder
==================================================================

概述
------

该工程主要演示通过SEI接口获取BISSC编码器位置数据。通过锁存采样时刻和锁存数据的边沿时刻来实现总线延时检测，并自动计算出合理的采样点。此外，也会自动计算出数据采样时刻和更新时刻延时，并通过串口将信息打印出来。

配置
------

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。

- Slave的sample的路径为：samples/drivers/sei/slave/bissc，master即为本示例。

- 将Master的SEI_CLK跨针跨至Master侧，将Slave的SEI_CLK跨针跨至Slave侧。

- 将Master的SEI接口信号DATA_P/DATA_N与Slave的SEI接口信号DATA_P/DATA_N相连接。

- 将Master的SEI接口信号CLKO_P/CLKO_N与Slave的SEI接口信号CLKI_P/CLKI_N相连接。

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
   SEI master BiSS_C sample
   Started sei engine!
   delta: 1280 ns
   MT:0xa5a, ST:0xfa500000, EW:0x3, CRC:0x19, sample_tm:391973, update_tm:399174, TimeDelay:360*0.1us
   delta: 1275 ns
   MT:0xa5a, ST:0xfa600000, EW:0x3, CRC:0xd, sample_tm:40391973, update_tm:40399174, TimeDelay:360*0.1us
   delta: 1275 ns
   rx point changed!
   MT:0xa5a, ST:0xfa700000, EW:0x3, CRC:0x1, sample_tm:80391973, update_tm:80399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfa800000, EW:0x3, CRC:0x6, sample_tm:120391973, update_tm:120399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfa900000, EW:0x3, CRC:0xa, sample_tm:160391973, update_tm:160399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfaa00000, EW:0x3, CRC:0x1e, sample_tm:200391973, update_tm:200399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfab00000, EW:0x3, CRC:0x12, sample_tm:240391973, update_tm:240399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfac00000, EW:0x3, CRC:0x36, sample_tm:280391973, update_tm:280399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfad00000, EW:0x3, CRC:0x3a, sample_tm:320391973, update_tm:320399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfae00000, EW:0x3, CRC:0x2e, sample_tm:360391973, update_tm:360399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfaf00000, EW:0x3, CRC:0x22, sample_tm:400391973, update_tm:400399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb000000, EW:0x3, CRC:0x20, sample_tm:440391973, update_tm:440399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb100000, EW:0x3, CRC:0x2c, sample_tm:480391973, update_tm:480399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb200000, EW:0x3, CRC:0x38, sample_tm:520391973, update_tm:520399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb300000, EW:0x3, CRC:0x34, sample_tm:560391973, update_tm:560399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb400000, EW:0x3, CRC:0x10, sample_tm:600391973, update_tm:600399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb500000, EW:0x3, CRC:0x1c, sample_tm:640391973, update_tm:640399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb600000, EW:0x3, CRC:0x8, sample_tm:680391973, update_tm:680399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb700000, EW:0x3, CRC:0x4, sample_tm:720391973, update_tm:720399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb800000, EW:0x3, CRC:0x3, sample_tm:760391973, update_tm:760399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfb900000, EW:0x3, CRC:0xf, sample_tm:800391973, update_tm:800399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfba00000, EW:0x3, CRC:0x1b, sample_tm:840391973, update_tm:840399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbb00000, EW:0x3, CRC:0x17, sample_tm:880391973, update_tm:880399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbc00000, EW:0x3, CRC:0x33, sample_tm:920391973, update_tm:920399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbd00000, EW:0x3, CRC:0x3f, sample_tm:960391973, update_tm:960399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbe00000, EW:0x3, CRC:0x2b, sample_tm:1000391973, update_tm:1000399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfbf00000, EW:0x3, CRC:0x27, sample_tm:1040391973, update_tm:1040399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc000000, EW:0x3, CRC:0x3b, sample_tm:1080391973, update_tm:1080399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc100000, EW:0x3, CRC:0x37, sample_tm:1120391973, update_tm:1120399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc200000, EW:0x3, CRC:0x23, sample_tm:1160391973, update_tm:1160399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc300000, EW:0x3, CRC:0x2f, sample_tm:1200391973, update_tm:1200399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc400000, EW:0x3, CRC:0xb, sample_tm:1240391973, update_tm:1240399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc500000, EW:0x3, CRC:0x7, sample_tm:1280391973, update_tm:1280399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc600000, EW:0x3, CRC:0x13, sample_tm:1320391973, update_tm:1320399174, TimeDelay:360*0.1us
   MT:0xa5a, ST:0xfc700000, EW:0x3, CRC:0x1f, sample_tm:1360391973, update_tm:1360399174, TimeDelay:360*0.1us

