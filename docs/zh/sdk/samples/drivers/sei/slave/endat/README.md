# SEI Slave: Simulate ENDAT Encoder

## 概述

该工程主要演示通过SEI接口模拟ENDAT编码器，产生编码器位置数据等，并通过串口将信息打印出来。

## 配置

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。
- Master的sample的路径为：samples/drivers/sei/master/endat，Slave即为本示例。
- 将Master的SEI_CLK跨针跨至Master侧，将Slave的SEI_CLK跨针跨至Slave侧。
- 将Master的SEI接口信号DATA_P/DATA_N与Slave的SEI接口信号DATA_P/DATA_N相连接。
- 将Master的SEI接口信号CLKO_P/CLKO_N与Slave的SEI接口信号CLKI_P/CLKI_N相连接。
- 将Master的GND与Slave的GND相连接。

## 运行现象

- 通过串口终端查看到的log如下，ST数值每次加1，两次采样间隔时间为200ms。

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
SEI slave EnDat sample
Started sei engine!
CMD:0x7, E:0x0, ST:0xa5a5, CRC:0x0, sample_tm1:160292400, sample_tm2:0, sample_interval:890513 us
CMD:0x7, E:0x0, ST:0xa5a6, CRC:0xb837d65, sample_tm1:196292609, sample_tm2:160292400, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5a7, CRC:0x6e707af, sample_tm1:232292818, sample_tm2:196292609, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5a8, CRC:0xf5f54da, sample_tm1:268293027, sample_tm2:232292818, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5a9, CRC:0x16230a7, sample_tm1:304293236, sample_tm2:268293027, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5aa, CRC:0x8da63d2, sample_tm1:340293445, sample_tm2:304293236, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5ab, CRC:0x5be1918, sample_tm1:376293654, sample_tm2:340293445, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5ac, CRC:0xc064a6d, sample_tm1:412293863, sample_tm2:376293654, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5ad, CRC:0x30c247d, sample_tm1:448294072, sample_tm2:412293863, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5ae, CRC:0xab47708, sample_tm1:484294281, sample_tm2:448294072, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5af, CRC:0x7d00dc2, sample_tm1:520294489, sample_tm2:484294281, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b0, CRC:0xe685eb7, sample_tm1:556294698, sample_tm2:520294489, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b1, CRC:0xcebff9, sample_tm1:592294907, sample_tm2:556294698, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b2, CRC:0x976ec8c, sample_tm1:628295117, sample_tm2:592294907, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b3, CRC:0x4129646, sample_tm1:664295326, sample_tm2:628295117, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b4, CRC:0xdaac533, sample_tm1:700295535, sample_tm2:664295326, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b5, CRC:0x2a0ab23, sample_tm1:736295743, sample_tm2:700295535, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b6, CRC:0xb18f856, sample_tm1:772295951, sample_tm2:736295743, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b7, CRC:0x67c829c, sample_tm1:808296158, sample_tm2:772295951, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b8, CRC:0xfc4d1e9, sample_tm1:844296367, sample_tm2:808296158, sample_interval:200001 us
CMD:0x7, E:0x0, ST:0xa5b9, CRC:0x1f9b594, sample_tm1:880296575, sample_tm2:844296367, sample_interval:200001 us


```
