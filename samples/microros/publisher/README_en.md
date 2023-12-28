# Microros Publisher Performance Test

## Overview

The publisher example demonstrates the message publishing function of microros nodes through XRCE-Client。

## Board Setting

- One ubuntu host with ROS2 foxy version installed
- Connect the host to the development board using a USB to serial port. The serial port number used by the development board can be found in board.h.

## Environment Setup
ros2 foxy install:

`1.`Installation steps on the official website [https://docs.ros.org/en/foxy/Installation/Alternatives/Ubuntu-Development-Setup.html].

`2.`Installing through APT(If necessary，change the source of ubuntu)
```bash
sudo apt update
sudo apt install ros-foxy-desktop
```
After installing ROS2 foxy, it is necessary to install the agent on the host side, which is installed through Docker
```bash
docker run -it --rm --net=host -v /dev/shm:/dev/shm -v /dev:/dev --privileged microros/micro-ros-agent:foxy serial --dev ${serial}
```
If you encounter a situation where the serial port cannot be opened, please change the permissions for the serial port
```bash
sudo chmod -R 777 ${serial}
```
After running the agent, reset the board.

## Running the example
Open a new terminal
```bash
source /opt/ros/foxy/setup.bash
```
type
  `ros2 node list`
You can see the "publisher" node.

type
  `ros2 topic list`
You can see/int32_ Publisher topic name.

type
  `ros2 node info /publisher`
You can query node functions and message types

type
  `ros2 topic echo /int32_publisher`
You can see the content of the published message. In this example, a 32-bit number is sent every 1 second, increasing each time.
```bash
data: 0
---
data: 1
---
data: 2
---
data: 3
---
data: 4
---
data: 5
---
data: 6
---
```
