# DMA Manager
## Overview

DMA Manager example demonstrates the following features in DMA Manager component.
- Request/release DMA resource
- Enable DMA interrupt for target DMA resource
- Install DMA callback for target DMA resource


## DMA Manager Tips

- The DMA Manager handles the interrupt inside the component, users must not declare ISR in the application

## Board Setting

No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
``` shell
 ============================================================================


    This example demonstrates the following features with DMA manager

      1. Request/release DMA resource
      2. Enable DMA interrupt
      3. DMA callback installation


=============================================================================
DMA resource 0 completed transfer, no errors
DMA resource 1 completed transfer, no errors
DMA resource 2 completed transfer, no errors
DMA resource 3 completed transfer, no errors
DMA resource 4 completed transfer, no errors
DMA resource 5 completed transfer, no errors
DMA resource 6 completed transfer, no errors
DMA resource 7 completed transfer, no errors
DMA resource 8 completed transfer, no errors
DMA resource 9 completed transfer, no errors
DMA resource 10 completed transfer, no errors
DMA resource 11 completed transfer, no errors
DMA resource 12 completed transfer, no errors
DMA resource 13 completed transfer, no errors
DMA resource 14 completed transfer, no errors
DMA resource 15 completed transfer, no errors
DMA resource 0 completed transfer, no errors
DMA resource 1 completed transfer, no errors
DMA resource 2 completed transfer, no errors
DMA resource 3 completed transfer, no errors
DMA resource 4 completed transfer, no errors
DMA resource 5 completed transfer, no errors
DMA resource 6 completed transfer, no errors
DMA resource 7 completed transfer, no errors
DMA resource 8 completed transfer, no errors
DMA resource 9 completed transfer, no errors
DMA resource 10 completed transfer, no errors
DMA resource 11 completed transfer, no errors
DMA resource 12 completed transfer, no errors
DMA resource 13 completed transfer, no errors
DMA resource 14 completed transfer, no errors
DMA resource 15 completed transfer, no errors
DMA Manager test PASSED

```