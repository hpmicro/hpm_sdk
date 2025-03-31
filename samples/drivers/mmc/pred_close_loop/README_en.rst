.. _mmc_pred_close_loop:

MMC_PRED_CLOSE_LOOP
======================================

Overview
--------

The MMC closed-loop prediction project demonstrated the function of MMC to predict and compensate the position of the rotor at the future time and output it according to the rotor position and the time stamp of the Motor system provided by other peripherals (such as SEI).
This example project demonstrates how SEI obtains sensor positions and outputs them to MMC. MMC predicts and compensates for future positions, and then outputs the positions to QEO. Based on the obtained positions, QEO generates PWM signals through PWM peripherals.

硬件设置
------------

Please refer to  :ref:`Pin Description <board_resource>`
- connect bissc sensor to RS485 interface on board to SEI

- Connect the PWM pins to the HPMicro's stepper drive board to drive the stepper motor

Running the example
-------------------

When the project runs correctly, the serial port terminal will receive the following information:

.. code-block:: console

   MMC example
   MMC tracks position from SEI and predicts postion to QEO, QEO controls PWM module generates signals

   speed: 0.00000000
   rev  : 0
   pos  : 0xe5700000

   speed: 0.00000000
   rev  : 0
   pos  : 0xe5700000

   speed: 0.02145767
   rev  : 0
   pos  : 0xe5201335

   speed: 2.12848091
   rev  : 0
   pos  : 0xf5a41e09

   speed: 3.19600106
   rev  : 1
   pos  : 0x4436226c

   speed: 2.19464302
   rev  : 1
   pos  : 0x87cbb8d1

   speed: 3.45587732
   rev  : 1
   pos  : 0xd4acf18b

   speed: 2.91764640
   rev  : 2
   pos  : 0x26966f29

   speed: 2.05099487
   rev  : 2
   pos  : 0x764ed45d
