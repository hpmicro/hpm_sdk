.. _mtg_sei_tamagawa_example:

MTG SEI tamagawa example
================================================

Overview
--------

The MTG SEI tamagawa routine demonstrates the function of using SEI to communicate with the tamagawa encoder to obtain data, which is processed by the MTG filter, and time compensated by the trajectory prediction module. The routine outputs the raw data of SEI and the data processed by MTG, and outputs the difference between the two.

Board Setting
-------------

- Connect master's SEI pins DATA_P/DATA_N to encoder data pins.

- Connect encoder VDD/GND to board +5V/GND.

Running the example
-------------------

Started sei engine!
mtg sei tamagawa example
SEI ISR CNT, TIME, SEI_POS, MTG_POS, MTG_VEL, MTG_POS-QEI_POS

8,0.112206,0.468197,0.468151,0,-0.000047
12,0.115955,0.468197,0.468151,0,-0.000046
16,0.119720,0.468197,0.468151,0,-0.000046
20,0.123484,0.468197,0.468151,0,-0.000046
23,0.127249,0.468197,0.468151,0,-0.000046
27,0.131013,0.468197,0.468151,0,-0.000047
31,0.134777,0.468197,0.468151,0,-0.000046
35,0.138542,0.468197,0.468151,0,-0.000047
38,0.142306,0.468197,0.468151,0,-0.000047
42,0.146071,0.468197,0.468151,0,-0.000046
46,0.149835,0.468197,0.468151,0,-0.000047
50,0.153599,0.468197,0.468151,0,-0.000046
53,0.157363,0.468197,0.468151,0,-0.000046
57,0.161128,0.468197,0.468151,0,-0.000047
61,0.164892,0.468197,0.468151,0,-0.000046