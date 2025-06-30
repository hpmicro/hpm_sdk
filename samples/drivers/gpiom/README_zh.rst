.. _gpiom:

GPIOM
==========

概述
------

GPIOM示例工程演示了如何使用GPIOM模块设置引脚的控制模块，并使用指定的控制模块改变引脚的状态。

硬件设置
------------

无特别设置

运行现象
------------

当工程正确运行后，LED灯会闪烁5次， 串口终端会输出如下信息：

.. code-block:: console

   gpio example
   using gpiom configures pin control module
   toggling led 5 times in total
   toggling led 1/5 times
   toggling led 2/5 times
   toggling led 3/5 times
   toggling led 4/5 times
   toggling led 5/5 times


提示
------------

- goio部分可参考 :ref:`gpio <gpio>` 例子

- 如果IO需要使用FGPIO控制，需要使用GPIOM模块管理分配。可参考例子中 `gpiom_configure_pin_control_setting` 函数。