# OTP 烧写计划

## 概述

本示例提供了如下功能：
- 向空白芯片写入必须的OTP字段

## 硬件设置

BOOT PIN需设置为 2'b10

## 运行工程

- 用img_util生成可启动镜像，步骤如下：
  - ./img_util.exe -- gen_image <app_binary_path> <bootable_image_file> 0x90 0 0 0

- 利用rom_host加载启动镜像，步骤如下：
  -  ./rom_host.exe -p <COMx> -- load-image <bootable_image_file>

 - Boot ROM会在加载完镜像后自动执行该应用，当烧写OTP的操作完成后，应用会通过串口打印操作结果