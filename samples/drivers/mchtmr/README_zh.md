# 机器定时器
## 概述
***
**mchtmr**机器定时器驱动示例工程展示了如何通过机器定时器模块，实现定时功能
- 示例代码将**led8**配置为输出功能，配置机器定时器周期**3秒**，使LED灯按照定时器配置定时闪烁

## 硬件设置
***
-  无特殊设置

## 运行现象
***
当工程正确运行后，``led8``被点亮，间隔3秒定时闪烁，而串口终端会输出如下信息：
```
> mchtmr example
> LED shall be flashing every 3s
> mchtmr interrupt!
```
若开发板上没有可用LED，将会显示
```
> mchtmr example
> mchtmr interrupt will occur every 3s
> mchtmr interrupt!
```

