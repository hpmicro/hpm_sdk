.. _smix_dao:

SMIX_DAO
================

概述
------

该示例工程展示了使用SMIX（Sound Mixer）音频混音器配合DAO（Digital Audio Output）进行音频播放和音量控制的功能。
SMIX可以对不同采样率的音频进行采样率转换和混音处理，然后通过DAO输出差分PWM信号驱动CLASSD功放芯片。
该例程支持通过GPIO按键控制播放的暂停和恢复，以及通过按键切换SMIX混音器的增益。

工作流程
------------

1. 根据目标采样率，配置系统音频时钟MCLK

2. 初始化I2S外设和DAO外设：

   - 配置I2S外设，包括音频数据格式（位宽、通道数、采样率）
   - 使能I2S DMA发送请求
   - 配置DAO外设

3. 配置GPIO按键中断，用于控制音频播放的暂停、恢复和增益切换：

   - 设置按键中断触发条件
   - 配置按键防抖处理
   - 使能GPIO中断（KEY1用于暂停/恢复，KEY2用于增益切换）

4. 配置SMIX音频混音器：

   - 配置源通道DMA，将音频数据从内存传输到SMIX源通道
   - 配置目标通道DMA，将SMIX处理后的数据传输到I2S发送FIFO
   - 配置突发传输模式（单声道：DMA_NUM_TRANSFER_PER_BURST_1T；立体声：DMA_NUM_TRANSFER_PER_BURST_2T）
   - 设置采样率转换参数（支持1x、2x、3x、4x、6x、8x、12x倍率）
   - 配置混音器增益和通道掩码
   - 使能对应通道

5. 启动I2S外设和DAO外设：

   - 软件重置I2S外设和DAO外设
   - 启动I2S外设和DAO外设

6. SMIX自动进行采样率转换和混音处理

7. SMIX DMA将处理后的音频数据传输到I2S的TX FIFO，DAO将I2S数据转换后输出

8. SMIX DMA完成后停止I2S外设、DAO外设和SMIX通道

9. 支持通过KEY1按键暂停/恢复播放

10. 支持通过KEY2按键切换SMIX混音器增益

音频配置
------------

1. 音频格式配置

   - 支持多种输入采样率（8kHz、16kHz等）
   - 支持16位和32位音频深度
   - 支持单声道(须支持HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX)和立体声模式
   - 根据编译选项选择音频数据源：

     - 单声道模式：``audio_mono.h``
     - 立体声模式：``audio_stereo.h``

2. SMIX配置

   - 支持多种采样率转换倍率：1x、2x、3x、4x、6x、8x、12x

   - 支持多种增益调节：-12dB、-6dB、0dB、+6dB

3. 声道配置

   - channel_slot_mask参数值

      - 左声道：0x1
      - 右声道：0x2
      - 立体声：0x3

   - DAO必须配置为双声道模式（channel_slot_mask = 3），可通过配置I2S的声道模式，实现播放单双声道

      - 需要支持HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX
      - 需要将I2S的channel_slot_mask设置为单声道, channel_slot_mask = 0x1, DAO的左声道播放声音, channel_slot_mask = 0x2, DAO的右声道播放声音。
      - 如果不支持HPM_IP_FEATURE_DAO_AUDIO_MONO_FIX，可以单声道音频数据复制后形成立体声音频源，使用播放立体声音频方式播放。

   - DAO实现播放立体声音频源，设置I2S的channel_slot_mask = 0x3, DAO的左右声道同时播放声音。

硬件设置
------------

- 连接喇叭到开发板的 :ref:`DAO接口 <board_resource>` 用于播放音频

- 使用开发板上的按键 :ref:`按键接口 <board_resource>` 用于控制播放：

  - KEY1：控制播放暂停/恢复
  - KEY2：切换SMIX混音器增益(-12dB - 0dB)，初始增益为-12dB

已知问题
------------

- 使用DAO播放某些音频时可能产生噪声

- 板上按键，受机械抖动影响，可能出现多次触发GPIO中断的情况


运行现象
------------

当工程正确运行后，可以观察到以下现象：

1. 串口终端输出信息：

   .. code-block:: console

      Sound mixer change DAO volume example
      Press Key1 will pause/resume audio play, Press Key2 will change audio play gain.

      -- Select audio play action --
       1 - audio play

2. 选择选项1后：

   - 串口显示"mixer play one sound"
   - 显示输入和输出采样率信息
   - 音频设备开始播放示例音频(初始增益为-12dB)

3. 播放过程中的按键控制：

   - 按下KEY1按键可以暂停播放，再次按下KEY1可以恢复播放
   - 按下KEY2按键可以在-12dB和0dB之间切换SMIX混音器增益
   - 串口会显示相应的控制信息：

     - "dao_i2s pause" - 暂停播放
     - "dao_i2s resume" - 恢复播放
     - "set SMIX gain to 0dB" - 设置增益为0dB
     - "set SMIX gain to -12dB" - 设置增益为-12dB

