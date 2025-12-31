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
   MT:0xa5a, ST:0xfa5, sample_tm:388891, update_tm:393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfa6, sample_tm:40388891, update_tm:40393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfa7, sample_tm:80388891, update_tm:80393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfa8, sample_tm:120388891, update_tm:120393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfa9, sample_tm:160388891, update_tm:160393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfaa, sample_tm:200388891, update_tm:200393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfab, sample_tm:240388891, update_tm:240393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfac, sample_tm:280388891, update_tm:280393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfad, sample_tm:320388891, update_tm:320393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfae, sample_tm:360388891, update_tm:360393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfaf, sample_tm:400388891, update_tm:400393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb0, sample_tm:440388891, update_tm:440393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb1, sample_tm:480388891, update_tm:480393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb2, sample_tm:520388891, update_tm:520393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb3, sample_tm:560388891, update_tm:560393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb4, sample_tm:600388891, update_tm:600393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb5, sample_tm:640388891, update_tm:640393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb6, sample_tm:680388891, update_tm:680393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb7, sample_tm:720388891, update_tm:720393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb8, sample_tm:760388891, update_tm:760393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfb9, sample_tm:800388891, update_tm:800393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfba, sample_tm:840388891, update_tm:840393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbb, sample_tm:880388891, update_tm:880393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbc, sample_tm:920388891, update_tm:920393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbd, sample_tm:960388891, update_tm:960393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbe, sample_tm:1000388891, update_tm:1000393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfbf, sample_tm:1040388891, update_tm:1040393992, TimeDelay:255*0.1us
   MT:0xa5a, ST:0xfc0, sample_tm:1080388891, update_tm:1080393992, TimeDelay:255*0.1us


