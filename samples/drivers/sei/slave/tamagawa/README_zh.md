# SEI Slave: Simulate TAMAGAWA Encoder

## 概述

- 该工程主要演示通过SEI接口模拟TAMAGAWA编码器，产生编码器位置数据等，并通过串口将信息打印出来。
- 模拟的TAMAGAWA编码器的型号为TS5700N8401。

## 配置

- 准备一个USB转485模块
- 将SEI接口信号DATA_P/DATA_N与USB转485的A/B信号相连接。

## 运行现象

- 将程序下载至开发板并运行。
- 通过PC的串口调试助手与开发板进行通讯，模拟获取编码器数据。
- 串口调试助手设置为：波特率：2500000；起始位：1位；数据位：8位；停止位：1位；校验位：无。通过串口调试助手发送数据：0x1A，开发板模拟的编码器将会进行响应。
- 同时，可通过串口终端查看开发板输出的log信息，ST数值每次加1。

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
SEI slave tamagawa sample
Started sei engine!
CMD:0x1a, SF:0x0, ST:0xa5a5, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x2c2, sample_tm1:15994438, sample_tm2:0, sample_interval:88857 us
CMD:0x1a, SF:0x0, ST:0xa5a6, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x0, sample_tm1:51994618, sample_tm2:15994438, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5a7, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x61707070, sample_tm1:87994798, sample_tm2:51994618, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5a8, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0xe1f0f0f0, sample_tm1:123994978, sample_tm2:87994798, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5a9, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x11000000, sample_tm1:159995159, sample_tm2:123994978, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5aa, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x91808080, sample_tm1:195995339, sample_tm2:159995159, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5ab, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x51404040, sample_tm1:231995520, sample_tm2:195995339, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5ac, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0xd1c0c0c0, sample_tm1:304667289, sample_tm2:231995520, sample_interval:403732 us
CMD:0x1a, SF:0x0, ST:0xa5ad, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x31202020, sample_tm1:340667470, sample_tm2:304667289, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5ae, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0xb1a0a0a0, sample_tm1:376667651, sample_tm2:340667470, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5af, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x71606060, sample_tm1:412667833, sample_tm2:376667651, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5b0, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0xf1e0e0e0, sample_tm1:448668014, sample_tm2:412667833, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5b1, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x9181818, sample_tm1:484668195, sample_tm2:448668014, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5b2, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x89989898, sample_tm1:520668377, sample_tm2:484668195, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5b3, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x49585858, sample_tm1:556668558, sample_tm2:520668377, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5b4, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0xc9d8d8d8, sample_tm1:592668740, sample_tm2:556668558, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5b5, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x29383838, sample_tm1:628668922, sample_tm2:592668740, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5b6, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0xa9b8b8b8, sample_tm1:664669104, sample_tm2:628668922, sample_interval:200001 us
CMD:0x1a, SF:0x0, ST:0xa5b7, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x69787878, sample_tm1:700669286, sample_tm2:664669104, sample_interval:200001 us

```

