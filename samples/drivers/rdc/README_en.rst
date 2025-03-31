.. _rdc:

RDC
======

Overview
--------

The rdc example project demo position information from a rotary encoder.

Board Setting
-------------

Connect the rotary encoder driver board to the motor interface (Please refer to   :ref:`RDC Pin Description <board_resource>` ), then connect the rotary encoder to the driver board.

- Driver board 14V power supply

- RDC model **SG52-4-G12.7**

- RDC encoder have 4 pole, so one turn of the electrical angle corresponds to 1/4 turns of the actual position.

- The start position of the RDC encoder needs to ensure that there is enough voltage in both phases to acquire the over-zero position, otherwise there will be an error in the position during the movement.

Running the example
-------------------

Rotate the motor and the serial terminal will output the angle information as follows:

.. code-block:: console

    theta: 1.
    theta: 20.5.

