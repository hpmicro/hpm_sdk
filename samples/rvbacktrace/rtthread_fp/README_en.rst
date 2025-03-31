.. _rvbacktrace_rtthread:

rvbacktrace_RTThread
========================================

Overview
--------

This example demonstrates the stack backtracking of threads in ``RTThread-nano``  on a riscv32 machine by using the frame pointer.

Hardware Configuration
----------------------

No special configuration is required.

Running the example
-------------------

Once this program runs correctly, the thread stack information of the corresponding thread will be output on the console. Entering ``rv_backtrace_all``  in finsh will output the information of the thread stacks of other threads except the current thread.

- single thread


.. code-block:: console

       initialize rti_board_start:0 done

       \ | /
       - RT -     Thread Operating System
       / | \     3.1.5 build Nov 20 2024
       2006 - 2020 Copyright by rt-thread team
       do components initialization.
       initialize rti_board_end:0 done
       initialize finsh_system_init:0 done

       ---- RV_Backtrace Call Frame Start: ----
       ###Please consider the value of ra as accurate and the value of sp as only for reference###
       ----------s--------------------Thread: main backtrace------------------------------
       Current Thread Name:  main
       [0]Stack interval :[0x10819a0 - 0x10819c0]  ra 0x800103ea
       [1]Stack interval :[0x10819c0 - 0x10819d0]  ra 0x8000ff9c
       [2]Stack interval :[0x10819d0 - 0x10819e0]  ra 0x80006708
       [3]Stack interval :[0x10819e0 - 0x1081a00]  ra 0x800094f6
       [4]Stack interval :[0x1081a00 - 0xdeadbeef]  ra 0x8000c65a

       addr2line -e xxx.elf -a -f 800103e6 8000ff98 80006704 800094f2 8000c656
       ---- RV_Backtrace Call Frame End:----




- threads


.. code-block:: console

       rv_backtrace_all

       ---- RV_Backtrace Call Frame Start: ----
       ------------------------------Thread: event_send backtrace------------------------------
       [0]Thread Name:  event_send
       [0]Stack interval :[0x108299c - 0x10829cc]  ra 0x8000cb7a
       [1]Stack interval :[0x10829cc - 0x10829fc]  ra 0x8000cbdc
       [2]Stack interval :[0x10829fc - 0x1082a1c]  ra 0x8000667c
       [3]Stack interval :[0x1082a1c - 0xdeadbeef]  ra 0x8000c65a

       addr2line -e xxx.elf -a -f 8000cb76 8000cbd8 80006678 8000c656
       ------------------------------Thread: event_rec backtrace------------------------------
       [1]Thread Name:  event_rec
       [0]Stack interval :[0x1082504 - 0x1082544]  ra 0x8000a0a6
       [1]Stack interval :[0x1082544 - 0x1082574]  ra 0x8000663e
       [2]Stack interval :[0x1082574 - 0xdeadbeef]  ra 0x8000c65a

       addr2line -e xxx.elf -a -f 8000a0a2 8000663a 8000c656
       ------------------------------Thread: tidle backtrace------------------------------
       [3]Thread Name:  tidle
       [0]Stack interval :[0x10804e0 - 0x1080510]  ra 0x800096ee
       [1]Stack interval :[0x1080510 - 0xdeadbeef]  ra 0x8000c65a

       addr2line -e xxx.elf -a -f 800096ea 8000c656
       ------------------------------Thread: timer backtrace------------------------------
       [4]Thread Name:  timer
       [0]Stack interval :[0x1080928 - 0x1080958]  ra 0x8000d8d6
       [1]Stack interval :[0x1080958 - 0xdeadbeef]  ra 0x8000c65a

       addr2line -e xxx.elf -a -f 8000d8d2 8000c656
       Thread Total Num: 5
       ---- RV_Backtrace Call Frame End:----




.. note::

  - Enabling ``BACKTRACE_ALL``  in the ``backtrace_config.h``  file allows for the backtracking of all thread stacks except the current one.

  - This component is generally used for software debugging. It is recommended to use it under the debug level.

  - To view the call chain of function names, the ``addr2line``  tool needs to be used

       .. code-block:: console

              addr2line -e xxx.elf -a -f <addr1> <addr2> ...
