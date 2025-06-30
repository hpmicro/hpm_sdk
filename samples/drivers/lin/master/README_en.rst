.. _lin_master:

lin_master
====================

Overview
--------
This example demonstrates the basic functionality of HPM LIN peripheral in master mode, including:

- Support for LIN 2.2A protocol specification
- Data transmission and reception in master mode
- Enhanced checksum mode support
- Wakeup signal transmission
- Frame interval control
- Maximum 8-byte data transfer support

Operation Flow
--------------
1. Initialization Configuration

   - Initialize LIN pin configuration
   - Configure LIN clock to 20MHz
   - Configure interrupt priority
   - Initialize transmit data buffer

2. Main Loop Processing

   - Send wakeup signal
   - Execute data transfer operations:

     * ID 0x31: Transmit 8 bytes of data
     * ID 0x30: Receive 8 bytes of data

   - Handle various events:

     * Transfer completion event
     * Wakeup signal transmission complete
     * Error event

3. Interrupt Handling

   - Handle transfer errors
   - Process wakeup signal completion
   - Process transfer completion

Board Setting
-------------
This example requires the following hardware:

- A LIN transceiver
- A USB_LIN debugger (as slave)
- Connection cables

Please refer to :ref:`Pin Description <board_resource>` for specific board.
Connect LIN transceiver MCU signal to LIN's TX and RX on board, connect LIN transceiver LIN signal to USB_LIN debugger.

Running the Example
-------------------
To run this program, USB_LIN debugger configuration is required:

1. Configure Basic Parameters

   - Select correct COM port and baud rate
   - Click "Set to Debugger" button

   .. image:: ../doc/lin_debugger_configuration.png
      :alt: lin_debugger_configuration

2. Configure Slave Transmission Options

   - Set ID, data and checksum mode
   - Enable the configuration
   - Click "Configure to Debugger" button

   .. image:: doc/lin_debugger_slave_sent_config.png
      :alt: lin_debugger_slave_sent

When the example runs successfully, the log would be seen on the terminal like:

.. code-block:: console

   LIN master example
   LIN master sent wakeup signal
   ID: 31, sent 8 bytes:
   0 1 2 3 4 5 6 7
   ID: 30, receive 8 bytes
   7 6 5 4 3 2 1 0

The debugger window will show the following result:

.. image:: doc/lin_debugger_slave_result.png
   :alt: lin_debugger_slave_result

Debugging Tips
---------------
1. Hardware Connection Check

   - Ensure proper connection between LIN transceiver and development board
   - Check power and ground connections

2. Baud Rate Configuration

   - Ensure baud rate matches between master and slave devices
   - Default baud rate is 19200bps

3. Frame Format Issues

   - ID mismatch: Verify configured ID matches master's transmitted ID
   - Verify checksum mode configuration matches

4. Common Error Analysis

   - Check if interrupts are triggered normally, examine status register to locate specific errors
   - Use oscilloscope to observe LIN bus signal waveform

