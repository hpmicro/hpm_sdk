.. _tamagawa_encoder_communication:

Tamagawa Encoder Communication
==============================

Overview
--------

This example demonstrates communication with Tamagawa encoders using multiple peripherals (PLA, DMA, SPI, QEI, PWM). Main features include:

- Support for multiple DataID data acquisition:
  - ID0/ID7/ID8/IDC: Get absolute position data
  - ID1: Get multi-turn data
  - ID2: Get ID information
  - ID3: Get complete status (including position, multi-turn, ID and alarm information)

- EEPROM operations:
  - Support EEPROM read/write
  - Automatic EEPROM busy status detection
  - Address range: 0-127

- Complete error detection mechanism:
  - CRC verification
  - Communication error detection
  - Encoder error status monitoring
  - Command response verification

- High-precision timing control:
  - Automatic communication delay compensation
  - PLA-based precise timing
  - DMA chained transfer

Board Setting
-------------

- Core board
- Expansion board: TMAGAWA RevA
- Supported encoder model: TS5700N8401
- Wiring requirements:
  - Plug TMAGAWA RevA into the core board
  - Correctly connect encoder to expansion board
  - Ensure J2's P1 and P2 on expansion board are connected

Software Architecture
---------------------

Main modules:

1. Protocol Layer (tamagawa_proc):
   - Command formatting
   - CRC verification
   - Data parsing
   - Error handling

2. Hardware Driver Layer:
   - PLA configuration: For precise timing and signal synchronization
   - DMA setup: Implement efficient data transfer
   - SPI configuration: Communication interface with encoder
   - QEI: Capture timing signals
   - PWM: Generate reference clock

3. Application Layer:
   - Device initialization
   - Communication management
   - Data processing and display

Running the example
-------------------

When the project is running correctly, the serial terminal will output the following information, and the position information will change when the encoder axis is rotated:

.. code-block:: console

   cmd:3 time:638, abs pos:4170741, abm pos:0, id:23.
   cmd:3 time:631, abs pos:4170741, abm pos:0, id:23.
   cmd:3 time:631, abs pos:4170740, abm pos:0, id:23.
   cmd:3 time:634, abs pos:4170740, abm pos:0, id:23.
   cmd:3 time:639, abs pos:4170741, abm pos:0, id:23.
   cmd:3 time:632, abs pos:4170741, abm pos:0, id:23.
   cmd:3 time:630, abs pos:4170742, abm pos:0, id:23.

Display information explanation:
- cmd: Current command ID
- time: Communication delay time (unit: system clock cycles)
- abs pos: Absolute position value
- abm pos: Multi-turn position value
- id: Encoder ID

.. note::
 - Call tmgw_message_process function to update the data of tmgw_msg, which contains the information of the above function
 - Switching command, you need to modify tmgw_dev.comm_time_delay_config to turn off the communication function, and then do the configuration of PLA, DMA. Otherwise, it will generate an error message once and then return to normal

