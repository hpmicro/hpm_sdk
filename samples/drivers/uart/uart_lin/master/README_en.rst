.. _uart_lin_master:

UART LIN Master Example
========================

Overview
--------
This example demonstrates how to use UART peripheral to simulate LIN master functionality, including:

- LIN master polling mode for sending and receiving data
- LIN master interrupt mode for sending and receiving data
- LIN enhanced checksum functionality

Working Principle
------------------
This example implements the following functions:

1. Polling mode:

   - Send LIN frame (ID: 0x31, Data: 0x0-0x7)
   - Receive LIN frame (ID: 0x30, Expected data: 0x7-0x0)

2. Interrupt mode:

   - Send LIN frame (ID: 0x31, Data: 0x0-0x7)
   - Receive LIN frame (ID: 0x30, Expected data: 0x7-0x0)

Workflow
--------
1. Initialization Process:

   a) UART Initialization:

      - Configure baudrate
      - Configure RX FIFO trigger level to be greater than one complete data frame
      - Configure frame format as 8-bit data, no parity

2. Polling Mode Workflow:

   a) Send LIN Frame:

      - Call hpm_uart_lin_master_send_frame to send complete LIN frame
      - Function internally handles break, sync, PID, data, and checksum transmission
      - Return after transmission completes

   b) Receive LIN Frame:

      - Call hpm_uart_lin_master_receive_frame to receive LIN frame
      - Function first sends break, sync, and PID
      - Then waits for data and checksum reception
      - Return after successful verification

3. Interrupt Mode Workflow:

   a) Send LIN Frame:

      - Call hpm_uart_lin_master_send_head to send frame header (break, sync, PID)
      - Call hpm_uart_lin_master_send_data to send data and checksum
      - Enable TX FIFO interrupt
      - Detect completion in interrupt (Note: when uart_intr_tx_slot_avail interrupt occurs, the last byte may still be in TX FIFO, additional status check or TXIDLE interrupt feature may be needed depending on version)

   b) Receive LIN Frame:

      - Call hpm_uart_lin_master_send_head to send frame header
      - Enable RX timeout interrupt
      - Call hpm_uart_lin_master_receive_data in interrupt to receive and verify data

4. Interrupt Handling Process:

   a) TX FIFO Interrupt (uart_intr_tx_slot_avail):

      - Check TX FIFO availability
      - Set transmission complete flag
      - Disable TX FIFO interrupt
      - Note: The last byte may still be transmitting, additional status check needed

   b) RX Timeout Interrupt (uart_intr_rx_timeout):

      - Detect reception timeout
      - Read and verify received data
      - Set reception complete flag
      - Disable RX timeout interrupt
      - Note: RX timeout method requires interval between data frames ≥ 4 UART byte transmission time

Hardware Requirements
---------------------
- One development board
- One USB to LIN debugger
- LIN transceiver (if not integrated on board)

Hardware Setup
--------------
Choose the appropriate connection method based on your board type:

1. For boards with onboard LIN transceiver:

   - Connect LIN signal line from USB-LIN debugger to board's LIN signal pin
   - Connect GND

2. For boards without onboard LIN transceiver:

   - Connect board's UART TX and RX pins to MCU side of LIN transceiver
   - Connect USB-LIN debugger to LIN bus side of transceiver
   - Connect GND

Please refer to :ref:`Pin Description <board_resource>` section for specific pin connections.

Running Steps
--------------
1. Configure USB-LIN debugger:

   - Select correct COM port and baudrate (19200)
   - Click "Set to Debugger" button

   .. image:: ../../../lin/doc/lin_debugger_configuration.png

2. Configure slave transmission:

   - Set ID to 0x30
   - Configure transmission data as: 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0
   - Select enhanced checksum
   - Enable the option
   - Click "Configure to Debugger" button

   .. image:: ../../../lin/master/doc/lin_debugger_slave_sent_config.png

3. Compile and download the program to the board and run

Running Results
----------------
1. Terminal output:

   .. code-block:: console

      Test uart lin master polling transfer
      uart lin master send ID: 0x31
      uart send 8 data:
      0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7
      uart lin master send ID: 0x30
      uart receive 8 data:
      0x7 0x6 0x5 0x4 0x3 0x2 0x1 0x0

      Test uart lin master interrupt transfer
      uart lin master send ID: 0x31
      uart send 8 data:
      0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7
      uart lin master send ID: 0x30
      uart receive 8 data:
      0x7 0x6 0x5 0x4 0x3 0x2 0x1 0x0

2. USB-LIN debugger communication data display:

   .. image:: ../../../lin/master/doc/lin_debugger_slave_result.png

Debugging Suggestions
----------------------
1. Hardware Connection Check

2. Use Logic Analyzer to observe LIN bus signals, verify the waveforms of break, sync, data section and checksum

3. When using interrupt mode, check if interrupts are generated as expected

4. When using polling mode, verify if the driver timeout settings match the data frame baudrate

