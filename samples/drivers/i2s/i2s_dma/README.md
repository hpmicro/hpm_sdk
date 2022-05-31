# I2S DMA
## Overview

The audio_codec example shows how to use codec component to play audio information.
MCU send back the received data to codec component, then codec component decodes and plays the audio data.

## Board Setting

1. Connect headphone to the HP_MIC interface on the board

## Running the example

When the example runs successfully, the headset will play the sound of a sine wave with a frequency of 1KHZ. The 1KHZ sine wave can be obtained by measuring the left channel or the right channel with an oscilloscope, and the following message is displayed in the terminal:
```

> I2S DMA example
> Test Codec playback

```

