.. _dmav2_fixed_burst_transfer_and_swap_transfer:

DMAV2 Fixed Burst Transfer and Swap Transfer
========================================================================================

Overview
--------

- DMAV2 has added fixed burst transfer and swap transfer feature, and this example project demonstrates those.

- Fixed burst transfer: It can achieve the transmission of data in a fixed address area for each DMA request.

- Swap transfer: It can achieve any arrangement of byte order for transmission.

Board Setting
-------------

- no need.

Running the example
-------------------

- After running, the serial console prints the following output


.. code-block:: console

   1. start dma src address fix burst none swap transfer!
   1. dma transfer check ok!
   2. start dma src address fix burst swap by byte transfer!
   2. dma transfer check ok!
   3. start dma src address fix burst swap by half word transfer!
   3. dma transfer check ok!
   4. start dma src address fix burst swap by swap table transfer!
   4. dma transfer check ok!

