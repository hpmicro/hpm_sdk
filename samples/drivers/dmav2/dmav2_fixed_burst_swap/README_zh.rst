.. _dmav2_fixed_burst_transfer_and_swap_transfer:

DMAV2固定地址区域burst传输和swap传输
==================================================

概述
------

- DMAV2新增了固定地址区域burst传输和swap传输，本示例工程展示了这两个传输特性。

- 固定地址区域burst传输：可实现每次DMA请求发送一片固定地址区域的数据。

- SWAP传输：可实现传输字节顺序的任意排列。

硬件设置
------------

- 无

运行现象
------------

- 运行完毕后，串口控制台打印输出如下：


.. code-block:: console

   1. start dma src address fix burst none swap transfer!
   1. dma transfer check ok!
   2. start dma src address fix burst swap by byte transfer!
   2. dma transfer check ok!
   3. start dma src address fix burst swap by half word transfer!
   3. dma transfer check ok!
   4. start dma src address fix burst swap by swap table transfer!
   4. dma transfer check ok!

