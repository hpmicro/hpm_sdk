.. _sei_master_connect_ssi_encoder:

SEI Master: Connect SSI Encoder
==============================================================

概述
------

该工程主要演示通过SEI接口获取SSI编码器位置数据，自动计算采样/更新延时，并通过串口将信息打印出来。

配置
------

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。

- Slave的sample的路径为：samples/drivers/sei/slave/ssi，master即为本示例。

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
   SEI master SSI sample
   Started sei engine!
   MT:0xa5a, ST:0x5a5, sample_tm:330792, update_tm:335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5a6, sample_tm:36330792, update_tm:36335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5a7, sample_tm:72330792, update_tm:72335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5a8, sample_tm:108330792, update_tm:108335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5a9, sample_tm:144330792, update_tm:144335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5aa, sample_tm:180330792, update_tm:180335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ab, sample_tm:216330792, update_tm:216335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ac, sample_tm:252330792, update_tm:252335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ad, sample_tm:288330792, update_tm:288335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ae, sample_tm:324330792, update_tm:324335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5af, sample_tm:360330792, update_tm:360335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b0, sample_tm:396330792, update_tm:396335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b1, sample_tm:432330792, update_tm:432335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b2, sample_tm:468330792, update_tm:468335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b3, sample_tm:504330792, update_tm:504335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b4, sample_tm:540330792, update_tm:540335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b5, sample_tm:576330792, update_tm:576335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b6, sample_tm:612330792, update_tm:612335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b7, sample_tm:648330792, update_tm:648335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b8, sample_tm:684330792, update_tm:684335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5b9, sample_tm:720330792, update_tm:720335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5ba, sample_tm:756330792, update_tm:756335383, TimeDelay:255*0.1us
   MT:0xa5a, ST:0x5bb, sample_tm:792330792, update_tm:792335383, TimeDelay:255*0.1us

