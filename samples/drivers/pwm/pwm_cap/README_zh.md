# Pwm 捕获
## 概述


**pwm 捕获**工程主要演示了通过PWM CMP的捕获一个中心对称的PWM波形，并通过计算获取PWM的占空比。

- PWM捕获通道
- 中心对齐PWM

## 配置


- 安装串口终端，查看[板子信息](lab_board_overiew)并配置串口终端参数
- [PWM_P0和PWM_P1引脚](lab_board_drv_pwm_pin)根据板子型号查看具体信息
- 连接PWM_P0和PWM_P1引脚


## 运行现象

- 上电后，PWM输出中心对称的PWM，间隔改变占空比
- PWM捕获当前的PWM波形，计算后输出占空比
- 成功捕获到PWM信号，且占空比一致，打印占空比信息，程序返回PASS
- 没有捕获到PWM信号或计算出的占空比和输出占空比不一致返回FAIL
- 串口打印如下信息:

```
pwm capture example


>> Test PWM output on P4, PWM capture P5

Pwm output duty: 0.049995, Pwm cap duty: 0.054994.
Pwm output duty: 0.049995, Pwm cap duty: 0.049995.
Pwm output duty: 0.039996, Pwm cap duty: 0.044996.
Pwm output duty: 0.039996, Pwm cap duty: 0.039996.
Pwm output duty: 0.029997, Pwm cap duty: 0.034996.
Pwm output duty: 0.029997, Pwm cap duty: 0.029997.
Pwm output duty: 0.019998, Pwm cap duty: 0.024998.
Pwm output duty: 0.019998, Pwm cap duty: 0.019998.
Pwm output duty: 0.009999, Pwm cap duty: 0.014998.
Pwm output duty: 0.009999, Pwm cap duty: 0.009999.
PASS.
test done

```
