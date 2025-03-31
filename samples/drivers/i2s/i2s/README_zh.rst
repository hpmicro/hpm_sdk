.. _i2s:

I2S
======

概述
------

该工程演示了使用I2S接口（PDM和DAO外设）记录和播放声音的功能。

硬件设置
------------

连接扬声器到板上DAO接口。

已知问题
------------

使用DAO播放某些音频时会产生噪声。

运行现象
------------

当工程正确运行后，按照log提示发出声音，声音会被记录并播放出来。串口终端会输出如下信息：

.. code-block:: console

   i2s example
   testing pdm to dao
   Recording...
   Please make some sound to mic0
   Playing...
   Recording...
   Please make some sound to mic0
   testing wav playback
   testing sine wave playback

