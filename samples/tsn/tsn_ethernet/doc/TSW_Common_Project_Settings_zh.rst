.. _tsw_port_configurations_zh:

**TSW端口配置**
======================

- 对于仅支持单网口的sample：开发板上有双网口，在`CMakeLists.txt`中，支持如下配置：

  - set(APP_USE_TSW_PORT_COUNT 1): 使能开发板上TSW对应的RGMII端口 <sup> **1/2** </sup>

**注** :

1. 端口类型取决于硬件设计
2. 以太网PHY类型由CMake构建过程根据硬件设计自动匹配
