# Pmbus master

## Overview

The PMbus master example demonstrates the functionality of I2C in master mode as a PMbus for receiving and sending data. It uses loops with different PMbus commands, including byte, word, and block instructions, and prints out the length of the read or written data or error codes.

## Board Setting

connect [I2C Pins](lab_board_app_i2c_pin) on the two boards。

## Running the example

- Run slave first, and then run master.

- When the example runs successfully, the log would be seen on the terminal like:

```console

PMbus master test
pmbus master write test begin...
pmbus master write test end...

pmbus master read test begin...
pmbus(cmd:0x00) master read len:1
pmbus(cmd:0x06) master read len:128
pmbus(cmd:0x22) master read len:2
pmbus master read test end...
```