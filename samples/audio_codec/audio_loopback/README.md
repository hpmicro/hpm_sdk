# Audio Codec Loopback
## Overview

The audio_codec example shows how to use codec component to record and play audio information.
In this example, codec component records the sound and sents out encoded data by I2S interface,
MCU send back the received data to codec component, then codec component decodes and plays the audio data.

## Board Setting

1. Connect audio player to the LINE_IN interface on the board
2. Connect headphone to the HP_MIC interface on the board

## Running the example

When the example runs successfully, the headphone will play the sound and the following message is displayed in the terminal:
```

> Audio Codec example
> Test Codec playback and record

```

