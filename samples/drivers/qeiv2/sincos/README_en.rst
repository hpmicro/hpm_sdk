.. _qeiv2_sin_cos_encoder:

QEIV2 Sin/Cos Encoder
==========================================

Overview
--------

**Qeiv2 Sin/Cos Encoder**  project demonstrates configuring the Sin/Cos encoder and obtaining its data, then printing the data through a serial console.

Configurations
--------------

- Use a signal generator to generate a set of sine and cosine waves, with a frequency of 1kHz, a peak to peak value of 3V, and a center voltage offset of 1.65V. The phase difference between the two is 90°.

- Connect sine signal to QEIV2 Sin pin, and cosine signal to QEIV2 Cos pin, as detailed in the development board documentation :ref:`QEIV2 Sin/Cos Pin <board_resource>` .

Running the Demo
----------------

- The injection of sine and cosine waves is equivalent to simulating the encoder rotating at a uniform speed.

- Power on, open the serial terminal, log as following:


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


- After the above printing is completed, the following information will be printed periodically, that is, the analog encoder will output position information once per revolution.


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

