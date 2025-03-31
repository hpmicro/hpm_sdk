.. _audio_v1_microphone_speaker_and_midi:

Audio V1 Microphone, Speaker and Midi
==========================================================================

Overview
--------

This example project shows USB Audio V1 speaker, microphone and midi device.

Board Setting
-------------

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable

- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable

- Connect speaker to DAO interface

PC Synth Software
-----------------

 - To test midi on PC, you need to install synth software.

    - Windows: MIDI-OX

    - Linux (Ubuntu): qsynth, qjackctl.

    - MacOS: SimpleSynth

Running the example
-------------------

- Download the program and run. The computer can automatically install the USB audio driver.

- Select the speaker device as the default player, and the PC will play audio through the DAO interface

- Select the microphone device as the default audio input device, and the PC will input audio through the PDM interface.

- Taking Windows MIDI-OX as an example

    - Open MIDI-OX Software, click menu **"options-MIDI devices..."** , MIDI Inputs select "HPMicro UAC V1 DEMO"，MIDI Outputs select "Microsoft GS Wavetable Synth".

    - Pushing PBUTN or User key button that will simulate pressing the piano keys, and users can listen to them through computer default speaker.
