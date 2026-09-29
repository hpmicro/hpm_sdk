.. _audio_v2_speaker_fb:

Audio V2 Speaker Feedback
==========================================

概述
------

本示例工程展示USB Audio V2喇叭设备，它使用反馈端点实现同步。

- PC可以看到一个CherryUSB喇叭设备。

功能特性
--------

- USB Audio Class 2.0。
- 双声道、24-bit 喇叭音频流，使用 4-byte audio slot。
- 支持 96 kHz、48 kHz 和 16 kHz。
- 采用 asynchronous OUT 和 explicit feedback 端点同步。
- 运行时 DMA slot 长度和 feedback 调节周期根据 USB 服务周期自动计算。

工程配置
------------

- 当 ``app.yaml`` 声明依赖 ``board_audio_codec``，且开发板同时声明 ``board_audio_codec`` 和 ``board_codec_*`` feature 时，自动使用 Audio Codec；否则使用 DAO。

- 如需手动覆盖自动检测，在 ``CMakeLists.txt`` 的 ``find_package`` 之前设置：

  - ``set(CONFIG_CODEC 1)`` — 强制使用 Codec 播放
  - ``set(CONFIG_CODEC 0)`` — 强制使用 DAO 播放

- Audio Codec 类型缺省条件下依据 ``boards/<board_name>/<board_name>.yaml`` 中的 board feature 自动匹配（如 ``board_codec_es8389``）。如需手动指定，可在 ``CMakeLists.txt`` 中设置：``set(CONFIG_CODEC_NAME "wm8960")``。

- USB High Speed 和 Full Speed 的服务周期分别由 ``EP_INTERVAL_HS`` 和 ``EP_INTERVAL_FS`` 配置。对应的 packets/s、USB 最大包长、运行时 DMA slot 长度和 feedback 调节周期会根据 interval 自动计算。

- USB 描述符最大包长为 nominal service-interval slot 加一个音频帧。打开音频流时，还会检查环形缓冲区总容量是否为运行时 DMA slot 长度的整数倍。


硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

- 将耳机接到音频编解码芯片的 :ref:`耳机 <board_resource>` 接口（Audio Codec 为默认播放器件），如配置为 DAO 播放则将喇叭接到 :ref:`DAO <board_resource>` 接口。

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并安装喇叭驱动，枚举出一个喇叭设备

- 选择该喇叭设备作为默认播放器，PC将通过耳机（Codec）接口或 DAO 接口播放音频（取决于工程配置）。

- 应在 streaming interface 未激活时修改采样率，然后重新打开接口，使运行时 DMA slot 长度和 nominal feedback 值重新计算。

- 在播放过程中，串口会周期性打印反馈同步状态信息，格式如下：

  .. code-block:: console

     fb: used=12288/24576 (50%) val=786432 hz=96000

  各字段含义：

  - ``used``：当前环形缓冲区中已缓存的数据量（字节）
  - ``total``：环形缓冲区总容量（字节）
  - ``percent``：缓冲区使用百分比，反映设备与主机之间的速率同步状况
  - ``val``：通过 explicit feedback 端点发送的原始 feedback 计算值
  - ``hz``：feedback 值所表示的采样率

  百分比接近 50% 表示设备与主机速率同步良好。feedback 控制器约每 100 ms 更新一次；缓冲区持续偏高时请求主机降低发送速率，持续偏低时请求主机提高发送速率。

  使用默认 HS ``bInterval=4`` 或 FS ``bInterval=1`` 时，两种模式均为每秒 1000 个服务周期。在 96 kHz、双声道、4-byte slot 配置下，nominal DMA slot 为 768 字节，32-slot 环形缓冲区总容量为 24576 字节。
