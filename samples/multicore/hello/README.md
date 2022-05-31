# Hello demo
## Overview

Multi-core example project runs the "hello world" example on core0 and runs the "rgb_led" example on core1.

In this project:
 - The serial port outputs "hello world"; Manually input the stirng information through the keyboard and print it through the serial port
 - The RGB leds are switching among RED, GREEN, BLUE respectively.


## Board Setting

  BOOT_PIN should be configured to 0-OFF, 1-OFF




## Generate and Build Multi-core projects


In this project, the core0 application runs in FLASH while the core1 application runs in its own ILM

__Core1__ project must be generated and built first

__Core0__ project must be generated after the core1 project has been built successfully.

### Generate core1 project
__CMAKE_BUILD_TYPE__ must be one of the following options:
- *"SEC_CORE_IMG_RELEASE"*
- *"SEC_CORE_IMG_DEBUG"*

The *"-t sec_core_img"* should be used when generating the project in the SDK env package

### Generate core0 project
__CMAKE_BUILD_TYPE__ has no restriction


## Debugging the example

- Download the core0 example to the target and run core0 example first
- Download the core1 example to the target and run the core1 example

*NOTE*
- If users expects to debug the core0 and core1 example step by step, users must ensure the *board_init()* function is executed before debugging the core1 example as some hardware resoruces needs to be initialized by *board_init()* in core0 example.

When the project runs successfully,
- The serial port terminal will output the following information:
    ```shell
        Copying secondary core image to destination memory...
        Starting secondary core...
        Secondary core started, RGB leds are blinking...

        hello world
    ```
- The RGB leds are switching among RED, GREEN, BLUE respectively.

## Running the example
- Download the core0 example to the target, stop debugging and reset the board. When the project runs successfully, the serial port terminal will output the following information:
    ```shell
        Copying secondary core image to destination memory...
        Starting secondary core...
        Secondary core started, RGB leds are blinking...

        hello world
    ```
- The RGB leds are switching among RED, GREEN, BLUE respectively.