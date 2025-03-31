.. _midi:

MIDI
========

概述
------

本示例工程展示MIDI示例

- PC 通过设备管理器查看得到一个MIDI设备

硬件设置
------------

- 使用USB Type-C线缆连接PC USB端口和PWR DEBUG端口

- 使用USB Type-C线缆连接PC USB端口和开发板USB0端口

电脑上位机
---------------

 - 在电脑上测试时，需要安装并使用合成器软件，如下所示。

    - Windows: MIDI-OX

    - Linux(Ubuntu): qsynth, qjackctl

    - MAC: SimpleSynth

运行现象
------------

- 以Windows MIDI-OX为例

    - 将程序下载至开发板运行，电脑可自动识别并枚举出一个MIDI设备。

    - 打开MIDI-OX软件，点击菜单 **"options-MIDI devices..."** ，MIDI Inputs选择 "HPMicro MIDI DEMO"，MIDI Outputs选择 "Microsoft GS Wavetable Synth"

    .. image:: doc/MIDI-OX_select_device.png
       :alt: MIDI-OX_select_device.png

    - 串口终端显示log如下，用户选择工作模式：1-自动弹奏，2-手动弹奏。


.. code-block:: console

           ----------------------------------------------------------------------
           $$\   $$\ $$$$$$$\  $$\      $$\ $$\
           $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
           $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
           $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
           $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
           $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
           $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
           \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
           ----------------------------------------------------------------------
           CherryUSB midi device sample.

           ***********Select Work Mode**********
           1 - Auto Play
           2 - Manual Play

           Please Enter Play Number:




    - 选择1时，MIDI设备周期性重复地将MIDI Event Packets数据上发给PC，即实现自动弹奏琴键，用户可通过电脑音频收听。

    - 选择2时，MIDI设备通过按键触发将MIDI Event Packets数据上发给PC，即按压PBUTN或UserKey按钮来模拟按压琴键，用户可通过电脑音频收听。
