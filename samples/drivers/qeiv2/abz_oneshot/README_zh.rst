.. _qeiv2_abz_encoder_oneshot_mode:

QEIV2 ABZ Encoder Oneshot Mode
============================================================

概述
------

**Qeiv2 ABZ Encoder Oneshot Mode** 工程主要演示配置ABZ编码器并获取其单次的数据，然后通过串口终端将数据打印出来。

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

   cycle0 --- pulse_snap0: 0xffffffff, cylce_snap0:        0x0, spd : 0 deg/s
   cycle0 --- pulse_snap1:        0x0, cylce_snap1:        0x0, spd : 0 deg/s
   cycle1 --- pulse_snap0: 0xffffffff, cylce_snap0:        0x0, spd : 0 deg/s
   cycle1 --- pulse_snap1:        0x0, cylce_snap1:        0x0, spd : 0 deg/s
   pulse1 --- cycle_snap0: 0x6cf42561, spd: 5 deg/s,
   pulse1 --- cycle_snap1: 0x1a119d, spd: 5 deg/s,
   pulse0 --- cycle_snap0: 0x160b51b, spd: 5 deg/s,
   pulse0 --- cycle_snap1: 0x25bfd0, spd: 5 deg/s,

