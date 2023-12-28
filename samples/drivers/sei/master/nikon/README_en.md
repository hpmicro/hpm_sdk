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
ST:0xfffa5, MT:0x8888, sample_tm:224294997, update_tm:224299900, TimeDelay:306*0.1us
ST:0xfffa6, MT:0x8888, sample_tm:256294997, update_tm:256299900, TimeDelay:306*0.1us
ST:0xfffa7, MT:0x8888, sample_tm:288294997, update_tm:288299900, TimeDelay:306*0.1us
ST:0xfffa8, MT:0x8888, sample_tm:320294997, update_tm:320299900, TimeDelay:306*0.1us
ST:0xfffa9, MT:0x8888, sample_tm:352294997, update_tm:352299900, TimeDelay:306*0.1us
ST:0xfffaa, MT:0x8888, sample_tm:384294997, update_tm:384299900, TimeDelay:306*0.1us
ST:0xfffab, MT:0x8888, sample_tm:416294997, update_tm:416299900, TimeDelay:306*0.1us
ST:0xfffac, MT:0x8888, sample_tm:448294997, update_tm:448299900, TimeDelay:306*0.1us
ST:0xfffad, MT:0x8888, sample_tm:480294997, update_tm:480299900, TimeDelay:306*0.1us
ST:0xfffae, MT:0x8888, sample_tm:512294997, update_tm:512299900, TimeDelay:306*0.1us
ST:0xfffaf, MT:0x8888, sample_tm:544294997, update_tm:544299900, TimeDelay:306*0.1us
ST:0xfffb0, MT:0x8888, sample_tm:576294997, update_tm:576299901, TimeDelay:306*0.1us
ST:0xfffb1, MT:0x8888, sample_tm:608294997, update_tm:608299900, TimeDelay:306*0.1us
ST:0xfffb2, MT:0x8888, sample_tm:640294997, update_tm:640299901, TimeDelay:306*0.1us


```

