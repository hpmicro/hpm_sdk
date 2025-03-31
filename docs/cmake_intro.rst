SDK CMake API Reference
=============================
This page was auto-generated from cmake source files using generate_cmake_rst.py

.. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
.. !!!!!! Auto-generated file, do not modify
.. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

.. contents::
   :local:


Public CMake functions / macros
-------------------------------

 * :cmake:command:`add_subdirectory_ifdef`
 * :cmake:command:`sdk_app_inc`
 * :cmake:command:`sdk_app_src`
 * :cmake:command:`sdk_app_src_glob`
 * :cmake:command:`sdk_clear_compile_options`
 * :cmake:command:`sdk_compile_definitions`
 * :cmake:command:`sdk_compile_definitions_ifdef`
 * :cmake:command:`sdk_compile_options`
 * :cmake:command:`sdk_gcc_inc`
 * :cmake:command:`sdk_gcc_src`
 * :cmake:command:`sdk_gcc_src_glob`
 * :cmake:command:`sdk_gcc_startup_src`
 * :cmake:command:`sdk_get_compile_options`
 * :cmake:command:`sdk_iar_asm_preinclude`
 * :cmake:command:`sdk_iar_cc_preinclude`
 * :cmake:command:`sdk_iar_compile_definitions`
 * :cmake:command:`sdk_iar_compile_options`
 * :cmake:command:`sdk_iar_enable_andesperf`
 * :cmake:command:`sdk_iar_enable_dsp`
 * :cmake:command:`sdk_iar_enable_no_size_constraints`
 * :cmake:command:`sdk_iar_inc`
 * :cmake:command:`sdk_iar_link_libraries`
 * :cmake:command:`sdk_iar_src`
 * :cmake:command:`sdk_iar_src_glob`
 * :cmake:command:`sdk_iar_startup_src`
 * :cmake:command:`sdk_inc`
 * :cmake:command:`sdk_inc_ifdef`
 * :cmake:command:`sdk_inc_ifndef`
 * :cmake:command:`sdk_ld_options`
 * :cmake:command:`sdk_ld_options_ifdef`
 * :cmake:command:`sdk_link_libraries`
 * :cmake:command:`sdk_link_libraries_ifdef`
 * :cmake:command:`sdk_linker_global_symbols`
 * :cmake:command:`sdk_nds_compile_definitions`
 * :cmake:command:`sdk_nds_compile_options`
 * :cmake:command:`sdk_nds_ld_options`
 * :cmake:command:`sdk_nds_link_libraries`
 * :cmake:command:`sdk_raise_fatal_error_if_all_invalid`
 * :cmake:command:`sdk_raise_fatal_error_if_all_valid`
 * :cmake:command:`sdk_raise_fatal_error_if_valid_at_least_one`
 * :cmake:command:`sdk_ses_compile_definitions`
 * :cmake:command:`sdk_ses_compile_options`
 * :cmake:command:`sdk_ses_inc`
 * :cmake:command:`sdk_ses_link_libraries`
 * :cmake:command:`sdk_ses_options`
 * :cmake:command:`sdk_ses_src`
 * :cmake:command:`sdk_ses_src_glob`
 * :cmake:command:`sdk_ses_startup_src`
 * :cmake:command:`sdk_src`
 * :cmake:command:`sdk_src_glob`
 * :cmake:command:`sdk_src_glob_ifdef`
 * :cmake:command:`sdk_src_glob_ifndef`
 * :cmake:command:`sdk_src_ifdef`
 * :cmake:command:`sdk_src_ifndef`
 * :cmake:command:`sdk_sys_inc`
 * :cmake:command:`sdk_zcc_compile_definitions`
 * :cmake:command:`sdk_zcc_compile_options`
 * :cmake:command:`sdk_zcc_ld_options`
 * :cmake:command:`sdk_zcc_link_libraries`

.. _`add_subdirectory_ifdef_ref`:

`add_subdirectory_ifdef`
~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: add_subdirectory_ifdef(feature)

 *[function defined in cmake-ext.cmake]*

 Add subdirectory if feature is true

 Example:
   add_subdirectory_ifdef(FEATUREA paths)
 :param FEATUREA: if FEATUREA is defined, paths will be added
 :param paths: directories to be added


.. _`sdk_app_inc_ref`:

`sdk_app_inc`
~~~~~~~~~~~~~

.. cmake:command:: sdk_app_inc()

 *[function defined in cmake-ext.cmake]*

 Add application specific include path

 Example:
   sdk_app_inc(INC_PATH)
 :param INC_PATH: add include path for application


.. _`sdk_app_src_ref`:

