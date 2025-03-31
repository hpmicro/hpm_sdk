.. _rvbacktrace_no_fp:

rvbacktrace_no_fp
==================================

Overview
--------

This example demonstrates the use of instruction parsing to perform stack backtracking for interrupts, function calls and exception on a riscv32 machine.

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
       [0]Stack interval :[0x87f10 - 0x87f50]  ra 0x800086c6
       [1]Stack interval :[0x87f50 - 0x87f60]  ra 0x80005f82
       [2]Stack interval :[0x87f60 - 0x87f70]  ra 0x22c
       [3]Stack interval :[0x87f70 - 0x87f80]  ra 0x274
       [4]Stack interval :[0x87f80 - 0x87fd0]  ra 0x80006084
       [5]Stack interval :[0x87fd0 - 0x87ff0]  ra 0x80006406
       [6]Stack interval :[0x87ff0 - 0x88000]  ra 0x8000305a

       addr2line -e xxx.elf -a -f 800086c6 80005f82 22c 274 80006084 80006406 8000305a




- caller and callee


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




- exception


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
           mepc: 		 0x80005f2c
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

  - The first stack frame of the print information is the stack frame of the stack backtrace API, and the call starts at ``[1]Stack interval`` .

  - In exception situations, ``rvbacktrace_trap()``  needs to be used. For interrupts and function calls, ``rvbacktrace()``  is used.

  - This component is generally used for software debugging. It is recommended to use it under the debug level.

  - The method of rvbacktrace through instruction parsing is applicable to the situation where instructions are in a separate `text` segment. However, the presence of gaps between instructions or `RODATA` may lead to incorrect instruction parsing and ultimately result in the failure of rvbacktrace.

  - To view the call chain of function names, the ``addr2line``  tool needs to be used

       .. code-block:: console

              addr2line -e xxx.elf -a -f <addr1> <addr2> ...