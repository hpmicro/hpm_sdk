.. _canopen_master:

CANOPEN_MASTER
==============

Overview
--------

The canopen_master_demo provides the following features:

- HeartBeat test

- NMT test

- SDO test

- Supports hot-swapping

Board Setting
-------------

Download master routines on one board, download slave routines on another board, connect two boards

Running Steps
-------------

After the program is successfully downloaded, the master station will automatically enter master station mode and listen for slave devices on the bus (nodes 10-11, which can be configured through the master station's object dictionary). When a slave device is found, the master station will read the relevant information of the slave device through protocols such as NMT and SDO, and set the heartbeat cycle of the slave device.

Running the example
-------------------

When the example runs successfully, the board shows the following:

.. code-block:: console

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
   rxSize = 18, txSize = 11
   CANopen Master Started. Monitoring IDs 10-11

Test steps
~~~~~~~~~~

The current routine is the master role of CANopen. The board runs the current routine and connects to the board running the slave role.

If success, Master board show as：

.. code-block:: console

   [BUS-0] Configuring Node 10...
   [Device Type]: 0x00000000
   [Device Name]: HPMICRO CANopen Demo
   [HW Version ]: 1.0
   [SW Version ]: 1.0
   [Set HB (ms) ]: Success
   [BUS-0] Node 10 Operational. Config Complete.

