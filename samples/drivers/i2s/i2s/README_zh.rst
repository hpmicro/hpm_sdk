.. _i2s_dao_pdm:

I2S_DAO_PDM音频示例
============================

概述
------

本工程演示了如何使用I2S接口实现音频录制和播放功能。示例包含了PDM（脉冲密度调制）录音和DAO（数字音频输出）播放功能。

演示包括以下功能：

- DAO音频播放
- PDM录音并通过DAO播放

工作流程
--------

DAO播放流程：

1. 配置系统音频时钟：
   - 默认配置为24.576MHz，适用于48KHz采样率
   - 根据实际采样率需求可调整系统时钟
2. 配置I2S1外设用于DAO播放：
   - 设置采样率（如48KHz）
   - 配置双声道模式
3. 配置DAO：
4. 启动音频播放：
   - 使能I2S1外设
   - 使能DAO
5. 通过I2S1的TX0 FIFO发送音频数据给DAO
   - 查询FIFO状态， 根据FIFO状态填入音频数据
6. 播放完成后关闭DAO和I2S1外设

PDM录音流程：

1. 配置系统音频时钟：
   - 默认配置为24.576MHz，适用于48KHz采样率
   - 根据实际采样率需求可调整系统时钟
2. 配置I2S0外设用于PDM录音：
   - 设置采样率
   - 配置通道（通过channel_slot_mask）
   - 设置32位数据位宽
3. 配置PDM：
   - 设置时钟分频
   - 配置CIC下采样率
   - 选择工作通道
4. 开始录音：
   - 使能I2S0外设
   - 使能PDM
5. 通过I2S0的RX0 FIFO接收PDM录音数据
   - 查询FIFO状态，根据FIFO状态读取数据
6. 录音完成后关闭PDM和I2S0外设

技术说明
--------

- DAO播放特性：

  - 采样频率：DAO的最优工作频率为48K
  - 采样通道：固定为双声道，对于单声道音频需要对数据源进行处理
  - 采样位宽：可配置
  - DAO与I2S1协调工作，通过I2S1的TX0 FIFO发送音频数据给DAO

- PDM录音特性：

  - 采样频率：可配置
  - 采样通道：最大支持8个通道（4对立体声）
  - 采样位宽：24位PCM + 4位索引，使用32位数据位宽配置
  - PDM与I2S0协同工作， PDM通过I2S0 的RX0 FIFO接收音频数据

- 使用注意：

  - I2S的寄存器数据接口为有效数据在32位寄存器的高位，当使用DAO播放16bit位宽的音频时，需要将16bit音频数据写在32位寄存器的高16位
    当使用PDM录音时，如果想将录音结果保留16bit格式，需要将32位寄存器的高16位数据读出
  - 使用PDM录音时，需要同步调整PDM的采样率和I2S0的采样率，并保持一致
    PDM支持通过调整时钟分频和CIC下采样率来调整采样率
  - PDM的通道设置：通道通过channel_slot_mask参数配置， 通道按照D0L\D1L\D2L\D3L\D0R\D1R\D2R\D3R顺序排列
    例如：channel_slot_mask = 0x11，表示使用D0数据线的左右通道
  - 默认系统音频时钟为24.576MHz，适用于48KHz、16KHz等采样率，如果需要使用其他采样率如44.1KHz，须修改系统时钟配置实现

已知问题
------------

使用DAO播放某些音频时会产生噪声。

硬件要求
--------

- 连接扬声器到板上DAO接口

运行现象
--------

上电后，程序将依次执行以下测试：

1. DAO WAV文件播放：

   - 播放一段预置的8KHz采样率、单声道、16位深度的WAV音频

2. DAO正弦波播放：

   - 播放1KHz正弦波, 48KHz采样率、单声道、32位深度, 重复播放200次

3. PDM录音并DAO播放：

   - 首先进行PDM录音
   - 录音完成后通过DAO播放录制的音频

串口将显示每个测试的进度：

.. code-block:: console

   DAO and PDM with I2S example

   1. Testing DAO wav playback

   2. Testing DAO sine wave playback

   3. Testing PDM record and DAO playback
   Please enter any character to start recording
   Recording finish
   Please enter any character to start playing
   Playing finish

