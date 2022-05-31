# Pwm
## 概述


**pwm**工程主要演示了如何配置几种不同类型的pwm波形并通过MCU的引脚进行输出。

- 强制输出
- 边沿对齐pwm
- 中心对齐pwm
- 中心对齐互补pwm

## 配置


- 一个双通道示波器
- 安装串口终端，查看[板子信息](lab_board_overiew)并配置串口终端参数
- 电机使用的是雷赛智能 **BLM57050-1000** 无刷电机，电机具体参数请参考[雷赛智能官网](https://leisai.com/)。
- 点击查看[DRV-LV50A-MP1907电机驱动板](lab_drv_lv50a_mp1907)章节并进行配置
- [PWM_P0和PWM_P1引脚](lab_board_drv_pwm_pin)根据板子型号查看具体信息


## 运行现象

- 上电后，通过示波器可依次观测到P0、P1输出高电平、低电平、边沿对齐PWM、中心对齐PWM、中心对齐互补PWM
- 串口打印如下信息:

```
pwm example


>> Test force PWM output on P0 and P1
Output high
Output low

>> Generate edge aligned waveform
Two waveforms will be generated, PWM P0 is the target waveform
whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

>> Generate central aligned waveform
Two waveforms will be generated, PWM P0 is the target waveform
whose duty cycle will be updated from 0 - 100 and back to 0; PWM P1 is a reference

>> Generate central aligned waveform in pair
Two waveforms will be generated in pair, PWM P0 and P1 are target
waveforms whose duty cycle will be updated from 0 - 100 and back to 0
test done

```
```{note}
只依次输出一次PWM波形，如果需要再次观测波形，需要重新运行程序。
```
