.. _audio_v2_headset:

Audio V2 Headset
================

概述
----

本示例工程展示一个 USB Audio V2 Headset 设备。设备同时提供喇叭播放和麦克风录音功能：

- 喇叭使用显式 feedback 端点调节主机发送速率，避免播放缓冲区上溢或下溢。
- 麦克风根据录音缓冲区水位自适应调整 USB IN 包长度，保持采集与主机接收速率同步。
- PC 会枚举出一个包含喇叭和麦克风的 CherryUSB Headset 设备。

功能特性
--------

- USB Audio Class 2.0。
- 喇叭和麦克风均为双声道、24-bit 音频，使用 4-byte slot。
- High Speed 支持 192 kHz、96 kHz、48 kHz、32 kHz 和 16 kHz。
- Full Speed 支持 96 kHz、48 kHz 和 16 kHz。
- 喇叭和麦克风共用 Audio Codec 的时钟源和采样率。
- 喇叭采用 asynchronous OUT + explicit feedback 同步方式。
- 麦克风采用 adaptive IN 包长调整方式。

工程配置
--------

- 本示例依赖开发板提供 ``board_audio_codec`` feature。
- Audio Codec 类型根据 ``boards/<board_name>/<board_name>.yaml`` 中的 ``board_codec_*`` feature 自动匹配，例如 ``board_codec_es8389``、``board_codec_wm8960`` 或 ``board_codec_sgtl5000``。
- 如需手动指定 Audio Codec，可在 ``CMakeLists.txt`` 的 ``find_package`` 之前设置：

  .. code-block:: cmake

     set(CONFIG_CODEC_NAME "wm8960")

- USB High Speed 和 Full Speed 的服务周期分别由 ``EP_INTERVAL_HS`` 和 ``EP_INTERVAL_FS`` 配置。对应的 packets/s、USB 最大包长及 DMA 传输长度会根据 interval 自动计算。
- 打开音频流时，会根据当前采样率、音频帧大小和 packets/s 计算 nominal DMA slot 长度；USB 描述符最大包长额外预留一个音频帧，用于 feedback 或 adaptive 调节。
- 喇叭流仅在环形缓冲区总容量是运行时 DMA slot 长度的整数倍时启动，避免 DMA 传输跨越环形缓冲区尾部。

硬件设置
--------

- 使用 USB Type-C 线缆连接 PC USB 端口和开发板 PWR DEBUG 端口，用于下载程序和查看串口日志。
- 使用 USB Type-C 线缆连接 PC USB 端口和开发板 USB0 端口，用于 USB Audio 数据传输。
- 将耳机或有源喇叭连接到 Audio Codec 的 :ref:`耳机 <board_resource>` 接口。
- 将麦克风连接到 Audio Codec 的 :ref:`麦克风 <board_resource>` 接口。

运行现象
--------

- 将程序下载至开发板并运行，PC 会自动识别 USB Audio 设备，并枚举出喇叭和麦克风音频端点。
- 选择该喇叭设备作为默认播放器，PC 将通过 Audio Codec 耳机接口播放音频。
- 选择该麦克风设备作为默认音频输入设备，PC 将通过 Audio Codec 麦克风接口录制音频。
- 喇叭和麦克风使用同一个采样时钟，同时使用时应选择相同的采样率。
- 应在 streaming interface 未激活时修改采样率，然后重新打开接口，使运行时 DMA slot 长度重新计算。

喇叭 Feedback 状态
----------------------------

播放过程中，串口会周期性打印喇叭缓冲区、feedback 和 USB OUT 包长信息：

.. code-block:: console

   spk fb: used=12472/24576 (50%) val=1572864 hz=192000 pkt=192..192

各字段含义：

- ``used``：喇叭环形缓冲区中已缓存的数据量，单位为字节。
- ``total``：喇叭环形缓冲区总容量，单位为字节。
- ``percent``：喇叭缓冲区使用百分比，接近 50% 表示主机发送速率与设备播放速率同步良好。
- ``val``：当前 feedback 原始计算值。
- ``hz``：feedback 对应的采样率。
- ``pkt``：当前统计周期内，喇叭 USB OUT 端点实际接收到的最小和最大包长，单位为字节。

例如在 192 kHz、双声道、4-byte slot、HS ``bInterval=1`` 时，正常包长为 192 字节。当 feedback 要求主机降低发送速率时，可以观察到：

.. code-block:: console

   spk fb: used=12656/24576 (51%) val=1572858 hz=191999 pkt=184..192

``pkt=184..192`` 表示主机已经在 192 字节包之间插入一个少一帧的 184 字节包，feedback 同步机制正在生效。缓冲区在中心附近小范围波动属于正常现象。

麦克风 Adaptive 状态
------------------------------

录音过程中，串口会周期性打印麦克风缓冲区状态：

.. code-block:: console

   mic buf: used=12192/24576 (49%)

各字段含义：

- ``used``：麦克风环形缓冲区中的数据量，单位为字节。
- ``total``：麦克风环形缓冲区总容量，单位为字节。
- ``percent``：麦克风缓冲区使用百分比。

麦克风 adaptive 机制的行为如下：

- 缓冲区高于 62.5% 时，USB IN 包增加一个音频帧，加快数据发送。
- 缓冲区低于 37.5% 时，USB IN 包减少一个音频帧，让缓冲区重新积累数据。
- 缓冲区位于 37.5%～62.5% 时，发送正常包长。

缓冲区使用率长期保持在中间区域，表示采集速率和主机接收速率同步正常；持续接近满或空表示存在速率失配。
