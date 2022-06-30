# 一级高速缓存
## 概述
***
**l1c**本例程显示了一级高速缓存的配置信息

## 端口设置
***
-  串口波特率设置为``115200bps``，``1个停止位``，``无奇偶校验位``

## 运行现象
***
当工程正确运行后，串口终端会输出如下信息：
```

l1 cache driver example
icache config:
  set: 128
  way: 4-way
  size: 64 bytes
  lock: with locking support
  ecc: No parity/ECC
  local memory ecc: No parity/ECC
dcache config:
  set: 128
  way: 4-way
  size: 64 bytes
  lock: with locking support
  ecc: No parity/ECC
  local memory ecc: No parity/ECC
dcache is enabled
icache is enabled

```

