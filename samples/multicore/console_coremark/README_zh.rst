.. _console_coremark_demo:

Console Coremark 示例
======================================

概览
------

多核 命令上终端 Coremark 示例同时在核０和核１上跑 "coremark"并将结果打印到终端

开发板设置
---------------

BOOT_PIN 必须拨到 0-OFF, 1-OFF

生成、编译和调试多核工程
------------------------------------

请参见 :ref:`Multicore 通用说明 <multicore_general_description>`

运行现象
------------

- 以HPM6750EVKMINI为例，当该程序正确运行时，会打印类似如下示例


.. code-block:: console

   ----------------------------------------------------------------------
   $$\   $$\ $$$$$$$\  $$\      $$\ $$\
   $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
   $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
   $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
   $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
   $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
   $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
   \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
   ----------------------------------------------------------------------
   Dual-core Coremark example

   Copying secondary core image to destination memory: 0x1180000
   Run Coremark on Core1...

   Run Coremark on Core0...

   ------------------------------------------------------
   |                                                    |
   |          Core0 CoreMark Info:                      |
   |                                                    |
   ------------------------------------------------------
   2K performance run parameters for coremark.
   CoreMark Size    : 666
   Total ticks      : 371867026
   Total time (secs): 15.494459
   Iterations/Sec   : 3872.351941
   Iterations       : 60000
   Compiler version : GCC11.1.0
   Compiler flags   : -Wall -Wno-format -fomit-frame-pointer -fno-builtin -ffunction-sections -fdata-sections -mabi=ilp32 -march=rv32imac -g -O3 -funroll-all-loops -finline-limit=600 -ftree-dominator-optsg
   Memory location  : STACK
   seedcrc          : 0xe9f5
   [0]crclist       : 0xe714
   [0]crcmatrix     : 0x1fd7
   [0]crcstate      : 0x8e3a
   [0]crcfinal      : 0xbd59
   Correct operation validated. See README.md for run and reporting rules.
   CoreMark 1.0 : 3872.351941 / GCC11.1.0 -Wall -Wno-format -fomit-frame-pointer -fno-builtin -ffunction-sections -fdata-sections -mabi=ilp32 -march=rv32imac -g -O3 -funroll-all-loops -finline-limit=600 -K



   ------------------------------------------------------
   |                                                    |
   |               Core1 CoreMark Info:                 |
   |                                                    |
   ------------------------------------------------------
   2K performance run parameters for coremark.
   CoreMark Size    : 666
   Total ticks      : 15608229
   Total time (secs): 15.608229
   Iterations/Sec   : 3844.126070
   Iterations       : 60000
   Compiler version : GCC11.1.0
   Compiler flags   : -Wall -Wno-format -fomit-frame-pointer -fno-builtin -ffunction-sections -fdata-sections -mabi=ilp32 -march=rv32imac -g -O3 -funroll-all-loops -finline-limit=600 -ftree-dominator-optsg
   Memory location  : STACK
   seedcrc          : 0xe9f5
   [0]crclist       : 0xe714
   [0]crcmatrix     : 0x1fd7
   [0]crcstate      : 0x8e3a
   [0]crcfinal      : 0xbd59
   Correct operation validated. See README.md for run and reporting rules.
   CoreMark 1.0 : 3844.126070 / GCC11.1.0 -Wall -Wno-format -fomit-frame-pointer -fno-builtin -ffunction-sections -fdata-sections -mabi=ilp32 -march=rv32imac -g -O3 -funroll-all-loops -finline-limit=600 -K



   Press any key to start a new round of coremark test

用户按任意键可以开启新一轮多核coremark测试
