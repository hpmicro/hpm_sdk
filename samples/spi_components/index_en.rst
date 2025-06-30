.. _samples_spi_components:

SPI Components
==============

Reference
-----------

- For component documentation, refer to :ref:`spi_component <spi_component>`

Notes
-------

- CS signals in master examples are GPIO-emulated for flexible transaction control
- To enable hardware CS control in master examples, invoke `spi_master_enable_cs_select` API during initialization or pre-transaction
- `spi_master_enable_cs_select` availability depends on `HPM_IP_FEATURE_SPI_CS_SELECT` macro definition in SOC's `hpm_soc_ip_feature.h`

Examples
--------

.. toctree::
   :maxdepth: 3

   full_duplex/dma/master/README_en
   full_duplex/dma/slave/README_en
   full_duplex/polling/master/README_en
   full_duplex/polling/slave/README_en
   half_duplex/dma/master/README_en
   half_duplex/dma/slave/README_en
   half_duplex/polling/master/README_en
   half_duplex/polling/slave/README_en
