# SEI Slave: Simulate HIPERFACE Encoder

## Overview

- This demo shows SEI interface simulate Hiperface encoder and generate position data, then print related information by uart console.
- The simulated Hiperface encoder is SKM36-HFA0-K02。

## Configuration

- Prepare a USB to 485 module
- Connect SEI pins DATA_P/DATA_N to 485 module pins A/B

## Running the example

- Download and run the program in the development board.
- Use PC serial debugging assistant to communicate with the development board, obtain simulated encoder data.
- Set PC serial debugging assistant: baudrate-9600, startbits-1, databits-8, stopbits-1, paritybits-even. Then send data: 0x40 0x42 0x02, the development board will response.
![sei_slave_hiperface.png](doc/sei_slave_hiperface.png)
- Meanwhile, the console shows the following log. POS's value increasing by 1 each sample.

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
SEI slave hiperface sample
Started sei engine!
ADDR:0x40, CMD:0x42, POS:0xa5a5, sample_tm1:1952301066, sample_tm2:0, sample_interval:10846117 us
ADDR:0x40, CMD:0x42, POS:0xa5a6, sample_tm1:2088895956, sample_tm2:1952301066, sample_interval:758860 us
ADDR:0x40, CMD:0x42, POS:0xa5a7, sample_tm1:2263472347, sample_tm2:2088895956, sample_interval:969868 us
ADDR:0x40, CMD:0x42, POS:0xa5a8, sample_tm1:2294733614, sample_tm2:2263472347, sample_interval:173673 us
ADDR:0x40, CMD:0x42, POS:0xa5a9, sample_tm1:2341120092, sample_tm2:2294733614, sample_interval:257702 us
ADDR:0x40, CMD:0x42, POS:0xa5aa, sample_tm1:2378596173, sample_tm2:2341120092, sample_interval:208200 us
ADDR:0x40, CMD:0x42, POS:0xa5ab, sample_tm1:2412964847, sample_tm2:2378596173, sample_interval:190937 us
ADDR:0x40, CMD:0x42, POS:0xa5ac, sample_tm1:2443402462, sample_tm2:2412964847, sample_interval:169097 us
ADDR:0x40, CMD:0x42, POS:0xa5ad, sample_tm1:2475131711, sample_tm2:2443402462, sample_interval:176273 us
ADDR:0x40, CMD:0x42, POS:0xa5ae, sample_tm1:2507853084, sample_tm2:2475131711, sample_interval:181785 us
ADDR:0x40, CMD:0x42, POS:0xa5af, sample_tm1:2537055225, sample_tm2:2507853084, sample_interval:162234 us
ADDR:0x40, CMD:0x42, POS:0xa5b0, sample_tm1:2562569660, sample_tm2:2537055225, sample_interval:141746 us
ADDR:0x40, CMD:0x42, POS:0xa5b1, sample_tm1:2592295941, sample_tm2:2562569660, sample_interval:165146 us

```
