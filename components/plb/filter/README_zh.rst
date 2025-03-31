.. _plb_filter_component:

PLB Filter Component
====================

概述
----

HPMicro MCU的TRIGMUX具有滤波功能，为部分输入信号提供了数字滤波器，支持滤刺模式、延时模式、滤峰模式和滤谷模式。但这滤波器仅针对外部从引脚的输入信号，不能对内部的信号进行滤波。

为了解决这一问题，本组件使用PLB实现了滤波器，同样支持滤刺模式、延时模式、滤峰模式和滤谷模式，通过配置TRIGMUX，既能够对外部引脚输入信号进行滤波，又能够对能够连接到TRIGMUX的信号进行滤波。

滤波器设计介绍
----------------

滤刺模式（Rapid Change Mode）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- 滤波器输入翻转后，输出也会立即翻转，之后会在一定时间内无视滤波器的输入。这个模式下，滤波器输出会紧随输入，同时会避免输出信号出现毛刺。

  .. image:: doc/rapid_change.png

- 概要设计

  - 该设计分为2级，第1级为TYPE_B逻辑单元(输出OUT0作为锁存信号)，第2级为TYPE_A逻辑单元。

  - TYPE_B的设计

    - 原始信号的边沿信号作为PLB TYPE_B逻辑单元的输入信号IN0；
    - PLB输出信号为OUT0，同时作为PLB TYPE_B逻辑单元的输入信号IN1；
    - PLB输出信号为OUT1，同时作为PLB TYPE_B逻辑单元的输入信号IN2；
    - 定义真值表如下：

      .. list-table::
         :header-rows: 1

         * - IN2
           - IN1
           - IN0
           - Operation
         * - 0
           - 0
           - 0
           - cnt值保持不变
         * - 0
           - 0
           - 1
           - cnt值加1
         * - 0
           - 1
           - 0
           - cnt值加1
         * - 0
           - 1
           - 1
           - cnt值加1
         * - 1
           - x
           - x
           - cnt值从cmp0中获取，cmp0的值为0

    - OUT0的输出逻辑为：cnt值跟cmp0相比较，当cnt值 `大于` cmp0时，out0输出为1，否则为0。
    - OUT1的输出逻辑为：cnt值跟cmp1相比较，当cnt值 `大于等于` cmp1时，out1输出为1，否则为0。cmp1的数值可设置，就是用户需要定义的延时时间，单位为 plb clock tick；
    - cnt初始值为0。

  - TYPE_A的设计

    - 原始信号作为PLB TYPE_A逻辑单元的输入信号IN0；
    - 原始信号的边沿信号作为PLB TYPE_A逻辑单元的输入信号IN1；
    - TYPE_B逻辑单元的输出信号OUT0作为TYPE_A逻辑单元的输入信号IN2;
    - TYPE_A逻辑单元的输出信号OUT0作为TYPE_A逻辑单元的输入信号IN3;
    - 定义真值表如下：

      .. list-table::
         :header-rows: 1

         * - IN3
           - IN2
           - IN1
           - IN0
           - Out0
         * - 0
           - 0
           - x
           - 0
           - 0
         * - 0
           - 0
           - 1
           - 1
           - 1
         * - 1
           - 1
           - x
           - x
           - 1
         * - 1
           - 0
           - x
           - 1
           - 1
         * - 1
           - 0
           - 0
           - 0
           - 0
         * - 1
           - 0
           - 1
           - 0
           - 0
         * - 0
           - 1
           - x
           - x
           - 0
         * - 0
           - 0
           - 0
           - 1
           - 1

    - OUT0即为滤波后的输出信号。

延时模式（Delay Mode）
~~~~~~~~~~~~~~~~~~~~~~

- 滤波器输入翻转后需要保持一定时间，滤波器输出才会翻转。

  .. image:: doc/delay_mode.png

