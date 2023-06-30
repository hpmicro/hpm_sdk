# Microros Service Performance Test

## Overview

The service example demonstrates the function of microros nodes performing integer addition operations through XRCE-Client.

## Board Setting

- One ubuntu host with ROS2 foxy version installed.
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
You can see the "service" node.

type
  `ros2 node info /service`
You can query the services and service information of nodes.

type
  `rqt`
Enter the rqt tool and perform the operation of adding two integers.

