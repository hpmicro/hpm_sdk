.. _dma_general_transfer:

DMA General Transfer
========================================

Overview
--------

DMA example project shows transfer data with unchined and chained way. This example checks if the data transferred by DMA is correct.

Board Setting
-------------

No special settings

Running the example
-------------------

Taking HPM6750EVK2 flash-xip build type as an example, when the project runs correctly, the serial port terminal will output the following information:

.. code-block:: console

   dma example start

   unchained transfer

   write testing
   dma transferring data from 0x80000 to 0x1100000, burst size: 8192 bytes
   rw throughput: 269284.71 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 4096 bytes
   rw throughput: 497409.31 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 2048 bytes
   rw throughput: 496765.84 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 1024 bytes
   rw throughput: 495483.87 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 512 bytes
   rw throughput: 491677.34 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 256 bytes
   rw throughput: 484848.50 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 128 bytes
   rw throughput: 471744.46 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 64 bytes
   rw throughput: 361922.71 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 32 bytes
   rw throughput: 247582.20 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 16 bytes
   rw throughput: 151539.06 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches
   dma transferring data from 0x80000 to 0x1100000, burst size: 8 bytes
   rw throughput: 85352.30 KB/s
   compare data between source address 0x80000 and destination address 0x1100000:  all data matches

   read testing
   dma transferring data from 0x1100000 to 0x80000, burst size: 8192 bytes
   rw throughput: 487928.84 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 4096 bytes
   rw throughput: 487928.84 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 2048 bytes
   rw throughput: 486692.00 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 1024 bytes
   rw throughput: 485461.43 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 512 bytes
   rw throughput: 481806.78 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 256 bytes
   rw throughput: 474660.06 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 128 bytes
   rw throughput: 460431.65 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 64 bytes
   rw throughput: 359887.53 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 32 bytes
   rw throughput: 238213.40 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 16 bytes
   rw throughput: 144252.43 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches
   dma transferring data from 0x1100000 to 0x80000, burst size: 8 bytes
   rw throughput: 80757.10 KB/s
   compare data between source address 0x1100000 and destination address 0x80000:  all data matches

   chained transfer
   dma setup channel done
   compare data between source address 0x1040000 and destination address 0x1100000:  all data matches
    [0]: data match
   compare data between source address 0x1040000 and destination address 0x1104000:  all data matches
    [1]: data match
   compare data between source address 0x1040000 and destination address 0x1108000:  all data matches
    [2]: data match

   dma example end
