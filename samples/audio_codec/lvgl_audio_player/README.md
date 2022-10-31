# lv_demo_music

## Overview

The music player demo shows what kind of modern, smartphone-like user interfaces can be created on LVGL， and can play music.

Please refer to [official lv_demos github](https://github.com/lvgl/lv_demos)

## Board Settings

Attach LCD panel to the board LCD interface.

Attach MicroSD card that stores songs in WAV format into the card slot;

Attach Trumpet to the board DAO interface.

## Running the Demo

littlevgl music will be shown on the LCD panel and can operate play/pause, next, previous.

![lv_demo_music](../../../doc/images/samples/lv_demo_music.gif "lv_demo_music")

## Remark

As framebuffer occupies a large amount of memory, please specify CMAKE_BUILD_TYPE as flash_sdram_xip. The following commands can be executed:
``` shell
cmake -GNinja -DBOARD=hpm6750evkmini -DCMAKE_BUILD_TYPE=flash_sdram_xip ..
```