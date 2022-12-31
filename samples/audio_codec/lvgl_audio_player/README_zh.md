# lv_demo_music

## 概述

音乐播放器示例展示了通过LVGL创建与当代智能手机相似的用户界面，并实现歌曲播放。

更多详细信息请参考 [官方 lv_demos github](https://github.com/lvgl/lv_demos)

## 工程配置

- 在文件[CMakeLists.txt](./CmakeLists.txt)中, 根据需求设置音频播放器件的类型，默认使用DAO播放。如存在板载音频编解码芯片， 可以设置匹配的音频编解码芯片类型， 例如："set(CONFIG_CODEC "sgtl5000")"。

## 硬件连接

将LCD屏连接到评估板；

将存储wav格式歌曲的MicroSD卡插入到卡槽中；

根据工程配置， 使用DAO播放时将喇叭接到[DAO](lab_board_app_dao)接口， 使用音频编解码芯片播放时将耳机接到音频编解码芯片的[耳机](lab_board_app_headphone)接口。

## 运行

littglevgl music将会显示在LCD屏上，可以操作播放/暂停、下一首、上一首。

![lv_demo_music](../../../doc/images/samples/lv_demo_music.gif "lv_demo_music")

## 备注

由于framebuffer所占内存较多，请指定CMAKE_BUILD_TYPE为flash_sdram_xip，可执行指令如下：
``` shell
cmake -GNinja -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=flash_sdram_xip ..
```