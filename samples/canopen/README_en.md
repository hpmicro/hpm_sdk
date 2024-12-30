# CANOPEN

## Overview

The canopen_demo provides the following features:

- HeartBeat test
- NMT test
- SDO test
- PDO test
- SYNC test

## Board Setting

Connect 2 boards

## Running the example

When the example runs successfully, the board shows the following:

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
*********************************************************************************
*                                                                               *
*                         CANOPEN Example Menu                                  *
*                                                                               *
* 0 - Master mode                                                               *
* 1 - Slave mode                                                                *
*                                                                               *
*********************************************************************************

```

### Test steps

users connect two boards and run this demo, when two boards all show select menu, then one choose slave, another choose master.

Slave board show as：
```console

rxSize = 11, txSize = 8
excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
enter slave mode ~ ~

```

Master board show as：
```console

enter master mode and find slave device ~~
rxSize = 11, txSize = 8
excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
find device, id:70a, status preoptional
slave enter to optional mode
PDO test successfully!
slave HBtime: 1000 ms
slave HBtime: 2000 ms
SDO test successfully!
canopen test done

```
