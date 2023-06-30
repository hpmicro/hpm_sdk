# Microros Subscriber Performance Test

## Overview

The subscriber example demonstrates the message subscription function of microros nodes through XRCE-Client.

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
You can see the 'subscriber' node.

type
  `ros2 topic list`
You can see/int32_subscriber topic name.

type
  `ros2 node info /subscriber`
You can query the functions and message types of nodes.

type
  `ros2 topic echo /int32_subscriber`
Print out /int32_subscriber messages.

Open a new terminal type
```bash
source /opt/ros/foxy/setup.bash

ros2 topic pub /int32_subscriber std_msgs/msg/Int32 "{data: 1234}"
```
The agent starts publishing messages under/int32_subscriber topics, and you can see the nodes that subscribe to /int32_subscriber topic printing out.
```bash
data: 1234
---
data: 1234
---
data: 1234
---
data: 1234
---
data: 1234
---
```

