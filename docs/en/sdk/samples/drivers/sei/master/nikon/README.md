# SEI Master: Connect NIKON Encoder

## Overview

This demo shows SEI interface read NIKON encoder (Product: MAR-MC42AHN00) position data and automatically calculate sampling/update delay time, then print related information by uart console.

## Configration

- Two development boards are required, one as the master and one as the slave.
- Slave sample path is "samples/drivers/sei/slave/nikon", master is this example.
- Connect master's SEI pins DATA_P/DATA_N to slave's SEI pins DATA_P/DATA_N.
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
SEI master nikon sample
Started sei engine!
ST:0xfc1, MT:0x8889, sample_tm:332635, update_tm:337939, TimeDelay:294*0.1us
ST:0xfc2, MT:0x8889, sample_tm:36332635, update_tm:36337939, TimeDelay:294*0.1us
ST:0xfc3, MT:0x8889, sample_tm:72332635, update_tm:72337940, TimeDelay:294*0.1us
ST:0xfc4, MT:0x8889, sample_tm:108332635, update_tm:108337939, TimeDelay:294*0.1us
ST:0xfc5, MT:0x8889, sample_tm:144332635, update_tm:144337939, TimeDelay:294*0.1us
ST:0xfc6, MT:0x8889, sample_tm:180332635, update_tm:180337939, TimeDelay:294*0.1us
ST:0xfc7, MT:0x8889, sample_tm:216332635, update_tm:216337939, TimeDelay:294*0.1us
ST:0xfc8, MT:0x8889, sample_tm:252332635, update_tm:252337940, TimeDelay:294*0.1us
ST:0xfc9, MT:0x8889, sample_tm:288332635, update_tm:288337940, TimeDelay:294*0.1us
ST:0xfca, MT:0x8889, sample_tm:324332635, update_tm:324337939, TimeDelay:294*0.1us
ST:0xfcb, MT:0x8889, sample_tm:360332635, update_tm:360337940, TimeDelay:294*0.1us
ST:0xfcc, MT:0x8889, sample_tm:396332635, update_tm:396337939, TimeDelay:294*0.1us
ST:0xfcd, MT:0x8889, sample_tm:432332635, update_tm:432337939, TimeDelay:294*0.1us
ST:0xfce, MT:0x8889, sample_tm:468332635, update_tm:468337940, TimeDelay:294*0.1us
ST:0xfcf, MT:0x8889, sample_tm:504332635, update_tm:504337940, TimeDelay:294*0.1us
ST:0xfd0, MT:0x8889, sample_tm:540332635, update_tm:540337939, TimeDelay:294*0.1us
ST:0xfd1, MT:0x8889, sample_tm:576332635, update_tm:576337939, TimeDelay:294*0.1us
ST:0xfd2, MT:0x8889, sample_tm:612332635, update_tm:612337939, TimeDelay:294*0.1us
ST:0xfd3, MT:0x8889, sample_tm:648332635, update_tm:648337939, TimeDelay:294*0.1us


```

