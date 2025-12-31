.. _tensorflowlitemicro_persondetection:

TensorFlowLiteMicro-PersonDetection
======================================================================

概述
------

TFLM示例工程展示了使用摄像头采集图像信息，通过集成的TFLM机器学习平台进行人体检测，并通过屏幕显示结果

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

代码选项
------------

- 请在该工程的CMakeLists.txt的`find_package`前添加如下代码，获得更高性能


.. code-block:: cmake

           set(RV_ABI "ilp32d")
           set(RV_ARCH "rv32gc")
           set(CONFIG_HPM_MATH_NN_RVP32 1)



运行现象
------------

当工程正确运行后，LCD会显示摄像头采集的实时画面，以及处理后的图像和识别结果，而串口终端会输出是否有人的得分


.. code-block:: text

   person score:-23 no person score 23

