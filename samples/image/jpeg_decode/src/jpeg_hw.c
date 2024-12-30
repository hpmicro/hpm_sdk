/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "board.h"
#include "hpm_jpeg_drv.h"
#include "jpeg_hw.h"
#include "file_op.h"

#ifndef JPEG_DEC_HW_USE_DEFAULT_TABLES
#define JPEG_DEC_HW_USE_DEFAULT_TABLES  0
#endif

/* default tables */
const uint32_t huffmin[16] = {
    0xf0e0c082,
    0xf6f6f6f4,
    0x2b5d78f8,
    0x00000001,

    0xf0e0c080,
    0xfefefcf8,
    0xbbdf7efe,
    0x00000000,

    0xf0e0c288,
    0xf4f6f6f4,
    0x2b1c78f6,
    0x00000001,

    0xfcf8f0e0,
    0xfefefefe,
    0xbbdf7efe,
    0x00000001
};

const uint16_t huffbase[64] = {
#include "base.cdat"
};

const uint8_t huffsymb[336] = {
#include "symb.cdat"
};

const uint16_t huffenc[384] = {
#include "htable.cdat"
};

const uint16_t qdtable[256] = {
#include "qdtable.cdat"
};

bool extract_jpeg_tables(uint8_t *buf, int32_t len, jpeg_image_info_t *info)
{
    uint8_t c, qt_count = 0;
    uint8_t qt_share = 1, qt_index;
    uint32_t i, j, n, v, code, hid, z;
    uint32_t dc, ht, abase, bbase, hbase, hb;
    uint32_t l[16];
    uint32_t fp = 0;
    bool eoi_reached = false;
    uint32_t qt_len, qt_num;

    /*BASE mem (base.dat) - 9 bits array*/
    uint32_t hea[64] = {0};
    /*SYMB mem (symb.dat) - 8 bits array*/
    uint32_t heb[336] = {0};
    /*min values (min.dat)*/
    uint32_t min[64] = {0};
    /*QT (qdtable.dat)*/
    uint32_t qt[4][64] = {0};

    memcpy(info->huffenc, huffenc, sizeof(huffenc));
    memcpy(info->huffbase, huffbase, sizeof(huffbase));
    memcpy(info->huffmin, huffmin, sizeof(huffmin));
    memcpy(info->huffsymb, huffsymb, sizeof(huffsymb));
    memcpy(info->qdtable, qdtable, sizeof(qdtable));

    for (i = 162; i < 174; i++) {
        heb[i] = 0;
    }

    info->ecs_length = 0;
    /* scanning and extract tables from jpeg-data*/
    while ((len - fp) && (!eoi_reached)) {
        if (buf[fp++] != 0xff) {
            continue;
        }

        while ((c = buf[fp++]) == 0xff) {
        }
        if (c == 0) {
            continue;
        }
marker:
        switch (c) {
        /*Baseline (0, 0)*/
        case 0xc0:
            fp += 3;
            i = buf[fp++];
            info->height = (i << 8) | buf[fp++];
            i = buf[fp++];
            info->width = (i << 8) | buf[fp++];
            info->components = buf[fp++];
            if (info->components == 1) {
                info->sampling_format = JPEG_SUPPORTED_FORMAT_400;
            } else if (info->components == 3) {
                uint8_t y, c;
                fp++;
                y = buf[fp++];
                fp++;
                fp++;
                c = buf[fp++];
                /*Sampling factor*/
                if ((y == 0x22) && (c == 0x11)) {
                    info->sampling_format = JPEG_SUPPORTED_FORMAT_420;
                    i = (info->width) % 16;
                    if (i) {
                        info->width += (16 - i);
                    }
                    j = (info->height) % 16;
                    if (j) {
                        info->height += (16 - j);
                    }
                } else if ((y == 0x21) && (c == 0x11)) {
                    info->sampling_format = JPEG_SUPPORTED_FORMAT_422H;
                } else if ((y == 0x12) && (c == 0x11)) {
                    info->sampling_format = JPEG_SUPPORTED_FORMAT_422V;
                } else if ((y == 0x11) && (c == 0x11)) {
                    info->sampling_format = JPEG_SUPPORTED_FORMAT_444;
                    i = (info->width) % 8;
                    if (i) {
                        info->width += (8 - i);
                    }
                    j = (info->height) % 8;
                    if (j) {
                        info->height += (8 - j);
                    }
                } else {
                    printf("The sampling factor 0x%x is not supported\n", (uint16_t)(y << 8) | c);
                    return false;
                }
            }
            fp += 4;
            /* SOF */
            break;
        /*Ext. Sequential, Huffman (0, 0)*/
        case 0xc1:
        /*Progressive, Huffman (1, 0)*/
        case 0xc2:
        /*Lossless, Huffman*/
        case 0xc3:
        /*Differential Sequential, Huffman*/
        case 0xc5:
        /*Differential Progressive, Huffman*/
        case 0xc6:
        /*Differential Lossless, Huffman*/
        case 0xc7:
        /*Extended Sequential, Arithmetic (0, 1)*/
        case 0xc9:
        /*Progressive, Arithmetic (1, 1)*/
        case 0xca:
        /*Lossless, Huffman */
        case 0xcb:
        /*Differential Sequential, Arithmetic*/
        case 0xcd:
        /*Differential Progressive, Arithmetic*/
        case 0xce:
        /*Differential Lossless, Arithmetic*/
        case 0xcf:
            printf("Unsupported format\n");
            return false;
        /* Get the length of the marker segment */
        case 0xc4:
            /* Lh : HT length (16b)*/
            v = buf[fp++];
            n = (v << 8) | buf[fp++];
            /* Reduce marker segment byte count */
            n -= 2;
            while (n) {
                /* Get the type of table */
                v = buf[fp++]; /* Tc & Th*/
                n--;
                hid = v >> 4 ? 2 : 0;
                hid |= v & 15 ? 1 : 0;
                qt_count++;
                switch (hid) {
                case 1:
                    hbase = 368;
                    break;
                case 2:
                    hbase = 0;
                    break;
                case 3:
                    hbase = 176;
                    break;
                default:
                    hbase = 352;
                    break;
                }
                if ((v >> 4)) {
                    abase = 0;
                } else {
                    abase = 1;
                }
                dc = abase;
                ht = v & 15;
                abase |= ht << 1;
                switch (abase) {
                case 1:
                case 3:
                    bbase = 162;
                    break;
                case 2:
                    bbase = 174;
                    break;
                default:
                    bbase = 0;
                    break;
                }
                abase <<= 4;
                /* Memory initialization */
                for (i = abase; i < abase + 16; i++) {
                    hea[i] = 255;
                }
                /* Get the number of codes for each length */
                /* Lj : # of Huffman codes of length i
                 *- specifies the # of Huffman codes for each of 16 possible lengths
                 *allowed by spec. BITS
                 */
                for (i = 0; i < 16; i++) {
                    l[i] = buf[fp++];
                }
                /* Reduce marker segment byte count */
                n -= 16;
                code = 0;
                for (i = 0; i < 16; i++, abase++) {
                    min[abase] = code;
                    hea[abase] = bbase - code;
                    if (l[i]) {
                        /* Vi,j : associated with each Huffman code
                         *- specifies, for each i the value associated with each Huffman code
                         *of length i.  HUFFVAL
                         */
                        for (j = 0; j < l[i]; j++, bbase++) {
                            v = buf[fp++];
                            /* Reduce marker segment byte count */
                            n--;
                            if (dc) {
                                info->huffenc[hbase + v] = (i << 8) | (code & 0xff);
                                v &= 15;
                                if (ht) {
                                    v <<= 4;
                                }
                                heb[bbase] |= v;
                            } else {
                                if (v == 0) {
                                    hb = 160;
                                } else if (v == 0xf0) {
                                    hb = 161;
                                } else {
                                    hb = (v >> 4) * 10 + (v & 0xf) - 1;
                                }
                                info->huffenc[hbase + hb] = (i << 8) | (code & 0xff);
                                heb[bbase] = v;
                            }
                            code++;
                        }
                    }
                    code <<= 1;
                }
            }
            break;
        /*JPG extensions*/
        case 0xc8:
            /* JPG extensions */
            return false;
        /*arithmatic*/
        case 0xcc:
            /* arithmatic coding not supported */
            return false;
        case 0xd0:
        case 0xd1:
        case 0xd2:
        case 0xd3:
        case 0xd4:
        case 0xd5:
        case 0xd6:
        case 0xd7:
            break;
        /*SOI*/
        case 0xd8:
            break;
        /*EOI*/
        case 0xd9:
            info->ecs_length += 2;
            eoi_reached = true;
            break;
        case 0xda:
            /*Ls (scan header length)*/
            fp += 2;
            /*Ns (# of image components)*/
            n = buf[fp++];
            fp += 2 * n + 3; /* 2 * components + 3 dummy bytes*/
            info->ecs_start = fp;
            for (;;) {
                c = buf[fp++];
                if (c == 0xFF) {
                    c = buf[fp++];
                    if ((c != 0x0) && ((c & 0xF8) != 0xD0)) {
                        goto marker;
                    }
                    info->ecs_length += 2;
                }
                info->ecs_length += 1;
            }
            break;
        case 0xdb:
            /*Quantization Table  */
            /*Lq : QT Length (16b)*/
            v = buf[fp++];
            qt_len = (v << 8) | buf[fp++];
            qt_len -= 2;
            qt_num = qt_len / 65;
            while (qt_num--) {
                v = buf[fp++];
                /* Pq : QT element precision (4b)
                * - specifies the precision of the Qk values.
                *   0 indicates 8-bits Qk values.
                *   1 indicates 16-bits Qk values
                * Tq : QT destination identifier (4b)
                * - specifies one of 4 possible destnations at the decoder into
                *  which the QT shall be installed.
                */
                n = v & 15;
                if (n > 0)
                    qt_share = 0;

                for (i = 0; i < 64; i++) {
                    /* Qk: Quantization table element
                    * k is the index in the zigzag ordering of the DCT coeff
                    * JPC only do 8-bit Qk! (ie, Pq shall be 0)
                    */
                    qt[n][i] = buf[fp++];
                }
            }
            break;
        case 0xdd:
            /*Restart Interval Definition*/
            /* Lr : restart interval segment length (16b)
             *- specifies the length of the paramenters in the DRI segment
             */
            fp += 2;
            /* Ri : restart interval (16b)
             *- specifies the number of MCU in the restart interval.
             */
            fp += 2;
            break;
        /* All these markers are ignored */
        case 0xe0:
        case 0xe1:
        case 0xe2:
        case 0xe3:
        case 0xe4:
        case 0xe5:
        case 0xe6:
        case 0xe7:
        case 0xe8:
        case 0xe9:
        case 0xea:
        case 0xeb:
        case 0xec:
        case 0xed:
        case 0xee:
        case 0xef:
        case 0xf0:
        case 0xf1:
        case 0xf2:
        case 0xf3:
        case 0xf4:
        case 0xf5:
        case 0xf6:
        case 0xf7:
        case 0xf8:
        case 0xf9:
        case 0xfa:
        case 0xfb:
        case 0xfc:
        case 0xfd:
        case 0xfe:
            v = buf[fp++];
            v = (v << 8) | buf[fp++];
            v -= 2;
            fp += v;
            break;
        default:
            return false;
        }
    }

    /* Outputs the min values */
    for (i = 0, j = 3; i < 64; j += 7) {
        v = min[i++] & 1;
        v <<= 2;
        v |= min[i++] & 3;
        v <<= 3;
        v |= min[i++] & 7;
        info->huffmin[j--] = v >> 2;

        v <<= 4;
        v |= min[i++] & 15;
        v <<= 5;
        v |= min[i++] & 31;
        v <<= 6;
        v |= min[i++] & 63;
        v <<= 7;
        v |= min[i++] & 127;
        v <<= 8;
        v |= min[i++] & 255;
        info->huffmin[j--] = v;

        v = min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        info->huffmin[j--] = v;

        v = min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        info->huffmin[j] = v;
    }

    /* Outputs the BASE mem values */
    for (i = 0; i < 64; i++) {
        info->huffbase[i] = hea[i] & 511;
    }

    /* Outputs the SYMB mem values */
    for (i = 0; i < 336; i++) {
        info->huffsymb[i] = heb[i] & 255;
    }

    /* Outputs the quantization table */
    for (i = 0, z = 0; i < 4; i++) {
        qt_index = qt_share ? 0 : i;

        for (j = 0; j < 64; j++, z++) {
            info->qdtable[z] = qt[qt_index][j];
        }
    }
    info->qt_count = qt_count;
    return true;
}

