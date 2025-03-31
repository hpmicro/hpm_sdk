.. _qeiv2_abz_encoder:

QEIV2 ABZ Encoder
==================================

概述
------

**Qeiv2 ABZ Encoder** 工程主要演示配置ABZ编码器并获取其数据，然后通过串口终端将数据打印出来。

配置
------

- 安装串口终端，查看 :ref:`板子信息 <board_resource>` 并配置串口终端参数

- 电机使用的是雷赛智能 **BLM57050-1000**  无刷电机，电机具体参数请参考`雷赛智能官网 <https://leisai.com/>`_。

- 点击查看  :ref:`DRV-LV50A-MP1907电机驱动板 <drv_lv50a_mp1907>`  章节并进行配置

运行现象
------------

- 上电后，打开串口终端，打印启动信息。


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
   qeiv2 abz encoder example
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4
   z: 0x64, phase: 0x1f4


- 转动电机就会触发编码器计算速度的中断，打印如下信息。


.. code-block:: console

   cycle0 --- pulse_snap0:        0x2, cylce_snap0:  0x1686b6d, spd : 1 deg/s
   cycle0 --- pulse_snap0:        0x3, cylce_snap0:  0x16627c4, spd : 1 deg/s
   cycle0 --- pulse_snap1:        0x2, cylce_snap1:  0x1686b6d, spd : 1 deg/s
   cycle0 --- pulse_snap0:        0x3, cylce_snap0:   0x24dd73, spd : 19 deg/s
   cycle0 --- pulse_snap0:        0xd, cylce_snap0:   0x25a71f, spd : 82 deg/s
   cycle0 --- pulse_snap1:        0x3, cylce_snap1:   0x24dd73, spd : 19 deg/s
   cycle0 --- pulse_snap0:        0xc, cylce_snap0:   0x2315c9, spd : 82 deg/s
   cycle0 --- pulse_snap1:        0xd, cylce_snap1:   0x25a71f, spd : 82 deg/s
   cycle0 --- pulse_snap0:        0x3, cylce_snap0:   0x176666, spd : 30 deg/s
   cycle0 --- pulse_snap1:        0xc, cylce_snap1:   0x2315c9, spd : 82 deg/s
   cycle0 --- pulse_snap1:        0x3, cylce_snap1:   0x176666, spd : 30 deg/s
   cycle0 --- pulse_snap0:       0x22, cylce_snap0:   0x90d929, spd : 53 deg/s
   cycle0 --- pulse_snap1:        0x3, cylce_snap1:  0x16627c4, spd : 1 deg/s
   cycle0 --- pulse_snap0:        0x1, cylce_snap0:   0x1986a2, spd : 9 deg/s
   cycle0 --- pulse_snap1:        0x1, cylce_snap1:   0x1986a2, spd : 9 deg/s
   cycle0 --- pulse_snap0:        0x2, cylce_snap0:   0x25e74f, spd : 12 deg/s
   cycle0 --- pulse_snap0:        0x2, cylce_snap0:   0x1a119d, spd : 18 deg/s
   cycle0 --- pulse_snap1:        0x2, cylce_snap1:   0x25e74f, spd : 12 deg/s
   cycle0 --- pulse_snap0:        0x8, cylce_snap0:  0x160b51b, spd : 4 deg/s
   cycle0 --- pulse_snap1:       0x22, cylce_snap1:   0x90d929, spd : 53 deg/s
   cycle0 --- pulse_snap1:        0x2, cylce_snap1:   0x1a119d, spd : 18 deg/s
   cycle0 --- pulse_snap0:        0x1, cylce_snap0:   0x14aa84, spd : 11 deg/s
   cycle0 --- pulse_snap0:        0x2, cylce_snap0:   0x1f9b08, spd : 15 deg/s
   cycle0 --- pulse_snap1:        0x1, cylce_snap1:   0x14aa84, spd : 11 deg/s
   cycle0 --- pulse_snap0:        0x3, cylce_snap0:   0x25bfd0, spd : 18 deg/s
   cycle0 --- pulse_snap1:        0x2, cylce_snap1:   0x1f9b08, spd : 15 deg/s