`sdk_app_src`
~~~~~~~~~~~~~

.. cmake:command:: sdk_app_src()

 *[function defined in cmake-ext.cmake]*

 Add application source

 Example:
   sdk_app_src(SOURCE_FILE)
 :param SOURCE_FILE: source files to be added to application


.. _`sdk_app_src_glob_ref`:

`sdk_app_src_glob`
~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_app_src_glob()

 *[function defined in cmake-ext.cmake]*

 Add application source file (glob pattern)

 Example:
   sdk_app_src_glob(SOURCE_FILE_GLOB)
 :param SOURCE_FILE_GLOB: source files to be added to application,
    like ./**/*.c to add all .c files under current directory recursively


.. _`sdk_clear_compile_options_ref`:

`sdk_clear_compile_options`
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_clear_compile_options()

 *[function defined in cmake-ext.cmake]*

 Clear all compile options


.. _`sdk_compile_definitions_ref`:

`sdk_compile_definitions`
~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_compile_definitions()

 *[function defined in cmake-ext.cmake]*

 Set compile definitions

 Example:
   sdk_compile_definitions(def)
 :param def: compiler preprocesing definition


.. _`sdk_compile_definitions_ifdef_ref`:

`sdk_compile_definitions_ifdef`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_compile_definitions_ifdef(feature)

 *[function defined in cmake-ext.cmake]*

 set compile definitions if feature is true

 Example:
   sdk_compile_definitions(def)
 :param def: compiler preprocesing definition


.. _`sdk_compile_options_ref`:

`sdk_compile_options`
~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_compile_options()

 *[function defined in cmake-ext.cmake]*

 Set compile options

 Example:
   sdk_compile_options(opts)
 :param opts: compile options


.. _`sdk_gcc_inc_ref`:

`sdk_gcc_inc`
~~~~~~~~~~~~~

.. cmake:command:: sdk_gcc_inc()

 *[function defined in cmake-ext.cmake]*

 Add include path for gcc

 Example:
   sdk_gcc_inc(INC_PATH)
 :param INC_PATH: add include path


.. _`sdk_gcc_src_ref`:

`sdk_gcc_src`
~~~~~~~~~~~~~

.. cmake:command:: sdk_gcc_src()

 *[function defined in cmake-ext.cmake]*

 Add source specifically for gcc

 Example:
   sdk_gcc_src(SOURCE_FILE)
 :param SOURCE_FILE: source files to be added to HPM_SDK_LIB


.. _`sdk_gcc_src_glob_ref`:

`sdk_gcc_src_glob`
~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_gcc_src_glob()

 *[function defined in cmake-ext.cmake]*

 Add source file (glob pattern) for gcc

 Example:
   sdk_gcc_src_glob(SOURCE_FILE_GLOB)
 :param SOURCE_FILE_GLOB: source files to be added to gcc,
    like ./**/*.c to add all .c files under current directory recursively


.. _`sdk_gcc_startup_src_ref`:

`sdk_gcc_startup_src`
~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_gcc_startup_src()

 *[function defined in cmake-ext.cmake]*

 Add source specifically for gcc startup

 Example:
   sdk_gcc_startup_src(SOURCE_FILE)
 :param SOURCE_FILE: source files to be added to HPM_SDK_GCC_STARTUP_LIB


.. _`sdk_get_compile_options_ref`:

`sdk_get_compile_options`
~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_get_compile_options(opts)

 *[function defined in cmake-ext.cmake]*

 Return all compiler options, separated by single space

 Example:
   sdk_get_copmile_options(opts)
 :param opts: all compile options configured for current project


.. _`sdk_iar_asm_preinclude_ref`:

`sdk_iar_asm_preinclude`
~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_asm_preinclude(file)

 *[function defined in ide\iar.cmake]*

 Set asm preinclude for IAR


.. _`sdk_iar_cc_preinclude_ref`:

`sdk_iar_cc_preinclude`
~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_cc_preinclude(file)

 *[function defined in ide\iar.cmake]*

 Set cc preinclude for IAR


.. _`sdk_iar_compile_definitions_ref`:

`sdk_iar_compile_definitions`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_compile_definitions()

 *[function defined in ide\iar.cmake]*

 Set compile definitions for IAR

 Example:
   sdk_iar_compile_definitions(def)
 :param def: compiler preprocesing definition


.. _`sdk_iar_compile_options_ref`:

`sdk_iar_compile_options`
~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_compile_options()

 *[function defined in ide\iar.cmake]*

 Set compile options for IAR

 Example:
   sdk_iar_compile_options(opts)
 :param opts: compile options


