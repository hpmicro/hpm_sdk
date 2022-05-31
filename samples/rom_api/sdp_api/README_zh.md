# SDP API demo

## 概述

sdp_api示例工程展示了运行ROM SDP API测试程序的情况。

## 硬件设置

无特殊设置

## 运行现象

当工程正确运行后，串口终端会输出SDP API的测试结果：
```shell
SDP API demo
This demo will test the Crypto algorithms, HASH algorithms and the memory operations
rom_sdp_aes_crypt_ecb, 128-bit key test.
rom_sdp_aes_crypt_ecb encryption test passed
rom_sdp_aes_crypt_ecb decryption test passed
rom_sdp_aes_crypt_ecb, 256-bit key test.
rom_sdp_aes_crypt_ecb() encryption completed
rom_sdp_aes_crypt_ecb encryption test passed
rom_sdp_aes_crypt_ecb decryption test passed
SDP AES ECB demo execution passed
AES-128 CBC test...
sdp_aes_crypt_cbc() encryption passed
sdp_aes_crypt_cbc() decryption passed
AES-256 CBC test...
sdp_aes_crypt_cbc() encryption passed
sdp_aes_crypt_cbc() decryption passed
SDP AES CBC demo execution passed
HASH calculation for abc passed
HASH calculation for abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq passed
HASH calculation for 1000 bytes passed
SDP SHA256 demo execution passed
SDP SHA1 demo execution passed
SDP CRC32 demo execution passed
sdp_api_memcpy_demo, desc_addr=1140184, src_addr=1043fe8, dst_addr=1043fd8
sdp_memcpy succeeded
SDP MEMCPY demo execution passed
sdp_api_memset_demo, desc_addr=1140184, dst_addr=1043fdc
sdp_memset succeeded
SDP MEMSET demo execution passed
```