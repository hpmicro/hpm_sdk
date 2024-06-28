# LOBS

## 概述

LOBS示例工程展示LOBS实现的Trace功能。

本示例中，Trace Buffer设置可以记录1024笔数据，每笔数据16字节。设计为：触发点之前抓64笔数据，触发点之后抓（1024-64）=960 笔数据。触发信号引脚参见board文件夹下的readme，高电平触发。

用户可在CMakeLists.txt文件中，修改宏定义 LOBS_USE_TWO_GROUP 选择group工作模式。

## 硬件设置

无特殊设置

## 运行现象

- 将触发信号引脚接GND低电平。
- 将程序下载至开发板运行。
- 将触发信号引脚接高电平，完成触发后，串口将打印 `trace finish`。

备注： Trace数据存在AHB RAM中，用户可通过调试器查看memory的方式查看。

```console
lobs example

lobs trace finish!
trace memory base addr: 0xf0200000, trace data final addr: 0xf020150c
```