- 概要设计

  - 原始信号作为PLB TYPE_B逻辑单元的输入信号IN0，原始信号的边沿信号作为PLB TYPE_B逻辑单元的输入信号IN2；
  - PLB输出信号为OUT0，同时作为PLB TYPE_B逻辑单元的输入信号IN1；
  - 定义真值表如下：

    .. list-table::
       :header-rows: 1

       * - IN2
         - IN1
         - IN0
         - Operation
         - 备注
       * - 0
         - 0
         - 0
         - cnt值保持不变
         - 稳定态
       * - 0
         - 0
         - 1
         - cnt值加1
         -
       * - 0
         - 1
         - 0
         - cnt值减1
         -
       * - 0
         - 1
         - 1
         - cnt值保持不变
         - 稳定态
       * - 1
         - 0
         - 0
         - cnt值加1
         - 信号抖动
       * - 1
         - 0
         - 1
         - cnt值从cmp1中获取，cmp1的值为0
         - 上升沿
       * - 1
         - 1
         - 0
         - cnt值从cmp2中获取，cmp2的值为2*cmp0
         - 下降沿
       * - 1
         - 1
         - 1
         - cnt值减1
         - 信号抖动

  - OUT0的输出逻辑为：cnt值跟cmp0相比较，当cnt值 `大于等于` cmp0时，out0输出为1，否则为0。cmp0的数值可设置，就是用户需要定义的延时时间，单位为 plb clock tick；
  - cnt初始值为0；
  - OUT0即为滤波后的输出信号。

滤峰模式（Stable Low Mode）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- 滤波器输入置逻辑0后，需要保持一定时间，滤波器输出才会置逻辑0，而滤波器输入置1，滤波器输出会立即置1。

  .. image:: doc/stable_low.png

- 概要设计

  - 原始信号作为PLB TYPE_B逻辑单元的输入信号IN0；
  - PLB输出信号为OUT0，同时作为PLB TYPE_B逻辑单元的输入信号IN1；
  - 定义真值表如下：

    .. list-table::
       :header-rows: 1

       * - IN1
         - IN0
         - Operation
       * - x
         - 1
         - cnt值从cmp1中获取，cmp1的值为0
       * - 1
         - 0
         - cnt值加1
       * - 0
         - 0
         - cnt值保持不变

  - OUT0的输出逻辑为：cnt值跟cmp0相比较，当cnt值 `小于` cmp0时，out0输出为1，否则为0。cmp0的数值可设置，就是用户需要定义的延时时间，单位为 plb clock tick;
  - cnt初始值为cmp0；
  - OUT0即为滤波后的输出信号。

滤谷模式（Stable High Mode）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- 滤波器输入置逻辑1后，需要保持一定时间，滤波器输出才会置逻辑1，而滤波器输入置0，滤波器输出会立即置0。

  .. image:: doc/stable_high.png

- 概要设计

  - 原始信号作为PLB TYPE_B逻辑单元的输入信号IN0；
  - PLB输出信号为OUT0，同时作为PLB TYPE_B逻辑单元的输入信号IN1；
  - 定义真值表如下：

    .. list-table::
       :header-rows: 1

       * - IN1
         - IN0
         - Operation
       * - x
         - 0
         - cnt值从cmp1中获取，cmp1的值为0
       * - 0
         - 1
         - cnt值加1
       * - 1
         - 1
         - cnt值保持不变

  - OUT0的输出逻辑为：cnt值跟cmp0相比较，当cnt值 `大于等于` cmp0时，out0输出为1，否则为0。cmp0的数值可设置，就是用户需要定义的延时时间，单位为 plb clock tick；
  - cnt初始值为0；
  - OUT0即为滤波后的输出信号。

使用
----

- 在应用程序中的CMakeLists.txt中添加如下内容，使能PLB滤波器组件：

  .. code-block:: cmake

    set(CONFIG_HPM_PLB_FILTER 1)

- 应用程序调用如下API进行配置：

  .. code-block:: c

    void plb_filter_init(filter_config_t *cfg)
