.. _hpm5301evklite:

HPM5301EVKLITE
==============

Overview
--------

HPM5301EVKLite is a development board based on Xianji's entry-level high-performance MCU HPM5301. HPM5301EVKLite provides a USB Type-C interface for high-speed USB-OTG functionality, with onboard buttons and LEDs for convenient user interaction. It also provides an extension interface that is compatible with Raspberry Pi and a standard JTAG debugging interface.

.. image:: doc/hpm5301evklite.png
   :alt: hpm5301evklite

Console information printing
----------------------------

By default, UART0 is used for console printing. Connect UART0.TXD (J3.36) and UART0.RXD (J3.38) externally through the USB to serial port tool.

.. list-table:: Boot Switch
   :widths: 20 80
   :header-rows: 1

   * - KEY
     - Description
   * - OFF
     - Boot from Quad SPI NOR flash
   * - ON
     - ISP

.. _hpm5301evklite_buttons:

Button
------

.. list-table:: Button
   :widths: 20 80
   :header-rows: 1

   * - Name
     - FUNCTIONS
   * - RESET
     - Reset Button
   * - KEY&BOOT
     - User Key & Boot switch

.. _hpm5301evklite_pins:

Pin Description
---------------

- **UART Pin**: modbus_rtu sample
  - The UART0 used for debugger console or some functional testing using UART
  - The UART3 is used for some functional testing using UART, such as MICROROS_UART, USB_CDC_ACM_UART, MODBUS_RTU etc.

.. list-table:: UART Pin
   :widths: 20 20 60
   :header-rows: 1

   * - Function
     - Position
     - Remark
   * - UART3.TXD
     - J3[8]
     -
   * - UART3.RXD
     - J3[10]
     -
   * - UART0.TXD
     - J3[36]
     -
   * - UART0.RXD
     - J3[38]
     -
   * - UART3.break
     - J3[24]
     - generate uart break signal

- **SPI Pin**:

.. list-table:: SPI Pin
   :widths: 50 50
   :header-rows: 1

   * - Function
     - Position
   * - SPI1.CSN
     - J3[24]
   * - SPI1.SCLK
     - J3[23]
   * - SPI1.MISO
     - J3[21]
   * - SPI1.MOSI
     - J3[19]

- **I2C Pin**:

.. list-table:: I2C Pin
   :widths: 50 50
   :header-rows: 1

   * - Function
     - Position
   * - I2C3.SCL
     - J3[28]
   * - I2C3.SDA
     - J3[27]

- **ACMP Pin**:

.. list-table:: ACMP Pin
   :widths: 50 50
   :header-rows: 1

   * - Function
     - Position
   * - ACMP.CMP1.INN4
     - J3[13]
   * - ACMP.COMP_1
     - J3[3]

- **ADC16 Pin**:

.. list-table:: ADC16 Pin
   :widths: 50 50
   :header-rows: 1

   * - Function
     - Position
   * - ADC0.INA2
     - J3[26]
   * - ADC1.INA1
     - J3[3]

- **TinyUF2 Pin**:

  .. note::

     - PA9 connect GND, and press reset, board enter DFU mode, then PA9 connect 3.3V, drag app to U disk, will download app and enter app directly if successfully;
     - PA9 connect 3.3V, and press reset, board enter bootloader mode, if flash has the valid app, will directly enter app;

.. list-table:: TinyUF2 Pin
   :widths: 50 50
   :header-rows: 1

   * - Function
     - Position
   * - TinyUF2 Button
     - J3[32]

- **GPTMR Pin**:

.. list-table:: GPTMR Pin
   :widths: 20 20 60
   :header-rows: 1

   * - Function
     - Position
     - Remark
   * - GPTMR0.CAPT_1
     - J3[3]
     -
   * - GPTMR0.COMP_1
     - J3[5]
     -
   * - GPTMR0.COMP_3
     - J3[8]
     - BCLK of i2s emulation
   * - GPTMR0.COMP_2
     - J3[26]
     - LRCK of i2s emulation
   * - GPTMR1.COMP_1
     - J3[7]
     - MLCK of i2s emulation

- **CS Pin of i2s emulation**

.. list-table:: CS Pin of i2s emulation
   :widths: 20 20 60
   :header-rows: 1

   * - Function
     - Position
     - Remark
   * - PA31
     - J3[11]
     - the pin that controls the SPI slave CS

- CLOCK REF Pin

  .. list-table::
     :header-rows: 1

     * - Function
       - Position
     * - PA09
       - J3[32]


