# Hello 示例

## 概述

多核示例工程在Core0上运行"hello word"示例，在core1上运行"rgb_led"示例。

在本工程中:
 - 串口输出 "hello world"; 键盘手动输入字符串信息，通过串口打印出来
 - RGB LED会在红、绿、蓝三色中依次切换

## 硬件设置

  BOOT_PIN 应该设置为：0-OFF, 1-OFF

## 生成、编译和调试多核工程

请参见[Multicore通用说明.md](../README_zh.md)

## 执行结果
- 以HPM6750EVKMINI为例，当双核示例正常运行的时候，会打印如下信息：
    ```console
        Copying secondary core image to destination memory: 0x1180000
        Hello world for multicore! Secondary core control RGB leds blinking...
    ```
- RGB LED会在红、绿、蓝三色中依次切换
