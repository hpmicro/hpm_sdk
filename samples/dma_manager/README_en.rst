.. _dma_manager:

DMA Manager
======================

Overview
--------

DMA Manager example demonstrates the following features in DMA Manager component.
- Request/release DMA resource

- Setup DMA config

- Enable DMA interrupt for target DMA resource

- Install DMA callback for target DMA resource

Tips
-------------

The DMA Manager handles the interrupt inside the component, users must not declare ISR in the application

Board Setting
-------------

No special settings

Running the example
-------------------

When the project runs correctly, and the serial port terminal will output the following information.
The total number of resource is decided by SOC DMA maximum channel count.


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
