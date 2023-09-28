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
ST:0xfc1, MT:0x8889, sample_tm:332635, update_tm:337939, TimeDelay:294*0.1us
ST:0xfc2, MT:0x8889, sample_tm:36332635, update_tm:36337939, TimeDelay:294*0.1us
ST:0xfc3, MT:0x8889, sample_tm:72332635, update_tm:72337940, TimeDelay:294*0.1us
ST:0xfc4, MT:0x8889, sample_tm:108332635, update_tm:108337939, TimeDelay:294*0.1us
ST:0xfc5, MT:0x8889, sample_tm:144332635, update_tm:144337939, TimeDelay:294*0.1us
ST:0xfc6, MT:0x8889, sample_tm:180332635, update_tm:180337939, TimeDelay:294*0.1us
ST:0xfc7, MT:0x8889, sample_tm:216332635, update_tm:216337939, TimeDelay:294*0.1us
ST:0xfc8, MT:0x8889, sample_tm:252332635, update_tm:252337940, TimeDelay:294*0.1us
ST:0xfc9, MT:0x8889, sample_tm:288332635, update_tm:288337940, TimeDelay:294*0.1us
ST:0xfca, MT:0x8889, sample_tm:324332635, update_tm:324337939, TimeDelay:294*0.1us
ST:0xfcb, MT:0x8889, sample_tm:360332635, update_tm:360337940, TimeDelay:294*0.1us
ST:0xfcc, MT:0x8889, sample_tm:396332635, update_tm:396337939, TimeDelay:294*0.1us
ST:0xfcd, MT:0x8889, sample_tm:432332635, update_tm:432337939, TimeDelay:294*0.1us
ST:0xfce, MT:0x8889, sample_tm:468332635, update_tm:468337940, TimeDelay:294*0.1us
ST:0xfcf, MT:0x8889, sample_tm:504332635, update_tm:504337940, TimeDelay:294*0.1us
ST:0xfd0, MT:0x8889, sample_tm:540332635, update_tm:540337939, TimeDelay:294*0.1us
ST:0xfd1, MT:0x8889, sample_tm:576332635, update_tm:576337939, TimeDelay:294*0.1us
ST:0xfd2, MT:0x8889, sample_tm:612332635, update_tm:612337939, TimeDelay:294*0.1us
ST:0xfd3, MT:0x8889, sample_tm:648332635, update_tm:648337939, TimeDelay:294*0.1us

```
