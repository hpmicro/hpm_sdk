# 国密 API demo

## 概述

gm_api示例工程展示了运行ROM GM (国密) API测试程序的情况。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出GM API的测试结果：
```shell
sm4_api_ecb_demo, start...
SM4 ECB encryption start...
SM4 ECB encryption test passed!
SM4 ECB decryption start...
SM4 ECB decryption test passed!
SM4 ECB demo execution passed
sm4_api_cbc_demo, start...
SM4 CBC encryption start...
SM4 CBC encryption test passed!
SM4 CBC decryption start...
SM4 CBC decryption test passed!
SM4 CBC demo execution passed
sm3_api_demo(), start...
test1 started
test1 finished
test1 SM3 calculation passed
test2 started
test2 finished
test2 SM3 calculation passed
SM3 demo execution passed

```