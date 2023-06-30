# I2S DMA

## Overview

This example shows I2S transfer audio data with DMA to audio codec on board.

## Project Configuration

- In the file `CMakeLists.txt`, set a matched audio codec type according to the development board schematic，e.g. "set(CONFIG_CODEC "sgtl5000")"

## Board Setting

- Connect headphone to the [headphone](lab_board_app_headphone) interface on the audio codec chip.

## Running the example

When the example runs successfully, the headset will play the sound, and the following message is displayed in the terminal:
```console
I2S DMA example
I2C bus is ready
i2s dma play finished
```

