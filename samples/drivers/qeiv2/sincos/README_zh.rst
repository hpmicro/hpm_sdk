.. _qeiv2_sin_cos_encoder:

QEIV2 Sin/Cos Encoder
==========================================

概述
------

**Qeiv2 Sin/Cos Encoder** 工程主要演示配置Sin/Cos编码器并获取其数据，然后通过串口终端将数据打印出来。

配置
------

- 使用信号发生器生成一组Sin和Cos正余弦波，二者的频率为1kHz、峰峰值为3V、中心偏移为1.65V，二者的相位相差90°。

- 将Cos信号输入至QEIV2 Cos引脚，将Sin信号输入至QEIV2 Sin引脚，详见开发板文档 :ref:`QEIV2 Sin/Cos引脚 <board_resource>` 相关内容。

运行现象
------------

- Sin和Cos正余弦波的注入，相当于模拟编码器在匀速旋转。

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
   qeiv2 sincos encoder example
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x8c12be00, ph:0x3
   ch0:0xea4, ch1:0x5d64, pos:0x718257c, ang:0x94756100, ph:0x3
   ch0:0x17e1, ch1:0x46d4, pos:0x728eac2, ang:0x94756100, ph:0x3
   ch0:0x17e1, ch1:0x46d4, pos:0x728eac2, ang:0x94756100, ph:0x3
   ch0:0x17e1, ch1:0x46d4, pos:0x728eac2, ang:0x94756100, ph:0x3
   ch0:0x17e1, ch1:0x46d4, pos:0x728eac2, ang:0x94756100, ph:0x3
   ch0:0x17e1, ch1:0x46d4, pos:0x728eac2, ang:0x94756100, ph:0x3
   ch0:0x17e1, ch1:0x46d4, pos:0x728eac2, ang:0x94756100, ph:0x3


- 当上述打印完毕后，将周期性打印如下信息，即模拟编码器每转一圈，输出位置信息一次。


.. code-block:: console

   z: 0x0
   phase: 0x80000040
   position: 0x8004a42a
   ang: 0x2521500
   z: 0x15
   phase: 0x80000040
   position: 0x800373a6
   ang: 0x1b9d300
   z: 0x16
   phase: 0x80000040
   position: 0x8001b834
   ang: 0xdc1a00
   z: 0x17
   phase: 0x80000040
   position: 0x80113a66
   ang: 0x89d3300
   z: 0x18
   phase: 0x80000040
   position: 0x800f8dfc
   ang: 0x7c6fe00
   z: 0x19
   phase: 0x80000040
   position: 0x800dde1e
   ang: 0x6ef0f00
   z: 0x1a
   phase: 0x80000040
   position: 0x800c0d92
   ang: 0x606c900
   z: 0x1b

