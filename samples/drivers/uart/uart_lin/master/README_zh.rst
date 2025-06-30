.. _uart_lin_master:

UART LIN主机示例
================

概述
----
本示例展示了如何使用UART外设模拟LIN主机功能。主要演示：

- LIN主机轮询方式发送和接收数据
- LIN主机中断方式发送和接收数据
- LIN增强型校验和功能

工作原理
--------
本示例实现了以下功能：

1. 轮询方式：

   - 发送LIN帧（ID: 0x31，数据：0x0-0x7）
   - 接收LIN帧（ID: 0x30，预期数据：0x7-0x0）

2. 中断方式：

   - 发送LIN帧（ID: 0x31，数据：0x0-0x7）
   - 接收LIN帧（ID: 0x30，预期数据：0x7-0x0）

工作流程
--------
1. 初始化流程：

   a) UART初始化：

      - 配置波特率
      - 配置RX FIFO触发等级，设置RX FIFO大于一整个数据帧的长度
      - 配置帧格式为8位数据位，无校验位

2. 轮询方式工作流程：

   a) 发送LIN帧：

      - 调用hpm_uart_lin_master_send_frame发送完整LIN帧
      - 函数内部完成break、sync、PID、数据和校验和的发送
      - 等待发送完成后返回

   b) 接收LIN帧：

      - 调用hpm_uart_lin_master_receive_frame接收LIN帧
      - 函数首先发送break、sync和PID
      - 然后等待接收数据和校验和
      - 校验成功后返回

3. 中断方式工作流程：

   a) 发送LIN帧：

      - 调用hpm_uart_lin_master_send_head发送帧头（break、sync、PID）
      - 调用hpm_uart_lin_master_send_data发送数据和校验和
      - 使能TX FIFO中断
      - 在中断中检测发送完成(uart_intr_tx_slot_avail中断发生时尚有最后一个字节数据在发送FIFO中， 可以添加状态查询或者依据版本使用新特性TXIDLE中断)

   b) 接收LIN帧：

      - 调用hpm_uart_lin_master_send_head发送帧头
      - 使能RX超时中断
      - 在中断中调用hpm_uart_lin_master_receive_data接收和校验数据

4. 中断处理流程：

   a) TX FIFO中断（uart_intr_tx_slot_avail）：

      - 检测TX FIFO可用
      - 设置发送完成标志
      - 关闭TX FIFO中断
      - 注意：此时最后一个字节可能仍在发送，需要额外状态检查

   b) RX超时中断（uart_intr_rx_timeout）：

      - 检测接收超时
      - 读取并校验接收数据
      - 设置接收完成标志
      - 关闭RX超时中断
      - 注意：RX timeout方式需要数据帧之间间隔大于等于4个UART字节的传输时间

使用说明
--------

硬件要求
~~~~~~~~
- 开发板一块
- USB转LIN调试器一个
- LIN收发器（如果开发板未集成）

硬件设置
~~~~~~~~
根据开发板类型选择相应的连接方式：

1. 带板载LIN收发器的开发板：

   - 将USB转LIN调试器的LIN信号线连接到开发板的LIN信号引脚
   - 连接GND

2. 不带板载LIN收发器的开发板：

   - 将开发板的UART TX和RX引脚连接到LIN收发器的MCU端
   - 将USB转LIN调试器连接到LIN收发器的LIN总线端
   - 连接GND

具体引脚连接请参考开发板 :ref:`引脚描述 <board_resource>` 部分。

运行步骤
~~~~~~~~
1. 配置USB转LIN调试器：

   - 选择正确的串口和波特率（19200）
   - 点击"设置到调试器"按钮

   .. image:: ../../../lin/doc/lin_debugger_configuration.png

2. 配置从机发送功能：

   - 设置ID为0x30
   - 配置发送数据为：0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0
   - 选择增强型校验和
   - 勾选使能选项
   - 点击"配置到调试器"按钮

   .. image:: ../../../lin/master/doc/lin_debugger_slave_sent_config.png

3. 编译下载程序到开发板并运行

运行现象
--------
1. 串口终端输出信息：

   .. code-block:: console

      Test uart lin master polling transfer
      uart lin master send ID: 0x31
      uart send 8 data:
      0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7
      uart lin master send ID: 0x30
      uart receive 8 data:
      0x7 0x6 0x5 0x4 0x3 0x2 0x1 0x0

      Test uart lin master interrupt transfer
      uart lin master send ID: 0x31
      uart send 8 data:
      0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7
      uart lin master send ID: 0x30
      uart receive 8 data:
      0x7 0x6 0x5 0x4 0x3 0x2 0x1 0x0

2. USB转LIN调试器显示通信数据：

   .. image:: ../../../lin/master/doc/lin_debugger_slave_result.png

调试建议
--------
1. 硬件连接检查
2. 使用逻辑分析仪观察LIN总线信号，确认break、sync、数据段和检验和的信号波形是否正确
3. 使用中断方式时，检查中断是否按预期产生
4. 使用轮询方式时，检查驱动的timeout设置是否与数据帧波特率相匹配

