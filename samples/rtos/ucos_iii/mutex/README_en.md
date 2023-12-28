# uC/OS-III Mutex

## Overview

The example shows hwo to use uC/OS-III's mutex。

## Board Setting

No special settings are required

## Running the example

There are two tasks. Both of them print message without mutex's protect, and then print message within mutex's protect.
As the message shows, message within mutex's protect should not appear mixedly, but it can also be interrupted by message without mutex's protect.
That is, the statements of Print 2 and Print 4 are mutually exclusive, and the number 4 cannot appear during the process of Print 2, but the number 3 can appear; During the process of printing 4, the number 2 cannot appear, but the number 1 can appear.
When the example runs successfully, following message is displayed in the terminal:
```console
This thread will print 3 without mutex protection, and will print 4 within mutex protection.
This thread will print 1 without mutex protection, and will print 2 within mutex protection.


11111111111111111111111111111



22222222222222222222222222222



11111111111111111111111111111



22222222222222222222222222222



3333333333333333333333333333



11111111111111111111111111111



4444444444444444444444444444





222222222
33333333332222222222223333333333322222222

3333333



11111111111111111111111111111



4444444444444444444444444444





222222222
33333333332222222222223333333333322222222

3333333
```
