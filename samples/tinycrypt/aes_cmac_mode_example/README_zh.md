# 等对称加解密算法AES-CMAC
## 概述

**aes_cmac_mode_example**示例工程展示了等对称加解密算法AES-CMAC模式的实现。
- 此工程的等对称加解密算法采用AES-CMAC模式，通过5项测试完成CMAK模块加解密信息完整性验证。

## 硬件设置

- 无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
```
> tc_start() - Performing CMAC tests:
> Performing CMAC test #1 (GF(2^128) double):
> ===================================================================
> PASS - verify_gf_2_128_double.
> Performing CMAC test #2 (SP 800-38B test vector #1):
> ===================================================================
> PASS - verify_cmac_null_msg.
> Performing CMAC test #3 (SP 800-38B test vector #2):
> ===================================================================
> PASS - verify_cmac_1_block_msg.
> Performing CMAC test #4 (SP 800-38B test vector #3):
> ===================================================================
> PASS - verify_cmac_320_bit_msg.
> Performing CMAC test #5 (SP 800-38B test vector #4)
> ===================================================================
> PASS - verify_cmac_512_bit_msg.
> All CMAC tests succeeded!
> ===================================================================
> PASS - main.
> ===================================================================
> PROJECT EXECUTION SUCCESSFUL
```