.. _`sdk_iar_enable_andesperf_ref`:

`sdk_iar_enable_andesperf`
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_enable_andesperf()

 *[function defined in ide\iar.cmake]*

 Enable iar andes performance ext


.. _`sdk_iar_enable_dsp_ref`:

`sdk_iar_enable_dsp`
~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_enable_dsp()

 *[function defined in ide\iar.cmake]*

 Enable DSP in IAR


.. _`sdk_iar_enable_no_size_constraints_ref`:

`sdk_iar_enable_no_size_constraints`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_enable_no_size_constraints()

 *[function defined in ide\iar.cmake]*

 Enable no size constraints


.. _`sdk_iar_inc_ref`:

`sdk_iar_inc`
~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_inc()

 *[function defined in ide\iar.cmake]*

 Add include path for IAR

 Example:
   sdk_iar_link_libraries(libs)
 :param libs: libraries to be linked for IAR


.. _`sdk_iar_link_libraries_ref`:

`sdk_iar_link_libraries`
~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_link_libraries()

 *[function defined in ide\iar.cmake]*

 link libraries for IAR

 Example:
   sdk_iar_link_libraries(libs)
 :param libs: standard libraries to be linked for IAR


.. _`sdk_iar_src_ref`:

`sdk_iar_src`
~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_src()

 *[function defined in ide\iar.cmake]*

 Add source file for IAR

 Example:
   sdk_iar_src(SOURCE_FILE)
 :param SOURCE_FILE: source file added for IAR


.. _`sdk_iar_src_glob_ref`:

`sdk_iar_src_glob`
~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_src_glob()

 *[function defined in ide\iar.cmake]*

 Add source file (glob pattern) for IAR

 Example:
   sdk_gcc_src_glob(SOURCE_FILE_GLOB)
 :param SOURCE_FILE_GLOB: source files to be added to IAR,
    like ./**/*.c to add all .c files under current directory recursively


.. _`sdk_iar_startup_src_ref`:

`sdk_iar_startup_src`
~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_iar_startup_src()

 *[function defined in ide\iar.cmake]*

 Add source file for IAR startup

 Example:
   sdk_iar_startup_src(STARTUP_SOURCE_FILE)
 :param STARTUP_SOURCE_FILE: startup source file added for IAR


.. _`sdk_inc_ref`:

`sdk_inc`
~~~~~~~~~

.. cmake:command:: sdk_inc()

 *[function defined in cmake-ext.cmake]*

 Add include path

 Example:
   sdk_inc(INC_PATH)
 :param INC_PATH: add include path


.. _`sdk_inc_ifdef_ref`:

`sdk_inc_ifdef`
~~~~~~~~~~~~~~~

.. cmake:command:: sdk_inc_ifdef(feature)

 *[function defined in cmake-ext.cmake]*

 Add include path if feature is true


.. _`sdk_inc_ifndef_ref`:

`sdk_inc_ifndef`
~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_inc_ifndef(feature)

 *[function defined in cmake-ext.cmake]*

 Add include path if feature is false


.. _`sdk_ld_options_ref`:

`sdk_ld_options`
~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_ld_options()

 *[function defined in cmake-ext.cmake]*

 Set linker options

 Example:
   sdk_ld_options(opts)
 :param opts: linker options


.. _`sdk_ld_options_ifdef_ref`:

`sdk_ld_options_ifdef`
~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_ld_options_ifdef(feature)

 *[function defined in cmake-ext.cmake]*

 Set linker options if feature is true

 Example:
   sdk_ld_options_ifdef(FEATUREA opts)
 :param FEATUREA: if FEATUREA is true, opts will be added for linker
 :param opts: linker options


.. _`sdk_link_libraries_ref`:

`sdk_link_libraries`
~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_link_libraries()

 *[function defined in cmake-ext.cmake]*

 Link libraries

 Example:
   sdk_link_libraries(libs)
 :param libs: libraries to be linked, support both file path
   (like USER_LIB.a) and standard libraries provided by toolchain (like m)


.. _`sdk_link_libraries_ifdef_ref`:

`sdk_link_libraries_ifdef`
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_link_libraries_ifdef(feature)

 *[function defined in cmake-ext.cmake]*

 link libraries if feature is true

 Example:
   sdk_link_libraries_ifdef(FEATUREA libs)
 :param FEATUREA: if FEATUREA is true, opts will be added for linker
 :param libs: libraries to be linked, support both file path
   (like USER_LIB.a) and standard libraries provided by toolchain (like m)


.. _`sdk_linker_global_symbols_ref`:

