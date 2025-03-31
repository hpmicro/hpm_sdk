.. _rvbacktrace_fp:

rvbacktrace_fp
============================

概述
------

该示例展示了在riscv32机器上，使用frame pointer完成对中断，函数调用以及异常三种情况的栈回溯。

硬件设置
------------

无特殊硬件配置

运行
------

一旦此程序正确运行，在console上会输出对应情况的函数调用信息，在异常的情况会输出发生异常时保存到栈中的寄存器值。

- 中断情况


.. code-block:: console

       ENTER ISR

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f10 - 0x87f40] ra 0x800090f8
       [1]Stack interval :[0x87f40 - 0x87f50] ra 0x80006072
       [2]Stack interval :[0x87f50 - 0x87f60] ra 0x230
       [3]Stack interval :[0x87f60 - 0x87f70] ra 0x282
       [4]Stack interval :[0x87f70 - 0x87fd0] ra 0x800061c8
       [5]Stack interval :[0x87fd0 - 0x87ff0] ra 0x8000664c

       addr2line -e xxx.elf -a -f 800090f8 80006072 230 282 800061c8 8000664c
       ---- RV_Backtrace Call Frame End:----




- 函数调用


.. code-block:: console

       F[function_recursion]: recursion cycle[1]

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f70 - 0x87fa0] ra 0x800090f8
       [1]Stack interval :[0x87fa0 - 0x87fb0] ra 0x80005fca
       [2]Stack interval :[0x87fb0 - 0x87fd0] ra 0x800061de
       [3]Stack interval :[0x87fd0 - 0x87ff0] ra 0x8000664c

       addr2line -e xxx.elf -a -f 800090f8 80005fca 800061de 8000664c
       ---- RV_Backtrace Call Frame End:----
       F[function_recursion]: recursion cycle[2]

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f50 - 0x87f80] ra 0x800090f8
       [1]Stack interval :[0x87f80 - 0x87f90] ra 0x80005fca
       [2]Stack interval :[0x87f90 - 0x87fb0] ra 0x80005fd4
       [3]Stack interval :[0x87fb0 - 0x87fd0] ra 0x800061de
       [4]Stack interval :[0x87fd0 - 0x87ff0] ra 0x8000664c

       addr2line -e xxx.elf -a -f 800090f8 80005fca 80005fd4 800061de 8000664c
       ---- RV_Backtrace Call Frame End:----
       F[function_recursion]: recursion cycle[3]

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f30 - 0x87f60] ra 0x800090f8
       [1]Stack interval :[0x87f60 - 0x87f70] ra 0x80005fca
       [2]Stack interval :[0x87f70 - 0x87f90] ra 0x80005fd4
       [3]Stack interval :[0x87f90 - 0x87fb0] ra 0x80005fd4
       [4]Stack interval :[0x87fb0 - 0x87fd0] ra 0x800061de
       [5]Stack interval :[0x87fd0 - 0x87ff0] ra 0x8000664c

       addr2line -e xxx.elf -a -f 800090f8 80005fca 80005fd4 80005fd4 800061de 8000664c
       ---- RV_Backtrace Call Frame End:----
       F[function_recursion]: recursion cycle[4]

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87f10 - 0x87f40] ra 0x800090f8
       [1]Stack interval :[0x87f40 - 0x87f50] ra 0x80005fca
       [2]Stack interval :[0x87f50 - 0x87f70] ra 0x80005fd4
       [3]Stack interval :[0x87f70 - 0x87f90] ra 0x80005fd4
       [4]Stack interval :[0x87f90 - 0x87fb0] ra 0x80005fd4
       [5]Stack interval :[0x87fb0 - 0x87fd0] ra 0x800061de
       [6]Stack interval :[0x87fd0 - 0x87ff0] ra 0x8000664c

       addr2line -e xxx.elf -a -f 800090f8 80005fca 80005fd4 80005fd4 80005fd4 800061de 8000664c
       ---- RV_Backtrace Call Frame End:----




- 异常情况


.. code-block:: console

       GET FAULT ADDRESS, ENTER TRAP

       ---- RV_Backtrace Call Frame Start: ----
       [0]Stack interval :[0x87e80 - 0x87eb0] ra 0x8000911c
       [1]Stack interval :[0x87eb0 - 0x87ed0] ra 0x80006052
       [2]Stack interval :[0x87ed0 - 0x87ef0] ra 0x422
       [3]Stack interval :[0x87ef0 - 0x87f80] ra 0x80005ff8
       [4]Stack interval :[0x87f80 - 0x87fb0] ra 0x80006026
       [5]Stack interval :[0x87fb0 - 0x87fd0] ra 0x800061e0
       [6]Stack interval :[0x87fd0 - 0x87ff0] ra 0x8000664c

       addr2line -e xxx.elf -a -f 8000911c 80006052 422 80005ff8 80006026 800061e0 8000664c
       ---- RV_Backtrace Call Frame End:----
       ---- RV Core Dump:----
           mcause: 	 0x5
           mepc: 		 0x80005ffc
           ra: 		 0x80005ff8
           t0: 		 0x0
           t1: 		 0x87ed3
           t2: 		 0x20
           s0: 		 0x87fb0
           a0: 		 0x1e
           a1: 		 0xa
           a2: 		 0x1e
           a3: 		 0xf0040000
           a4: 		 0x6e1
           a5: 		 0xffffffff
           a6: 		 0x8000f158
           a7: 		 0x73
           t3: 		 0x271a
           t4: 		 0x10bacb8
           t5: 		 0x3acb8
           t6: 		 0x10bf9e0
       ---- RV Core Dump End:----




.. note::

  - 打印信息的第一层栈帧为栈回溯api的栈帧，调用开始在 ``[1]Stack interval``  开始

  - 在异常情况下，需要使用 ``rvbacktrace_trap()`` , 中断以及函数调用使用 ``rvbacktrace()``

  - 该组件一般用于软件调试，建议在debug等级下使用

  - 查看函数名调用关系链，需要使用addr2line工具

       .. code-block:: console

              addr2line -e xxx.elf -a -f <addr1> <addr2> ...
