.. _lv_demo_music:

lv_demo_music
==========================

概述
------

音乐播放器示例展示了通过LVGL创建与当代智能手机相似的用户界面，并实现歌曲播放。

更多详细信息请参考 `官方 lv_demos github <https://github.com/lvgl/lv_demos>`_

工程配置
------------

- 当本示例的 ``app.yaml`` 声明依赖 ``board_audio_codec``，且开发板同时声明 ``board_audio_codec`` 和 ``board_codec_*`` feature 时，自动使用 Audio Codec；否则使用 DAO。

- 如需手动覆盖自动检测，在 ``CMakeLists.txt`` 的 ``find_package`` 之前设置：

  - ``set(CONFIG_CODEC 1)`` — 强制使用 Codec 播放
  - ``set(CONFIG_CODEC 0)`` — 强制使用 DAO 播放

- Audio Codec 类型缺省条件下依据 ``boards/<board_name>/<board_name>.yaml`` 中的 board feature 自动匹配（如 ``board_codec_es8389``）。如需手动指定，可在 ``CMakeLists.txt`` 中设置：``set(CONFIG_CODEC_NAME "wm8960")``。

硬件连接
------------

将LCD屏连接到评估板；

将存储wav格式歌曲的MicroSD卡插入到卡槽中；

根据工程配置， 使用DAO播放时将喇叭接到 :ref:`DAO <board_resource>` 接口， 使用音频编解码芯片播放时将耳机接到音频编解码芯片的 :ref:`耳机 <board_resource>` 接口。

已知问题
------------

使用DAO播放某些音频时会产生噪声。

运行
------

lvgl music将会显示在LCD屏上，可以操作播放/暂停、下一首、上一首。

备注
------

- 由于framebuffer和mp3解码所占内存较多，请指定HPM_BUILD_TYPE为flash_sdram_xip，可执行指令如下：


.. code-block:: text

   cmake -GNinja -DBOARD=hpm6750evkmini -DHPM_BUILD_TYPE=flash_sdram_xip -DCMAKE_BUILD_TYPE=debug ..


- MP3文件的采样率 Sample rate 需小于等于 48000 Hz。
