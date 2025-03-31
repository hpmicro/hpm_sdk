.. _qeiv2_sin_mode_togi:

QEIV2 Sin Mode TOGI
======================================

Overview
--------

**Qeiv2 Sin TOGI Mode**  project demonstrates the configuration of the single Sin mode TOGI function. In single Sin mode, when TOGI is enabled and w_param is set, QEIV2 will calculate another set of orthogonal signals and use the orthogonal sine and cosine signals to calculate the phase position, thereby improving the accuracy of phase position calculation.

This application can be used for phase calculation during grid connection, such as solar power generation. To integrate the generated electricity into the grid, it is necessary to calculate the phase of the grid, so that the phase of the electricity to be connected to the grid is consistent with the phase of the grid.

Configurations
--------------

- Use a signal generator to generate a set of Sin sine waves with a frequency of 50Hz, peak to peak value of 2V, and center offset of 1.65V.

- Connect sine signal to QEIV2 Sin pin, as detailed in the development board documentation :ref:`QEIV2 Sin/Cos Pin <board_resource>` .

Running the Demo
----------------

- Sin sine and cosine wave injection, simulating single-phase signals in the power grid.

- After powering on, open the serial port terminal and print 2000 data messages. ADC is the sine waveform, pos and ang are the phases, and 0-360 degrees correspond to 0x0-0xFFFFFFFF.


.. code-block:: console

   adc:0x7b96, pos:0x205aff, ang:0x205b00, z:0x1
   adc:0x7c42, pos:0x91d5ff, ang:0x91d600, z:0x1
   adc:0x7c42, pos:0x91d5ff, ang:0x91d600, z:0x1
   adc:0x7d46, pos:0x1038fff, ang:0x1039000, z:0x1
   adc:0x7d46, pos:0x1038fff, ang:0x1039000, z:0x1
   adc:0x7e14, pos:0x17544ff, ang:0x1754500, z:0x1
   adc:0x7e6c, pos:0x1e677ff, ang:0x1e67800, z:0x1
   adc:0x7e6c, pos:0x1e677ff, ang:0x1e67800, z:0x1
   adc:0x7f43, pos:0x257b1ff, ang:0x257b200, z:0x1
   adc:0x80c1, pos:0x2c9a7ff, ang:0x2c9a800, z:0x1
   adc:0x80c1, pos:0x2c9a7ff, ang:0x2c9a800, z:0x1
   adc:0x8129, pos:0x33b29ff, ang:0x33b2a00, z:0x1
   adc:0x8129, pos:0x33b29ff, ang:0x33b2a00, z:0x1
   adc:0x8273, pos:0x3ad2aff, ang:0x3ad2b00, z:0x1
   adc:0x82c7, pos:0x41ea4ff, ang:0x41ea500, z:0x1
   adc:0x82c7, pos:0x41ea4ff, ang:0x41ea500, z:0x1
   adc:0x8362, pos:0x48fe5ff, ang:0x48fe600, z:0x1
   adc:0x845f, pos:0x50150ff, ang:0x5015100, z:0x1
   adc:0x845f, pos:0x50150ff, ang:0x5015100, z:0x1
   adc:0x8494, pos:0x5721dff, ang:0x5721e00, z:0x1
   adc:0x8494, pos:0x5721dff, ang:0x5721e00, z:0x1
   adc:0x86fa, pos:0x5e475ff, ang:0x5e47600, z:0x1
   adc:0x8690, pos:0x65595ff, ang:0x6559600, z:0x1
   adc:0x8690, pos:0x65595ff, ang:0x6559600, z:0x1
   adc:0x8883, pos:0x6c7c0ff, ang:0x6c7c100, z:0x1
   adc:0x8883, pos:0x6c7c0ff, ang:0x6c7c100, z:0x1
   adc:0x87ba, pos:0x73863ff, ang:0x7386400, z:0x1
   adc:0x89c7, pos:0x7aa29ff, ang:0x7aa2a00, z:0x1
   adc:0x89c7, pos:0x7aa29ff, ang:0x7aa2a00, z:0x1
   adc:0x8a08, pos:0x81b60ff, ang:0x81b6100, z:0x1
   adc:0x8b1c, pos:0x88cceff, ang:0x88ccf00, z:0x1
   adc:0x8b1c, pos:0x88cceff, ang:0x88ccf00, z:0x1
   adc:0x8c30, pos:0x8fe6bff, ang:0x8fe6c00, z:0x1


- After the above printing is completed, the following information will be printed periodically, which is interrupted when the phase is at 0 degrees.


.. code-block:: console

   position: 0x2c29ff
   ang: 0x2c2a00
   z: 0x1bf
   position: 0x229dff
   ang: 0x229e00
   z: 0x1c0
   position: 0x1c2dff
   ang: 0x1c2e00
   z: 0x1c1
   position: 0xebdff
   ang: 0xebe00
   z: 0x1c2
   position: 0x647ff
   ang: 0x64800
   z: 0x1c3
   position: 0x6c7fff
   ang: 0x6c8000
   z: 0x1c4
   position: 0x63cbff
   ang: 0x63cc00
   z: 0x1c5
   position: 0x5e46ff
   ang: 0x5e4700
   z: 0x1c6

