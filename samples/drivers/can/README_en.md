# CAN Demo

## Overview

The can_demo provides the following features:

- loopback test for all SoC supported CAN controllers, focusing on the CAN and CAN-FD test
- loopback test for the board supported CAN controllers using interrupt mode
- Dual-board Echo test
- Send out multiple messages for the CAN bus connection check

## Board Setting

No special settings are required

## Running the example

When the example runs successfully, the board shows the following menu:

```console

*********************************************************************************
*                                                                               *
*                         CAN Example Menu                                      *
*                                                                               *
* 0 - Run loopback test for all supported CAN controllers (CAN and CANFD)       *
* 1 - Run loopback test for board supported CAN controller (interrupt mode)     *
* 2 - Echo test between two boards:initiator                                    *
* 3 - Echo test between two boards:responder                                    *
* 4 - Send mulitple messages for transmission check                             *
* 5 - CAN error test (Need to remove current node from CAN BUS for this test)   *
* 6 - CAN filter test                                                           *
* 7 - Send multiple CANFD messages for transmission check                       *
*                                                                               *
*********************************************************************************


```

### Note

- For option 0 and option 1, users can start the test by typine the option directly
- For option 2 and 3, users must connect two boards and run this demo, start the responder first, then start the initiator.
- For option 4, users must ensure that the board is attached to a valid CAN bus. If the board is connected to a CAN monitor, users can see multiple CAN messages will be sent out after this option is chosen.
- For option 7, users must ensure that the board is attached to a valid CAN bus and the bus now supports 5Mbis/s CANFD buadrate. If the board is connected
to a CANFD monitor, users will see multiple CANFD messages are sent out after this option is chosen.
