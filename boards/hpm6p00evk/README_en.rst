.. _hpm6p00evk:

HPM6P00EVK
==========

Overview
--------

The HPM6P00 is a dual-core MCU running 600MHz with high computing power and abundant peripheral resources.

The HPM6P00EVK is an evaluation board for the HPM6P00 series MCU, providing a range of interfaces including ADC input with SMA interface, SDM input with SMA interface, motor control interface (QEO/QEI/SEI/PWM/ADC), CAN interface, Ethernet interface, USB interface, audio interface, PPI/FREMC interface, and Raspberry Pi interface. It also integrates an onboard debugger FT2232 for easy debugging by users.

.. image:: doc/hpm6p00evk.png
   :alt: hpm6p00evk

Hardware
--------

- HPM6P00 MCU (600MHz dual-core, 1MB Flash and 800KB SRAM on chip)
- Onboard Memory

  - 16MB Quad SPI NOR Flash

- Ethernet

  - 1000 Mbits PHY

- USB

  - USB type C (USB 2.0 OTG) connector x1

- Audio

  - Line in
  - Mic
  - DAO

- Motor

  - RS422
  - RS485

- AD sensor

  - NSI1306W25

- Others

  - LED
  - CAN

.. note::

  - **When using FEMC(SDRAM) or PPI peripherals, please insert the corresponding expansion board into the PPI/FEMC interface**

    - The standard expansion board for HPM6P00EVK evaluation purposes is as follows, with a 16 bits SDRAM (FEMC access), a 16 bits SRAM (FEMC or PPI access) and a parallel ADC (PPI access).

      .. image:: doc/hpm6p00evk_ext.png
        :alt: hpm6p00evk_ext

  - The FEMC/PPI interface has high flexibility. If you need to evaluate other parallel port devices, such as FPGA, ASYNC SRAM, etc., you can design your own expansion board or contact us.

DIP Switch
----------

.. list-table:: DIP Switch
   :header-rows: 1

   * - bit[2:1]
     - Description
   * - OFF, OFF
     - Boot from Quad SPI NOR flash
   * - OFF, ON
     - Boot from eMMC
   * - ON, OFF
     - ISP

.. note::
  - **On HPM6P00EVKRevB, when the ISP mode is selected and Segger or Ozone is used for online debugging, network initialization will fail due to the multiplexing of USB0_P and ENet RXCK signals and the differential output from the USB controller.**

.. _hpm6p00evk_buttons:

Button
------

.. list-table:: Button Functions
   :header-rows: 1

   * - Name
     - FUNCTIONS
   * - PC23 (KEYA)
     - GPIO Button
   * - WBUTN (WKUP)
     - WAKE UP Button
   * - RESETN (RESET)
     - Reset Button

Plug-in
-------

- SEI CLK Section:

  .. list-table:: SEI CLK Section
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - SEI.CLK Section
       - J3
       - Master side, CLKO active. Slave side, CLKI active

- PWM/RDC interface section

  .. list-table:: PWM/RDC interface section
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - PWM/RDC interface
       - J20
       - PWM side, the J4.PWM_UH pin is the PWM_UH function.

         RDC side, the J4.PWM_UH pin is the RDC0 function

- USB/RGMII interface section

  .. list-table:: USB/RGMII interface section
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - USB/RGMII interface
       - JP8/JP9
       - All disconnected: can use USB interface.

         All connected: can use RGMII interface

- SDM/QEO interface section

  .. list-table:: SDM/QEO interface section
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - SDM interface
       - JP1/JP2/JP7/JP4/JP5/JP6
       - Disconnect JP1/JP2/JP7

         Connect JP4/JP5/JP6, R71(0Ω) needs to be connected on HPM6P00EVKRevB.
     * - QEO interface
       - JP1/JP2/JP7/JP6
       - Connect JP1/JP2/JP7

         Disconnect JP6, R71(0Ω) needs to be removed on HPM6P00EVKRevB.

- Debug interface select

  .. list-table:: Debug interface select
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - Debug interface select
       - J17
       - All connected: use the on-board ft2232,

         All disconnected: use JTAG interface

