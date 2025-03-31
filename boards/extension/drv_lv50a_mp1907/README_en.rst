.. _drv_lv50a_mp1907:

DRV-LV50A-MP1907 Motor Driver Board
===================================

- Connect J12, J7, J6 to HW, HV, HW
- Connect VCC of J10 to 3V3
- Connect the three phases of the motor U, V, W and J4
- Plug the signal cable of the motor into the J14 socket
- Connect 24V power supply to J3

.. note::

    1. **Note the direction of the positive and negative power supply**
    2. **Never operate with electricity**
    3. **Apply power to the motor driver board and observe whether the current is abnormal**
    4. **Make sure the motor driver board is powered down before proceeding to the next step**

- Connecting the HPM6750EVK and DRV-LV50A-MP1907

  - J8 of the HPM6750EVK is connected to J12 of the DRV-LV50A-MP1907. **Be careful to check the silkscreen, do not insert the reverse**
  - The figure below shows the overall effect, with red boxes marking the locations of the devices that may need to be operated:

  .. image:: ../doc/drv_lv50a_mp1907__oee.png
     :alt: image-1