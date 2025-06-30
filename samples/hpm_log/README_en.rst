.. _hpm_log:

HPM Log
======================

Overview
--------

The hpm_log project demonstrates how to use the hpm_log component. It illustrates the adaptation method of the hardware engine by using the serial port as the output engine. Compared with the traditional printf, hpm_log has features such as non-blocking operation and timestamp, which make it easier to debug and analyze application programs.

Board Setting
-------------

No special settings

Running the example
-------------------

When the project runs correctly, and the serial port terminal will output the following information:

.. code-block:: console

   ==============================
   hpm6800evk clock summary
   ==============================
   cpu0:		 600000000Hz
   gpu0:		 400000000Hz
   axis:		 200000000Hz
   axic:		 200000000Hz
   axif:		 400000000Hz
   axid:		 266666666Hz
   axiv:		 200000000Hz
   axig:		 400000000Hz
   mchtmr0:	 24000000Hz
   xpi0:		 133333333Hz
   ==============================

   ----------------------------------------------------------------------
   $$\   $$\ $$$$$$$\  $$\      $$\ $$\
   $$ |  $$ |$$  __$$\ $$$\    $$$ |\__|
   $$ |  $$ |$$ |  $$ |$$$$\  $$$$ |$$\  $$$$$$$\  $$$$$$\   $$$$$$\
   $$$$$$$$ |$$$$$$$  |$$\$$\$$ $$ |$$ |$$  _____|$$  __$$\ $$  __$$\
   $$  __$$ |$$  ____/ $$ \$$$  $$ |$$ |$$ /      $$ |  \__|$$ /  $$ |
   $$ |  $$ |$$ |      $$ |\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |
   $$ |  $$ |$$ |      $$ | \_/ $$ |$$ |\$$$$$$$\ $$ |      \$$$$$$  |
   \__|  \__|\__|      \__|     \__|\__| \_______|\__|       \______/
   ----------------------------------------------------------------------
   hpm log
   0.084890 [D] main:82:  TICK 0
   0.084928 [I] main:83:  TICK 1
   0.084940 [W] main:84:  TICK 2
   0.084952 [E] main:85:  TICK 3
   1.086635 [D] main:82:  TICK 0
   1.086651 [I] main:83:  TICK 1
   1.086662 [W] main:84:  TICK 2
   1.086674 [E] main:85:  TICK 3
   2.088353 [D] main:82:  TICK 0
   2.088369 [I] main:83:  TICK 1
   2.088380 [W] main:84:  TICK 2
   2.088392 [E] main:85:  TICK 3
   3.090071 [D] main:82:  TICK 0
   3.090087 [I] main:83:  TICK 1
   3.090098 [W] main:84:  TICK 2
   3.090110 [E] main:85:  TICK 3
   4.091789 [D] main:82:  TICK 0
   4.091804 [I] main:83:  TICK 1
   4.091816 [W] main:84:  TICK 2
   4.091829 [E] main:85:  TICK 3


