.. _audio_v1_microphone_speaker_and_midi:

Audio V1 Microphone, Speaker and Midi
==========================================================================

概述
------

本示例工程展示USB Audio V1喇叭、麦克风和Midi设备。

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

- 将喇叭连接至DAO接口

电脑上位机
---------------

 - 在电脑上测试MIDI时，需要安装并使用合成器软件，如下所示。

    - Windows: MIDI-OX

    - Linux(Ubuntu): qsynth, qjackctl

    - MAC: SimpleSynth

运行现象
------------

- 将程序下载至开发板运行，电脑可自动安装喇叭、麦克风、MIDI驱动。

- 选择该喇叭设备作为默认播放器，PC将通过DAO接口播放音频

- 选择该麦克风设备作为默认音频输入设备，PC将通过PDM接口输入音频。

- 以Windows MIDI-OX软件为例介绍MIDI测试。

    - 打开MIDI-OX软件，点击菜单 **"options-MIDI devices..."** ，MIDI Inputs选择 "HPMicro UAC V1 DEMO"，MIDI Outputs选择 "Microsoft GS Wavetable Synth"。

    - 按压PBUTN或UserKey按钮来模拟按压琴键，电脑音频将播放出琴键声音。
