# Algorithm of error detection and correction ECC-DH
## Overview

**ecc_dh_example** the example project shows the implementation of error detection and correction algorithm ECC.
- The error detection and correction algorithm of this project adopts ECC DH to complete the module encryption and decryption test.

## Board Setting

- No special settings

## Running the example

When the project runs correctly, and the serial port terminal will output the following information:
```
> tc_start() - Performing ECC-DH tests:
> Performing cavp_ecdh test:
> Test #1: ECDHNIST-p256
>   Vector #00 check Z - success
>   Vector #01 check Z - success
>   Vector #02 check Z - success
>   Vector #03 check Z - success
>   Vector #04 check Z - success
>   Vector #05 check Z - success
>   Vector #06 check Z - success
>   Vector #07 check Z - success
>   Vector #08 check Z - success
>   Vector #09 check Z - success
>   Vector #10 check Z - success
>   Vector #11 check Z - success
>   Vector #12 check Z - success
>   Vector #13 check Z - success
>   Vector #14 check Z - success
>   Vector #15 check Z - success
>   Vector #16 check Z - success
>   Vector #17 check Z - success
>   Vector #18 check Z - success
>   Vector #19 check Z - success
>   Vector #20 check Z - success
>   Vector #21 check Z - success
>   Vector #22 check Z - success
>   Vector #23 check Z - success
>   Vector #24 check Z - success
> ===================================================================
> PASS - cavp_ecdh.
> Performing cavp_keygen test:
> Test #2: ECC KeyGen NIST-p256
>   Vector #00 check prv   - success
>   Vector #00 check pub.x - success
>   Vector #00 check pub.y - success
>   Vector #01 check prv   - success
>   Vector #01 check pub.x - success
>   Vector #01 check pub.y - success
>   Vector #02 check prv   - success
>   Vector #02 check pub.x - success
>   Vector #02 check pub.y - success
>   Vector #03 check prv   - success
>   Vector #03 check pub.x - success
>   Vector #03 check pub.y - success
>   Vector #04 check prv   - success
>   Vector #04 check pub.x - success
>   Vector #04 check pub.y - success
>   Vector #05 check prv   - success
>   Vector #05 check pub.x - success
>   Vector #05 check pub.y - success
>   Vector #06 check prv   - success
>   Vector #06 check pub.x - success
>   Vector #06 check pub.y - success
>   Vector #07 check prv   - success
>   Vector #07 check pub.x - success
>   Vector #07 check pub.y - success
>   Vector #08 check prv   - success
>   Vector #08 check pub.x - success
>   Vector #08 check pub.y - success
>   Vector #09 check prv   - success
>   Vector #09 check pub.x - success
>   Vector #09 check pub.y - success
> ===================================================================
> PASS - cavp_keygen.
> Performing cavp_pkv test:
> Test #3: PubKeyVerify NIST-p256-SHA2-256
>   Vector #00 check P (0 ) - success (0=0)
>   Vector #01 check F (1 - Q_x or Q_y out of range) - success (1=1)
>   Vector #02 check F (1 - Q_x or Q_y out of range) - success (1=1)
>   Vector #03 check F (2 - Point not on curve) - success (2=2)
>   Vector #04 check F (1 - Q_x or Q_y out of range) - success (1=1)
>   Vector #05 check P (0 ) - success (0=0)
>   Vector #06 check F (2 - Point not on curve) - success (2=2)
>   Vector #07 check P (0 ) - success (0=0)
>   Vector #08 check F (1 - Q_x or Q_y out of range) - success (1=1)
>   Vector #09 check P (0 ) - success (0=0)
>   Vector #10 check F (2 - Point not on curve) - success (2=2)
>   Vector #11 check F (2 - Point not on curve) - success (2=2)
> ===================================================================
> PASS - pkv_vectors.
> Performing montecarlo_ecdh test:
> Test #4: Monte Carlo (10 Randomized EC-DH key-exchange) NIST-p256
>   ..........
> ===================================================================
> PASS - montecarlo_ecdh.
> All EC-DH tests succeeded!
> ===================================================================
> PASS - main.
> ===================================================================
> PROJECT EXECUTION SUCCESSFUL
```