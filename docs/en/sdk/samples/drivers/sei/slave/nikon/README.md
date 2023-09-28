# SEI Slave: Simulate NIKON Encoder

## Overview

This demo shows SEI interface simulate NIKON encoder (Product: MAR-MC42AHN00) and generate position data, then print related information by uart console.

## Configration

- Two development boards are required, one as the master and one as the slave.
- Master sample path is "samples/drivers/sei/master/nikon", slave is this example.
- Connect master's SEI pins DATA_P/DATA_N to slave's SEI pins DATA_P/DATA_N.
- Connect master's GND to slave's GND.

## Running the example

- When the example runs successfully, the console shows the following log. ST's value increasing by 1 each sample, sample interval is 200ms.

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
SEI slave nikon sample
Started sei engine!
EAX:0x4, CC:0x0, ST:0xfffa5, MT:0x8888, sample_tm1:7149133, sample_tm2:0, sample_interval:39717 us
EAX:0x4, CC:0x0, ST:0xfffa6, MT:0x8888, sample_tm1:43148906, sample_tm2:7149133, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffa7, MT:0x8888, sample_tm1:79148679, sample_tm2:43148906, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffa8, MT:0x8888, sample_tm1:115148453, sample_tm2:79148679, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffa9, MT:0x8888, sample_tm1:151148227, sample_tm2:115148453, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffaa, MT:0x8888, sample_tm1:187148001, sample_tm2:151148227, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffab, MT:0x8888, sample_tm1:223147775, sample_tm2:187148001, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffac, MT:0x8888, sample_tm1:259147549, sample_tm2:223147775, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffad, MT:0x8888, sample_tm1:295147322, sample_tm2:259147549, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffae, MT:0x8888, sample_tm1:331147096, sample_tm2:295147322, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffaf, MT:0x8888, sample_tm1:367146869, sample_tm2:331147096, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb0, MT:0x8888, sample_tm1:403146643, sample_tm2:367146869, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb1, MT:0x8888, sample_tm1:439146417, sample_tm2:403146643, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb2, MT:0x8888, sample_tm1:475146190, sample_tm2:439146417, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb3, MT:0x8888, sample_tm1:511145964, sample_tm2:475146190, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb4, MT:0x8888, sample_tm1:547145737, sample_tm2:511145964, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb5, MT:0x8888, sample_tm1:583145511, sample_tm2:547145737, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb6, MT:0x8888, sample_tm1:619145285, sample_tm2:583145511, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb7, MT:0x8888, sample_tm1:655145058, sample_tm2:619145285, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb8, MT:0x8888, sample_tm1:691144832, sample_tm2:655145058, sample_interval:199998 us
EAX:0x4, CC:0x0, ST:0xfffb9, MT:0x8888, sample_tm1:727144605, sample_tm2:691144832, sample_interval:199998 us
```

