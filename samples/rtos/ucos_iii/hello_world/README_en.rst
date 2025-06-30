.. _uc_os_iii_hello_world:

uC/OS-III Hello World
==========================================

Overview
--------

The example shows uC/OS-III task scheduling function.

Board Setting
-------------

No special settings are required

Project Configuration
---------------------

set(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR 1) to use the gptmr as system's tick source.

Running the example
-------------------

When the example runs successfully, the LED starts flashing and following message is displayed in the terminal:

.. code-block:: console

   Toggle the led pin level.
   LED is off.
   LED is off.
   LED is off.
   LED is off.
   Toggle the led pin level.
   LED is on.
   LED is on.
   LED is on.
   Toggle the led pin level.
   LED is off.
   LED is off.
   LED is off.
   LED is off.

