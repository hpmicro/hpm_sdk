.. _uart_lin_slave_baudrate_adaptive:

UART LIN从机波特率自适应示例
=============================

概述
----
本示例展示了如何使用UART外设和PLB外设实现LIN从机的波特率自适应功能。主要演示：

- 使用PLB检测LIN总线波特率
- LIN从机根据检测结果自动调整波特率
- LIN从机接收主机发送的数据
- LIN从机响应主机的数据请求


工作原理
--------
本示例实现了以下功能：

1. 波特率自适应：

   - 使用PLB测量LIN break和同步字段的时序
   - 计算实际的波特率
   - 动态调整UART波特率以匹配主机

2. 接收主机数据：

   - 接收LIN帧（ID: 0x31，数据：0x0-0x7）
   - 校验数据正确性

3. 响应主机请求：

   - 发送LIN帧（ID: 0x30，数据：0x7-0x0）
   - 使用增强型校验和

工作流程
--------
1. 初始化流程：

   a) PLB初始化：

      - 配置PLB通道用于时序测量
      - 配置DMA用于数据采集
      - 设置波特率容差和过滤参数

   b) UART初始化：

      - 配置初始波特率（19200）
      - 使能line status中断检测break信号
      - 配置帧格式为8位数据位，无校验位

2. 波特率检测流程：

   a) Break和同步字段检测：

      - PLB捕获LIN总线信号
      - 测量break和同步字段的时序
      - 计算实际波特率

   b) 波特率调整：

      - 验证计算得到的波特率是否在支持范围内
      - 动态更新UART波特率配置
      - 准备接收PID

3. 数据处理流程：

   a) 接收数据（ID: 0x31）：

      - 配置RX FIFO触发等级大于剩余数据帧长度
      - 使能RX超时中断
      - 在中断中接收和校验数据

   b) 发送数据（ID: 0x30）：

      - 准备发送数据（0x7-0x0）
      - 使能TX FIFO中断
      - 在中断中确认数据发送完成

4. 中断处理流程：

   a) DMA中断：

      - 处理PLB采集的数据
      - 计算波特率并更新UART配置

   b) UART RX数据中断：

      - 接收PID并校验
      - 配置后续操作

   c) UART TX FIFO中断：

      - 检测发送数据完成
      - 注意：uart_intr_tx_slot_avail中断发生时尚有最后一个字节数据在发送FIFO中，可以添加状态查询或者依据版本使用新特性TXIDLE中断

   d) UART RX超时中断：

      - 接收完整数据帧
      - 校验数据正确性
      - 关闭RX超时中断
      - 注意：RX timeout方式需要数据帧之间间隔大于等于4个UART字节的传输时间

注意事项
--------
1. 不支持从机自动休眠，无需主机唤醒
2. 波特率检测需要额外的硬件连接（UART_RX到PLB输入）
3. 波特率调整有一定的容差范围（默认±5%）

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
   - 将UART_RX引脚与PLB LIN时钟检测引脚相连

2. 不带板载LIN收发器的开发板：

   - 将开发板的UART TX和RX引脚连接到LIN收发器的MCU端
   - 将USB转LIN调试器连接到LIN收发器的LIN总线端
   - 连接GND
   - 将UART_RX引脚与PLB LIN时钟检测引脚相连

具体引脚连接请参考开发板 :ref:`引脚描述 <board_resource>` 部分。

运行步骤
~~~~~~~~
1. 配置USB转LIN调试器：

   - 选择正确的串口和初始波特率（可以是任意支持的波特率）
   - 点击"设置到调试器"按钮

   .. image:: ../../../lin/doc/lin_debugger_configuration.png

2. 配置主机发送功能和主机接收功能：

   - 设置发送ID为0x31，配置发送数据为：0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7和增强型校验和
   - 设置接收ID为0x30
   - 勾选使能选项
   - 点击"配置到调试器"按钮

   .. image:: ../../../lin/slave/doc/lin_debugger_master_sent_config.png

3. 编译下载程序到开发板并运行

4. 可以尝试调整USB转LIN调试器的波特率，验证自适应功能

运行现象
--------
1. 串口终端输出信息：

   .. code-block:: console

      Test uart lin slave baudrate adaptive example
      uart lin receive ID: 0x31
      uart receive 8 data:
      0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7
      uart lin receive ID: 0x30
      uart send 8 data:
      0x7 0x6 0x5 0x4 0x3 0x2 0x1 0x0

2. USB转LIN调试器显示通信数据：

   .. image:: ../../../lin/slave/doc/lin_debugger_master_result.png

调试建议
--------
1. 硬件连接检查
2. 使用逻辑分析仪观察LIN总线信号，确认break、sync、数据段和检验和的信号波形是否正确
3. 检查中断是否按照预期产生，PLB的中断和计算出的波特率以及UART的中断是否正确

