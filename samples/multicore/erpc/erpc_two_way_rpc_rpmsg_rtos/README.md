# eRPC two_way_rpc demo
## Overview

This project executes two way eRPC example, uses RPMSG as transport layer protocol, uses MBX as message notification.

Core0 is both the client side and the server side, and similarly, core1 is also both the client side and the server side. Core0 can call the erpc function in Core1, and Core1 can also call the erp function on Core0.

Please refer to [official eRPC github](https://github.com/EmbeddedRPC/erpc)和[official rpmsg_lite github](https://github.com/NXPmicro/rpmsg-lite)


## IDL file generate project files

IDL files contains all declarations used in eRPC calls. This demo IDL file is __erpc_two_way_rpc.erpc__ .

When function is declared in erpc file as callback type function, so the definition will exists on both sides (user and erpc implementation). This way you can call function from client side and server side (as local function or erpc call).

Please refer to [docs/IDL-Reference](../../../../middleware/erpc/erpc/docs/IDL-Reference.md)

The easiest way for generating code is have both the tool and the IDL file in same directory (generated). Then, the following command needs be executed:
```
../../../../../middleware/erpc/erpc/tools/erpcgen/Windows/erpcgen.exe erpc_two_way_rpc.erpc
```
This action creates eight files in current folder:
 - erpc_two_way_rpc_Core0Interface.h
 - erpc_two_way_rpc_Core0Interface_client.cpp
 - erpc_two_way_rpc_Core0Interface_server.cpp
 - erpc_two_way_rpc_Core0Interface_server.h
 - erpc_two_way_rpc_Core1Interface.h
 - erpc_two_way_rpc_Core1Interface_client.cpp
 - erpc_two_way_rpc_Core1Interface_server.cpp
 - erpc_two_way_rpc_Core1Interface_server.h

## Importing client/server files

`Core0 project` needs to import  __erpc_two_way_rpc_Core0Interface_client.cpp__ and __erpc_two_way_rpc_Core1Interface_server.cpp__ files. The __erpc_two_way_rpc_Core0Interface.h__ and __erpc_two_way_rpc_Core1Interface_server.h__ header file needs to be included.

`Core1 project` needs to import  __erpc_two_way_rpc_Core1Interface_client.cpp__ and __erpc_two_way_rpc_Core0Interface_server.cpp__ files. The __erpc_two_way_rpc_Core1Interface.h__ and __erpc_two_way_rpc_Core0Interface_server.h__ header file needs to be included.

## Generate core1 project
__CMAKE_BUILD_TYPE__ must be *"sec_core_img"*

## Generate core0 project
__CMAKE_BUILD_TYPE__ user can specify *"flash_sdram_xip"* or *"flash_xip"*, etc.

## Build project
__Core1__ project must be generated and built first
__Core0__ project must be generated after the core1 project has been built successfully.

## Debugging
- Download the core0 example to the target and run core0 example first
- Download the core1 example to the target and run the core1 example
- If users expects to debug the core0 and core1 example step by step, users must ensure the *board_init()* function is executed before debugging the core1 example as some hardware resoruces needs to be initialized by *board_init()* in core0 example.

## Running Results
- When the project runs successfully, The serial port terminal will output the following information:
    ```shell
    Primary core started
    Starting secondary core...

    Secondary core started...
    Get number from core1:
    Got number: 1
    Start normal rpc call example.
    RPC call example finished.


    Get number from core0:
    getNumberFromCore0 function call: Actual number is 2
    Got number: 2
    Start nested rpc call example.
    getNumberFromCore0 function call: Actual number is 2
    RPC call example finished.


    Get number from core1:
    Got number: 3
    Start normal rpc call example.
    RPC call example finished.


    Get number from core0:
    getNumberFromCore0 function call: Actual number is 5
    Got number: 5
    Start nested rpc call example.
    getNumberFromCore0 function call: Actual number is 5
    RPC call example finished.


    Get number from core1:
    Got number: 6
    Start normal rpc call example.
    RPC call example finished.


    Get number from core0:
    getNumberFromCore0 function call: Actual number is 8
    Got number: 8
    Start nested rpc call example.
    getNumberFromCore0 function call: Actual number is 8
    RPC call example finished.
    ```
