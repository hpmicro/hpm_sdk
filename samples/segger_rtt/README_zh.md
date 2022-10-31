#  Segger RTT
## 概述
Segger Real Time Transfer (RTT) 示例程序。https://wiki.segger.com/RTT
- input_echo
- printf_test
- menu

## 硬件设置
- 使用J-Link（若有板载调试器，需要将其断开，TCK/TMS/TRST）

## 运行现象
- 将开发板上电
- 下载编译好的示例程序
- 正确运行时：
  - 板载LED灯会闪烁
  - RTT输入输出可以直接在Segger Embedded Studio的“Debug Terminal”中查看

## 建议
若使用J-Link RTT Viewer，请注意以下事项（在Segger Embedded Studio内建RTT Viewer对应的配置中已经在工程生成过程中完成配置）:
- 选择正确的device
- 填写正确的RTT control block地址，该地址可以在map文件中找到（符号: \_SEGGER\_RTT）
- 对于menu这个示例程序，将传输速度设置成最高
