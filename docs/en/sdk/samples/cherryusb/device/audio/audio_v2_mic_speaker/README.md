# USB AUDIO V2 SPEAKER and MICROPHONE

## Overview

This example project shows USB AUDIO V2 speaker and microphone device.

- PC sees a cherry usb speaker and microphone via Device Manager

## Board Setting

- Connect a USB port on PC to the PWR DEBUG port on the development board with a USB Type-C cable
- Connect a USB port on PC to one of USB port on the development board with a USB Type-C cable
- Connect speaker to DAO interface

## Running the example

- Download the program and run. The computer can automatically recognize and install the USB audio driver and enumerate a device with a speaker device and a microphone device.
- Select the speaker device as the default player, and the PC will play audio through the DAO interface
- Select the microphone device as the default audio input device, and the PC will input audio through the PDM interface.