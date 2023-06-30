# uart_lin_master
## Overview

This project demonstrates the functionality of UART simulation LIN master.

## Note
The interval between LIN frames should be greater than or equal to the transmission time of 4 UART bytes。

## Board Setting

Requires a LIN transceiver and USB_LIN debugger
Please refer to [Pin Description](lab_board_resource) for specific board.
connect LIN transceiver mcu signal to UART's TX and RX on board, connect LIN transceiver LIN signal to USB_LIN debugger.

## Running the example

Configure usb_lin debugger:
- Configure com and baudrate, then click `Set to Debugger` option
  ![lin_debugger_configuration](../../../../../../../assets/sdk/samples/lin_debugger_configuration.png)
- Set ID, data and checksum mode, tick `enable option`， then click `Configure to Debugger` option：
  ![lin_debugger_slave_sent](../../../../../../../assets/sdk/samples/lin_debugger_slave_sent_config.png)
- Running the example, check the result in debugger window
  ![lin_debugger_slave_result](../../../../../../../assets/sdk/samples/lin_debugger_slave_result.png)

When the example runs successfully, the log would be seen on the terminal like:
```console
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
```
