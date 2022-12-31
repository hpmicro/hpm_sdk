# eRPC 双向调用示例
## 概述

本工程执行eRPC示例，采用rpmsg作为传输层协议，采用mbx作为消息通知。Client端通过简单的本地函数调用使用Server端提供的函数服务。

本示例是双向eRPC示例，core0既作为Client端也作为Server端，同样core1也既作为Client端也作为Server端。Core0可调用Core1中的erpc函数，Core1也可调用Core0端的erpc函数。

更多详细信息请参考 [官方 eRPC github](https://github.com/EmbeddedRPC/erpc)和[官方 rpmsg_lite github](https://github.com/NXPmicro/rpmsg-lite)

## IDL 文件生成工程文件
IDL文件包含erpc调用的函数申明，本示例中的IDL文件为【erpc_two_way_rpc.erpc】。

当函数在IDL文件中申明为Callback类型的函数时，将在Client端和Server端两端都会生成该函数，两端都可以调用，一个是本地函数调用，一个是erpc函数调用。

更多详细语法请参考 [docs/IDL-Reference](../../../../middleware/erpc/erpc/docs/IDL-Reference.md)

在该目录下（generated），执行如下命令：
```
../../../../../middleware/erpc/erpc/tools/erpcgen/Windows/erpcgen.exe erpc_two_way_rpc.erpc
```

将会生成如下8个文件：
 - erpc_two_way_rpc_Core0Interface.h
 - erpc_two_way_rpc_Core0Interface_client.cpp
 - erpc_two_way_rpc_Core0Interface_server.cpp
 - erpc_two_way_rpc_Core0Interface_server.h
 - erpc_two_way_rpc_Core1Interface.h
 - erpc_two_way_rpc_Core1Interface_client.cpp
 - erpc_two_way_rpc_Core1Interface_server.cpp
 - erpc_two_way_rpc_Core1Interface_server.h


## 导入至 client/server 工程中

`Core0 project` 需要导入 __erpc_two_way_rpc_Core0Interface_client.cpp__ 和 __erpc_two_way_rpc_Core1Interface_server.cpp__ 文件。需要 __include__ __erpc_two_way_rpc_Core0Interface.h__ 和 __erpc_two_way_rpc_Core1Interface_server.h__ 头文件。即Core0需要实现Core1的server函数。


`Core1 project` 需要导入 __erpc_two_way_rpc_Core1Interface_client.cpp__ 和 __erpc_two_way_rpc_Core0Interface_server.cpp__ 文件。 需要 __include__ __erpc_two_way_rpc_Core1Interface.h__ 和 __erpc_two_way_rpc_Core0Interface_server.h__ 头文件。即Core1需要实现Core0的server函数。

## 生成core1工程
__CMAKE_BUILD_TYPE__ 为 *"sec_core_img"*

## 生成core0工程
__CMAKE_BUILD_TYPE__ 用户可指定为 *"flash_sdram_xip"* 或 *"flash_xip"* 等。

## 编译工程
用户必须在编译完 __core1__ 工程后，再编译 __core0__ 工程。

## 下载调试
- 下载core0示例到设备并运行
- 下载core1示例到设备并运行
- 如果用户希望单步调试core0和core1的示例，用户需确保在core0示例中的board_init()执行完后再调试core1示例。因为core1示例使用了需要core0 board_init()函数初始化的硬件资源

## 执行结果
- 当core示例正常运行的时候，串口终端会打印如下信息:
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