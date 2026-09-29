.. _sei_slave_simulate_hiperface_encoder:

SEI Slave: Simulate HIPERFACE Encoder
==========================================================================

概述
------

- 该工程主要演示通过SEI接口模拟HIPERFACE编码器，产生编码器位置数据等，并通过串口将信息打印出来。

- 模拟的HIPERFACE编码器的型号为SKM36-HFA0-K02。

配置
------

- 准备一个USB转485模块

- 将SEI接口信号DATA_P/DATA_N与USB转485的A/B信号相连接。

运行现象
------------

- 将程序下载至开发板并运行。

- 通过PC的串口调试助手与开发板进行通讯，模拟获取编码器数据。

- 串口调试助手设置为：波特率：9600；起始位：1位；数据位：8位；停止位：1位；校验位：偶校验。通过串口调试助手发送数据：0x40 0x42 0x02，开发板模拟的编码器将会进行响应。


.. image:: doc/sei_slave_hiperface.png
   :alt: sei_slave_hiperface.png

- 同时，可通过串口终端查看开发板输出的log信息，POS数值每次加1。


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
   SEI slave hiperface sample
   Started sei engine!
   ADDR:0x40, CMD:0x42, POS:0xa5a5, CRC:0x2, sample_interval:1514121 us
   ADDR:0x40, CMD:0x42, POS:0xa5a6, CRC:0x1, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5a7, CRC:0x0, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5a8, CRC:0xf, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5a9, CRC:0xe, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5aa, CRC:0xd, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5ab, CRC:0xc, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5ac, CRC:0xb, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5ad, CRC:0xa, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5ae, CRC:0x9, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5af, CRC:0x8, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b0, CRC:0x17, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b1, CRC:0x16, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b2, CRC:0x15, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b3, CRC:0x14, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b4, CRC:0x13, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b5, CRC:0x12, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b6, CRC:0x11, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b7, CRC:0x10, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b8, CRC:0x1f, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5b9, CRC:0x1e, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5ba, CRC:0x1d, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5bb, CRC:0x1c, sample_interval:199996 us
   ADDR:0x40, CMD:0x42, POS:0xa5bc, CRC:0x1b, sample_interval:199996 us
