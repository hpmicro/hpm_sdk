/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
  .section .func_table, "ax"
  .global _init
_init:
  jal flash_init
  ebreak
  jal flash_erase
  ebreak
  jal flash_program
  ebreak
  jal flash_read
  ebreak
  jal flash_get_info
  ebreak
  jal flash_erase_chip
  ebreak
  jal flash_deinit
  ebreak
