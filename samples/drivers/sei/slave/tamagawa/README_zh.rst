.. _sei_slave_simulate_tamagawa_encoder:

SEI Slave: Simulate TAMAGAWA Encoder
========================================================================

概述
------

- 该工程主要演示通过SEI接口模拟TAMAGAWA编码器，产生编码器位置数据等，并通过串口将信息打印出来。

- 模拟的TAMAGAWA编码器的型号为TS5700N8401。

配置
------

- 准备一个USB转485模块（注意：该模块的最大波特率需不小于2.5Mbps）

- 将SEI接口信号DATA_P/DATA_N与USB转485的A/B信号相连接。

运行现象
------------

- 将程序下载至开发板并运行。

- 通过PC的串口调试助手与开发板进行通讯，模拟获取编码器数据。

- 串口调试助手设置为：波特率：2500000；起始位：1位；数据位：8位；停止位：1位；校验位：无。

- 通过串口调试助手发送Hex数据：`1A` 或 `02` 或 `8A` 或 `92`，开发板模拟的编码器将会进行响应。同时，可通过串口终端查看开发板输出的log信息，ST数值每次加1。


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
   SEI slave tamagawa sample
   Started sei engine!
   CMD:0x1a, SF:0x0, ST:0xa5a5, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x0, sample_tm1:135187399, sample_tm2:0, sample_interval:844921 us
   CMD:0x1a, SF:0x0, ST:0xa5a6, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:167187056, sample_tm2:135187399, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5a7, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:199186714, sample_tm2:167187056, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5a8, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:231186372, sample_tm2:199186714, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5a9, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:263186030, sample_tm2:231186372, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5aa, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:295185688, sample_tm2:263186030, sample_interval:199997 us
   CMD:0x1a, SF:0x0, ST:0xa5ab, ENID:0x17, MT:0x8888, ALMC:0x0, CRC:0x58, sample_tm1:327185345, sample_tm2:295185688, sample_interval:199997 us


- 通过串口调试助手发送Hex数据：`32 7F 01 4C`，将EEPROM的page号设成1，开发板模拟的编码器将会进行响应。同时，可通过串口终端查看开发板输出的log信息：


.. code-block:: text

   Change EEPORM page to 1


- 通过串口调试助手发送Hex数据：`32 01 5A 69`，向EEPROM的01地址写入数据0x5A，开发板模拟的编码器将会进行响应。同时，可通过串口终端查看开发板输出的log信息：


.. code-block:: text

   Write EEPORM - Page: 1, Addr: 1, Data: 90


- 通过串口调试助手发送Hex数据：`EA 01 EB`，读取EEPROM地址01的数据，开发板模拟的编码器将会进行响应。同时，可通过串口终端查看开发板输出的log信息：


.. code-block:: text

   Read EEPORM - Page: 1, Addr: 1, Data: 90

