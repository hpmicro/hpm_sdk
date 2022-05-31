# 等对称加解密算法AES-CCM
## 概述

**aes_ccm_mode_example**示例工程展示了等对称加解密算法AES-CCM模式的实现。
- 此工程的等对称加解密算法采用AES128加密算法CCM模式加解密RFC 3610，通过8项测试完成模块加解密。

## 硬件设置

- 无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出如下信息：
```
> tc_start() - Performing CCM tests:
> test_vector_1: Performing CCM test #1 (RFC 3610 test vector #1):
> ===================================================================
> PASS - do_test.
> test_vector_2: Performing CCM test #2 (RFC 3610 test vector #2):
> ===================================================================
> PASS - do_test.
> test_vector_3: Performing CCM test #3 (RFC 3610 test vector #3):
> ===================================================================
> PASS - do_test.
> test_vector_4: Performing CCM test #4 (RFC 3610 test vector #7):
> ===================================================================
> PASS - do_test.
> test_vector_5: Performing CCM test #5 (RFC 3610 test vector #8):
> ===================================================================
> PASS - do_test.
> test_vector_6: Performing CCM test #6 (RFC 3610 test vector #9):
> ===================================================================
> PASS - do_test.
> test_vector_7: Performing CCM test #7 (no associated data):
> ===================================================================
> PASS - test_vector_7.
> test_vector_8: Performing CCM test #8 (no payload data):
> ===================================================================
> PASS - test_vector_8.
> All CCM tests succeeded!
> ===================================================================
> PASS - main.
> ===================================================================
> PROJECT EXECUTION SUCCESSFUL
```
