.. _mquickjs:

mquickjs
======================

Overview
--------

The **mquickjs** example demonstrates how to run MicroQuickJS (a lightweight JavaScript engine) on HPMicro MCUs.

This example does the following:

- Initializes a JavaScript runtime with a fixed heap (32 KB in this demo)
- Executes embedded JavaScript demo code
- Controls on-board LED through the JavaScript ``HPM_Pin`` API
- Enters a simple REPL so users can input and execute JavaScript commands from serial terminal

Notes
-----

- The project enables mquickjs through ``CONFIG_MQUICKJS=1`` in ``CMakeLists.txt``.
- mquickjs supports a subset of JavaScript (ES5 strict style):

  - No arrow function syntax (``=>``)
  - No ``class`` syntax
  - No ``let``/``const`` (use ``var``)

Board Setting
-------------

- Connect the board to the host PC through the debug UART.
- Ensure the on-board user LED is available (the sample uses ``BOARD_LED_GPIO_NAME``).

Running the example
-------------------

When the example runs successfully, the serial terminal prints startup information, runs embedded JS code, then enters REPL mode.

Expected log is similar to:

.. code-block:: console

   ========================================
       HPM SDK + mquickjs Example
       MicroQuickJS for Embedded Systems
   ========================================

   Initializing JavaScript engine...
   Memory size: 32768 bytes
   JavaScript engine ready!

   --- Running embedded JavaScript example ---
   === HPM SDK + mquickjs Demo ===
   ...
   Blinking complete!

   mquickjs REPL (type 'exit()' to quit)
   js>

REPL Usage
----------

- Input a JavaScript expression or statement and press Enter to execute.
- Expression results are printed automatically when the result is not ``undefined``.
- Type ``exit()`` or ``quit`` to leave REPL.
