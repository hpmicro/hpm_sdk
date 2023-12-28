# Microros Publisher功能测试

## 概述

publisher例子展示了microros的节点通过XRCE-Client进行消息发布功能。

## 硬件设置

- 准备一台装有ros2 foxy版本的ubuntu主机
- 使用USB转串口将主机与开发板相连,开发板使用的串口号，见board.h

## 环境设置
ros2 foxy版本的安装:

`1.`可见[https://docs.ros.org/en/foxy/Installation/Alternatives/Ubuntu-Development-Setup.html]，根据官网步骤进行安装。

`2.`也可通过apt进行安装(如有需要，请更换源)
```bash
sudo apt update
sudo apt install ros-foxy-desktop
```
安装完毕ros2 foxy后，需要安装主机端的agent，这里通过docker安装。
```bash
docker run -it --rm --net=host -v /dev/shm:/dev/shm -v /dev:/dev --privileged microros/micro-ros-agent:foxy serial --dev ${serial}
```
如果遇到打不开串口的情况，请给串口改变权限
```bash
sudo chmod -R 777 ${serial}
```
运行agent后，复位板子，主机和板子之间建立通信。

## 运行现象
新开一个终端
```bash
source /opt/ros/foxy/setup.bash
```
输入
  `ros2 node list`
可以看到“publisher”节点。

输入
  `ros2 topic list`
可以看到/int32_publisher话题名称。

输入
  `ros2 node info /publisher`
可以查询节点的功能以及消息类型。

输入
  `ros2 topic echo /int32_publisher`
可以看到发布的消息内容，在本例中每隔1秒发送一个32位数，逐次增加。
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