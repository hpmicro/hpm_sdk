# AES CBC example
## Overview

aes_ccm_mode_example the example project shows the implementation of aes-ccm mode of equal symmetric encryption and decryption algorithm. In this project, aes128 encryption algorithm CCM mode is used to encrypt and decrypt RFC 3610, and the module encryption and decryption is completed through 8 tests.

## Board Setting

No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
tc_start() - Performing AES128 tests:
Performing CBC tests:
CBC test #1 (encryption SP 800-38a tests):
===================================================================
PASS - test_1_and_2.
CBC test #2 (decryption SP 800-38a tests):
===================================================================
PASS - test_1_and_2.
All CBC tests succeeded!
===================================================================
PASS - main.
===================================================================
PROJECT EXECUTION SUCCESSFUL

```