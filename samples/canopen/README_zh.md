# CANOPEN

## 概述

canopen_demo 例程提供如下功能：

- HeartBeat 心跳测试
- NMT 测试
- SDO 测试
- PDO 测试
- SYNC 测试

## 硬件设置

将两块开发板连接

## 运行现象

当该例程成功运行后，会在串口终端打印如下:

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

### 测试

用户连接两块开发板，并在两个开发板上同时运行该例程。必须等两块板子都出现选择菜单后，一块选择 slave，一块选择 master。

Slave 端打印如下所示：
```console

rxSize = 11, txSize = 8
excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
enter slave mode ~ ~

```

Master 端打印如下所示：
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


