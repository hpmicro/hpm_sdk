.. _audio_v2_speaker:

Audio V2 Speaker
================================

概述
------

本示例工程展示USB Audio V2喇叭设备。

- PC可以看到一个CherryUSB喇叭设备。

工程配置
------------

- 当 ``app.yaml`` 声明依赖 ``board_audio_codec``，且开发板同时声明 ``board_audio_codec`` 和 ``board_codec_*`` feature 时，自动使用 Audio Codec；否则使用 DAO。

- 如需手动覆盖自动检测，在 ``CMakeLists.txt`` 的 ``find_package`` 之前设置：

  - ``set(CONFIG_CODEC 1)`` — 强制使用 Codec 播放
  - ``set(CONFIG_CODEC 0)`` — 强制使用 DAO 播放

- Audio Codec 类型缺省条件下依据 ``boards/<board_name>/<board_name>.yaml`` 中的 board feature 自动匹配（如 ``board_codec_es8389``）。如需手动指定，可在 ``CMakeLists.txt`` 中设置：``set(CONFIG_CODEC_NAME "wm8960")``。

- USB High Speed 和 Full Speed 的服务周期分别由 ``EP_INTERVAL_HS`` 和 ``EP_INTERVAL_FS`` 配置。对应的 packets/s 和 USB 最大包长会根据 interval 自动计算。


硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

- 将耳机接到音频编解码芯片的 :ref:`耳机 <board_resource>` 接口（Audio Codec 为默认播放器件），如配置为 DAO 播放则将喇叭接到 :ref:`DAO <board_resource>` 接口。

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并安装喇叭驱动，枚举出一个喇叭设备

- 选择该喇叭设备作为默认播放器，PC将通过耳机（Codec）接口或 DAO 接口播放音频（取决于工程配置）。
