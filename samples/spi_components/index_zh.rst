.. _samples_spi_components:

SPI Components
==============

参考
------

- 组件详情文档请参见 :ref:`spi_component <spi_component>`

注意
------

- 主机例子中的CS使用GPIO模拟，方便管理控制收发。
- 主机例子中的CS如果需要使用硬件CS自动控制，可以初始化或者收发之前使用 `spi_master_enable_cs_select` API接口进行切换。
- `spi_master_enable_cs_select` API接口支持需要看SOC的 `hpm_soc_ip_feature.h` 是否定义 `HPM_IP_FEATURE_SPI_CS_SELECT` 宏。

示例
------

.. toctree::
   :maxdepth: 3

   full_duplex/dma/master/README_zh
   full_duplex/dma/slave/README_zh
   full_duplex/polling/master/README_zh
   full_duplex/polling/slave/README_zh
   half_duplex/dma/master/README_zh
   half_duplex/dma/slave/README_zh
   half_duplex/polling/master/README_zh
   half_duplex/polling/slave/README_zh
