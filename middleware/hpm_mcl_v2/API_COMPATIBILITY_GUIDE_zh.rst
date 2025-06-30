HPM MCL V2 API兼容性指南
========================

概述
----

本文档记录HPM MCL V2不同版本间的API变化，帮助用户了解兼容性问题和迁移要求。

API变化
-------

v1.9.0 到 v1.10.0
~~~~~~~~~~~~~~~~~~

**函数：** ``hpm_mcl_loop_init``

**变化类型：** 参数增加

**v1.9.0及之前版本（8个参数）：**

.. code-block:: c

    hpm_mcl_stat_t hpm_mcl_loop_init(mcl_loop_t *loop, mcl_loop_cfg_t *cfg, mcl_cfg_t *mcl_cfg,
                                    mcl_encoder_t *encoder, mcl_analog_t *analog,
                                    mcl_control_t *control, mcl_drivers_t *drivers,
                                    mcl_path_plan_t *path);

**v1.10.0及之后版本（9个参数）：**

.. code-block:: c

    hpm_mcl_stat_t hpm_mcl_loop_init(mcl_loop_t *loop, mcl_loop_cfg_t *cfg, mcl_cfg_t *mcl_cfg,
                                    mcl_encoder_t *encoder, mcl_analog_t *analog,
                                    mcl_control_t *control, mcl_drivers_t *drivers,
                                    mcl_path_plan_t *path, mcl_hw_loop_t *hw_loop);

**新增参数：**
- ``mcl_hw_loop_t *hw_loop`` - 硬件混合环路支持（可以为NULL）

**目的：** 支持硬件混合环路功能

兼容性解决方案
--------------

库提供基于宏的函数重载自动兼容性：

- **现有代码（8个参数）：** 无需修改即可工作
- **新代码（9个参数）：** 可以使用硬件混合环路功能
- **显式版本：** 提供 ``hpm_mcl_loop_init_v1()`` 和 ``hpm_mcl_loop_init_v2()``

迁移指南
--------

对于v1.9.0用户
~~~~~~~~~~~~~~

**无需任何操作。** 现有代码将继续正常工作，无需修改。

对于新项目（v1.10.0+）
~~~~~~~~~~~~~~~~~~~~~

**选项1：** 使用现有8参数格式（兼容模式）

.. code-block:: c

    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                      &motor0.encoder, &motor0.analog, &motor0.control,
                      &motor0.drivers, NULL);

**选项2：** 使用新9参数格式（带硬件混合环路）

.. code-block:: c

    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                      &motor0.encoder, &motor0.analog, &motor0.control,
                      &motor0.drivers, NULL, &hw_loop);

**选项3：** 使用新9参数格式（不使用硬件混合环路）

.. code-block:: c

    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                      &motor0.encoder, &motor0.analog, &motor0.control,
                      &motor0.drivers, NULL, NULL);

要求
----

- **编译器：** C99或更高版本（支持可变参数宏）
- **性能：** 兼容性函数无额外开销
- **调试：** 调试信息中函数名可能显示为 ``hpm_mcl_loop_init_v1`` 或 ``hpm_mcl_loop_init_v2``