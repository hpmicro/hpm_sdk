# Equal symmetric encryption and decryption algorithm AES-CMAC
## Overview

**aes_cmac_mode_example** the example project shows the implementation of aes-cmac mode of equal symmetric encryption and decryption algorithm.
- The equal symmetry encryption and decryption algorithm of this project adopts aes-cmac mode, and the integrity verification of encryption and decryption information of cmak module is completed through 5 tests.

## Board Setting

- No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
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