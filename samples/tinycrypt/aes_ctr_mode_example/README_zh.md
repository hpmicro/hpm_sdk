# 等对称加解密算法AES-CTR
## 概述

**aes_ctr_mode_example**示例工程展示了等对称加解密算法AES-CTR模式的实现。
- 此工程的等对称加解密算法采用AES128加密算法CTR计数模式完成模块加解密测试。

## 硬件设置

- 无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
```
> tc_start() - Performing AES128-CTR mode tests:
> Performing CTR tests:
> CTR test #1 (encryption SP 800-38a tests):
> ===================================================================
> PASS - test_1_and_2.
> CTR test #2 (decryption SP 800-38a tests):
> ===================================================================
> PASS - test_1_and_2.
> All CTR tests succeeded!
> ===================================================================
> PASS - main.
> ===================================================================
> PROJECT EXECUTION SUCCESSFUL
```
