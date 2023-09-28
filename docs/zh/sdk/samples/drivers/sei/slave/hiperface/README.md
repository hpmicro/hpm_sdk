# SEI Slave: Simulate HIPERFACE Encoder

## 概述

- 该工程主要演示通过SEI接口模拟HIPERFACE编码器，产生编码器位置数据等，并通过串口将信息打印出来。
- 模拟的HIPERFACE编码器的型号为SKM36-HFA0-K02。

## 配置

- 准备一个USB转485模块
- 将SEI接口信号DATA_P/DATA_N与USB转485的A/B信号相连接。

## 运行现象

- 将程序下载至开发板并运行。
- 通过PC的串口调试助手与开发板进行通讯，模拟获取编码器数据。
- 串口调试助手设置为：波特率：9600；起始位：1位；数据位：8位；停止位：1位；校验位：偶校验。通过串口调试助手发送数据：0x40 0x42 0x02，开发板模拟的编码器将会进行响应。
![sei_slave_hiperface.png](../../../../../../../assets/sdk/samples/sei_slave_hiperface.png)
- 同时，可通过串口终端查看开发板输出的log信息，POS数值每次加1。

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
SEI slave hiperface sample
Started sei engine!
ADDR:0x40, CMD:0x42, POS:0xa5a5, sample_tm1:1952301066, sample_tm2:0, sample_interval:10846117 us
ADDR:0x40, CMD:0x42, POS:0xa5a6, sample_tm1:2088895956, sample_tm2:1952301066, sample_interval:758860 us
ADDR:0x40, CMD:0x42, POS:0xa5a7, sample_tm1:2263472347, sample_tm2:2088895956, sample_interval:969868 us
ADDR:0x40, CMD:0x42, POS:0xa5a8, sample_tm1:2294733614, sample_tm2:2263472347, sample_interval:173673 us
ADDR:0x40, CMD:0x42, POS:0xa5a9, sample_tm1:2341120092, sample_tm2:2294733614, sample_interval:257702 us
ADDR:0x40, CMD:0x42, POS:0xa5aa, sample_tm1:2378596173, sample_tm2:2341120092, sample_interval:208200 us
ADDR:0x40, CMD:0x42, POS:0xa5ab, sample_tm1:2412964847, sample_tm2:2378596173, sample_interval:190937 us
ADDR:0x40, CMD:0x42, POS:0xa5ac, sample_tm1:2443402462, sample_tm2:2412964847, sample_interval:169097 us
ADDR:0x40, CMD:0x42, POS:0xa5ad, sample_tm1:2475131711, sample_tm2:2443402462, sample_interval:176273 us
ADDR:0x40, CMD:0x42, POS:0xa5ae, sample_tm1:2507853084, sample_tm2:2475131711, sample_interval:181785 us
ADDR:0x40, CMD:0x42, POS:0xa5af, sample_tm1:2537055225, sample_tm2:2507853084, sample_interval:162234 us
ADDR:0x40, CMD:0x42, POS:0xa5b0, sample_tm1:2562569660, sample_tm2:2537055225, sample_interval:141746 us
ADDR:0x40, CMD:0x42, POS:0xa5b1, sample_tm1:2592295941, sample_tm2:2562569660, sample_interval:165146 us

```
