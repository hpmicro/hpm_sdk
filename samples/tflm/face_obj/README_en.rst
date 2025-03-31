.. _tensorflowlitemicro_facedetection:

TensorFlowLiteMicro-FaceDetection
==================================================================

Overview
--------

The TFLM example demos the use of a camera to capture an image, perform face detection via the integrated TFLM machine learning platform, and display the results on screen

Configurations
--------------

- Connect the camera device to the CAM interface on the board

- Click   :ref:`LCD-800480W070TC <lcd_800480w070tc>`   section and connect

Code Options
------------

- Add code before `find_package` to the project's CMakeLists.txt to get a higher performance


.. code-block:: cmake

           set(RV_ABI "ilp32d")
           set(RV_ARCH "rv32gc")
           set(CONFIG_HPM_MATH_NN_RVP32 1)

- Compiling with zcc libnn 3.2.5 requires the following changes in the hpm_math.h file at the tpt_elementwise_add_s8 function call:

Add negative sign for in_rshift1 parameter
Add a negative sign to the in_rshift2 parameter.

.. code-block:: c

           return tpt_elementwise_add_s8(out, out_offset, out_scale, -out_rshift, act_min,
                               act_max, in_tensor1, in_tensor2, in_offset1, in_scale1,
                               -in_rshift1, in_offset2, in_scale2, -in_rshift2,
                               lshift, size);


Running the example
-------------------

When the example runs successfully, The LCD will display the live screen, the processed image and the recognition result
