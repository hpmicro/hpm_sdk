.. _rvbacktrace_no_fp:

rvbacktrace_no_fp
==================================

概述
------

该示例展示了在riscv32机器上，使用指令解析的方式完成对中断，函数调用以及异常三种情况的栈回溯。

硬件配置
------------

无特殊硬件配置

运行
------

一旦此程序正确运行，在console上会输出对应情况的函数调用信息，在异常的情况会输出发生异常时保存到栈中的寄存器值。

- 中断情况


.. code-block:: console

       ENTER ISR

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f10 - 0x87f50]  ra 0x800086c6
       [1]Stack interval :[0x87f50 - 0x87f60]  ra 0x80005f82
       [2]Stack interval :[0x87f60 - 0x87f70]  ra 0x22c
       [3]Stack interval :[0x87f70 - 0x87f80]  ra 0x274
       [4]Stack interval :[0x87f80 - 0x87fd0]  ra 0x80006084
       [5]Stack interval :[0x87fd0 - 0x87ff0]  ra 0x80006406
       [6]Stack interval :[0x87ff0 - 0x88000]  ra 0x8000305a

       addr2line -e xxx.elf -a -f 800086c6 80005f82 22c 274 80006084 80006406 8000305a




- 函数调用


.. code-block:: console

       F[function_recursion]: recursion cycle[1]

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f60 - 0x87fa0]  ra 0x800086c6
       [1]Stack interval :[0x87fa0 - 0x87fb0]  ra 0x80005f06
       [2]Stack interval :[0x87fb0 - 0x87fd0]  ra 0x80006098
       [3]Stack interval :[0x87fd0 - 0x87ff0]  ra 0x80006406
       [4]Stack interval :[0x87ff0 - 0x88000]  ra 0x8000305a

       addr2line -e xxx.elf -a -f 800086c6 80005f06 80006098 80006406 8000305a
       ---- RV_Backtrace Call Frame End:----

       F[function_recursion]: recursion cycle[2]

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f40 - 0x87f80]  ra 0x800086c6
       [1]Stack interval :[0x87f80 - 0x87f90]  ra 0x80005f06
       [2]Stack interval :[0x87f90 - 0x87fb0]  ra 0x80005f0e
       [3]Stack interval :[0x87fb0 - 0x87fd0]  ra 0x80006098
       [4]Stack interval :[0x87fd0 - 0x87ff0]  ra 0x80006406
       [5]Stack interval :[0x87ff0 - 0x88000]  ra 0x8000305a

       addr2line -e xxx.elf -a -f 800086c6 80005f06 80005f0e 80006098 80006406 8000305a
       ---- RV_Backtrace Call Frame End:----

       F[function_recursion]: recursion cycle[3]

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f20 - 0x87f60]  ra 0x800086c6
       [1]Stack interval :[0x87f60 - 0x87f70]  ra 0x80005f06
       [2]Stack interval :[0x87f70 - 0x87f90]  ra 0x80005f0e
       [3]Stack interval :[0x87f90 - 0x87fb0]  ra 0x80005f0e
       [4]Stack interval :[0x87fb0 - 0x87fd0]  ra 0x80006098
       [5]Stack interval :[0x87fd0 - 0x87ff0]  ra 0x80006406
       [6]Stack interval :[0x87ff0 - 0x88000]  ra 0x8000305a

       addr2line -e xxx.elf -a -f 800086c6 80005f06 80005f0e 80005f0e 80006098 80006406 8000305a
       ---- RV_Backtrace Call Frame End:----

       F[function_recursion]: recursion cycle[4]

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f00 - 0x87f40]  ra 0x800086c6
       [1]Stack interval :[0x87f40 - 0x87f50]  ra 0x80005f06
       [2]Stack interval :[0x87f50 - 0x87f70]  ra 0x80005f0e
       [3]Stack interval :[0x87f70 - 0x87f90]  ra 0x80005f0e
       [4]Stack interval :[0x87f90 - 0x87fb0]  ra 0x80005f0e
       [5]Stack interval :[0x87fb0 - 0x87fd0]  ra 0x80006098
       [6]Stack interval :[0x87fd0 - 0x87ff0]  ra 0x80006406
       [7]Stack interval :[0x87ff0 - 0x88000]  ra 0x8000305a

       addr2line -e xxx.elf -a -f 800086c6 80005f06 80005f0e 80005f0e 80005f0e 80006098 80006406 8000305a




- 异常情况


.. code-block:: console

       GET FAULT ADDRESS, ENTER TRAP

       ---- RV_Backtrace In Trap Start: ----
       [0]Stack interval :[0x87e80 - 0x87ec0]  ra 0x800086e0
       [1]Stack interval :[0x87ec0 - 0x87ee0]  ra 0x80005f66
       [2]Stack interval :[0x87ee0 - 0x87f00]  ra 0x3cc
       [3]Stack interval :[0x87f00 - 0x87f80]  ra 0x80005f2a
       [4]Stack interval :[0x87f80 - 0x87fb0]  ra 0x80005f48
       [5]Stack interval :[0x87fb0 - 0x87fd0]  ra 0x8000609a
       [6]Stack interval :[0x87fd0 - 0x87ff0]  ra 0x80006406
       [7]Stack interval :[0x87ff0 - 0x88000]  ra 0x8000305a

       addr2line -e xxx.elf -a -f 800086e0 80005f66 3cc 80005f2a 80005f48 8000609a 80006406 8000305a
       ---- RV_Backtrace In Trap End:----

       ---- RV Core Dump:----
           mcause: 	 0x5
           smepc: 		 0x80005f2c
           ra: 		 0x80005f2a
           t0: 		 0x0
           t1: 		 0x87ecc
           t2: 		 0x20
           a0: 		 0x1e
           a1: 		 0xa
           a2: 		 0x1e
           a3: 		 0xf0040000
           a4: 		 0x6e1
           a5: 		 0xffffffff
           a6: 		 0x8000ea18
           a7: 		 0x73
           t3: 		 0x271a
           t4: 		 0x10bacb8
           t5: 		 0x3acb8
           t6: 		 0x10bf9e0




.. note::

  - 打印信息的第一层栈帧为栈回溯api的栈帧，调用开始在 ``[1]Stack interval`` 开始。

  - 在异常情况下，需要使用 ``rvbacktrace_trap()`` , 中断以及函数调用使用 ``rvbacktrace()`` 。

  - 该组件一般用于软件调试，建议在debug等级下使用。

  - 使用指令解析进行栈回溯的方式，适用于指令在单独的`text`段的情况，指令之间存在空洞或者`RODATA`会导致指令解析错误，造成栈回溯失败。

  - 查看函数名调用关系链，需要使用addr2line工具

       .. code-block:: console

              addr2line -e xxx.elf -a -f <addr1> <addr2> ...