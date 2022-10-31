# MBX dualcore
## 概述
***
该示例程序演示了MBX如何工作。Core 0会发送信息给Core 1，Core 1收到之后并将之返回。

## Board Setting
***
使用USB转串口模块，连接评估板上Core 1调试串口终端(请确认具体开发板 {ref}`引脚描述 <lab_board_resource>` 部分描述)

## Running the Example
***
运行此程序，需要使用双核调试脚本，并需要两个gdb客户端分别对Core 0和Core 1进行调试

- 使用双核调试脚本启动openocd，此时可以看到gdb server将会为两个核分别开启一个端口
![dualcore_openocd](../../../../doc/images/samples/mbx_dualcore_openocd.png)
- 为每个核生成工程，使用ninja编译工程
  - Core 0：
  ![build_core0](../../../../doc/images/samples/mbx_gen_proj_build_core0.png)
  - Core 1:
  ![build_core1](../../../../doc/images/samples/mbx_gen_proj_build_core1.png)
- 启动两个gdb client连接gdb server
  - 为Core 0将gdb client连接到端口3333
  ![gdb_core0](../../../../doc/images/samples/mbx_gdb_load_core0.png)
  - 为Core 1将gdb client连接到端口3334
  ![gdb_core1](../../../../doc/images/samples/mbx_gdb_load_core1.png)
- 启动串口工具，分别连接到Core 0和Core 1两个调试串口
  ![console](../../../../doc/images/samples/mbx_console.png)
- 在gdb中启动示例程序
  - Core 0:
  ![core0_console_output](../../../../doc/images/samples/mbx_core0_console_output.png)
  - Core 1:
  ![core1_console_output](../../../../doc/images/samples/mbx_core1_console_output.png)
  - 一旦Core1程序启动之后, Core 1将会将从Core 0接收到的信息返回给Core 0，测试将在Core 0的调试串口将可以看到:
  ![core0_console_output2](../../../../doc/images/samples/mbx_core0_console_output2.png)
