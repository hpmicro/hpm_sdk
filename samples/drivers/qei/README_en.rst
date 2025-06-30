.. _qei:

QEI
======

Overview
--------

The **QEI** project demonstrates how to use the Quadrature Encoder Interface to read motor encoder data and output it through the serial port.

- Configuring QEI interrupts and trigger conditions
- Reading QEI registers including Z, phase, speed and timer values
- Using TRGM (Trigger Mux) to route encoder signals

Configurations
--------------

- Install a serial terminal, refer to :ref:`board information <board_resource>` for serial port configuration

- This program uses the **BLM57050-1000** brushless motor from "Leisai Intelligence". For detailed motor specifications, please visit the `Leisai Intelligence <https://leisai.com/>`_ website.

- Refer to :ref:`DRV-LV50A-MP1907 Motor Driver Board <drv_lv50a_mp1907>` section for motor driver configuration

- The QEI is configured with:
  - Phase count per revolution from motor specifications
  - Z count increment on phase count maximum
  - Phase comparison trigger point set to capture data at specific motor position
  - Positive compare flag trigger for data capture

Running the Demo
----------------

- Power on the board and open the serial terminal
- The program will initialize QEI and TRGM
- When the motor shaft is rotated, QEI interrupts will be triggered and the following data will be printed:
  - Z count: Encoder Z signal count
  - Phase: Current phase position
  - Speed: Sum of 4 consecutive speed history samples (his0 to his3)
  - Timer: Timer value at capture time

.. code-block:: console

   > z: 0xffffff9c
   > phase: 0xc200014d
   > speed: 0xa0000000
   > timer: 0xa8d08799

The values will change based on:
- Motor rotation speed
- Rotation direction
- Encoder resolution
- Phase count configuration

