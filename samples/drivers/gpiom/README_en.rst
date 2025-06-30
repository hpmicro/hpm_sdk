.. _gpiom:

GPIOM
==========

Overview
--------

The GPIOM example shows how to use GPIOM to configure pin control module, and use the specific control module to change outpin level.

Board Setting
-------------

No special settings are required.

Running the example
-------------------

When the example runs successfully, the LED will flash 5 times, and the following message is displayed in the terminal:

.. code-block:: console

   gpio example
   using gpiom configures pin control module
   toggling led 5 times in total
   toggling led 1/5 times
   toggling led 2/5 times
   toggling led 3/5 times
   toggling led 4/5 times
   toggling led 5/5 times

Tips
------------

- For GPIO-related operations, refer to the :ref:`gpio <gpio>` example
- To use FGPIO control for IOs, manage allocation through GPIOM module. See the `gpiom_configure_pin_control_setting` function in the example
