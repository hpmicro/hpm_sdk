.. _mbx_dualcore:

MBX dualcore
========================

Overview
--------

This example demonstrate how MBX works. Core 0 will send message to core 1 which will echo back what's been received.

Board Setting
-------------

debug console for core1 can be found in  :ref:`Pin Description <board_resource>`  for specific board

Generate, Build and Debug Multi-core projects
---------------------------------------------

Please reference :ref:`Multicore General Description <multicore_general_description>`

Running the Example
-------------------

- start to run application for each core, following content should be shown on debug console

  - for core0:

  .. image:: doc/mbx_core0_console_output.png
     :alt: core0_console_output

  - for core1:

  .. image:: doc/mbx_core1_console_output.png
     :alt: core1_console_output

  - once core1 application starts, core1 will receive messages from core0 then send them back, then in the console of core0:

  .. image:: doc/mbx_core0_console_output2.png
     :alt: core0_console_output2
