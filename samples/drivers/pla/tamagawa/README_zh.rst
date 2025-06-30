.. _tamagawa_encoder_communication:

多摩川编码器通讯
================

概述
------

演示了使用PLA、DMA、SPI、QEI、PWM等外设和多摩川编码器进行通讯。主要功能包括：

- 支持多种DataID数据获取：
  - ID0/ID7/ID8/IDC：获取绝对位置数据
  - ID1：获取多圈数据
  - ID2：获取ID信息
  - ID3：获取完整状态（包含位置、多圈、ID和报警信息）

- EEPROM操作：
  - 支持EEPROM读写
  - 自动检测EEPROM忙状态
  - 地址范围：0-127

- 完整的错误检测机制：
  - CRC校验
  - 通讯错误检测
  - 编码器错误状态监测
  - 指令响应验证

- 高精度时序控制：
  - 通讯延时自动补偿
  - 基于PLA的精确定时
  - DMA链式传输

硬件设置
------------

- 核心板
- 扩展电路板：TMAGAWA RevA
- 支持的编码器型号：TS5700N8401
- 接线要求：
  - 将TMAGAWA RevA对插到核心板
  - 正确连接编码器到扩展板
  - 确保扩展板J2的P1和P2已连接

软件架构
------------

主要模块：

1. 协议层 (tamagawa_proc)：
   - 指令格式化
   - CRC校验
   - 数据解析
   - 错误处理

2. 硬件驱动层：
   - PLA配置：用于精确定时和信号同步
   - DMA设置：实现高效数据传输
   - SPI配置：与编码器通讯接口
   - QEI：捕获时序信号
   - PWM：生成基准时钟

3. 应用层：
   - 设备初始化
   - 通讯管理
   - 数据处理和显示

运行现象
------------

当工程正确运行后,串口终端会输出如下信息,转动编码器轴，位置信息会改变：

.. code-block:: console

   cmd:3 time:638, abs pos:4170741, abm pos:0, id:23.
   cmd:3 time:631, abs pos:4170741, abm pos:0, id:23.
   cmd:3 time:631, abs pos:4170740, abm pos:0, id:23.
   cmd:3 time:634, abs pos:4170740, abm pos:0, id:23.
   cmd:3 time:639, abs pos:4170741, abm pos:0, id:23.
   cmd:3 time:632, abs pos:4170741, abm pos:0, id:23.
   cmd:3 time:630, abs pos:4170742, abm pos:0, id:23.

显示信息说明：
- cmd：当前执行的指令ID
- time：通讯延时时间（单位：系统时钟周期）
- abs pos：绝对位置值
- abm pos：多圈位置值
- id：编码器ID

.. note::
 - tmgw_message_process函数调用后，会更新tmgw_msg的数据，tmgw_msg包含上述功能的信息
 - 动态切换命令，需要修改tmgw_dev.comm_time_delay_config来关闭通讯功能，然后重新进行PLA DMA的配置，否则会产生一次的数据非法指令，然后恢复正常

