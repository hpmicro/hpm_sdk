.. _erpc_two_way_rpc_demo:

eRPC 双向调用示例
======================

概述
------

本工程执行eRPC示例，采用rpmsg作为传输层协议，采用mbx作为消息通知。Client端通过简单的本地函数调用使用Server端提供的函数服务。

本示例是双向eRPC示例，core0既作为Client端也作为Server端，同样core1也既作为Client端也作为Server端。Core0可调用Core1中的erpc函数，Core1也可调用Core0端的erpc函数。

更多详细信息请参考 `[官方 eRPC github] <https://github.com/EmbeddedRPC/erpc>`_ 和 `[官方 rpmsg_lite github] <https://github.com/NXPmicro/rpmsg-lite>`_

目前基于erpc v1.10.0和rpmsg-lite v5.0.0开发。

IDL 文件生成工程文件
----------------------------

IDL文件包含erpc调用的函数申明，本示例中的IDL文件为【erpc_two_way_rpc.erpc】。

当函数在IDL文件中申明为Callback类型的函数时，将在Client端和Server端两端都会生成该函数，两端都可以调用，一个是本地函数调用，一个是erpc函数调用。

更多详细语法请参考 `middleware/erpc/erpc/docs/IDL-Reference.md`

在该目录下（generated），执行如下命令：

.. code-block:: text

   ../../../../../middleware/erpc/erpc/tools/erpcgen/Windows/erpcgen.exe erpc_two_way_rpc.erpc


将会生成如下8个文件：
 - erpc_two_way_rpc_Core0Interface.h

 - erpc_two_way_rpc_Core0Interface_client.cpp

 - erpc_two_way_rpc_Core0Interface_server.cpp

 - erpc_two_way_rpc_Core0Interface_server.h

 - erpc_two_way_rpc_Core1Interface.h

 - erpc_two_way_rpc_Core1Interface_client.cpp

 - erpc_two_way_rpc_Core1Interface_server.cpp

 - erpc_two_way_rpc_Core1Interface_server.h

导入至 client/server 工程中
---------------------------------

`Core0 project` 需要导入 **erpc_two_way_rpc_Core0Interface_client.cpp** 和 **erpc_two_way_rpc_Core1Interface_server.cpp** 文件。需要 **include** **erpc_two_way_rpc_Core0Interface.h** 和 **erpc_two_way_rpc_Core1Interface_server.h** 头文件。即Core0需要实现Core1的server函数。

`Core1 project` 需要导入 **erpc_two_way_rpc_Core1Interface_client.cpp** 和 **erpc_two_way_rpc_Core0Interface_server.cpp** 文件。 需要 **include** **erpc_two_way_rpc_Core1Interface.h** 和 **erpc_two_way_rpc_Core0Interface_server.h** 头文件。即Core1需要实现Core0的server函数。

生成、编译和调试多核工程
------------------------------------

请参见 :ref:`Multicore 通用说明 <multicore_general_description>`

执行结果
------------

- 以HPM6750EVKMINI为例，当示例正常运行的时候，串口终端会打印如下信息:


.. code-block:: console

       Primary core started

       Copying secondary core image to destination memory: 0x1180000
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


