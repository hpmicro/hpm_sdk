# SPI SDCARD FATFS

## 概览

本示例使用SPI接口操作sdcard，并且使用fatfs文件系统。测试需要格式化SD卡为FAT32，提供如下功能:
- 创建hello.txt
- 从hello.txt读出一行文本
- 创建并删除文件夹
- 大文件测试

## 板级设置

- [SPI引脚](lab_board_app_spi_pin)根据板子型号查看具体信息
- SPI引脚对应好SD卡(模块)引脚

## 运行示例

当示例成功运行后，会显示如下菜单，选择相对应的测试项会提示。

```console
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


```
