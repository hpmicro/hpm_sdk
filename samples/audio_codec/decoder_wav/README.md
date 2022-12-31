# Audio Wav Decoder
## Overview

The audio codec sample project shows how to play WAV format audio.
In this project, the wav format file from the SD card is read and then played,
MCU send back the received data to codec component, then codec component decodes and plays the audio data.

## Project Configuration

- In the file [cmakelists.txt](./CmakeLists.txt), set a matched audio codec type according to the development board schematic，e.g. "set(CONFIG_CODEC "sgtl5000")"

## Board Setting

- According to project configuration, connect speaker to [DAO](lab_board_app_dao) interface if using DAO as player, connect headphone to [headphone](lab_board_app_headphone) interface if using audio codec as player.

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
