# Audio Wav Decoder
## Overview

The audio codec sample project shows how to play WAV format audio.
In this project, the wav format file from the SD card is read and then played,
MCU send back the received data to codec component, then codec component decodes and plays the audio data.

## Board Setting

1. Connect headphone to the HP_MIC interface on the board

## Running the example

When the project runs correctly, select the music name and the headset will play this music, while the serial terminal will output the following message:

```

I2C bus is ready
audio codec example
SD card has been mounted successfully
Test WAV decode
Please input wav name:
wav_name:music1.wav.
music playing time:202 seconds ...
music end.

```
