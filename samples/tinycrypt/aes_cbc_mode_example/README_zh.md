# AES CBC 示例
## 概述

aes_ccm_mode_example示例工程展示了等对称加解密算法AES-CCM模式的实现。在这个工程中，采用AES128加密算法CCM模式加解密RFC 3610，通过8项测试完成模块加解密。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：

```
tc_start() - Performing AES128 tests:
Performing CBC tests:
CBC test #1 (encryption SP 800-38a tests):
===================================================================
PASS - test_1_and_2.
CBC test #2 (decryption SP 800-38a tests):
===================================================================
PASS - test_1_and_2.
All CBC tests succeeded!
===================================================================
PASS - main.
===================================================================
PROJECT EXECUTION SUCCESSFUL

```