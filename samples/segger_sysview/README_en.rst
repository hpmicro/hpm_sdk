.. _segger_sysview:

Segger SYSVIEW
====================

Overview
--------

The Segger SystemView sample demonstrates the basic usage of the SysView API. Supported system types include:

- baremetal

- freertos

- ucos_iii

Hardware setup
--------------

- Use a J-Link (if the board has an onboard debugger, disconnect it; TCK/TMS/TRST).

Notes
-----

- When debugging with SystemView, regardless of the IDE or debugging environment (e.g., SES, IAR, Keil, etc.), you must use the **J-Link native driver** to connect to the target device. **Do NOT use J-Link's OpenOCD mode**, otherwise SystemView will be unable to connect and capture data.

Observed behavior
-----------------

- Power on the development board.

- Download the compiled sample program.

- Connect the board using SystemView V3.62 or later.

- Enter the correct RTT control block address; this address can be found in the map file (symbol: \_SEGGER\_RTT).

- In the SystemView application open "Start Recording".

- Reset the development board.

- When running correctly:

 SystemView will display the system's real-time execution.
