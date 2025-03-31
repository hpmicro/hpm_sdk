.. _rvbacktrace_fp:

rvbacktrace_fp
============================

Overview
--------

This example demonstrates the use of frame pointer to perform stack backtracking for interrupts, function calls and exception on a riscv32 machine.

Hardware Configuration
----------------------

No special configuration is required.

Running the example
-------------------

Once this program runs correctly, the function call information corresponding to the situation will be output on the console. In case of an exception, the register values saved in the stack when the exception occurs will be output.

- interrupt


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




- caller and callee


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




- exception


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

  - The first stack frame of the print information is the stack frame of the stack backtrace API, and the call starts at ``[1]Stack interval`` .

  - In exception situations, ``rvbacktrace_trap()``  needs to be used. For interrupts and function calls, ``rvbacktrace()``  is used.

  - This component is generally used for software debugging. It is recommended to use it under the debug level.

  - To view the call chain of function names, the ``addr2line``  tool needs to be used

       .. code-block:: console

              addr2line -e xxx.elf -a -f <addr1> <addr2> ...

