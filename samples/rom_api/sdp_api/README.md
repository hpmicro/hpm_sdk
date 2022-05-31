# SDP API Demo
## Overview

The sdp_api shows the results of running the basic ROM SDP APIs.

## Board Setting

No special settings are required

## Running the example

When the example runs successfully, the API test result is displayed in the terminal:

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