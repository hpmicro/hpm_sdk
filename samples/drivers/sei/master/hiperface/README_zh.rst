.. _sei_master_connect_hiperface_encoder:

SEI Master: Connect HIPERFACE Encoder
==========================================================================

概述
------

该工程主要演示通过SEI接口获取HIPERFACE编码器位置数据，自动计算采样/更新延时，并通过串口将信息打印出来。

配置
------

- HIPERFACE编码器的型号为SKM36-HFA0-K02。

- 将SEI接口信号DATA_P/DATA_N与编码器的数据信号相连接，编码器通过外接电源+9V供电。

运行现象
------------

- 通过串口终端查看到的log如下，旋转编码器，相应的数值会变化。


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
   SEI master hiperface sample
   Started sei engine!
   rev:0xa, pos:0x5a500000, addr:0x40, CRC:0x2, TimeDelta:7760 us
   rev:0xa, pos:0x5a600000, addr:0x40, CRC:0x1, TimeDelta:7760 us
   rev:0xa, pos:0x5a700000, addr:0x40, CRC:0x0, TimeDelta:7760 us
   rev:0xa, pos:0x5a800000, addr:0x40, CRC:0xf, TimeDelta:7760 us
   rev:0xa, pos:0x5a900000, addr:0x40, CRC:0xe, TimeDelta:7760 us
   rev:0xa, pos:0x5aa00000, addr:0x40, CRC:0xd, TimeDelta:7760 us
   rev:0xa, pos:0x5ab00000, addr:0x40, CRC:0xc, TimeDelta:7760 us
   rev:0xa, pos:0x5ac00000, addr:0x40, CRC:0xb, TimeDelta:7760 us
   rev:0xa, pos:0x5ad00000, addr:0x40, CRC:0xa, TimeDelta:7760 us
   rev:0xa, pos:0x5ae00000, addr:0x40, CRC:0x9, TimeDelta:7760 us
   rev:0xa, pos:0x5af00000, addr:0x40, CRC:0x8, TimeDelta:7760 us
   rev:0xa, pos:0x5b000000, addr:0x40, CRC:0x17, TimeDelta:7760 us
   rev:0xa, pos:0x5b100000, addr:0x40, CRC:0x16, TimeDelta:7760 us
   rev:0xa, pos:0x5b200000, addr:0x40, CRC:0x15, TimeDelta:7760 us
   rev:0xa, pos:0x5b300000, addr:0x40, CRC:0x14, TimeDelta:7760 us
   rev:0xa, pos:0x5b400000, addr:0x40, CRC:0x13, TimeDelta:7760 us
