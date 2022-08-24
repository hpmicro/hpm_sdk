# Hello 示例
## 概述

多核示例工程在Core0上运行"hello word"示例，在core1上运行"rgb_led"示例。

在本工程中:
 - 串口输出 "hello world"; 键盘手动输入字符串信息，通过串口打印出来
 - RGB LED会在红、绿、蓝三色中依次切换

## 硬件设置

  BOOT_PIN 应该设置为：0-OFF, 1-OFF


## 生成和编译多核工程

本示例中：core0示例在FLASH中原地执行， core1工程在ILM里执行。

用户须先生成 __Core0__ 工程，__core1__ 工程作为关联工程会被自动生成

用户必须在编译完core1工程后再编译__Core0__工程

### 生成core0工程
__CMAKE_BUILD_TYPE__ 必须是 下列选项中的一种:
- *"debug"*
- *"release"*



## 调试示例

- 下载core0示例到设备并运行
- 下载core1示例到设备并运行

*注意*
- 如果用户希望单步调试core0和core1的示例，用户需确保在core0示例中的board_init()执行完后再调试core1示例。因为core1示例使用了需要core0 board_init()函数初始化的硬件资源
当core示例正常运行的时候,
- 串口终端会打印如下信息:
    ```shell
        Copying secondary core image to destination memory...
        Starting secondary core...
        Secondary core started, RGB leds are blinking...

        hello world
    ```
- RGB LED会在红、绿、蓝三色中依次切换

## 运行示例
- 下载core0示例到设备，然后断开调试，按板子上的reset键。当双核示例正常支持的时候，会打印如下信息：
- 串口终端会打印如下信息:
    ```shell
        Copying secondary core image to destination memory...
        Starting secondary core...
        Secondary core started, RGB leds are blinking...

        hello world
    ```
- RGB LED会在红、绿、蓝三色中依次切换