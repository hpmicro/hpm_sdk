/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef JPEG_HW_H
#define JPEG_HW_H
/*--------------------------------------------------------------------*
 * Macro Definitions
 *---------------------------------------------------------------------
 */
/*JPEG Definitions*/
#ifndef TEST_JPEG
#define TEST_JPEG HPM_JPEG
#endif
/*SOS Maximum number of scans*/
#define MAX_COMPS_IN_ONE_SCAN 4
/*huffmin data buff length*/
#define HUFFMINLEN 16
/*huffbase data buff length*/
#define HUFFBASELEN 64
/*huffsymb data buff length*/
#define HUFFSYMBLEN 336
/*qdtable data buff length*/
#define QDTABLELEN 256
/*huffenc data buff length*/
#define HUFFENCLEN 384
/*Input file esc-data length*/
#define INECSLEN 10000
/*---------------------------------------------------------------------*
 * Define function
 *---------------------------------------------------------------------
 */
void jpeg_convert_hw(int32_t fileLen, int32_t *width, int32_t *height);

/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
typedef struct jpeg_huffman_table {
    /*huffmin data buff*/
    uint32_t huffmin[HUFFMINLEN];
    /*huffbase data buff*/
    uint16_t huffbase[HUFFBASELEN];
    /*huffsymb data buff*/
    uint8_t huffsymb[HUFFSYMBLEN];
    /*qdtable data buff*/
    uint16_t qdtable[QDTABLELEN];
    /*huffenc data buff*/
    uint16_t huffenc[HUFFENCLEN];
    /*JPG file data esc-data buff*/
    uint8_t in_ecs[INECSLEN];
} jpeg_huffman_table_t;

#endif  /* JPEG_HW_H */