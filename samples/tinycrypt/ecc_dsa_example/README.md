# Algorithm of error detection and correction ECC-DSA
## Overview

**ecc_dsa_example** the example project shows the implementation of error detection and correction algorithm ECC.
- The error detection and correction algorithm of this project adopts ECC DSA to complete the module encryption and decryption test.

## Board Setting

- No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
> tc_start() - Performing ECC-DSA tests:
> Performing cavp_sign test:
> Test #1: ECDSAsign NIST-p256, SHA2-256
>   Vector #00 check sig.r - success
>   Vector #00 check sig.s - success
>   Vector #01 check sig.r - success
>   Vector #01 check sig.s - success
>   Vector #02 check sig.r - success
>   Vector #02 check sig.s - success
>   Vector #03 check sig.r - success
>   Vector #03 check sig.s - success
>   Vector #04 check sig.r - success
>   Vector #04 check sig.s - success
>   Vector #05 check sig.r - success
>   Vector #05 check sig.s - success
>   Vector #06 check sig.r - success
>   Vector #06 check sig.s - success
>   Vector #07 check sig.r - success
>   Vector #07 check sig.s - success
>   Vector #08 check sig.r - success
>   Vector #08 check sig.s - success
>   Vector #09 check sig.r - success
>   Vector #09 check sig.s - success
>   Vector #10 check sig.r - success
>   Vector #10 check sig.s - success
>   Vector #11 check sig.r - success
>   Vector #11 check sig.s - success
>   Vector #12 check sig.r - success
>   Vector #12 check sig.s - success
>   Vector #13 check sig.r - success
>   Vector #13 check sig.s - success
>   Vector #14 check sig.r - success
>   Vector #14 check sig.s - success
> ===================================================================
> PASS - sign_vectors.
> Performing cavp_verify test:
> Test #2: ECDSAvrfy NIST-p256, SHA2-256
>   Vector #00 check F (3 - S changed) - success (3=3)
>   Vector #01 check F (2 - R changed) - success (2=2)
>   Vector #02 check F (4 - Q changed) - success (4=4)
>   Vector #03 check P (0 ) - success (0=0)
>   Vector #04 check P (0 ) - success (0=0)
>   Vector #05 check F (2 - R changed) - success (2=2)
>   Vector #06 check F (4 - Q changed) - success (4=4)
>   Vector #07 check F (1 - Message changed) - success (1=1)
>   Vector #08 check F (3 - S changed) - success (3=3)
>   Vector #09 check F (2 - R changed) - success (2=2)
>   Vector #10 check F (3 - S changed) - success (3=3)
>   Vector #11 check F (1 - Message changed) - success (1=1)
>   Vector #12 check F (4 - Q changed) - success (4=4)
>   Vector #13 check F (1 - Message changed) - success (1=1)
>   Vector #14 check P (0 ) - success (0=0)
> ===================================================================
> PASS - vrfy_vectors.
> Performing montecarlo_signverify test:
> Test #3: Monte Carlo (10 Randomized EC-DSA signatures) NIST-p256, SHA2-256
>   ....................
>
> All ECC-DSA tests succeeded.
> ===================================================================
> PASS - main.
> ===================================================================
> PROJECT EXECUTION SUCCESSFUL
```