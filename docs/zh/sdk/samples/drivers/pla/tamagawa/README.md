# 多摩川编码器通讯
## 概述

演示了使用PLA、DMA、SPI等外设和多摩川编码器进行通讯

- 获取不同DataID对应的数据
- 读写EEPROM
- 错误信息（CRC错误，通讯错误，编码器错误信息）
- 通讯延时补偿

## 硬件设置

- 扩展电路板`TMAGAWA RevA`对插到核心板
- 连接编码器`TS5700N8401`

## 运行现象

当工程正确运行后,串口终端会输出如下信息,转动编码器轴，位置信息会改变：
```console

cmd:3 time:638, abs pos:4170741, abm pos:0, id:23.
cmd:3 time:631, abs pos:4170741, abm pos:0, id:23.
cmd:3 time:631, abs pos:4170740, abm pos:0, id:23.
cmd:3 time:634, abs pos:4170740, abm pos:0, id:23.
cmd:3 time:639, abs pos:4170741, abm pos:0, id:23.
cmd:3 time:632, abs pos:4170741, abm pos:0, id:23.
cmd:3 time:630, abs pos:4170742, abm pos:0, id:23.

```

:::{note}

- `tmgw_message_process`函数调用后，会更新`tmgw_msg`的数据，`tmgw_msg`包含上述功能的信息
- 动态切换命令，需要修改`tmgw_dev.comm_time_delay_config`来关闭通讯功能，然后重新进行PLA DMA的配置，否则会产生一次的数据非法指令，然后恢复正常

:::
