# SEI Master: Connect HIPERFACE Encoder

## Overview

This demo shows SEI interface read HIPERFACE encoder position data and automatically calculate sampling/update delay time, then print related information by uart console.

## Configration

- HIPERFACE encoder is SKM36-HFA0-K02.
- Connect master's SEI pins DATA_P/DATA_N to encoder data pins.
- Connect encoder VDD/GND to an external power supply +9V.

## Running the example

- When the example runs successfully, the console shows the following log. Rotate the encoder and the POS value will change.

```console

----------------------------------------------------------------------
$$\   $$\ $$$$$$$\  $$\      $$\ $$\
$$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
$$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
$$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
$$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
$$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
$$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
\__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
----------------------------------------------------------------------
SEI master hiperface sample
Started sei engine!
POS:0xe0531, addr:0x40, sample_tm:2408695, update_tm:3887689, TimeDelay:8216 us
POS:0xe0531, addr:0x40, sample_tm:38405107, update_tm:39884102, TimeDelay:8216 us
POS:0xe0531, addr:0x40, sample_tm:74403305, update_tm:75882301, TimeDelay:8216 us
POS:0xe0531, addr:0x40, sample_tm:110400555, update_tm:111879550, TimeDelay:8216 us
POS:0xe0531, addr:0x40, sample_tm:146399103, update_tm:147878098, TimeDelay:8216 us
POS:0xe0531, addr:0x40, sample_tm:182396922, update_tm:183875916, TimeDelay:8216 us
POS:0xe0531, addr:0x40, sample_tm:218412594, update_tm:219891590, TimeDelay:8216 us
POS:0xe0532, addr:0x40, sample_tm:254412789, update_tm:255891784, TimeDelay:8216 us
POS:0xe0532, addr:0x40, sample_tm:290411717, update_tm:291890712, TimeDelay:8216 us
POS:0xe0532, addr:0x40, sample_tm:326410428, update_tm:327889424, TimeDelay:8216 us
POS:0xe0642, addr:0x40, sample_tm:362409265, update_tm:363888261, TimeDelay:8216 us
POS:0xe063d, addr:0x40, sample_tm:398410889, update_tm:399889885, TimeDelay:8216 us
POS:0xe05d9, addr:0x40, sample_tm:434409464, update_tm:435888460, TimeDelay:8216 us
POS:0xe0622, addr:0x40, sample_tm:470407089, update_tm:471886086, TimeDelay:8216 us
POS:0xe0628, addr:0x40, sample_tm:506406550, update_tm:507885546, TimeDelay:8216 us
POS:0xe0637, addr:0x40, sample_tm:542406014, update_tm:543885011, TimeDelay:8216 us
POS:0xe0601, addr:0x40, sample_tm:578405749, update_tm:579884746, TimeDelay:8216 us
POS:0xe05d5, addr:0x40, sample_tm:614403168, update_tm:615882165, TimeDelay:8216 us
POS:0xe0349, addr:0x40, sample_tm:650405205, update_tm:651884202, TimeDelay:8216 us
POS:0xe049a, addr:0x40, sample_tm:686404461, update_tm:687883458, TimeDelay:8216 us
POS:0xe0ac7, addr:0x40, sample_tm:722406505, update_tm:723885502, TimeDelay:8216 us
POS:0xe1187, addr:0x40, sample_tm:758406768, update_tm:759885765, TimeDelay:8216 us
POS:0xe12dc, addr:0x40, sample_tm:794407035, update_tm:795886032, TimeDelay:8216 us


```
