# SEI Master: Connect ENDAT Encoder

## Overview

This demo shows SEI interface read Endat encoder position data and automatically calculate sampling/update delay time, then print related information by uart console.

## Configration

- Two development boards are required, one as the master and one as the slave.
- Slave sample path is "samples/drivers/sei/slave/endat", master is this example.
- Master's SEI_CLK pins should bridging master side，and slave's SEI_CLK pins should bridging slave side.
- Connect master's SEI pins DATA_P/DATA_N to slave's SEI pins DATA_P/DATA_N.
- Connect master's SEI pins CLKO_P/CLKO_N to slave's SEI pins CLKI_P/CLKI_N.
- Connect master's GND to slave's GND.

## Running the example

- When the example runs successfully, the console shows the following log. ST's value increasing by 1 each sample.

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
SEI master EnDat sample
Started sei engine!
E:0, ST:0xa5a5, CRC:0x5, sample_tm:2465400001, update_tm:2465409502, TimeDelay:475*0.1us
E:0, ST:0xa5a6, CRC:0xf, sample_tm:2505400001, update_tm:2505409502, TimeDelay:475*0.1us
E:0, ST:0xa5a7, CRC:0x1a, sample_tm:2545400001, update_tm:2545409502, TimeDelay:475*0.1us
E:0, ST:0xa5a8, CRC:0x7, sample_tm:2585400001, update_tm:2585409502, TimeDelay:475*0.1us
E:0, ST:0xa5a9, CRC:0x12, sample_tm:2625400001, update_tm:2625409502, TimeDelay:475*0.1us
E:0, ST:0xa5aa, CRC:0x18, sample_tm:2665400001, update_tm:2665409502, TimeDelay:475*0.1us


```

