.. _dao_i2s_dma:

DAO_I2S_DMA
==============

概述
------

该示例工程展示了使用DAO（Digital Audio Output）+ I2S外设配合DMA传输音频数据进行播放的功能。
DAO是数字音频输出模块，可以直接输出差分PWM信号，直接驱动CLASSD功放芯片，实现音频播放。
该例程还支持通过GPIO按键控制播放的暂停和恢复功能。

工作流程
------------

1. 根据音频采样率，配置系统音频时钟MCLK

2. 初始化I2S外设和DAO外设：

   - 配置I2S外设，包括音频数据格式（位宽、通道数、采样率）
   - 使能I2S DMA发送请求
   - 配置DAO外设

3. 配置GPIO按键中断，用于控制音频播放的暂停和恢复：

   - 设置按键中断触发条件
   - 配置按键防抖处理
   - 使能GPIO中断

4. 配置DMA搬运音频数据：

   - 配置DMAMUX，选择I2S作为DMA请求源
   - 使能DMA中断
   - 设置源地址为音频数据存放地址
   - 设置目标地址为I2S发送FIFO
   - 配置传输大小和传输宽度
   - 配置突发传输模式（单声道：DMA_NUM_TRANSFER_PER_BURST_1T；立体声：DMA_NUM_TRANSFER_PER_BURST_2T）

5. 启动I2S外设和DAO外设：

   - 软件重置I2S外设和DAO外设
   - 启动I2S外设和DAO外设

6. DMA自动将音频数据传输到I2S的TX FIFO，DAO将I2S数据转换后输出

7. DMA完成后停止I2S外设和DAO外设

8. 重复4-7步骤，实现音频循环播放

9. 音频播放时支持通过按键暂停/恢复播放

音频配置
------------

1. 音频格式配置

   - 支持多种音频采样率
   - 支持16位和32位音频深度
   - 支持单声道(须支持HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX)和立体声模式
   - 根据编译选项选择音频数据源：

     - 单声道模式：``audio_mono.h``
     - 立体声模式：``audio_stereo.h``

2. 声道配置

   - channel_slot_mask参数值

      - 左声道：0x1
      - 右声道：0x2
      - 立体声：0x3

   - DAO声道参数必须配置为双声道模式（channel_slot_mask = 3）

   - DAO实现播放单声道音频源，可通过设置I2S的channel_slot_mask参数值实现

      - 需要支持HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX
      - 需要将I2S的channel_slot_mask设置为单声道, channel_slot_mask = 0x1, DAO的左声道播放声音, channel_slot_mask = 0x2, DAO的右声道播放声音。
      - 如果不支持HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX，可以单声道音频数据复制后形成立体声音频源，使用播放立体声音频方式播放。

   - DAO实现播放立体声音频源，设置I2S的channel_slot_mask = 0x3, DAO的左右声道同时播放声音。

硬件设置
------------

- 连接喇叭到开发板的 :ref:`DAO接口 <board_resource>` 用于播放音频

- 使用开发板上的按键 :ref:`按键接口 <board_resource>` 用于控制播放暂停/恢复

已知问题
------------

- 使用DAO播放某些音频时可能产生噪声

- 板上按键，受机械抖动影响，可能出现多次触发GPIO中断的情况

运行现象
------------

当工程正确运行后，可以观察到以下现象：

1. 串口终端输出信息：

   .. code-block:: console

      DAO_I2S with DMA example
      DAO_I2S with DMA play test start, press button can pause/resume audio play.
      DAO_I2S with DMA play one cycle finished
      DAO_I2S with DMA play one cycle finished
      DAO_I2S with DMA play one cycle finished
      DAO_I2S with DMA play one cycle finished
      Test DAO_I2S DMA play success

2. 音频设备会循环播放示例音频（共4次）

3. 音频播放时按下按键可以暂停播放，再次按下按键可以恢复播放

4. 串口会显示相应的控制信息：

   - "dao_i2s pause" - 暂停播放
   - "dao_i2s resume" - 恢复播放

