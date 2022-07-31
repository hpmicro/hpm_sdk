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

audio codec example
SD card has been mounted successfully


***********Music List**********
0: music1.wav

1: music2.wav

2: music3.wav

3: music4.wav

4: music5.wav

5: music6.wav

6: mucsic7.wav

7: Ring10.wav

8: on_phone.wav

9: calling.wav


**Any non-numeric key to change pages**

Enter Music Number:
2

wav_name:music3.wav.
music playing time:174 seconds ...
music end.

```
