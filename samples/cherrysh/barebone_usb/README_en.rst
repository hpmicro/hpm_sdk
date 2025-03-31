.. _cherry_shell_barebone_usb:

Cherry Shell Barebone USB
==================================================

Overview
--------

The Cherry Shell Barebone example project demonstrates the basic usage of shell in bare-metal mode. In this project, you need to connect via USB PORT, and the shell requires a login. The default password is 12345678. After entering the password and pressing Enter, you will enter the shell. You can use the "help" command to view the available commands and variables.

Please refrain from using USB PORT assistants and consider downloading and using `MobaXterm <https://mobaxterm.mobatek.net/download.html>`_

Board Setting
-------------

No special settings

Running the example
-------------------

- After the project runs successfully, the serial terminal will output the following information:


.. code-block:: console

   [I/USB] Open ep:0x83 type:3 mps:8
   [I/USB] Open ep:0x01 type:2 mps:512
   [I/USB] Open ep:0x81 type:2 mps:512
   Initialize shell successfully


- If you manually enter the correct password "12345678" and press Enter, the terminal will output the following information:


.. code-block:: console

   [I/USB] Open ep:0x83 type:3 mps:8
   [I/USB] Open ep:0x01 type:2 mps:512
   [I/USB] Open ep:0x81 type:2 mps:512
   Initialize shell successfully

- open usb cdc port

- If you manually enter the command "help" and press Enter, the terminal will output the following information:


.. code-block:: console

   cherry@hpm5301evklite:/$ help
   total function 6
     test       -> /bin
     toggle_led -> /bin
     write_led  -> /bin
     exit       -> /sbin
     help       -> /sbin
     shsize     -> /sbin

   total variable 2
     $PATH r-  11
     $ZERO r-   1
   cherry@hpm5301evklite:/$


- If you manually enter the command "toggle_led" and press Enter, the LED will toggle.

- If you manually enter the command "write_led 1" and press Enter, the LED will turn on.

- If you manually enter the command "write_led 0" and press Enter, the LED will turn off.
