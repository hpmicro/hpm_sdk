.. _mtg_trajectory:

MTG Trajectory
============================

Overview
--------

The MTG trajectory generation example project demonstrates the function of using MTG to generate the number of turns, position, velocity, and acceleration of the target trajectory. In this routine, the initial position and velocity are configured as 0, and the acceleration is configured as 10r/s2; When the speed increases to 50r/s, MTG will automatically configure the acceleration to 0 and keep the speed unchanged; After a period of time, MTG will automatically configure the acceleration to -10r/s2; When the speed reaches -50r/s, configure the acceleration to 10r/s2. The above operation can be automatically triggered by configuring the MTG register, and there is no need for software to participate during the operation.

 .. image:: doc/acc.png
    :alt: acc

 .. image:: doc/vel.png
    :alt: vel

 .. image:: doc/pos.png
    :alt: turns+pos

Board Setting
-------------

No special settings

Running the example
-------------------

mtg basic example
/*LOCK_REV_POS=0.000199,LOCK_VEL=5,LOCK_ACC=1*/
/*LOCK_REV_POS=0.108655,LOCK_VEL=119,LOCK_ACC=1*/
/*LOCK_REV_POS=0.417401,LOCK_VEL=233,LOCK_ACC=1*/
/*LOCK_REV_POS=0.926403,LOCK_VEL=348,LOCK_ACC=1*/
/*LOCK_REV_POS=1.635660,LOCK_VEL=463,LOCK_ACC=1*/
/*LOCK_REV_POS=2.545297,LOCK_VEL=577,LOCK_ACC=1*/
/*LOCK_REV_POS=3.655098,LOCK_VEL=692,LOCK_ACC=1*/
/*LOCK_REV_POS=4.965163,LOCK_VEL=806,LOCK_ACC=1*/
/*LOCK_REV_POS=6.475487,LOCK_VEL=921,LOCK_ACC=1*/
/*LOCK_REV_POS=8.186073,LOCK_VEL=1035,LOCK_ACC=1*/