`sdk_linker_global_symbols`
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_linker_global_symbols()

 *[function defined in cmake-ext.cmake]*

 Define global symbols for linker

 Example:
   sdk_linker_global_symbols(syms)
 :param syms: symboles for linker


.. _`sdk_nds_compile_definitions_ref`:

`sdk_nds_compile_definitions`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_nds_compile_definitions()

 *[function defined in cmake-ext.cmake]*

 Set defnitions for nds toolchain

 Example:
   sdk_nds_compie_definitions(defs)
 :param defs: definitions for andes toolchain


.. _`sdk_nds_compile_options_ref`:

`sdk_nds_compile_options`
~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_nds_compile_options()

 *[function defined in cmake-ext.cmake]*

 Set compile options for nds toolchain

 Example:
   sdk_nds_compile_options(opts)
 :param opts: options for nds toolchain


.. _`sdk_nds_ld_options_ref`:

`sdk_nds_ld_options`
~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_nds_ld_options()

 *[function defined in cmake-ext.cmake]*

 Set linker options

 Example:
   sdk_nds_ld_options(opts)
 :param opts: linker options


.. _`sdk_nds_link_libraries_ref`:

`sdk_nds_link_libraries`
~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_nds_link_libraries()

 *[function defined in cmake-ext.cmake]*

 Link libraries for andes toolchains

 Example:
   sdk_nds_link_libraries(libs)
 :param libs: libraries to be linked, support both file path
    (like USER_LIB.a) and standard libraries provided by toolchain (like m)

.. _`sdk_raise_fatal_error_if_all_invalid_ref`:

`sdk_raise_fatal_error_if_all_invalid`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_raise_fatal_error_if_all_invalid()

 *[function defined in cmake-ext.cmake]*

 Raise fatal error if symbols given are all not set or set to 0 or n

 Example:
   sdk_raise_fatal_error_if_all_invalid(FEATUREA FEATUREB)
 :param FEATUREA FEAUTREB: features to be checked


.. _`sdk_raise_fatal_error_if_all_valid_ref`:

`sdk_raise_fatal_error_if_all_valid`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_raise_fatal_error_if_all_valid()

 *[function defined in cmake-ext.cmake]*

 Raise fatal error if symbols given are all set or set to 1 or y

 Example:
   sdk_raise_fatal_error_if_all_valid(FEATUREA FEATUREB)
 :param FEATUREA FEAUTREB: features to be checked


.. _`sdk_raise_fatal_error_if_valid_at_least_one_ref`:

`sdk_raise_fatal_error_if_valid_at_least_one`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_raise_fatal_error_if_valid_at_least_one()

 *[function defined in cmake-ext.cmake]*

 Raise fatal error if at least one of them given symbols is set or set to 1 or y

 Example:
   sdk_raise_fatal_error_if_valid_at_least_one(FEATUREA FEATUREB)
 :param FEATUREA FEAUTREB: features to be checked


.. _`sdk_ses_compile_definitions_ref`:

`sdk_ses_compile_definitions`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_ses_compile_definitions()

 *[function defined in ide\segger.cmake]*

 Set compile definitions for SES

 Example:
   sdk_ses_compile_definitions(def)
 :param def: compiler preprocesing definition


.. _`sdk_ses_compile_options_ref`:

`sdk_ses_compile_options`
~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_ses_compile_options()

 *[function defined in ide\segger.cmake]*

 Set compile options for SES

 Example:
   sdk_ses_compile_options(opts)
 :param opts: compile options


.. _`sdk_ses_inc_ref`:

`sdk_ses_inc`
~~~~~~~~~~~~~

.. cmake:command:: sdk_ses_inc()

 *[function defined in ide\segger.cmake]*

 Add include path for SES

 Example:
   sdk_ses_link_libraries(libs)
 :param libs: libraries to be linked for SES


.. _`sdk_ses_link_libraries_ref`:

`sdk_ses_link_libraries`
~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_ses_link_libraries()

 *[function defined in ide\segger.cmake]*

 link libraries for SES

 Example:
   sdk_ses_link_libraries(libs)
 :param libs: standard libraries to be linked for SES


.. _`sdk_ses_options_ref`:

`sdk_ses_options`
~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_ses_options()

 *[function defined in ide\segger.cmake]*

 Add options for SES project

 Example:
   sdk_ses_options(option_key_value_list)
 :param option_key_value_list: list of option key-value pair, split with space
    like "option_a_name=option_a_value" to add option_a_name with value of option_a_value in the project file


.. _`sdk_ses_src_ref`:

`sdk_ses_src`
~~~~~~~~~~~~~

