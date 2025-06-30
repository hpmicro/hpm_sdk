.. _canopen_master:

CANOPEN_MASTER
==============

Overview
--------

The canopen_master_demo provides the following features:

- HeartBeat test

- NMT test

- SDO test

- PDO test

Board Setting
-------------

Download master routines on one board, download slave routines on another board, connect two boards

Running Steps
-------------

After the program is successfully downloaded, master will enter a loop to wait for slave message. If the slave is in initial/pre optional/optional state, read the nodeid. Otherwise, continue the loop to wait

Read message from the slave through PDO based on the nodeid

Read/write message from/to the slave through SDO based on the nodeid

Note: This sample is intended to provide a simple presentation of the canopen communication process. Canopen supports one master and multiple slaves, but this sample only supports one master and one slave

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
   enter master mode and find slave device ~~
   rxSize = 11, txSize = 8
   excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)

Test steps
~~~~~~~~~~

The current routine is the master role of canopen. The board runs the current routine and connects to the board running the slave role.

If success, Master board show as：

.. code-block:: console

   enter master mode and find slave device ~~
   rxSize = 11, txSize = 8
   excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
   find device, id:70a, status preoptional
   slave enter to optional mode
   PDO test successfully!
   slave HBtime: 1000 ms
   slave HBtime: 2000 ms
   SDO test successfully!
   canopen test done

