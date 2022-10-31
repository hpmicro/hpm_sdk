# lv_demo_music

## 概述

音乐播放器示例展示了通过LVGL创建与当代智能手机相似的用户界面，并实现歌曲播放。

更多详细信息请参考 [官方 lv_demos github](https://github.com/lvgl/lv_demos)

## 硬件连接

将LCD屏连接到评估板；

将存储wav格式歌曲的MicroSD卡插入到卡槽中；

将喇叭接在DAO输出端口上。

## 运行

littglevgl music将会显示在LCD屏上，可以操作播放/暂停、下一首、上一首。

![lv_demo_music](../../../doc/images/samples/lv_demo_music.gif "lv_demo_music")

## 备注

由于framebuffer所占内存较多，请指定CMAKE_BUILD_TYPE为flash_sdram_xip，可执行指令如下：
``` shell
cmake -GNinja -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=flash_sdram_xip ..
```