.. cmake:command:: sdk_ses_src()

 *[function defined in ide\segger.cmake]*

 Add source file for SES

 Example:
   sdk_ses_src(SOURCE_FILE)
 :param SOURCE_FILE: source file added for SES


.. _`sdk_ses_src_glob_ref`:

`sdk_ses_src_glob`
~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_ses_src_glob()

 *[function defined in ide\segger.cmake]*

 Add source file (glob pattern) for SES

 Example:
   sdk_gcc_src_glob(SOURCE_FILE_GLOB)
 :param SOURCE_FILE_GLOB: source files to be added to SES,
    like ./**/*.c to add all .c files under current directory recursively


.. _`sdk_ses_startup_src_ref`:

`sdk_ses_startup_src`
~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_ses_startup_src()

 *[function defined in ide\segger.cmake]*

 Add source file for SES startup

 Example:
   sdk_ses_startup_src(STARTUP_SOURCE_FILE)
 :param STARTUP_SOURCE_FILE: source file added for SES startup


.. _`sdk_src_ref`:

`sdk_src`
~~~~~~~~~

.. cmake:command:: sdk_src()

 *[function defined in cmake-ext.cmake]*

 Add source file to sdk core (HPM_SDK_LIB)

 Example:
   sdk_src(SOURCE_FILE)
 :param SOURCE_FILE: source files to be added to HPM_SDK_LIB


.. _`sdk_src_glob_ref`:

`sdk_src_glob`
~~~~~~~~~~~~~~

.. cmake:command:: sdk_src_glob()

 *[function defined in cmake-ext.cmake]*

 Add source file (glob pattern) to sdk core (HPM_SDK_LIB)

 Example:
   sdk_src_glob(SOURCE_FILE_GLOB)
 :param SOURCE_FILE_GLOB: source files to be added to HPM_SDK_LIB,
    like ./**/*.c to add all .c files under current directory recursively


.. _`sdk_src_glob_ifdef_ref`:

`sdk_src_glob_ifdef`
~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_src_glob_ifdef(feature)

 *[function defined in cmake-ext.cmake]*

 Add source file (glob pattern) for gcc if feature is true

.. _`sdk_src_glob_ifndef_ref`:

`sdk_src_glob_ifndef`
~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_src_glob_ifndef(feature)

 *[function defined in cmake-ext.cmake]*

 Add source file (glob pattern) for gcc if feature is false

.. _`sdk_src_ifdef_ref`:

`sdk_src_ifdef`
~~~~~~~~~~~~~~~

.. cmake:command:: sdk_src_ifdef(feature)

 *[function defined in cmake-ext.cmake]*

 Add source to SDK core if feature is true


.. _`sdk_src_ifndef_ref`:

`sdk_src_ifndef`
~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_src_ifndef(feature)

 *[function defined in cmake-ext.cmake]*

 Add source to SDK core if feature is false


.. _`sdk_sys_inc_ref`:

`sdk_sys_inc`
~~~~~~~~~~~~~

.. cmake:command:: sdk_sys_inc()

 *[function defined in cmake-ext.cmake]*

 Add system include path

 Example:
   sdk_sys_inc(SYS_INC_PATH)
 :param SYS_INC_PATH: add system include path


.. _`sdk_zcc_compile_definitions_ref`:

`sdk_zcc_compile_definitions`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_zcc_compile_definitions()

 *[function defined in cmake-ext.cmake]*

 Set defnitions for zcc toolchain

 Example:
   sdk_zcc_compile_definitions(defs)
 :param defs: definitions for zcc toolchain


.. _`sdk_zcc_compile_options_ref`:

`sdk_zcc_compile_options`
~~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_zcc_compile_options()

 *[function defined in cmake-ext.cmake]*

 Set compile options for zcc toolchain

 Example:
   sdk_zcc_compile_options(opts)
 :param opts: options for zcc toolchain


.. _`sdk_zcc_ld_options_ref`:

`sdk_zcc_ld_options`
~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_zcc_ld_options()

 *[function defined in cmake-ext.cmake]*

 Set linker options

 Example:
   sdk_zcc_ld_options(opts)
 :param opts: linker options


.. _`sdk_zcc_link_libraries_ref`:

`sdk_zcc_link_libraries`
~~~~~~~~~~~~~~~~~~~~~~~~

.. cmake:command:: sdk_zcc_link_libraries()

 *[function defined in cmake-ext.cmake]*

 Link libraries for zcc toolchains

 Example:
   sdk_zcc_link_libraries(libs)
 :param libs: libraries to be linked, support both file path
    (like USER_LIB.a) and standard libraries provided by toolchain (like m)
