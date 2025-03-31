.. _spi_sdcard_fatfs:

SPI SDCARD FATFS
================================

概览
------

本示例使用SPI接口操作sdcard，并且使用fatfs文件系统。测试需要格式化SD卡为FAT32，提供如下功能:
- 创建hello.txt

- 从hello.txt读出一行文本

- 创建并删除文件夹

- 大文件测试

板级设置
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

运行示例
------------

当示例成功运行后，会显示如下菜单，选择相对应的测试项会提示。


.. code-block:: console

   ---------------------------------------------------------------
   *                                                             *
   *                   spi sdcard fatfs demo                     *
   *                                                             *
   *        1 - Create hello.txt                                 *
   *        2 - Read 1st line from hello.txt                     *
   *        3 - Directory related test                           *
   *        4 - Large file write test                            *
   *-------------------------------------------------------------*
   SD card has been mounted successfully
   1
   Create new file successfully, status=0
   Write file　operation is successfully
   2
   Open file successfully
   Hello, this is SPI SD card FATFS demo

   3
   Creating new directory succeeded
   Removing new directory succeeded
   4
   Create new file successfully, now writing.....
   Write file operation is successful

