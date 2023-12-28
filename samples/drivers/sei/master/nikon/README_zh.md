# SEI Master: Connect NIKON Encoder

## 概述

该工程主要演示通过SEI接口获取NIKON编码器（型号为MAR-MC42AHN00）位置数据，自动计算采样/更新延时，并通过串口将信息打印出来。

## 配置

- 需要两块开发板，一块板子作为Master，一块板子作为Slave。
- Slave的sample的路径为：samples/drivers/sei/slave/nikon，master即为本示例。
- 将Master的SEI接口信号DATA_P/DATA_N与Slave的SEI接口信号DATA_P/DATA_N相连接。
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
SEI master nikon sample
Started sei engine!
ST:0xfffa5, MT:0x8888, sample_tm:224294997, update_tm:224299900, TimeDelay:306*0.1us
ST:0xfffa6, MT:0x8888, sample_tm:256294997, update_tm:256299900, TimeDelay:306*0.1us
ST:0xfffa7, MT:0x8888, sample_tm:288294997, update_tm:288299900, TimeDelay:306*0.1us
ST:0xfffa8, MT:0x8888, sample_tm:320294997, update_tm:320299900, TimeDelay:306*0.1us
ST:0xfffa9, MT:0x8888, sample_tm:352294997, update_tm:352299900, TimeDelay:306*0.1us
ST:0xfffaa, MT:0x8888, sample_tm:384294997, update_tm:384299900, TimeDelay:306*0.1us
ST:0xfffab, MT:0x8888, sample_tm:416294997, update_tm:416299900, TimeDelay:306*0.1us
ST:0xfffac, MT:0x8888, sample_tm:448294997, update_tm:448299900, TimeDelay:306*0.1us
ST:0xfffad, MT:0x8888, sample_tm:480294997, update_tm:480299900, TimeDelay:306*0.1us
ST:0xfffae, MT:0x8888, sample_tm:512294997, update_tm:512299900, TimeDelay:306*0.1us
ST:0xfffaf, MT:0x8888, sample_tm:544294997, update_tm:544299900, TimeDelay:306*0.1us
ST:0xfffb0, MT:0x8888, sample_tm:576294997, update_tm:576299901, TimeDelay:306*0.1us
ST:0xfffb1, MT:0x8888, sample_tm:608294997, update_tm:608299900, TimeDelay:306*0.1us
ST:0xfffb2, MT:0x8888, sample_tm:640294997, update_tm:640299901, TimeDelay:306*0.1us

```
