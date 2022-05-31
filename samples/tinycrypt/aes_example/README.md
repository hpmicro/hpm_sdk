# Equal symmetric encryption and decryption algorithm AES
## Overview

**aes_example** the example project shows the implementation of AES mode of equal symmetric encryption and decryption algorithm.
- The equal symmetry encryption and decryption algorithm of this project adopts aes128 NIST mode to complete the module encryption and decryption test.

## Board Setting

- No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
> tc_start() - Performing AES128 tests:
> AES128 test_1 (NIST key schedule test):
> ===================================================================
> PASS - test_1.
> AES128 test_2 (NIST encryption test):
> ===================================================================
> PASS - test_2.
> AES128 test_3 (NIST fixed-key and variable-text):
> ===================================================================
> PASS - test_3.
> AES128 test #4 (NIST variable-key and fixed-text):
> ===================================================================
> PASS - test_4.
> All AES128 tests succeeded!
> ===================================================================
> PASS - main.
> ===================================================================
> PROJECT EXECUTION SUCCESSFUL
```