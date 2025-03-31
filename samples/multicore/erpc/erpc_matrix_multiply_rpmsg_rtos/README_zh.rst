.. _erpc_rtos_demo:

eRPC rtos 示例
========================

概述
------

本工程执行eRPC示例，采用rpmsg作为传输层协议，采用mbx作为消息通知。

core0作为Client端，core1作为Server端。Client端通过简单的本地函数调用使用Server端提供的函数服务。

更多详细信息请参考 `[官方 eRPC github] <https://github.com/EmbeddedRPC/erpc>`_ 和 `[官方 rpmsg_lite github] <https://github.com/NXPmicro/rpmsg-lite>`_

目前基于erpc v1.10.0和rpmsg-lite v5.0.0开发。

IDL 文件生成工程文件
----------------------------

IDL文件包含erpc调用的函数申明，本示例中的IDL文件内容如下：

erpc_matrix_multiply.erpc:

.. code-block:: text

   /*!
    * You can write copyrights rules here. These rules will be copied into the outputs.
    */
   @outputDir("erpc_outputs") // output directory
   program erpc_matrix_multiply; // specify name of output files

   /*! This const defines the matrix size. The value has to be the same as the
       Matrix array dimension. Do not forget to re-generate the eRPC code once the
       matrix size is changed in the eRPC file */
   const int32 matrix_size = 5;

   /*! This is the matrix array type. The dimension has to be the same as the
       matrix size const. Do not forget to re-generate the eRPC code once the
       matrix size is changed in the eRPC file */
   type Matrix = int32[matrix_size][matrix_size];

   interface MatrixMultiplyService // cover functions for same topic
   {
       erpcMatrixMultiply(in Matrix matrix1, in Matrix matrix2, out Matrix result_matrix) -> void
       oneway erpcSwitchLightLed()
   }


更多详细语法请参考 `middleware/erpc/erpc/docs/IDL-Reference.md`

在该目录下（generated），执行如下命令：

.. code-block:: text

   ../../../../../middleware/erpc/erpc/tools/erpcgen/Windows/erpcgen.exe erpc_matrix_multiply.erpc


将会生成如下4个文件：
 - erpc_matrix_multiply.h

 - erpc_matrix_multiply_client.cpp

 - erpc_matrix_multiply_server.h

 - erpc_matrix_multiply_server.cpp

导入至 client/server 工程中
---------------------------------

`Client project` 需要导入 **erpc_matrix_multiply.h** 和 **erpc_matrix_multiply_client.cpp** 文件。调用eRPC函数时，需要 **include** **"erpc_matrix_multiply.h"** 头文件。

`Server project` 需要导入 **erpc_matrix_multiply.h**, **erpc_matrix_multiply_server.h** 和 **erpc_matrix_multiply_server.cpp** 文件。 eRPC函数在 **erpc_matrix_multiply_server.cpp** 文件中实现。

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

           Matrix #1
           =========
             38   13    1   10   12
             49   34   25   39   37
             16   45   17   31   32
             24   44    1    4   11
             28   35   17   15   12

           Matrix #2
           =========
             8   15   27   19   36
             17   10   43   33   16
             28   11    4   45   36
              5    2   17    3   46
             38   43    6    3   34

           eRPC request is sent to the server

           Result matrix
           =============
           1059 1247 1831 1262 2480
           3271 3019 3770 3406 6260
           2740 2315 3154 2743 4422
           1406 1292 2678 1998 2162
           1826 1503 2656 2533 3278

           Switch Light LED
           Press the PBUTN button to initiate the next matrix multiplication


- 按下 PBUTN 按钮，则会输出新的计算结果，并且红绿LED会翻转。
