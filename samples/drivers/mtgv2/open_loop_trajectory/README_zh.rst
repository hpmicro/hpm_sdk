.. _mtgv2_trajectory:

MTGV2 轨迹生成例程
========================

概述
------

MTGV2轨迹生成示例工程展示了使用mtg生成目标轨迹的圈数、位置、速度、加速度的功能。在此例程中，初始位置、速度配置为0，加速度配置为10r/s2；当速度增加到50r/s时，mtg会自动将加速度配置为0，保持速度不变；一段时间后，mtg会自动将加速度配置为-10r/s2；当速度到达-50r/s后配置加速度为10r/s2.上述操作经过配置mtg寄存器可以自动触发，运行过程中无需软件进行参与。

 .. image:: doc/acc.png
    :alt: 加速度曲线

 .. image:: doc/vel.png
    :alt: 速度曲线

 .. image:: doc/pos.png
    :alt: 圈数位置曲线

硬件设置
------------

无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：
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
