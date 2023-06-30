# HRPwm输出
## 概述


**pwm**工程主要演示了如何配置几种不同类型的pwm波形并通过MCU的引脚进行输出。

- 强制输出
- 边沿对齐pwm
- 中心对齐pwms

## 配置


- 一个双通道示波器
- 安装串口终端，查看[板子信息](lab_board_overiew)并配置串口终端参数
- 查看[HRPWM_P0和HRPWM_P2引脚](lab_board_drv_hrpwm_pin)根据板子型号查看具体信息


## 运行现象

- 上电后，通过示波器可依次观测到P0、P1输出高电平、低电平、边沿对齐PWM、中心对齐PWM
- 和参考PWM对比，HRPWM输出脉宽相差`HRPWM_SET_IN_PWM_CLK`个时钟周期
- 串口打印如下信息:

```console
hr pwm example


>> Test force HRPWM output on P0 and P2
Output high
Output low


>> Generate edge aligned waveform
Two waveforms will be generated, HRPWM P0 is the target waveform
whose duty cycle will be updated from 0 - 100; HRPWM P2 is a reference


>> Generate central aligned waveform
Two waveforms will be generated, HRPWM P0 is the target waveform
whose duty cycle will be updated from 0 - 100; HRPWM P2 is a reference
test done

```
```{note}
只依次输出一次PWM波形，如果需要再次观测波形，需要重新运行程序。
```
