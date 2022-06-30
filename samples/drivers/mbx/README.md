# MBX
## Overview
***
This example demonstrate how MBX works. Core 0 will send message to core 1 which will echo back what's been received.

## Board Setting
***
debug console for core1 can be found in [Pin Description](lab_board_resource) for specific board

## Running the Example
***
To run this example, a dual core debug script and two gdb client will be needed.

- start openocd with the config file for dual core, then two gdbserver will listen to two ports for each core
![dualcore_openocd](../../../doc/images/samples/mbx_dualcore_openocd.png)
- generate and build projects for each core using ninja
  - generate and build project for core0 with ninja
  ![build_core0](../../../doc/images/samples/mbx_gen_proj_build_core0.png)
  - generate and build project for core1 with ninja
  ![build_core1](../../../doc/images/samples/mbx_gen_proj_build_core1.png)
- start two gdb clients and connect two each port
  - connect and load application for core0 to port 3333
  ![gdb_core0](../../../doc/images/samples/mbx_gdb_load_core0.png)
  - connect and load application for core1 to port 3334
  ![gdb_core1](../../../doc/images/samples/mbx_gdb_load_core1.png)
- start console application connecting to two consoles
  ![console](../../../doc/images/samples/mbx_console.png)
- start to run application for each core, following content should be shown on debug console
  - for core0:
  ![core0_console_output](../../../doc/images/samples/mbx_core0_console_output.png)
  - for core1:
  ![core1_console_output](../../../doc/images/samples/mbx_core1_console_output.png)
  - once core1 application starts, core1 will receive messages from core0 then send them back, then in the console of core0:
  ![core0_console_output2](../../../doc/images/samples/mbx_core0_console_output2.png)
