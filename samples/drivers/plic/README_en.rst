.. _plic:

PLIC
========

Overview
--------

The PLIC example uses ptpc(or gptmr)/gpio/plicsw interrupts to shows interrupt nesting function.

ptpc(or gptmr) isr will not finish until gpio interrupt is triggered 5 times. Meanwhile in gpio isr, software interrupt will be triggered as well.

Board Setting
-------------

No special settings are required

Running the example
-------------------

Running the example, interrupt nesting will occur when pressing GPIO button (please check  :ref:`Button <board_resource>`  section of specific board) according to the log prompt. The following message is displayed in the terminal:

.. code-block:: console

   Machine mode (Vector Preemptive) interrupt test
   Press button will trigger GPIO interrupt, in which software interrupt will be triggered as well.
   ptpc(or gptmr) interrupt start
   + now next 5 gpio interrupts will occur in nested irq context
   gpio interrupt start
   plicsw start
   plicsw end
   gpio interrupt end

