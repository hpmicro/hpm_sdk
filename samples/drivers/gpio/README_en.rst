.. _gpio:

GPIO
========

Overview
--------

The GPIO example shows how to change GPIO output pin level and use GPIO input pin trigger interrupt.

Board Setting
-------------

No special settings are required

Known Issue
-----------

When GPIO is used as the key input pin detection, the input level jitter will cause GPIO to detect the change of input level multiple times due to the mechanical jitter of the key.

Running the example
-------------------

- chips that support both-edge trigger mode, the project are configured in both-edge trigger mode,so every time a button is pressed and released, the LED state will be toggle, and the LED toggle logs will be printed twice.

- When the example runs successfully, the LED will flash 5 times, then press GPIO button (please check  :ref:`Button <board_resource>`  section of specific board) will toggle the LED status, and the following message is displayed in the terminal:


.. code-block:: console

   toggling led 5 times in total
   toggling led 1/5 times
   toggling led 2/5 times
   toggling led 3/5 times
   toggling led 4/5 times
   toggling led 5/5 times
   input interrupt
   user led will be switched on off based on user switch
   toggle led pin output
   toggle led pin output

