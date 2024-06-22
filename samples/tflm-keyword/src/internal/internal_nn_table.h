/***************************************************************************
 *  Copyright (C) 2018 - 2023 Andes Technology Corporation                 *
 *  All rights reserved.                                                   *
 ***************************************************************************/

/** @file*/

#ifndef __NDS_NN_TABLE_H__
#define __NDS_NN_TABLE_H__

extern const q15_t sigmoidTable_q15[256];
extern const q7_t sigmoidTable_q7[256];

extern const q7_t tanhTable_q7[256];
extern const q15_t tanhTable_q15[256];

  /**
   * @brief 2-way tables for various activation functions
   *
   * 2-way table, H table for value larger than 1/4
   * L table for value smaller than 1/4, H table for remaining
   * We have this only for the q15_t version. It does not make
   * sense to have it for q7_t type
   */
extern const q15_t sigmoidHTable_q15[192];
extern const q15_t sigmoidLTable_q15[128];

extern const q15_t sigmoidLTable_q15[128];
extern const q15_t sigmoidHTable_q15[192];

#endif                          /*  ARM_NN_TABLES_H */
