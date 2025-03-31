.. _person_detection:

Person Detection
================================

Overview
--------

The person detection example demos the use of a camera to capture an image, detect person in images and locate them.

- Optimisation with the SIMD

- Worke with the mcunet

Configurations
--------------

- Connect the camera device to the CAM interface on the board

- Click   :ref:`LCD-800480W070TC <lcd_800480w070tc>`   section and connect

Code Generation
---------------

Following the steps in this `readme <https://github.com/mit-han-lab/tinyengine/tree/main/examples/openmv_person_detection>`_ will allow you to generate code in the codegen module. After generating the code, you can find the codegen directory in the source code directory.

Code Options
------------

- To get faster program running, you need modify the link file, place the data and bss segments into the RAM near the CPU. If there is not enough space, ensure that the arrays in genModel.h are located in the RAM near the CPU.

Running the example
-------------------

When the example runs successfully, The LCD will display the live screen, the processed image and the recognition result, serial output:


.. code-block:: console

   person object example
   Get: class 0: 1
   (332, 796, 127, 465)
   fps:5.846605.
   Get: class 0: 2
   (419, 799, 29, 476)
   (5, 785, 0, 479)
   fps:5.866579.
   Get: class 0: 2
   (442, 777, 44, 479)
   (5, 785, 0, 479)
   fps:5.873189

