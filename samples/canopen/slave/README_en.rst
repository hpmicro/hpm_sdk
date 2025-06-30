.. _canopen_slave:

CANOPEN_SLAVE
==============

Overview
--------

The canopen_demo provides the following features:

- HeartBeat test

- NMT test

- SDO test

- PDO test

Board Setting
-------------

Download master routines on one board, download slave routines on another board, connect two boards

Running Steps
-------------

After the program is successfully downloaded, it will actively send pre optional information and wait for the master to issue instructions

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
   rxSize = 11, txSize = 8
   excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
   enter slave mode ~ ~


Test steps
~~~~~~~~~~

The current routine is a canopen slave role. The board runs the current routine and connects to the board running the master role.

Slave board show as:

.. code-block:: console

   rxSize = 11, txSize = 8
   excessive number of concurrent CAN RX filters enabled (needs 11, 16 available)
   enter slave mode ~ ~

There will be no special printing on the slave side, and the test results will be judged based on the master side printing.

