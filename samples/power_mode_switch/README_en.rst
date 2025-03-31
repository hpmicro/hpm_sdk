.. _power_mode_switch:

Power Mode Switch
==================================

Overview
--------

power_mode_switch shows how different power modes are switched to/from

Board Setting
-------------

- There is a PUART development board available

  - Using PUART as Console

  - Using PUART as a wake-up source

  - Connect serial port (from PUART, refer to  :ref:`PUART pins <board_resource>` ) to PC

- There is no PUART development board available

  - Use normal uart console

  - Using WKUP pin as wake-up source

Note
--------------
- Please power cycle the board (boot from flash) is needed to run the demo application after this firmware is downloaded.

Running the example
-------------------

When the project runs correctly, and the serial port terminal will output the following information:


.. code-block:: console

   Power mode switch demo:
     1 - Enter wait mode
     2 - Enter stop mode
     3 - Enter standby mode
     4 - Enter shutdown mode


Switch to different power mode by enter different option on serial port terminal.
