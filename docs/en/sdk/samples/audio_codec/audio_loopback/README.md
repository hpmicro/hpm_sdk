# Audio Codec Loopback

## Overview

The audio_codec example shows how to use codec component to record and play audio information.
In this example, codec component records the sound and sents out encoded data by I2S interface,
MCU send back the received data to codec component, then codec component decodes and plays the audio data.

## Project Configuration

- In the file `CMakeLists.txt`, set a matched audio codec type according to the development board schematic，e.g. "set(CONFIG_CODEC "sgtl5000")"

## Board Setting

1. Choose one of the following audio input methods based on the board schematic:
    - Connect the music player to the LINE_IN interface of the onboard codec using a 3.5mm audio cable to record audio data.
    - Use the onboard codec's microphone to record audio data.
2. Connect headphone to the [headphone](lab_board_app_headphone) interface on the audio codec chip to play audio data

## Running the example

When the example runs successfully, the headphone will play the sound and the following message is displayed in the terminal:
```console
Audio Codec example
Test Codec playback and record
```

