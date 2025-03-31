.. _tensorflowlitemicro_facedetection:

TensorFlowLiteMicro-FaceDetection
==================================================================

概述
------

TFLM示例工程展示了使用摄像头采集图像信息，通过集成的TFLM机器学习平台进行人脸检测，并通过屏幕显示结果

配置
------

- 连接摄像头设备到开发板的CAM接口

- 连接显示屏到相应接口，显示屏型号  :ref:`LCD-800480W070TC <lcd_800480w070tc>`

代码选项
------------

- 请在该工程的CMakeLists.txt的`find_package`前添加如下代码，获得更高性能


.. code-block:: cmake

           set(RV_ABI "ilp32d")
           set(RV_ARCH "rv32gc")
           set(CONFIG_HPM_MATH_NN_RVP32 1)

- 使用zcc libnn 3.2.5 编译需要在hpm_math.h文件中对tpt_elementwise_add_s8函数调用处进行如下修改：

为in_rshift1参数添加负号
为in_rshift2参数添加负号

.. code-block:: c

           return tpt_elementwise_add_s8(out, out_offset, out_scale, -out_rshift, act_min,
                               act_max, in_tensor1, in_tensor2, in_offset1, in_scale1,
                               -in_rshift1, in_offset2, in_scale2, -in_rshift2,
                               lshift, size);



运行现象
------------

当工程正确运行后，LCD会显示摄像头采集的实时画面，以及处理后的图像和人脸框
