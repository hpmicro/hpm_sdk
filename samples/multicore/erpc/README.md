# eRPC demo
## Overview

This project executes eRPC example, uses RPMSG as transport layer protocol, uses MBX as message notification.

Core0 as the Client and Core1 as the Server.

Please refer to [official eRPC github](https://github.com/EmbeddedRPC/erpc)和[official rpmsg_lite github](https://github.com/NXPmicro/rpmsg-lite)


## IDL file generate project files

IDL files contains all declarations used in eRPC calls. This demo IDL file as follows:
erpc_matrix_multiply.erpc:
```C
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
```

Please refer to [docs/IDL-Reference](../../../middleware/erpc/erpc/docs/IDL-Reference.md)

The easiest way for generating code is have both the tool and the IDL file in same directory（middleware/erpc/erpc/erpc_sample_services）. Then, the following command needs be executed:
```
../../../../middleware/erpc/erpc/tools/erpcgen/Windows/erpcgen.exe erpc_matrix_multiply.erpc
```
This action creates four files in current folder:
 - erpc_matrix_multiply.h
 - erpc_matrix_multiply_client.cpp
 - erpc_matrix_multiply_server.h
 - erpc_matrix_multiply_server.cpp

## Importing client/server files

`Client project` needs to count with __erpc_matrix_multiply.h__ and __erpc_matrix_multiply_client.cpp__ files. In the source file where the eRPC functions are called, the __erpc_matrix_multiply.h__ header file needs to be included.

`Server project` needs to count with __erpc_matrix_multiply.h__, __erpc_matrix_multiply_server.h__, and __erpc_matrix_multiply_server.cpp__ files. In source file where eRPC functions calls are implemented, the __erpc_matrix_multiply_server.h__ header file needs to be included.

## Generate core1 project
__CMAKE_BUILD_TYPE__ must be *"sec_core_img"*

## Generate core0 project
__CMAKE_BUILD_TYPE__ forced to be *"debug"*, and users don't need to care.

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
    ```
- Press the PBUTN button to initiate the next matrix multiplication, and red led and green led will be toggle.
