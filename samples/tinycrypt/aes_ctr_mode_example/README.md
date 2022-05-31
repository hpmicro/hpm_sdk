# Equal symmetric encryption and decryption algorithm AES-CTR
## Overview

**aes_ctr_mode_example** the example project shows the implementation of aes-ctr mode of equal symmetric encryption and decryption algorithm.
- The equal symmetry encryption and decryption algorithm of this project adopts aes128 encryption algorithm CTR counting mode to complete the module encryption and decryption test.

## Board Setting

- No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
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