/*---------------------------------------------------------------------*
 * Initialize JPEG module
 *---------------------------------------------------------------------
 */
void init_jpeg(jpeg_image_info_t *info)
{
    /*JPEG default parameter table settings*/
    jpeg_init(TEST_JPEG);
    jpeg_enable(TEST_JPEG);

#if JPEG_DEC_HW_USE_DEFAULT_TABLES
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffmin, (uint8_t *)(huffmin), ARRAY_SIZE(huffmin));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffbase, (uint8_t *)(huffbase), ARRAY_SIZE(huffbase));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffsymb, (uint8_t *)(huffsymb), ARRAY_SIZE(huffsymb));
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffenc, (uint8_t *)(huffenc), ARRAY_SIZE(huffenc));
    jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)(qdtable), ARRAY_SIZE(qdtable));
#else
    if (info->components == 1) {
        /* use default tables while only one table is embedded in the file */
        jpeg_fill_table(TEST_JPEG, jpeg_table_huffmin, (uint8_t *)(huffmin), ARRAY_SIZE(huffmin));
        jpeg_fill_table(TEST_JPEG, jpeg_table_huffbase, (uint8_t *)(huffbase), ARRAY_SIZE(huffbase));
        jpeg_fill_table(TEST_JPEG, jpeg_table_huffsymb, (uint8_t *)(huffsymb), ARRAY_SIZE(huffsymb));
        jpeg_fill_table(TEST_JPEG, jpeg_table_huffenc, (uint8_t *)(huffenc), ARRAY_SIZE(huffenc));
        jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)(qdtable), ARRAY_SIZE(qdtable));
    } else {
        jpeg_fill_table(TEST_JPEG, jpeg_table_huffmin, (uint8_t *)(info->huffmin), ARRAY_SIZE(info->huffmin));
        jpeg_fill_table(TEST_JPEG, jpeg_table_huffbase, (uint8_t *)(info->huffbase), ARRAY_SIZE(info->huffbase));
        jpeg_fill_table(TEST_JPEG, jpeg_table_huffsymb, (uint8_t *)(info->huffsymb), ARRAY_SIZE(info->huffsymb));
        jpeg_fill_table(TEST_JPEG, jpeg_table_huffenc, (uint8_t *)(info->huffenc), ARRAY_SIZE(info->huffenc));
        jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)(info->qdtable), ARRAY_SIZE(info->qdtable));
    }
