.. _person_detection:

Person Detection
================================

概述
------

该示例工程展示了使用摄像头采集图像信息，检测出图像的人并标注

- 通过SIMD指令优化运行速度

- 使用McuNet框架

配置
------

- 连接摄像头设备到开发板的CAM接口

- 连接显示屏到相应接口，显示屏型号  :ref:`LCD-800480W070TC <lcd_800480w070tc>`

工具链要求
------------

本示例需要支持DSP的工具链才能使用硬件DSP指令进行神经网络加速：

- **IDE**: Segger Embedded Studio (或其他支持DSP工具链的IDE)
- **GCC工具链**: Andes RISC-V工具链（带DSP扩展支持）或 ZCC工具链（带DSP扩展支持）

.. note::
   不支持DSP扩展的标准GCC工具链无法正确编译和运行本示例。DSP指令对于高效的神经网络推理至关重要。

代码生成
------------

参考这个`readme <https://github.com/mit-han-lab/tinyengine/tree/main/examples/openmv_person_detection>`_的步骤可以生成codegen中的代码, 生成代码后，从源代码的目录中就可以找到codegen的目录

代码选项
------------

- 为了获得更快的程序运行速度，用户需要修改linkfile。将data 、bss段放到靠近CPU的ram中，如果无法放下请保证genModel.h中的数组放在靠近CPU的RAM中。

运行现象
------------

当工程正确运行后，LCD会显示摄像头采集的实时画面，以及处理后的图像和人脸框
