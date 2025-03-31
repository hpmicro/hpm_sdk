.. _qeiv2_abz_encoder:

QEIV2 ABZ Encoder
==================================

Overview
--------

**Qeiv2 ABZ Encoder**  project demonstrates configuring the ABZ encoder and obtaining its data, then printing the data through a serial console.

Configurations
--------------

- Install the serial terminal, view  :ref:`board information <board_resource>` , and configure the serial terminal parameters

- This program uses the **BLM57050-1000**  brushless motor of "Leisai Intelligence", refer to the `Leisai Intelligence <https://leisai.com/>`_ website for the specific parameters of the motor.

- Click   :ref:`DRV-LV50A-MP1907 Motor Driver Board <drv_lv50a_mp1907>`   section and configure

Running the Demo
----------------

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

- Rotating the motor will trigger an interrupt in the encoder's calculation of speed, then printing the following information.


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

