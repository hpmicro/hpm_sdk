.. _audio_v2_mic_adaptive:

Audio V2 Microphone Adaptive
======================================

概述
------

本示例工程展示USB Audio V2麦克风设备。

- PC可以看到一个CherryUSB麦克风设备。
- 麦克风会根据录音缓冲区水位，自适应调整 USB IN 包长度。

功能特性
--------

- USB Audio Class 2.0。
- 双声道、24-bit 麦克风音频流，使用 4-byte audio slot。
- Codec 模式支持 96 kHz、48 kHz 和 16 kHz。
- PDM 模式支持 16 kHz。
- 以录音缓冲区中心水位为目标，自适应调整 USB IN 包长度。

工程配置
------------

- 当 ``app.yaml`` 声明依赖 ``board_audio_codec``，且开发板同时声明 ``board_audio_codec`` 和 ``board_codec_*`` feature 时，自动使用 Audio Codec；否则使用 PDM。

- 如需手动覆盖自动检测，在 ``CMakeLists.txt`` 的 ``find_package`` 之前设置：

  - ``set(CONFIG_CODEC 1)`` — 强制使用 Codec 录音
  - ``set(CONFIG_CODEC 0)`` — 强制使用 PDM 录音

- Audio Codec 类型缺省条件下依据 ``boards/<board_name>/<board_name>.yaml`` 中的 board feature 自动匹配（如 ``board_codec_es8389``）。如需手动指定，可在 ``CMakeLists.txt`` 中设置：``set(CONFIG_CODEC_NAME "wm8960")``。

- USB High Speed 和 Full Speed 的服务周期分别由 ``EP_INTERVAL_HS`` 和 ``EP_INTERVAL_FS`` 配置。对应的 packets/s、USB 最大包长、运行时 DMA slot 长度和状态打印周期会根据 interval 自动计算。

- USB 描述符最大包长为 nominal service-interval slot 加一个音频帧，使 adaptive 机制能够在需要时增加一个音频帧。


硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

- 将麦克风接到音频编解码芯片的 :ref:`麦克风 <board_resource>` 接口（Audio Codec 为默认输入器件），如配置为 PDM 录音则使用板载PDM麦克风。

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并安装麦克风驱动，枚举出一个麦克风设备

- 选择该麦克风设备作为默认音频输入设备，PC将通过Codec接口或PDM接口输入音频（取决于工程配置）。

- 应在 streaming interface 未激活时修改采样率，然后重新打开接口，使运行时 DMA slot 长度重新计算。

- 在录音过程中，串口会周期性打印麦克风缓冲区状态信息。例如 Codec 配置可以打印：

  .. code-block:: console

    mic buf: used=12192/24576 (49%)

  各字段含义：

  - ``used``：当前环形缓冲区中的数据量（字节）
  - ``total``：环形缓冲区总容量（字节）
  - ``percent``：缓冲区使用百分比，反映设备与主机之间的速率同步状况

  缓冲区总容量取决于输入器件、最大采样率和 USB 服务周期。

  adaptive 机制以 50% 为中心设置 ±12.5% 的调整窗口：高于 62.5% 时增加一个音频帧，低于 37.5% 时减少一个音频帧，位于 37.5%～62.5% 时发送正常包长。
