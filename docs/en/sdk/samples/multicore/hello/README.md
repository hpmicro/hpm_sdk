# Hello demo

## Overview

Multi-core example project runs the "hello world" example on core0 and runs the "rgb_led" example on core1.

In this project:
 - The serial port outputs "hello world"; Manually input the stirng information through the keyboard and print it through the serial port
 - The RGB leds are switching among RED, GREEN, BLUE respectively.


## Board Setting

  BOOT_PIN should be configured to 0-OFF, 1-OFF


## Generate core1 project

__CMAKE_BUILD_TYPE__ must be *"sec_core_img"*

## Generate core0 project

__CMAKE_BUILD_TYPE__ user can specify *"debug"* or *"flash_xip"*, etc.

## Build project

__Core1__ project must be generated and built first
__Core0__ project must be generated after the core1 project has been built successfully.


## Debugging the example

- Download the core0 example to the target and run core0 example first
- Download the core1 example to the target and run the core1 example

*NOTE*
- If users expects to debug the core0 and core1 example step by step, users must ensure the *board_init()* function is executed before debugging the core1 example as some hardware resoruces needs to be initialized by *board_init()* in core0 example.

- When debug core0 and the project runs successfully, The serial port terminal will output the following information:
    ```console
        Hello world for multicore! Secondary core control RGB leds blinking...
    ```
- When debug core1 and the project runs successfully, The RGB leds are switching among RED, GREEN, BLUE respectively.

## Running the example

- When __CMAKE_BUILD_TYPE__ is *"flash_xip"* , download the core0 example to the target, stop debugging and reset the board. When the project runs successfully, the serial port terminal will output the following information:
    ```console
        Copying secondary core image to destination memory: 0x1180000
        Hello world for multicore! Secondary core control RGB leds blinking...
    ```
- The RGB leds are switching among RED, GREEN, BLUE respectively.