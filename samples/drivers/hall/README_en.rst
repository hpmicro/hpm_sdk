.. _hall:

Hall
========

Overview
--------

**Hall** project demonstrates the output of Hall timer data using the serial port after triggering the **hall** interrupt

- Hall interrupt configuration and handling
- Reading Hall `U\V\W\TIMER` register data
- TRGM (Trigger Manager) configuration

Implementation Details
----------------------

- Initial Configuration:
  - Configure TRGM input sources for U, V, W hall signals
  - Configure and enable interrupt
  - Configure hall counter and phase count
  - Configure data reading trigger on U phase signal change

- Interrupt Handling:
  - Clear status flags in hall interrupt
  - Set data ready flag to trigger data reading in main loop

- Data Reading:
  - Wait for data ready flag in main loop
  - Read U, V, W phase counter values and timer counter value
  - Print data through serial port

Configurations
--------------

- Install the serial terminal, view  :ref:`board information <board_resource>` , and configure the serial terminal parameters

- This program uses the **BLM57050-1000**  brushless motor of "Leisai Intelligence", refer to the `Leisai Intelligence <https://leisai.com/>`_ website for the specific parameters of the motor.

- Click   :ref:`DRV-LV50A-MP1907 Motor Driver Board <drv_lv50a_mp1907>`   section and configure

Running the Demo
----------------

- After power-on, open the serial terminal, startup message will be printed
- When rotating the motor shaft, hall interrupt will be triggered and print:
  - u: Hall counter value for U phase
  - v: Hall counter value for V phase
  - w: Hall counter value for W phase
  - timer: Timer counter value
- The data changes with rotation speed and direction, refer to user manual for details


.. code-block:: console

   > u: 0xe0000000
   > v: 0x87b6d17c
   > w: 0x46999a8
   > timer: 0xe149af9

