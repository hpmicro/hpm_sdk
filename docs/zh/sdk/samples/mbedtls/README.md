# Mbedtls示例

## 概述

本示例程序将演示mbedtls加解密接口的使用

## 硬件设置

无

## 运行现象

```console
begin to test mbedtls
- AES ECB encrypt: [F928012BC5D8CDE9101BCF4E51A6B7FB]
- AES ECB decrypt: [ABCDEFGHIJKLMNOP]
[OK] mbedtls_aes_ecb_demo calculation for passed
- AES CBC encrypt: [7649ABAC8119B246CEE98E9B12E9197D]
- AES CBC decrypt: [6BC1BEE22E409F96E93D7E117393172A]
[OK] mbedtls_aes_cbc_demo calculation for passed
- SHA1: [A9993E364706816ABA3E25717850C26C9CD0D89D]
[OK] HASH calculation for abc passed
- SHA256: [BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD]
[OK] HASH calculation for abc passed
- HMAC: 7FD04DF92F636FD450BC841C9418E5825C17F33AD9C87C518115A45971F7F77E
All Test Finished
```