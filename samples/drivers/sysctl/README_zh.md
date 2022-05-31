# 系统控制模块
## 概述
***
**sysctl**系统控制模块驱动示例工程展示了系统电源域的模块管理、资源节点的开启以及时钟源的配置、低功耗管理等
- 示例代码中，首先将CPU0外部资源节点开关使能，并分别配置**uart时钟源**、**ana时钟源**、**adc时钟源**
- 分别测试在**CPU0-LOCK寄存器**在锁定和非锁定情况下，写入**CPU0-GPR寄存器**的状态，测试低功耗使能配置
- 进行系统复位操作

## 硬件设置
***
-  无特殊设置

## 运行现象
***
当工程正确运行后，串口终端会输出如下信息：
```
> sysctl example
> test enable resource
> test clock config
> config uart2 to pll3_clk0 (@77529088 Hz) divided by 130: 4726272 Hz
> test clock adc/i2s config
> config ana2 clock source to pll1_clk1
> config adc2 to ana2_clk
> access_cpu_gpr: GPR lock disabled
> write data to gpr @0, 3
> [expected]: Write CPU GPR without lock done
> access_cpu_gpr: GPR lock enabled
> write data to gpr @0, 3
> [expected]: Write CPU GPR with lock done
> access_cpu_gpr: GPR lock enabled
> write data to gpr @0, 3
> result: 0xabd 0x31df 0x237c
> [expected]: Failed to write CPU GPR with lock
> access_cpu_gpr: GPR lock disabled
> write data to gpr @3, 4
> [expected]: Write CPU GPR without lock done
> reset_soc
```
如果Segger Embedded Studio工程构建类型采用flash_xip或flash_sdram_xip时，则会追加打印如下信息：
```
> Software reset has occurred
```


