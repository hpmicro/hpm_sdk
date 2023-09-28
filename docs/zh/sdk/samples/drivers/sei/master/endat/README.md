# SEI Master: Connect ENDAT Encoder

## 概述

该工程主要演示通过SEI接口获取ENDAT编码器位置数据，自动计算采样/更新延时，并通过串口将信息打印出来。

## 配置

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。
- Slave的sample的路径为：samples/drivers/sei/slave/endat，master即为本示例。
- 将Master的SEI_CLK跨针跨至Master侧，将Slave的SEI_CLK跨针跨至Slave侧。
- 将Master的SEI接口信号DATA_P/DATA_N与Slave的SEI接口信号DATA_P/DATA_N相连接。
- 将Master的SEI接口信号CLKO_P/CLKO_N与Slave的SEI接口信号CLKI_P/CLKI_N相连接。
- 将Master的GND与Slave的GND相连接。

## 运行现象

- 通过串口终端查看到的log如下，ST数值每次加1。

```console

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
SEI master EnDat sample
Started sei engine!
E:0x0, ST:0xa5a5, CRC:0xb837d65, sample_tm:331659, update_tm:340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5a6, CRC:0x6e707af, sample_tm:36331659, update_tm:36340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5a7, CRC:0xf5f54da, sample_tm:72331659, update_tm:72340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5a8, CRC:0x16230a7, sample_tm:108331659, update_tm:108340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5a9, CRC:0x8da63d2, sample_tm:144331659, update_tm:144340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5aa, CRC:0x5be1918, sample_tm:180331659, update_tm:180340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5ab, CRC:0xc064a6d, sample_tm:216331659, update_tm:216340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5ac, CRC:0x30c247d, sample_tm:252331659, update_tm:252340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5ad, CRC:0xab47708, sample_tm:288331659, update_tm:288340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5ae, CRC:0x7d00dc2, sample_tm:324331659, update_tm:324340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5af, CRC:0xe685eb7, sample_tm:360331659, update_tm:360340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5b0, CRC:0xcebff9, sample_tm:396331659, update_tm:396340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5b1, CRC:0x976ec8c, sample_tm:432331659, update_tm:432340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5b2, CRC:0x4129646, sample_tm:468331659, update_tm:468340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5b3, CRC:0xdaac533, sample_tm:504331659, update_tm:504340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5b4, CRC:0x2a0ab23, sample_tm:540331659, update_tm:540340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5b5, CRC:0xb18f856, sample_tm:576331659, update_tm:576340210, TimeDelay:475*0.1us
E:0x0, ST:0xa5b6, CRC:0x67c829c, sample_tm:612331659, update_tm:612340210, TimeDelay:475*0.1us



```
