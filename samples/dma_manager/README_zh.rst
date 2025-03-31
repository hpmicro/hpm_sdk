.. _dma_manager:

DMA Manager
======================

概述
------

DMA Manager 展示了如下功能。
- 申请/释放DMA资源

- 配置DMA通道

- 使用相应DMA资源的中断

- 安装DMA资源的中断回调函数

使用提示
------------

DMA Manager组件内部已经内置了中断处理函数，用户程序中不应当再声明中断处理函数

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息。
资源（resource）数量由SOC具备的DMA通道数决定。


.. code-block:: console

    ============================================================================


       This example demonstrates the following features with DMA manager

         1. Request/release DMA resource
         2. Setup DMA config
         3. Enable DMA interrupt
         4. DMA callback installation


   =============================================================================
   DMA resource 0 completed transfer, success
   DMA resource 1 completed transfer, success
   DMA resource 2 completed transfer, success
   DMA resource 3 completed transfer, success
   DMA resource 4 completed transfer, success
   DMA resource 5 completed transfer, success
   DMA resource 6 completed transfer, success
   DMA resource 7 completed transfer, success
   DMA resource 8 completed transfer, success
   DMA resource 9 completed transfer, success
   DMA resource 10 completed transfer, success
   DMA resource 11 completed transfer, success
   DMA resource 12 completed transfer, success
   DMA resource 13 completed transfer, success
   DMA resource 14 completed transfer, success
   DMA resource 15 completed transfer, success
   DMA resource 0 completed transfer, success
   DMA resource 1 completed transfer, success
   DMA resource 2 completed transfer, success
   DMA resource 3 completed transfer, success
   DMA resource 4 completed transfer, success
   DMA resource 5 completed transfer, success
   DMA resource 6 completed transfer, success
   DMA resource 7 completed transfer, success
   DMA resource 8 completed transfer, success
   DMA resource 9 completed transfer, success
   DMA resource 10 completed transfer, success
   DMA resource 11 completed transfer, success
   DMA resource 12 completed transfer, success
   DMA resource 13 completed transfer, success
   DMA resource 14 completed transfer, success
   DMA resource 15 completed transfer, success
   DMA Manager test PASSED

