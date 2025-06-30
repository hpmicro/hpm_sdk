HPM MCL V2 API Compatibility Guide
=====================================

Overview
--------

This document records API changes between different versions of HPM MCL V2 to help users understand compatibility issues and migration requirements.

API Changes
-----------

v1.9.0 to v1.10.0
~~~~~~~~~~~~~~~~~~

**Function:** ``hpm_mcl_loop_init``

**Change Type:** Parameter Addition

**v1.9.0 and Earlier (8 parameters):**

.. code-block:: c

    hpm_mcl_stat_t hpm_mcl_loop_init(mcl_loop_t *loop, mcl_loop_cfg_t *cfg, mcl_cfg_t *mcl_cfg,
                                    mcl_encoder_t *encoder, mcl_analog_t *analog,
                                    mcl_control_t *control, mcl_drivers_t *drivers,
                                    mcl_path_plan_t *path);

**v1.10.0 and Later (9 parameters):**

.. code-block:: c

    hpm_mcl_stat_t hpm_mcl_loop_init(mcl_loop_t *loop, mcl_loop_cfg_t *cfg, mcl_cfg_t *mcl_cfg,
                                    mcl_encoder_t *encoder, mcl_analog_t *analog,
                                    mcl_control_t *control, mcl_drivers_t *drivers,
                                    mcl_path_plan_t *path, mcl_hw_loop_t *hw_loop);

**Added Parameter:**
- ``mcl_hw_loop_t *hw_loop`` - Hardware hybrid loop support (can be NULL)

**Purpose:** Support for hardware hybrid loop functionality

Compatibility Solution
----------------------

The library provides automatic compatibility through macro-based function overloading:

- **Existing code (8 parameters):** Works without modification
- **New code (9 parameters):** Can use hardware hybrid loop features
- **Explicit versions:** ``hpm_mcl_loop_init_v1()`` and ``hpm_mcl_loop_init_v2()`` available

Migration Guide
---------------

For v1.9.0 Users
~~~~~~~~~~~~~~~~

**No action required.** Existing code will continue to work without modification.

For New Projects (v1.10.0+)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Option 1:** Use existing 8-parameter format (compatible mode)

.. code-block:: c

    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                      &motor0.encoder, &motor0.analog, &motor0.control,
                      &motor0.drivers, NULL);

**Option 2:** Use new 9-parameter format (with hardware hybrid loop)

.. code-block:: c

    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                      &motor0.encoder, &motor0.analog, &motor0.control,
                      &motor0.drivers, NULL, &hw_loop);

**Option 3:** Use new 9-parameter format (without hardware hybrid loop)

.. code-block:: c

    hpm_mcl_loop_init(&motor0.loop, &motor0.cfg.loop, &motor0.cfg.mcl,
                      &motor0.encoder, &motor0.analog, &motor0.control,
                      &motor0.drivers, NULL, NULL);

Requirements
------------

- **Compiler:** C99 or higher (for variadic macro support)
- **Performance:** No additional overhead for compatibility functions
- **Debugging:** Function names may appear as ``hpm_mcl_loop_init_v1`` or ``hpm_mcl_loop_init_v2`` in debug information