.. _tensorflowlitemicro_persondetection:

TensorFlowLiteMicro-PersonDetection
======================================================================

Overview
--------

The TFLM example demos the use of a camera to capture an image, perform human detection via the integrated TFLM machine learning platform, and display the results on screen

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



Running the example
-------------------

When the example runs successfully, The LCD will display the live screen, the processed image and the recognition result, while the serial terminal will output the score of whether someone is present or not


.. code-block:: text

   person score:-23 no person score 23