#endif

    jpeg_disable(TEST_JPEG);
}

/*---------------------------------------------------------------------*
 * Wait for codec to complete
 *---------------------------------------------------------------------
 */
bool wait_jpeg_finish(void)
{
    do {
        if (jpeg_get_status(TEST_JPEG) & JPEG_EVENT_OUT_DMA_FINISH) {
            jpeg_clear_status(TEST_JPEG, JPEG_EVENT_OUT_DMA_FINISH);
            return true;
        }
        if (jpeg_get_status(TEST_JPEG) & JPEG_EVENT_ERROR) {
            jpeg_clear_status(TEST_JPEG, JPEG_EVENT_ERROR);
            return false;
        }
    } while (1);
}

bool jpeg_hw_decode(uint8_t *in_buf, int32_t len, uint8_t *out_buf, jpeg_image_info_t *info)
{
    jpeg_job_config_t config = {0};

    /* get jpeg image information */
    if (!extract_jpeg_tables(in_buf, len, info)) {
        return false;
    }
    if (((info->width) * (info->height)) * 2 > (DECODE_BUFFER_LEN)) {
        printf("resolution is too large %d x %d\n", info->width, info->height);
        return false;
    }

    init_jpeg(info);

    config.jpeg_format = info->sampling_format;
    if (info->components == 3) {
        config.in_pixel_format = jpeg_pixel_format_yuv422h1p;
        config.out_pixel_format = jpeg_pixel_format_rgb565;
        config.enable_ycbcr = true;
        config.out_byte_order = JPEG_BYTE_ORDER_2301;
    } else {
        config.in_pixel_format = jpeg_pixel_format_y8;
        config.out_pixel_format = jpeg_pixel_format_y8;
        config.enable_ycbcr = false;
    }

    config.width_in_pixel = info->width;
    config.height_in_pixel = info->height;
    config.in_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)(in_buf + info->ecs_start));
    config.out_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)out_buf);

    if (status_success != jpeg_start_decode(TEST_JPEG, &config, info->ecs_length)) {
        printf("failed to decode\n");
        return false;
    }
    if (!wait_jpeg_finish()) {
        return false;
    }
    return true;
}
