# SEI Master: Connect TAMAGAWA Encoder

## Overview

This demo shows SEI interface read TAMAGAWA encoder position data and automatically calculate sampling/update delay time, then print related information by uart console.

## Configration

- HIPERFACE encoder is TS5700N8401.
- Connect master's SEI pins DATA_P/DATA_N to encoder data pins.
- Connect encoder VDD/GND to board +5V/GND.

## Running the example

- When the example runs successfully, the console shows the following log. Rotate the encoder and the ST and MT value will change.

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
SEI master tamagawa sample
Started sei engine!
MT:0x1, ST:0x6f8213, ALMC:0xc1, CRC:0x4b4b4bc8, sample_tm:281, update_tm:8696, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8212, ALMC:0xc1, CRC:0xcbcbcb48, sample_tm:36000281, update_tm:36008704, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8212, ALMC:0xc1, CRC:0xcbcbcb48, sample_tm:72000281, update_tm:72008695, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8210, ALMC:0xc1, CRC:0x8b8b8b08, sample_tm:108000281, update_tm:108008703, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8210, ALMC:0xc1, CRC:0x8b8b8b08, sample_tm:144000281, update_tm:144008693, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8212, ALMC:0xc1, CRC:0xcbcbcb48, sample_tm:180000281, update_tm:180008699, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8210, ALMC:0xc1, CRC:0x8b8b8b08, sample_tm:216000281, update_tm:216008689, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8213, ALMC:0xc1, CRC:0x4b4b4bc8, sample_tm:252000281, update_tm:252008696, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8210, ALMC:0xc1, CRC:0x8b8b8b08, sample_tm:288000281, update_tm:288008706, TimeDelay:468*0.1us
MT:0x1, ST:0x6f8212, ALMC:0xc1, CRC:0xcbcbcb48, sample_tm:324000281, update_tm:324008694, TimeDelay:467*0.1us
MT:0x1, ST:0x6f8212, ALMC:0xc1, CRC:0xcbcbcb48, sample_tm:360000281, update_tm:360008701, TimeDelay:467*0.1us
MT:0x1, ST:0x6f88c6, ALMC:0xc1, CRC:0xb0b0b033, sample_tm:396000281, update_tm:396008693, TimeDelay:467*0.1us
MT:0x1, ST:0x6ad736, ALMC:0xc1, CRC:0xe5e5e566, sample_tm:432000281, update_tm:432008701, TimeDelay:467*0.1us
MT:0x1, ST:0x625ad5, ALMC:0xc1, CRC:0x83838300, sample_tm:468000281, update_tm:468008706, TimeDelay:468*0.1us
MT:0x1, ST:0x4ca5e0, ALMC:0xc1, CRC:0xa4a4a427, sample_tm:504000281, update_tm:504008696, TimeDelay:467*0.1us
MT:0x1, ST:0x2aa3e5, ALMC:0xc1, CRC:0x2020281, sample_tm:540000281, update_tm:540008703, TimeDelay:467*0.1us
MT:0x0, ST:0x7ba2cd, ALMC:0xc1, CRC:0x9c9c9c1f, sample_tm:576000281, update_tm:576008693, TimeDelay:467*0.1us
MT:0x0, ST:0x48e44a, ALMC:0xc9, CRC:0xd3d3d340, sample_tm:612000281, update_tm:612008702, TimeDelay:467*0.1us
MT:0x0, ST:0x349661, ALMC:0xc9, CRC:0x777777e4, sample_tm:648000281, update_tm:648008690, TimeDelay:467*0.1us
MT:0x0, ST:0x2ec52c, ALMC:0xc9, CRC:0x575757c4, sample_tm:684000281, update_tm:684008698, TimeDelay:467*0.1us
MT:0x0, ST:0x261465, ALMC:0xc9, CRC:0x5e5e5ecd, sample_tm:720000281, update_tm:720008706, TimeDelay:468*0.1us
MT:0x0, ST:0x218a5, ALMC:0xc9, CRC:0x494949da, sample_tm:756000281, update_tm:756008696, TimeDelay:467*0.1us
MT:0xffff, ST:0x4ee4df, ALMC:0xc9, CRC:0xe51a1a89, sample_tm:792000281, update_tm:792008704, TimeDelay:467*0.1us
MT:0xffff, ST:0x3078b3, ALMC:0xc9, CRC:0x946b6bf8, sample_tm:828000281, update_tm:828008693, TimeDelay:467*0.1us
MT:0xffff, ST:0x1d8d8d, ALMC:0xc9, CRC:0xf30c0c9f, sample_tm:864000281, update_tm:864008699, TimeDelay:467*0.1us
MT:0xffff, ST:0x1d8d93, ALMC:0xc9, CRC:0x8b7474e7, sample_tm:900000281, update_tm:900008705, TimeDelay:468*0.1us
MT:0xffff, ST:0x1d8d97, ALMC:0xc9, CRC:0xab5454c7, sample_tm:936000281, update_tm:936008694, TimeDelay:467*0.1us


```


