.. _spi_sdcard:

SPI操作SDCARD
======================

概述
------

该实例工程展示了使用SPI接口读写SD卡。

硬件设置
------------

-  :ref:`SPI引脚 <board_resource>` 根据板子型号查看具体信息

- SPI引脚对应好SD卡(模块)引脚

注意事项
------------

- 该实例工程依赖SPI组件和DMA管理器组件，需要进行使能该组件。

- 考虑到各类SD卡兼容性，SPI sdcard库中的写操作中，有些SD卡需要进行写等待延时。移植时可以参考common/adapt/spi_sd_adapt.c, 需要注意以下三点：

- 1.内部延时默认100us，如果需要改其他延时可以通过重新定义SPI_SD_DELAY_DEFAULT_US宏

- 2.如果不需要该写等待延时, 可以把sdcard_spi_init API的传参的结构体成员delay_ms和delay_us置为NULL。或者SPI_SD_DELAY_DEFAULT_US宏定义为0

- 3.如果需要该写等待延时, 并且是在RTOS环境下，建议sdcard_spi_init初始化API在任务中调用，并且传参的结构体成员delay_ms和delay_us赋值为RTOS的任务延时API，比如freertos的vTaskDelay

运行现象
------------

- 确认硬件无误后，工程正确运行后，串口终端会输出如下信息：


.. code-block:: console

   [spi_sdcard] SPI CLK frequency:400000 Hz
   [spi_sdcard] SPI CLK frequency:40000000 Hz
   SD Card initialization succeeded
   Card Info:
   -----------------------------------------------
   Card Size in GBytes:    14.84GB
   Block count: 31116288 block
   Block Size: 512 Bytes

   -----------------------------------------------
   sdcard wiite/read verify ....
   sd block_31116268 write PASSED
   sd block_31116268 read PASSED
   SD write-read-verify block 0x01dacbec PASSED
   sd block_31116269 write PASSED
   sd block_31116269 read PASSED
   SD write-read-verify block 0x01dacbed PASSED
   sd block_31116270 write PASSED
   sd block_31116270 read PASSED
   SD write-read-verify block 0x01dacbee PASSED
   sd block_31116271 write PASSED
   sd block_31116271 read PASSED
   SD write-read-verify block 0x01dacbef PASSED
   sd block_31116272 write PASSED
   sd block_31116272 read PASSED
   SD write-read-verify block 0x01dacbf0 PASSED
   sd block_31116273 write PASSED
   sd block_31116273 read PASSED
   SD write-read-verify block 0x01dacbf1 PASSED
   sd block_31116274 write PASSED
   sd block_31116274 read PASSED
   SD write-read-verify block 0x01dacbf2 PASSED
   sd block_31116275 write PASSED
   sd block_31116275 read PASSED
   SD write-read-verify block 0x01dacbf3 PASSED
   sd block_31116276 write PASSED
   sd block_31116276 read PASSED
   SD write-read-verify block 0x01dacbf4 PASSED
   sd block_31116277 write PASSED
   sd block_31116277 read PASSED
   SD write-read-verify block 0x01dacbf5 PASSED
   sd block_31116278 write PASSED
   sd block_31116278 read PASSED
   SD write-read-verify block 0x01dacbf6 PASSED
   sd block_31116279 write PASSED
   sd block_31116279 read PASSED
   SD write-read-verify block 0x01dacbf7 PASSED
   sd block_31116280 write PASSED
   sd block_31116280 read PASSED
   SD write-read-verify block 0x01dacbf8 PASSED
   sd block_31116281 write PASSED
   sd block_31116281 read PASSED
   SD write-read-verify block 0x01dacbf9 PASSED
   sd block_31116282 write PASSED
   sd block_31116282 read PASSED
   SD write-read-verify block 0x01dacbfa PASSED
   sd block_31116283 write PASSED
   sd block_31116283 read PASSED
   SD write-read-verify block 0x01dacbfb PASSED
   sd block_31116284 write PASSED
   sd block_31116284 read PASSED
   SD write-read-verify block 0x01dacbfc PASSED
   sd block_31116285 write PASSED
   sd block_31116285 read PASSED
   SD write-read-verify block 0x01dacbfd PASSED
   sd block_31116286 write PASSED
   sd block_31116286 read PASSED
   SD write-read-verify block 0x01dacbfe PASSED

   SD write-read-verify reach the total count:19  PASSED!!!!

   sdcard wiite/read speed test....
   SD write-read-verify block range 0x01dacbec-0x01dacbff PASSED
   Write Speed: 2532.18KB/s, Read Speed: 3021.68KB/s
   Test completed, PASSED

