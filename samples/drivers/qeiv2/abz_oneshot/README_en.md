# QEIV2 ABZ Encoder Oneshot Mode

## Overview

**Qeiv2 ABZ Encoder Oneshot Mode** project demonstrates configuring the ABZ encoder and obtaining its data using oneshot mode, then printing the data through a serial console.

## Configurations

- Install the serial terminal, view [board information](lab_board_overiew), and configure the serial terminal parameters
- This program uses the **BLM57050-1000** brushless motor of "Leisai Intelligence", refer to the [Leisai Intelligence](https://leisai.com/) website for the specific parameters of the motor.
- Click [DRV-LV50A-MP1907 Motor Driver Board](lab_drv_lv50a_mp1907) section and configure

## Running the Demo

- Power on, open the serial terminal, log as following:

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
qeiv2 abz encoder example
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4
z: 0x64, phase: 0x1f4

```
- Rotating the motor will trigger an interrupt in the encoder's calculation of speed, then printing the following information.

```console

cycle0 --- pulse_snap0: 0xffffffff, cylce_snap0:        0x0, spd : 0 deg/s
cycle0 --- pulse_snap1:        0x0, cylce_snap1:        0x0, spd : 0 deg/s
cycle1 --- pulse_snap0: 0xffffffff, cylce_snap0:        0x0, spd : 0 deg/s
cycle1 --- pulse_snap1:        0x0, cylce_snap1:        0x0, spd : 0 deg/s
pulse1 --- cycle_snap0: 0x6cf42561, spd: 5 deg/s,
pulse1 --- cycle_snap1: 0x1a119d, spd: 5 deg/s,
pulse0 --- cycle_snap0: 0x160b51b, spd: 5 deg/s,
pulse0 --- cycle_snap1: 0x25bfd0, spd: 5 deg/s,

```
