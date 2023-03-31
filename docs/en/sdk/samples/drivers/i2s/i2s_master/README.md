# I2S Master

## Overview

The I2S master example shows i2s controller as i2s master with DMA  to communicate with i2s slave between board and board.
I2S master reads audio data from I2S slave, and  plays through DAO.

## Board Setting

- Connect [I2S Pins](lab_board_app_i2s_pin) between the two boards.

  Connect Master FCLK with Slave FCLK

  Connect Master BCLK with Slave BCLK

  Connect Master RXD with Slave TXD

  Connect GND together between board and board
  
  Connect a speaker on the DAO interface on the development board

## Running the example

- When the example runs successfully, you can hear a piece of audio about a brief introduction of  HPMicro