- PPI/FEMC interface

  .. list-table:: PPI/FEMC interface
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - PPI/FEMC interface
       - CN1
       - Connecting to PPI or FEMC expansion board

- PPI/FEMC expansion board interface select

  .. list-table:: PPI/FEMC expansion board interface
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - FEMC: SDRAM and SRAM can work simultaneously
       - J2、J3、J4、JP1、JP2
       - connect J2/J3/J4 to FEMC side, disconnect JP1/JP2
     * - PPI: ADC and SRAM can work simultaneously
       - J2、J3、J4、JP1、JP2
       - connect J2/J3/J4 to PPI side, connect JP1/JP2
     * - Other: ADC input select
       - J5
       - The ADC voltage can come from DAC or on-board adjustable resistor voltage division, selected by J5

.. _hpm6p00evk_pins:

Pin Description
---------------

- PUART Pin:
  The PUART is used for low power mode testing, such as wakeup, etc.

  .. list-table:: PUART Pin
     :header-rows: 1

     * - Function
       - Pin
       - Position
     * - PUART.TX
       - PY0
       - P5[10]
     * - PUART.RX
       - PY1
       - P5[8]

- UART0 Pin:

  The UART0 use for core0 debugger console:

  .. list-table:: UART0 Pin
     :header-rows: 1

     * - Function
       - Pin
       - Position
     * - UART0.TX
       - PA00
       - DEBUGUART0
     * - UART0.RX
       - PA01
       - DEBUGUART0

- UART4 Pin:

  The UART4 is used for core1 debugger console or some functional testing using UART, such as MICROROS_UART, USB_CDC_ACM_UART, MODBUS_RTU, lin etc.

  .. list-table:: UART4 Pin
     :header-rows: 1

     * - Function
       - Pin
       - Position
       - Remark
     * - UART4.TX
       - PY00
       - P5[8]
       -
     * - UART4.RX
       - PY01
       - P5[10]
       -
     * - UART4.break
       - PY05
       - P5[24]
       - Generate UART break signal

- CAN Pin

  .. list-table:: CAN Pin
     :header-rows: 1

     * - Function
       - Position
     * - CAN_H
       - J7[0]
     * - CAN_L
       - J7[2]

- Audio Pin

  .. list-table:: Audio Pin
     :header-rows: 1

     * - Function
       - Position
     * - Speaker Left Channel
       - J11
     * - Speaker Right Channel
       - J12
     * - 3.5mm
       - J10
     * - DAO Interface
       - J5

- ADC Pin

  .. list-table:: ADC Pin
     :header-rows: 1

     * - Function
       - Position
     * - ADC Input
       - J4[2]

- ACMP pin

  .. list-table:: ACMP pin
     :header-rows: 1

     * - Function
       - Position
     * - CMP4.INN6
       - J4[2]

- Quadrature Encoder Pin

  .. list-table:: Quadrature Encoder Pin
     :header-rows: 1

     * - Function
       - Position
     * - QEI.A / HALL.U
       - J4[1]
     * - QEI.B / HALL.V
       - J4[3]
     * - QEI.Z / HALL.W
       - J4[5]
     * - QEO.A
       - J4[26]
     * - QEO.B
       - J4[24]
     * - QEO.Z
       - J4[22]

- HALL Pin

The HALL pin of the hpm6200evk needs to be connected.

.. list-table::
    :header-rows: 1

    * - Function
      - EVK Position
      - Motor Position
    * - HALL.U
      - J4[1]
      - J22[3]
    * - HALL.V
      - J4[3]
      - J22[4]
    * - HALL.W
      - J4[5]
      - J22[5]
    * - GND
      - J4[32]
      - J22[1]

- PWM Output Pin

  .. list-table:: PWM Output Pin
     :header-rows: 1

     * - Function
       - Position
     * - PWM.WL / PWM1.P5
       - J4[12]
     * - PWM.WH / PWM1.P4
       - J4[11]
     * - PWM.VL / PWM1.P3
       - J4[10]
     * - PWM.VH / PWM1.P2
       - J4[9]
     * - PWM.UL / PWM1.P1
       - J4[8]
     * - PWM.UH / PWM1.P0
       - J4[7]
     * - PWM.FAULT
       - J4[22]

