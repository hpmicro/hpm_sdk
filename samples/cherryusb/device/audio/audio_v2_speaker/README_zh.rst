.. _audio_v2_speaker:

Audio V2 Speaker
================================

概述
------

本示例工程展示USB Audio V2喇叭设备。

- PC可以看到一个CherryUSB喇叭设备。

工程配置
------------

- 在文件`CMakeLists.txt`中, 根据需求设置音频播放器件的类型，默认使用DAO播放。

- 如使用板载Audio Codec芯片， Audio Codec类型缺省条件下依据`boards/<board_name>/<board_name>.yaml`自动匹配(feature: board_codec_xxxx)，如需指定audio codec类型，可在`CMakeLists.txt`中设置："set(CONFIG_CODEC_NAME "wm8960")"。

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

- 根据工程配置， 使用DAO播放时将喇叭接到 :ref:`DAO <board_resource>` 接口， 使用音频编解码芯片播放时将耳机接到音频编解码芯片的 :ref:`耳机 <board_resource>` 接口。

运行现象
------------

- 将程序下载至开发板运行，电脑可自动识别并安装喇叭驱动，枚举出一个喇叭设备

- 选择该喇叭设备作为默认播放器，PC将通过DAO接口播放音频
