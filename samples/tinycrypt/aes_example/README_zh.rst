.. _equal_symmetric_encryption_and_decryption_algorithm_aes:

等对称加解密算法AES
======================

概述
------

**aes_example** 示例工程展示了等对称加解密算法AES模式的实现。
- 此工程的等对称加解密算法采用AES128 NIST模式完成模块加解密测试。

硬件设置
------------

- 无特殊设置

运行现象
------------

当工程正确运行后，串口终端会输出如下信息：

.. code-block:: console

   tc_start() - Performing AES128 tests:
   AES128 test_1 (NIST key schedule test):
   ===================================================================
   PASS - test_1.
   AES128 test_2 (NIST encryption test):
   ===================================================================
   PASS - test_2.
   AES128 test_3 (NIST fixed-key and variable-text):
   ===================================================================
   PASS - test_3.
   AES128 test #4 (NIST variable-key and fixed-text):
   ===================================================================
   PASS - test_4.
   All AES128 tests succeeded!
   ===================================================================
   PASS - main.
   ===================================================================
   PROJECT EXECUTION SUCCESSFUL

