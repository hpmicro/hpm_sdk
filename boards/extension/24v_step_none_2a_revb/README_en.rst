.. _24v_step_none_2a_revb:

24V Stepper Motor Development Board
=======================================

Overview
--------

The 24V stepper motor development board is an expansion board for controlling stepper motors, supporting both open-loop and closed-loop control modes.

Hardware Connection
-------------------

1. Power Connection
   - Connect J6 to 24V power supply
   - Use jumper caps to connect J2 and J4

2. Motor Connection
   - Connect stepper motor driver wires to M1 interface
   - Wire sequence correspondence:

      * VB1 -> B+
      * VA1 -> A+
      * VB2 -> B-
      * VA2 -> A-

3. Control Mode
   - Closed-loop control: requires sensor wire connection
   - Open-loop control: no sensor wire connection required

4. Motor Interface
   - Use J8 interface for motor connection

.. figure:: ../doc/24v_step_none_2a_revb.jpg
   :alt: 24V Stepper Motor Development Board
   :align: center