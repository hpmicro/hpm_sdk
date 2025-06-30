.. _uart_lin_slave_baudrate_adaptive:

UART LIN Slave with Baudrate Adaptive Example
================================================

Overview
---------
This example demonstrates how to use UART peripheral with PLB peripheral to implement LIN slave baudrate adaptive functionality. The main features include:

- Using PLB to detect LIN bus baudrate
- LIN slave automatically adjusts baudrate based on detection results
- LIN slave receiving data from master
- LIN slave responding to master's data request

Working Principle
-------------------
This example implements the following functions:

1. Baudrate Adaptation:

   - Use PLB to measure LIN break and sync field timing
   - Calculate actual baudrate
   - Dynamically adjust UART baudrate to match master

2. Receiving Master Data:

   - Receive LIN frame (ID: 0x31, Data: 0x0-0x7)
   - Verify data correctness

3. Responding to Master Request:

   - Send LIN frame (ID: 0x30, Data: 0x7-0x0)
   - Use enhanced checksum

Workflow
---------
1. Initialization Process:

   a) PLB Initialization:

      - Configure PLB channel for timing measurement
      - Configure DMA for data acquisition
      - Set baudrate tolerance and filter parameters

   b) UART Initialization:

      - Configure initial baudrate (19200)
      - Enable line status interrupt for break detection
      - Configure frame format as 8-bit data, no parity

2. Baudrate Detection Process:

   a) Break and Sync Field Detection:

      - PLB captures LIN bus signal
      - Measure break and sync field timing
      - Calculate actual baudrate

   b) Baudrate Adjustment:

      - Verify if calculated baudrate is within supported range
      - Dynamically update UART baudrate configuration
      - Prepare for PID reception

3. Data Processing Flow:

   a) Receiving Data (ID: 0x31):

      - Configure RX FIFO trigger level greater than remaining frame length
      - Enable RX timeout interrupt
      - Receive and verify data in interrupt

   b) Sending Data (ID: 0x30):

      - Prepare response data (0x7-0x0)
      - Enable TX FIFO interrupt
      - Confirm data transmission completion in interrupt

4. Interrupt Handling Process:

   a) DMA Interrupt:

      - Process PLB acquired data
      - Calculate baudrate and update UART configuration

   b) UART RX Data Interrupt:

      - Receive PID and verify
      - Configure subsequent operations

   c) UART TX FIFO Interrupt:

      - Detect transmission completion
      - Note: When uart_intr_tx_slot_avail interrupt occurs, the last byte may still be in TX FIFO, additional status check or TXIDLE interrupt feature may be needed depending on version

   d) UART RX Timeout Interrupt:

      - Receive complete data frame
      - Verify data correctness
      - Disable RX timeout interrupt
      - Note: RX timeout method requires interval between data frames ≥ 4 UART byte transmission time

Important Notes
---------------
1. Slave auto-sleep is not supported, no need for master wakeup
2. Baudrate detection requires additional hardware connection (UART_RX to PLB input)
3. Baudrate adjustment has a tolerance range (default ±5%)

Usage Guide
-----------

Hardware Requirements
~~~~~~~~~~~~~~~~~~~~~

- One development board
- One USB to LIN debugger
- LIN transceiver (if not integrated on board)

Hardware Setup
~~~~~~~~~~~~~~
Choose the appropriate connection method based on your board type:

1. For boards with onboard LIN transceiver:

   - Connect LIN signal line from USB-LIN debugger to board's LIN signal pin
   - Connect GND
   - Connect UART_RX pin to PLB LIN clock detection pin

2. For boards without onboard LIN transceiver:

   - Connect board's UART TX and RX pins to MCU side of LIN transceiver
   - Connect USB-LIN debugger to LIN bus side of transceiver
   - Connect GND
   - Connect UART_RX pin to PLB LIN clock detection pin

Please refer to :ref:`Pin Description <board_resource>` section for specific pin connections.

Running Steps
~~~~~~~~~~~~~
1. Configure USB-LIN debugger:

   - Select correct COM port and initial baudrate (can be any supported baudrate)
   - Click "Set to Debugger" button

   .. image:: ../../../lin/doc/lin_debugger_configuration.png

2. Configure master send and receive functions:

   - Set send ID to 0x31, configure send data as: 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7 and enhanced checksum
   - Set receive ID to 0x30
   - Check enable option
   - Click "Configure to Debugger" button

   .. image:: ../../../lin/slave/doc/lin_debugger_master_sent_config.png

3. Compile and download program to development board and run

4. Try adjusting USB-LIN debugger's baudrate to verify adaptive functionality

Running Results
---------------
1. Serial terminal output:

   .. code-block:: console

      Test uart lin slave baudrate adaptive example
      uart lin receive ID: 0x31
      uart receive 8 data:
      0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7
      uart lin receive ID: 0x30
      uart send 8 data:
      0x7 0x6 0x5 0x4 0x3 0x2 0x1 0x0

2. USB-LIN debugger shows communication data:

   .. image:: ../../../lin/slave/doc/lin_debugger_master_result.png

Debugging Tips
--------------
1. Hardware connection check
2. Use logic analyzer to observe LIN bus signals, verify the waveforms of break, sync, data segments and checksum are correct
3. Check if interrupts are generated as expected, including PLB interrupts, calculated baudrate, and UART interrupts

