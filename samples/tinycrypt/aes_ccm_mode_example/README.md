# Equal symmetric encryption and decryption algorithm AES-CCM
## Overview
***
**aes_ccm_mode_example** the example project shows the implementation of aes-ccm mode of equal symmetric encryption and decryption algorithm.
- The equal symmetry encryption and decryption algorithm of this project adopts aes128 encryption algorithm CCM mode to encrypt and decrypt RFC 3610, and the module encryption and decryption is completed through 8 tests.

## Board Setting
***
- No special settings

## Running the example
***
When the project runs correctly, and the serial port terminal will output the following information:
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