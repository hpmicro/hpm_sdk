# I2S Interrupt
## Overview

The audio_codec example shows how to use codec component to play audio information.
MCU send back the data to codec component, then codec component decodes and plays the audio data.

## Project Configuration

- In the file [cmakelists.txt](./CmakeLists.txt), set a matched audio codec type according to the development board schematic，e.g. "set(CONFIG_CODEC "sgtl5000")"

## Board Setting

- Connect headphone to the [headphone](lab_board_app_headphone) interface on the audio codec chip.

## Running the example

When the example runs successfully, the headset will play the sound of a sine wave with a frequency of 1KHZ. The 1KHZ sine wave can be obtained by measuring the left channel or the right channel with an oscilloscope, and the following message is displayed in the terminal:
```

> I2S Interrupt example
> Test Codec playback

```