- SEI Pin

  .. list-table:: SEI Pin
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - SEI.CLK_IN_P
       - J4[29]
       - Clock differential output in host mode P
     * - SEI.CLK_IN_N
       - J4[31]
       - Clock differential output in host mode N
     * - SEI.CLK_OUT_P
       - J4[27]
       - Clock differential input in slave mode P
     * - SEI.CLK_OUT_N
       - J4[25]
       - Clock differential input in slave mode N
     * - SEI.DATA_P
       - J4[23]
       - Data differential Signal P
     * - SEI.DATA_N
       - J4[21]
       - Data differential Signal N

- QEIV2 Sin/Cos Pin

  .. list-table:: QEIV2 Sin/Cos Pin
     :header-rows: 1

     * - Function
       - Position
       - Note
     * - ADC2.INA09
       - J4[15]
       - ADC_IW (Cos)
     * - ADC0.INA14
       - J4[13]
       - ADC_IU (Sin)

- RDC pin

  .. list-table:: RDC pin
     :header-rows: 1

     * - Function
       - EVK board Position
       - RDC board Position
     * - RDC.PWM
       - J4[7]
       - J2[7]
     * - RDC.ADC0
       - J4[13]
       - J2[13]
     * - RDC.ADC1
       - J4[14]
       - J2[14]
     * - GND
       - J4[32]
       - J2[17]

- PLB Pulse Output Pin

  .. list-table:: PLB Pulse Output Pin
     :header-rows: 1

     * - Function
       - Position
     * - PLB.PULSE_OUT
       - J4[9]

- PLB LIN Clock Pin

  This pin is used for UART LIN Slave baudrate adaptive demo to detect the clock of RX signal

  .. list-table:: PLB LIN Clock Pin
     :header-rows: 1

     * - Function
       - Position
     * - PLB.TRGM_IN
       - J20[3]

- PLB Filter Output Pin

  .. list-table:: PLB Filter Output Pin
     :header-rows: 1

     * - Function
       - Position
     * - PLB.Filter_IN
       - J4[9]
     * - PLB.Filter_OUT
       - J4[11]

- LOBS Pin

  .. list-table:: LOBS Pin
     :header-rows: 1

     * - Function
       - Pin
       - Position
     * - Trig Pin0
       - PC28
       - P5[3]
     * - Trig Pin1
       - PC29
       - P5[5]

- GPTMR Pin

  .. list-table:: GPTMR Pin
     :header-rows: 1

     * - Function
       - Position
       - Remark
     * - GPTMR4.CAPT_0
       - J4[3]
       -
     * - GPTMR4.COMP_0
       - J4[1]
       - BLCK of i2s emulation
     * - GPTMR0.COMP_0
       - J4[26]
       - LRCK of i2s emulation
     * - GPTMR5.COMP_2
       - J4[5]
       - MCLK of i2s emulation

- CS Pin of i2s emulation  **todo**

  .. list-table:: CS Pin of i2s emulation
     :header-rows: 1

     * - Pin
       - Position
       - Remark
     * - PA11
       - P1[4]
       - The pin that controls the SPI slave CS

- SPI Pin

  .. list-table:: SPI Pin
     :header-rows: 1

     * - Function
       - Pin
       - Position
     * - SPI2.CSN
       - PY05
       - P5[24]
     * - SPI2.SCLK
       - PY04
       - P5[23]
     * - SPI2.MISO
       - PY06
       - P5[21]
     * - SPI2.MOSI
       - PY07
       - P5[19]

- I2C Pin

  .. list-table:: I2C Pin
     :header-rows: 1

     * - Function
       - Position
     * - I2C1.SCL
       - P5[5]
     * - I2C1.SDA
       - P5[3]

- Ethernet PPS PPS Pin  **todo**

  .. list-table:: Ethernet PPS PPS Pin
     :header-rows: 1

     * - Function
       - Pin
       - Position
     * - ENET0.EVTO0
       - PE06
       - J4[24]
     * - ENET0.EVTO1
       - PF20
       - J4[4]
     * - ENET0.EVTI1
       - PE07
       - J4[26]
