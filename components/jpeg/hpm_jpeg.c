
/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* #define HPM_JPEG_LOG_LEVEL HPM_JPEG_LOG_LEVEL_DEBUG */

#include <stdint.h>
#include <stdlib.h>
#include <hpm_jpeg_drv.h>
#include <hpm_l1c_drv.h>
#include "hpm_jpeg.h"
#include "hpm_interrupt.h"

#define HPM_JPEG_CACHE_LINE hpm_jpeg_cache_line_size()
#define HPM_JPEG_ROUNDUP(x, y) ((((x) + ((y) - 1)) / (y)) * y)
#define HPM_JPEG_ROUNDDOWN(x, y) ((x) - ((x) % (y)))

#define JPEG_MARKER_SOI  0xd8
#define JPEG_MARKER_APP0 0xe0
#define JPEG_MARKER_SOF0 0xc0
#define JPEG_MARKER_DHT  0xc4
#define JPEG_MARKER_SOS  0xda
#define JPEG_MARKER_DQT  0xdb
#define JPEG_MARKER_EOI  0xd9

typedef struct hpm_jpeg_core_htab {
    uint32_t huffmin[16];
    uint16_t huffbase[64];
    uint8_t huffsymb[336];
    uint16_t huffenc[384];
} hpm_jpeg_core_htab_t;

typedef struct hpm_jpeg_core_qtab {
    uint16_t qmem[256];
} hpm_jpeg_core_qtab_t;

typedef enum hpm_jpeg_codec_type {
    HPM_JPEG_CODEC_DECODE,
    HPM_JPEG_CODEC_ENCODE,
} hpm_jpeg_codec_type_t;

typedef struct hpm_jpeg_sof0_info {
    uint16_t lf;      /**< Frame header length */
    uint8_t p;        /**< Sample precision */
    uint16_t y;       /**< Number of lines */
    uint16_t x;       /**< Number of samples per line */
    uint8_t nf;       /**< Number of image components in frame */
    /**
     * Component-specification parameters
     */
    struct {
        uint8_t c;    /**< Component identifier. 1: Y, 2: Cb, 3: Cr */
        uint8_t v: 4; /**< Vertical sampling factor */
        uint8_t h: 4; /**< Horizontal sampling factor */
        uint8_t tq;   /**< Quantization table destination selector */
    } component[4];
} hpm_jpeg_sof0_info_t;

typedef struct hpm_jpeg_sos_info {
    uint16_t ls;      /**< Ls: Scan header length */
    uint8_t ns;       /**< Ns: Number of image components in scan – Specifies the number of source image components in the scan */
    /**
     * Component-specification parameters
     */
    struct {
        uint8_t cs;    /**< Csj: Scan component selector – Selects which of the Nf image components specified in the frame parameters shall be the jth component in the scan. 1: Y, 2: Cb, 3: Cr */
        uint8_t ta: 4; /**< Taj: AC entropy coding table destination selector – Specifies one of four possible AC entropy coding table destinations from which the entropy table needed for decoding of the AC coefficients of component Csj is retrieved */
        uint8_t td: 4; /**< Tdj: DC entropy coding table destination selector – Specifies one of four possible DC entropy coding table destinations from which the entropy table needed for decoding of the DC coefficients of component Csj is retrieved */
    } component[4];
    uint8_t ss;        /**< Ss: Start of spectral or predictor selection. This parameter shall be set to 0 for the sequential DCT processes */
    uint8_t se;        /**< Se: End of spectral selection. This parameter shall be set to 63 for the sequential DCT processes */
    uint8_t al: 4;     /**< Al: Successive approximation bit position low or point transform. This parameter shall be set to zero for the sequential DCT processes */
    uint8_t ah: 4;     /**< Ah: Successive approximation bit position high. This parameter shall be set to 0 for the first scan of each band of coefficients. In the lossless mode of operations this parameter has no meaning. It shall be set to 0 */
} hpm_jpeg_sos_info_t;

typedef struct hpm_jpeg_jpeg_info {
    uint8_t *soi;         /**< Start of image marker – Marks the start of a compressed image represented in the interchange format or abbreviated format */
    uint8_t *app0;        /**< APP0, length, identifier, version, units, Xdensity, Ydensity, Xthumbnail, Ythumbnai, (RGB)n */
    uint8_t *dqt[4];      /**< Define quantization table(s) */
    uint8_t *sof0;        /**< Start of frame marker – Marks the beginning of the frame parameters. SOF0: Baseline DCT */
    uint8_t *dht_dc[2];   /**< Define Huffman table(s) DC */
    uint8_t *dht_ac[2];   /**< Define Huffman table(s) AC*/
    uint8_t *sos;         /*< Start of scan marker – Marks the beginning of the scan parameters */
    uint8_t *ecs;         /**< Entropy-coded segment, MCU0, MCU1...RST0,..MCUn... */
    uint32_t ecs_len;     /**< All of MCUs +  All of [RSTs] + EOI */
    uint8_t *eoi;         /**< End of image marker – Marks the end of a compressed image represented in the interchange format or abbreviated format. */
} hpm_jpeg_jpeg_info_t;

typedef struct hpm_jpeg_sampling_tab {
    uint8_t hy; /**< Horizontal sampling factor to Y */
    uint8_t vy; /**< Vertical sampling factor to Y */
    uint8_t hc; /**< Horizontal sampling factor to UV */
    uint8_t vc; /**< Vertical sampling factor to UV */
    hpm_jpeg_sampling_format_t sampling;
} hpm_jpeg_sampling_tab_t;

typedef struct hpm_jpeg_std_tab {
    const uint8_t *app0;
    uint16_t app0_len;
    const uint8_t *dht_dc[2];
    uint16_t dht_dc_len[2];
    const uint8_t *dht_ac[2];
    uint16_t dht_ac_len[2];
} hpm_jpeg_std_tab_t;

typedef struct hpm_jpeg_buf {
    uint32_t len;
    void *alloced_buf;
    uint32_t alloced_len;
    void *aligned_buf;
    uint32_t aligned_len;
    int is_direct;
} hpm_jpeg_buf_t;

typedef struct jpeg_list {
    struct jpeg_list *pre;
    struct jpeg_list *nxt;
} jpeg_list_t;

typedef struct hpm_jpeg_context {
    void *jpeg_base;
    jpeg_list_t wait_jobs;
    jpeg_list_t *work_job;
    void *(*malloc)(uint32_t size);
    void (*free)(void *p);
    uint32_t cache_line_size;
    void (*cache_writeback)(void *start, uint32_t size);
    void (*cache_invalidate)(void *start, uint32_t size);
} hpm_jpeg_context_t;

typedef struct hpm_jpeg_base_job {
    jpeg_list_t list;
    hpm_jpeg_codec_type_t codec_type;
    hpm_jpeg_context_t *ctx;
    volatile hpm_jpeg_job_status_t status;
    volatile uint32_t hw_status;
    hpm_jpeg_job_state_cb_t cb;
    void *user_data;
} hpm_jpeg_base_job_t;

typedef struct hpm_jpeg_decode_job {
    hpm_jpeg_base_job_t base_job;
    hpm_jpeg_buf_t file_buf;
    hpm_jpeg_buf_t image_buf;
    hpm_jpeg_image_t image;
    hpm_jpeg_decode_cfg_t cfg;
    uint16_t x_align;
    uint16_t y_align;
    hpm_jpeg_core_qtab_t core_qtab;
    hpm_jpeg_core_htab_t core_htab;
    hpm_jpeg_sampling_format_t sampling;
    hpm_jpeg_jpeg_info_t jpeg_info;
    hpm_jpeg_sof0_info_t sof0_info;
    hpm_jpeg_sos_info_t sos_info;
} hpm_jpeg_decode_job_t;

typedef struct hpm_jpeg_encode_job {
    hpm_jpeg_base_job_t base_job;
    hpm_jpeg_buf_t file_buf;
    hpm_jpeg_buf_t image_buf;
    hpm_jpeg_image_t image;
    hpm_jpeg_file_t jpeg_file;
    hpm_jpeg_encode_cfg_t cfg;
    uint16_t x_align;
    uint16_t y_align;
    bool has_valid_qhtab;
    hpm_jpeg_core_qtab_t core_qtab;
    hpm_jpeg_core_htab_t core_htab;
    hpm_jpeg_jpeg_info_t jpeg_info;
    hpm_jpeg_sof0_info_t sof0_info;
    hpm_jpeg_sos_info_t sos_info;
    hpm_jpeg_std_tab_t std_tab;
} hpm_jpeg_encode_job_t;

static hpm_jpeg_context_t hpm_jpeg_context;

static inline uint32_t hpm_jpeg_lock(void)
{
    return disable_global_irq(CSR_MSTATUS_MIE_MASK);
}

static inline void hpm_jpeg_unlock(uint32_t restore)
{
    enable_global_irq(restore);
}

static void *hpm_jpeg_malloc(uint32_t size)
{
    hpm_jpeg_context_t *ctx = &hpm_jpeg_context;

    if (ctx->malloc)
        return ctx->malloc(size);

    return malloc(size);
}

static void hpm_jpeg_free(void *ptr)
{
    hpm_jpeg_context_t *ctx = &hpm_jpeg_context;

    if (ctx->free) {
        ctx->free(ptr);
        return;
    }

    free(ptr);
}

static inline uint32_t hpm_jpeg_cache_line_size(void)
{
    hpm_jpeg_context_t *ctx = &hpm_jpeg_context;

    if (ctx->cache_line_size)
        return ctx->cache_line_size;

    return 64;
}

static void hpm_jpeg_cache_writeback(void *start, uint32_t size)
{
    hpm_jpeg_context_t *ctx = &hpm_jpeg_context;

    if (ctx->cache_writeback) {
        ctx->cache_writeback(start, size);
        return;
    }

    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback((uint32_t)start, size);
    }
}

static void hpm_jpeg_cache_invalid(void *start, uint32_t size)
{
    hpm_jpeg_context_t *ctx = &hpm_jpeg_context;

    if (ctx->cache_invalidate) {
        ctx->cache_invalidate(start, size);
        return;
    }

    if (l1c_dc_is_enabled()) {
        l1c_dc_invalidate((uint32_t)start, size);
    }
}

static inline void jpeg_list_node_init(jpeg_list_t *node)
{
    node->nxt = node;
    node->pre = node;
}

static inline void jpeg_list_del(jpeg_list_t *node)
{
    node->pre->nxt = node->nxt;
    node->nxt->pre = node->pre;
}

#ifdef HPM_JPEG_ENABLE_LIST_ADD
static inline void jpeg_list_add(jpeg_list_t *head, jpeg_list_t *node)
{
    node->nxt = head->nxt;
    node->pre = head;
    head->nxt->pre = node;
    head->nxt = node;
}
#endif

static inline void jpeg_list_add_tail(jpeg_list_t *head, jpeg_list_t *node)
{
    node->nxt = head;
    node->pre = head->pre;
    head->pre->nxt = node;
    head->pre = node;
}

static inline jpeg_list_t *jpeg_list_node_get(jpeg_list_t *head)
{
    jpeg_list_t *node = NULL;

    if (head->nxt != head) {
        node = head->nxt;
        jpeg_list_del(node); /* NOLINT */
    }

    return node;
}

#define jpeg_list_for_each(node, head)\
for (jpeg_list_t *node = (head)->nxt; node != (head); node = node->nxt)

void hpm_jpeg_hexdump(void *str, const void *data, uint32_t data_len)
{
    const uint8_t *udata = data;
    char *cstr = str;
    for (uint32_t i = 0; i < data_len; i++) {
        cstr += sprintf(cstr, "%02X ", udata[i]);
    }
}

int hpm_jpeg_create_htab_for_core(hpm_jpeg_core_htab_t *htab_core, void *dht_ac0, void *dht_ac1, void *dht_dc0, void *dht_dc1)
{
    uint8_t *buf;
    uint32_t len;

    len = (64 + 64 + 336) * 4;
    buf = hpm_jpeg_malloc(len);

    if (!buf) {
        HPM_JPEG_ELOG("malloc failed\n");
        return HPM_JPEG_RET_MALLOC_ERR;
    }

    memset(buf, 0x00, len);

    uint32_t *min = (uint32_t *)buf;
    uint32_t *hea = (uint32_t *)(buf + 64 * 4);
    uint32_t *heb = (uint32_t *)(buf + (64 + 64) * 4);
    uint16_t *huffenc = htab_core->huffenc;

    uint32_t i, j, n, v, code, hid;
    uint32_t dc, ht, abase, bbase, hbase, hb;
    uint32_t l[16];

    uint8_t *htab[4];
    uint8_t *p;
    htab[0] = dht_dc0;
    htab[1] = dht_ac0;

    htab[2] = dht_dc1 ? dht_dc1 : dht_dc0;
    htab[3] = dht_ac1 ? dht_ac1 : dht_ac0;

    for (i = 0; i < 384; i++)
        huffenc[i] = 0xfff;
    for (i = 168, j = 0xfd0; i < 176; i++, j++)
        huffenc[i] = j;
    for (i = 344, j = 0xfd0; i < 352; i++, j++)
        huffenc[i] = j;
    for (i = 162; i < 174; i++)
        heb[i] = 0;

    for (int index = 0; index < 4; index++) {
        p = htab[index] + 2;
        /* Get the length of the marker segment
         *
         * Lh : HT length (16b)
         */
        v = *p++;
        n = (v<<8) | *p++;
        n -= 2;

        while (n) {
            /* Get the type of table
             *
             * Tc : Table class (4b)
             *      0 = DC or lossless table
             *      1 = AC table
             * Th : HT destination identifier
             * - specifies 1 of 4 possible destinations at the decoder into
             * which HT shall be installed.
             */

            v = *p++; /* Tc & Th */
            n--;
            hid = v>>4 ? 2 : 0;
            hid |= v & 15 ? 1 : 0;
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
            default :
                hbase = 352;
                break;
            }

            if ((v>>4))
                abase = 0;
            else
                abase = 1;

            dc = abase;
            ht = v & 15;
            abase |= ht<<1;
            switch (abase) {
            case 1:
            case 3:
                bbase = 162;
                break;
            case 2 :
                bbase = 174;
                break;
            default :
                bbase = 0;
                break;
            }
            abase <<= 4;

            /* Memory initialization */
            for (i = abase; i < abase + 16; i++)
                hea[i] = 255;

            /* Get the number of codes for each length
             *
             * Lj : # of Huffman codes of length i
             * - specifies the # of Huffman codes for each of 16 possible lengths
             * allowed by spec. BITS
             */
            for (i = 0; i < 16; i++)
                l[i] = *p++;

            n -= 16;
            code = 0;
            for (i = 0; i < 16; i++, abase++) {
                min[abase] = code;
                hea[abase] = bbase - code;
                if (l[i]) {
                    /* Vi,j : associated with each Huffman code
                     * - specifies, for each i the value associated with each Huffman code
                     * of length i.  HUFFVAL
                     */
                    for (j = 0; j < l[i]; j++, bbase++) {
                        v = *p++;
                        n--;

                        if (dc) {
                            huffenc[hbase + v] = (i<<8) | (code & 0xff);
                            v &= 15;
                            if (ht)
                                v <<= 4;
                            heb[bbase] |= v;
                        } else {
                            if (v == 0)
                                hb = 160;
                            else if (v == 0xf0)
                                hb = 161;
                            else
                                hb = (v>>4) * 10 + (v & 0xf) - 1;

                            huffenc[hbase + hb] = (i<<8) | (code & 0xff);
                            heb[bbase] = v;
                        }
                        code++;
                    }
                }
                code <<= 1;
            }
        }
    }

    for (i = 0; i < 64; i++)
        htab_core->huffbase[i] = hea[i] & 511;

    for (i = 0; i < 336; i++)
        htab_core->huffsymb[i] = heb[i] & 255;

    for (i = 0, j = 3; i < 64; j += 7) {
        v = min[i++] & 1;
        v <<= 2;
        v |= min[i++] & 3;
        v <<= 3;
        v |= min[i++] & 7;
        htab_core->huffmin[j--] = v >> 2;

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
        htab_core->huffmin[j--] = v;

        v = min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        htab_core->huffmin[j--] = v;

        v = min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        htab_core->huffmin[j] = v;
    }

    hpm_jpeg_free(buf);

    return 0;
}

static const uint16_t qlook[256] = {
    0x0000, 0x0001, 0x0400, 0x02ab, 0x0200, 0x0b33, 0x0155, 0x0a49,
    0x0100, 0x09c7, 0x00cd, 0x12e9, 0x0955, 0x093b, 0x1249, 0x0911,
    0x0080, 0x08f1, 0x11c7, 0x11af, 0x08cd, 0x08c3, 0x08ba, 0x08b2,
    0x1155, 0x251f, 0x113b, 0x1a5f, 0x1a49, 0x1a35, 0x1111, 0x2421,
    0x0880, 0x23e1, 0x10f1, 0x10ea, 0x19c7, 0x19bb, 0x19af, 0x10d2,
    0x10cd, 0x231f, 0x10c3, 0x197d, 0x10ba, 0x10b6, 0x10b2, 0x22b9,
    0x1955, 0x229d, 0x10a4, 0x2d05, 0x193b, 0x1935, 0x225f, 0x1095,
    0x2249, 0x223f, 0x2235, 0x2c57, 0x1911, 0x2219, 0x1084, 0x1082,
    0x1080, 0x18fc, 0x18f8, 0x21e9, 0x18f1, 0x21db, 0x18ea, 0x2b9b,
    0x21c7, 0x21c1, 0x21bb, 0x21b5, 0x21af, 0x2b53, 0x18d2, 0x367b,
    0x18cd, 0x2b29, 0x18c8, 0x218b, 0x18c3, 0x2b03, 0x217d, 0x2af1,
    0x18ba, 0x18b8, 0x18b6, 0x18b4, 0x18b2, 0x2ac1, 0x2ab9, 0x2159,
    0x2155, 0x3547, 0x2a9d, 0x214b, 0x18a4, 0x2a89, 0x2141, 0x189f,
    0x213b, 0x189c, 0x2135, 0x34c9, 0x2a5f, 0x2a59, 0x1895, 0x349d,
    0x2a49, 0x1891, 0x2a3f, 0x211d, 0x2a35, 0x188c, 0x2a2b, 0x344d,
    0x2111, 0x343b, 0x2a19, 0x2a15, 0x1884, 0x3419, 0x1882, 0x1881,
    0x1880, 0x20fe, 0x20fc, 0x33e9, 0x20f8, 0x3fb3, 0x29e9, 0x33cb,
    0x20f1, 0x33bd, 0x29db, 0x33af, 0x20ea, 0x29d1, 0x20e7, 0x3395,
    0x29c7, 0x20e2, 0x29c1, 0x20df, 0x29bb, 0x20dc, 0x29b5, 0x20d9,
    0x29af, 0x3359, 0x3353, 0x29a7, 0x20d2, 0x3343, 0x299f, 0x20ce,
    0x20cd, 0x2997, 0x3329, 0x20c9, 0x20c8, 0x298d, 0x298b, 0x3311,
    0x20c3, 0x3e0f, 0x3303, 0x3dfd, 0x297d, 0x297b, 0x2979, 0x32ed,
    0x20ba, 0x3dc9, 0x20b8, 0x20b7, 0x20b6, 0x20b5, 0x20b4, 0x20b3,
    0x20b2, 0x3d89, 0x20b0, 0x32bd, 0x295d, 0x3d6b, 0x2959, 0x2957,
    0x2955, 0x32a7, 0x20a9, 0x20a8, 0x20a7, 0x3299, 0x294b, 0x3293,
    0x20a4, 0x20a3, 0x3289, 0x2943, 0x2941, 0x3cff, 0x209f, 0x3279,
    0x293b, 0x3273, 0x209c, 0x326d, 0x2935, 0x3ccf, 0x2099, 0x3cc3,
    0x325f, 0x2097, 0x3259, 0x3cad, 0x2095, 0x3251, 0x2927, 0x2093,
    0x3249, 0x3c8d, 0x2091, 0x3c83, 0x323f, 0x3c79, 0x291d, 0x3c6f,
    0x3235, 0x3c65, 0x208c, 0x2917, 0x208b, 0x3229, 0x3227, 0x3c49,
    0x2911, 0x2088, 0x290f, 0x3c37, 0x3219, 0x3217, 0x3215, 0x3c25,
    0x2084, 0x3c1d, 0x2083, 0x2905, 0x2082, 0x2903, 0x2081, 0x2901
};

static int hpm_jpeg_create_qtab_for_core(hpm_jpeg_core_qtab_t *qtab_core, int is_encode, uint8_t *jpeg_qtabs[])
{
    uint8_t *qtabs_data[4] = {NULL, NULL, NULL, NULL};
    uint16_t lq;
    uint8_t pq, tq;

    if (!jpeg_qtabs[0]) {
        HPM_JPEG_ELOG("jpeg_qtabs[0] must exist\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    for (int i = 0; i < 4; i++) {
        if (jpeg_qtabs[i]) {
            lq = jpeg_qtabs[i][2]<<8 | jpeg_qtabs[i][3];
            pq = (jpeg_qtabs[i][4]>>4) & 0x0f;
            tq = jpeg_qtabs[i][4] & 0x0f;
            if (lq == 67 && pq == 0 && tq <= 3) {
                qtabs_data[i] = jpeg_qtabs[i] + 5;
            } else {
                HPM_JPEG_ELOG("jpeg_qtabs[%d] is bad\n", i);
                return HPM_JPEG_RET_PARA_ERR;
            }
        } else {
            qtabs_data[i] = qtabs_data[0];
        }
    }

    memset(qtab_core, 0x00, sizeof(hpm_jpeg_core_qtab_t));

    for (int i = 0; i < 4; i++) {
        if (!qtabs_data[i]) {
            continue;
        }
        if (is_encode) {
            for (int j = 0; j < 64; j++) {
                qtab_core->qmem[i * 64 + j] = qlook[qtabs_data[i][j]];
            }
        } else {
            for (int j = 0; j < 64; j++) {
                qtab_core->qmem[i * 64 + j] = qtabs_data[i][j];
            }
        }
    }

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_get_align_pixel(hpm_jpeg_sampling_format_t sampling, uint16_t *x, uint16_t *y)
{
    uint16_t x_align;
    uint16_t y_align;

    if (sampling == HPM_JPEG_SAMPLING_FORMAT_420) {
        x_align = 16;
        y_align = 16;
    } else if (sampling == HPM_JPEG_SAMPLING_FORMAT_422H) {
        x_align = 16;
        y_align = 8;
    } else if (sampling == HPM_JPEG_SAMPLING_FORMAT_422V) {
        x_align = 8;
        y_align = 16;
    } else {
        x_align = 8;
        y_align = 8;
    }

    *x = x_align;
    *y = y_align;

    return HPM_JPEG_RET_OK;
}

static const hpm_jpeg_sampling_tab_t sampling_tabs[] = {
    {2, 2, 1, 1, HPM_JPEG_SAMPLING_FORMAT_420},
    {2, 1, 1, 1, HPM_JPEG_SAMPLING_FORMAT_422H},
    {1, 2, 1, 1, HPM_JPEG_SAMPLING_FORMAT_422V},
    {1, 1, 1, 1, HPM_JPEG_SAMPLING_FORMAT_444},
    {2, 2, 0, 0, HPM_JPEG_SAMPLING_FORMAT_400},
};

static int hpm_jpeg_get_sof0_info_from_buf(const void *sof0, hpm_jpeg_sof0_info_t *info)
{
    const uint8_t *p = sof0;
    const uint8_t *index = p + 10;

    if (p[0] != 0xff || p[1] != JPEG_MARKER_SOF0) {
        HPM_JPEG_ELOG("sof0 mark error\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    info->lf = (uint16_t)p[2]<<8 | p[3];
    info->p = p[4];
    info->y = (uint16_t)p[5]<<8 | p[6];
    info->x = (uint16_t)p[7]<<8 | p[8];
    info->nf = p[9];

    if (info->nf > 4) {
        HPM_JPEG_ELOG("hpm_jpeg support max 4 components, buf nf is %u\n", info->nf);
        return HPM_JPEG_RET_PARA_ERR;
    }

    for (int i = 0; i < info->nf; i++) {
        info->component[i].c = *index++;
        info->component[i].h = (*index)>>4 & 0x0f;
        info->component[i].v = (*index++)>>0 & 0x0f;
        info->component[i].tq = *index++;
    }

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_set_sof0_info_to_buf(void *sof0, const hpm_jpeg_sof0_info_t *info)
{
    uint8_t *p = sof0;
    uint8_t *index = p + 10;

    if (info->nf > 4) {
        HPM_JPEG_ELOG("hpm_jpeg support max 4 components, buf nf is %u\n", info->nf);
        return HPM_JPEG_RET_PARA_ERR;
    }

    p[0] = 0xff;
    p[1] = JPEG_MARKER_SOF0;
    p[2] = info->lf>>8 & 0xff;
    p[3] = info->lf>>0 & 0xff;
    p[4] = info->p;
    p[5] = info->y>>8 & 0xff;
    p[6] = info->y>>0 & 0xff;
    p[7] = info->x>>8 & 0xff;
    p[8] = info->x>>0 & 0xff;
    p[9] = info->nf;

    for (int i = 0; i < info->nf; i++) {
        *index++ = info->component[i].c;
        *index++ = (info->component[i].h & 0x0f)<<4 | (info->component[i].v & 0x0f);
        *index++ = info->component[i].tq;
    }

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_get_sampling_format_from_sof0_info(hpm_jpeg_sof0_info_t *info, hpm_jpeg_sampling_format_t *sampling)
{
    const hpm_jpeg_sampling_tab_t *s;
    int n = ARRAY_SIZE(sampling_tabs);

    for (int i = 0; i < n; i++) {
        s = &sampling_tabs[i];
        if (info->component[0].h == s->hy &&
            info->component[0].v == s->vy &&
            info->component[1].h == s->hc &&
            info->component[1].v == s->vc &&
            info->component[2].h == s->hc &&
            info->component[2].v == s->vc) {
            *sampling = s->sampling;
            return HPM_JPEG_RET_OK;
        }
    }

    return HPM_JPEG_RET_PARA_ERR;
}

static int hpm_jpeg_init_sof0_info(hpm_jpeg_sof0_info_t *info, hpm_jpeg_sampling_format_t sampling, uint16_t x, uint16_t y)
{
    const hpm_jpeg_sampling_tab_t *s = NULL;
    int n = ARRAY_SIZE(sampling_tabs);

    for (int i = 0; i < n; i++) {
        if (sampling == sampling_tabs[i].sampling) {
            s = &sampling_tabs[i];
            break;
        }
    }

    if (!s)
        return HPM_JPEG_RET_PARA_ERR;

    info->p = 8;
    info->x = x;
    info->y = y;

    if (sampling == HPM_JPEG_SAMPLING_FORMAT_400) {
        info->nf = 1;
        info->component[0].c = 1;
        info->component[0].h = s->hy;
        info->component[0].v = s->vy;
        info->component[0].tq = 0;
    } else {
        info->nf = 3;
        info->component[0].c = 1;
        info->component[0].h = s->hy;
        info->component[0].v = s->vy;
        info->component[0].tq = 0;

        info->component[1].c = 2;
        info->component[1].h = s->hc;
        info->component[1].v = s->vc;
        info->component[1].tq = 1;

        info->component[2].c = 3;
        info->component[2].h = s->hc;
        info->component[2].v = s->vc;
        info->component[2].tq = 1;
    }

    info->lf = 2 + 1 + 2 + 2 + 1 + info->nf * 3;
    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_get_sos_info_from_buf(const void *sos, hpm_jpeg_sos_info_t *info)
{
    const uint8_t *p = sos;
    const uint8_t *index = p + 5;

    if (p[0] != 0xff || p[1] != JPEG_MARKER_SOS) {
        HPM_JPEG_ELOG("sos mark error\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    info->ls = (uint16_t)p[2]<<8 | p[3];
    info->ns = p[4];

    if (info->ns > 4) {
        HPM_JPEG_ELOG("hpm_jpeg support max 4 components, buf ns is %u\n", info->ns);
        return HPM_JPEG_RET_PARA_ERR;
    }

    for (int i = 0; i < info->ns; i++) {
        info->component[i].cs = *index++;
        info->component[i].td = (*index)>>4 & 0x0f;
        info->component[i].ta = (*index++)>>0 & 0x0f;
    }

    info->ss = *index++;
    info->se = *index++;
    info->ah = (*index)>>4 & 0x0f;
    info->al = (*index)>>0 & 0x0f;

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_set_sos_info_to_buf(void *sos, const hpm_jpeg_sos_info_t *info)
{
    uint8_t *p = sos;
    uint8_t *index = p + 5;

    if (info->ns > 4) {
        HPM_JPEG_ELOG("hpm_jpeg support max 4 components, buf ns is %u\n", info->ns);
        return HPM_JPEG_RET_PARA_ERR;
    }

    p[0] = 0xff;
    p[1] = JPEG_MARKER_SOS;
    p[2] = info->ls>>8 & 0xff;
    p[3] = info->ls>>0 & 0xff;
    p[4] = info->ns;

    for (int i = 0; i < info->ns; i++) {
        *index++ = info->component[i].cs;
        *index++ = (info->component[i].td & 0x0f)<<4 | (info->component[i].ta & 0x0f);
    }

    *index++ = info->ss;
    *index++ = info->se;
    *index++ = (info->ah & 0x0f)<<4 | info->al;

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_init_sos_info(hpm_jpeg_sos_info_t *info, hpm_jpeg_sampling_format_t sampling)
{
    if (sampling == HPM_JPEG_SAMPLING_FORMAT_400) {
        info->ns = 1;
        info->component[0].cs = 1;
        info->component[0].ta = 0;
        info->component[0].td = 0;
    } else {
        info->ns = 3;
        info->component[0].cs = 1;
        info->component[0].ta = 0;
        info->component[0].td = 0;

        info->component[1].cs = 2;
        info->component[1].ta = 1;
        info->component[1].td = 1;

        info->component[2].cs = 3;
        info->component[2].ta = 1;
        info->component[2].td = 1;
    }

    info->ls = 2 + 1 + info->ns * 2 + 1 + 1 + 1;
    info->ss = 0;
    info->se = 63;
    info->ah = 0;
    info->al = 0;

    return HPM_JPEG_RET_OK;
}

static const uint8_t std_app0[] = {
    0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x00, 0x48,
    0x00, 0x48, 0x00, 0x00
};

static const uint8_t jpeg_std_htab_ac0[] = {
    0xFF, 0xC4, 0x00, 0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04,
    0x04, 0x00, 0x00, 0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41,
    0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1,
    0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44,
    0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64,
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84,
    0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2,
    0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9,
    0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3,
    0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA
};

static const uint8_t jpeg_std_htab_ac1[] = {
    0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04,
    0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12,
    0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1,
    0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1,
    0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43,
    0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63,
    0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82,
    0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
    0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
    0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5,
    0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3,
    0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA
};

static const uint8_t jpeg_std_htab_dc0[] = {
    0xFF, 0xC4, 0x00, 0x1F, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B
};

static const uint8_t jpeg_std_htab_dc1[] = {
    0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B
};

/* These are the sample quantization tables given in JPEG spec section K.1.
 * The spec says that the values given produce "good" quality, and
 * when divided by 2, "very good" quality.
 */
static const uint8_t std_luminance_qtab[64] = {
    16,  11,  10,  16,  24,  40,  51,  61,
    12,  12,  14,  19,  26,  58,  60,  55,
    14,  13,  16,  24,  40,  57,  69,  56,
    14,  17,  22,  29,  51,  87,  80,  62,
    18,  22,  37,  56,  68, 109, 103,  77,
    24,  35,  55,  64,  81, 104, 113,  92,
    49,  64,  78,  87, 103, 121, 120, 101,
    72,  92,  95,  98, 112, 100, 103,  99
};

static const uint8_t std_chrominance_qtab[64] = {
    17,  18,  24,  47,  99,  99,  99,  99,
    18,  21,  26,  66,  99,  99,  99,  99,
    24,  26,  56,  99,  99,  99,  99,  99,
    47,  66,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99
};

/* Convert a user-specified quality rating to a percentage scaling factor
 * for an underlying quantization table, using our recommended scaling curve.
 * The input 'quality' factor should be 0 (terrible) to 100 (very good).
 */
static int hpm_jpege_quality_scaling(int quality)
{
    /* Safety limit on quality factor.  Convert 0 to 1 to avoid zero divide. */
    if (quality <= 0)
        quality = 1;
    if (quality > 100)
        quality = 100;

    /* The basic table is used as-is (scaling 100) for a quality of 50.
     * Qualities 50..100 are converted to scaling percentage 200 - 2*Q;
     * note that at Q=100 the scaling is 0, which will cause jpeg_add_quant_table
     * to make all the table entries 1 (hence, minimum quantization loss).
     * Qualities 1..50 are converted to scaling percentage 5000/Q.
     */
    if (quality < 50)
        quality = 5000 / quality;
    else
        quality = 200 - quality * 2;

    return quality;
}

static void hpm_jpege_scale_qtab(uint8_t *qtab_scaled, const uint8_t *qtab_basic, int quality)
{
    int i;
    long temp;

    /* Convert user 0-100 rating to percentage scaling */
    int scale_factor = hpm_jpege_quality_scaling(quality);
    for (i = 0; i < 64; i++) {
        temp = ((long)qtab_basic[i] * scale_factor + 50L) / 100L;
        /* limit the values to the valid range */
        if (temp <= 0L)
            temp = 1L;
        if (temp > 255L)
            temp = 255L; /* limit to baseline range if requested */
        qtab_scaled[i] = (uint8_t)temp;
    }
}

static void hpm_jpege_get_qtab_luminance(uint8_t *qtab_zig_zag, int quality)
{
    hpm_jpege_scale_qtab(qtab_zig_zag, std_luminance_qtab, quality);
}

static void hpm_jpege_get_qtab_chrominance(uint8_t *qtab_zig_zag, int quality)
{
    hpm_jpege_scale_qtab(qtab_zig_zag, std_chrominance_qtab, quality);
}

static int hpm_jpeg_fill_jpeg_info_to_file(hpm_jpeg_encode_job_t *job, void *buf, uint32_t len)
{
    hpm_jpeg_jpeg_info_t *info = &job->jpeg_info;
    hpm_jpeg_sof0_info_t *sof0_info = &job->sof0_info;
    hpm_jpeg_sos_info_t *sos_info = &job->sos_info;
    hpm_jpeg_std_tab_t *std_tab = &job->std_tab;
    hpm_jpeg_encode_cfg_t *cfg = &job->cfg;
    hpm_jpeg_file_t *jpeg_file = &job->jpeg_file;
    uint8_t *pos = (uint8_t *)buf;
    int ret;

    if (len < 1024) {
        return HPM_JPEG_RET_PARA_ERR;
    }
    /*
     * Reserve for jpeg head
     */
    pos += 1024;
    info->ecs = pos;

    /*
     * Fill sos
     */
    pos -= (sos_info->ls + 2);
    info->sos = pos;
    ret = hpm_jpeg_set_sos_info_to_buf(info->sos, sos_info);
    if (ret != HPM_JPEG_RET_OK)
        return ret;

    /*
     * Fill dht
     */
    if (sos_info->ns == 3) {
        pos -= std_tab->dht_ac_len[1];
        info->dht_ac[1] = pos;
        memcpy(info->dht_ac[1], std_tab->dht_ac[1], std_tab->dht_ac_len[1]);

        pos -= std_tab->dht_dc_len[1];
        info->dht_dc[1] = pos;
        memcpy(info->dht_dc[1], std_tab->dht_dc[1], std_tab->dht_dc_len[1]);
    }

    pos -= std_tab->dht_ac_len[0];
    info->dht_ac[0] = pos;
    memcpy(info->dht_ac[0], std_tab->dht_ac[0], std_tab->dht_ac_len[0]);

    pos -= std_tab->dht_dc_len[0];
    info->dht_dc[0] = pos;
    memcpy(info->dht_dc[0], std_tab->dht_dc[0], std_tab->dht_dc_len[0]);

    /*
     * Fill sof0
     */
    pos -= (sof0_info->lf + 2);
    info->sof0 = pos;
    ret = hpm_jpeg_set_sof0_info_to_buf(info->sof0, sof0_info);
    if (ret != HPM_JPEG_RET_OK)
        return ret;

    /*
     * Fill dqt
     */
    if (sof0_info->nf == 3) {
        pos -= 69;
        info->dqt[1] = pos;
        info->dqt[1][0] = 0xff;
        info->dqt[1][1] = JPEG_MARKER_DQT;
        info->dqt[1][2] = 0;
        info->dqt[1][3] = 67;
        info->dqt[1][4] = 1; /* Pq, Tq */
        hpm_jpege_get_qtab_chrominance(&info->dqt[1][5], cfg->jpeg_quality);
    }

    pos -= 69;
    info->dqt[0] = pos;
    info->dqt[0][0] = 0xff;
    info->dqt[0][1] = JPEG_MARKER_DQT;
    info->dqt[0][2] = 0;
    info->dqt[0][3] = 67;
    info->dqt[0][4] = 0; /* Pq, Tq */
    hpm_jpege_get_qtab_luminance(&info->dqt[0][5], cfg->jpeg_quality);

    /*
     * Fill app0
     */
    pos -= std_tab->app0_len;
    info->app0 = pos;
    memcpy(info->app0, std_tab->app0, std_tab->app0_len);

    /*
     * Fill soi
     */
    pos -= 2;
    info->soi = pos;
    info->soi[0] = 0xff;
    info->soi[1] = JPEG_MARKER_SOI;

    /*
     * Recode head length
     */
    jpeg_file->jpeg_buf = info->soi;
    jpeg_file->len = ((uint32_t)info->ecs - (uint32_t)info->soi);

    HPM_JPEG_DLOG("jpeg soi start: 0x%x\n", (uint32_t)jpeg_file->jpeg_buf);
    HPM_JPEG_DLOG("jpeg head length: %u\n", jpeg_file->len);

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_get_jpeg_info_from_file(hpm_jpeg_jpeg_info_t *info, const void *buf, uint32_t len)
{
    uint8_t *pos = (uint8_t *)buf;
    uint8_t *pos_eof = pos + len;
    uint32_t marker_len;
    uint32_t marker;
    uint8_t is_ac;
    uint8_t htid;
    uint8_t qtid;

    memset(info, 0x00, sizeof(hpm_jpeg_jpeg_info_t));

    while (pos < pos_eof && !info->sos && !info->eoi) {
        if (*pos++ == 0xff) {
            marker = *pos++;
            marker_len = *pos;
            marker_len = marker_len<<8 | *(pos + 1);
            switch (marker) {
            case JPEG_MARKER_SOI:
                info->soi = pos - 2;
                break;
            case JPEG_MARKER_APP0:
                info->app0 = pos - 2;
                pos += marker_len;
                break;
            case JPEG_MARKER_DHT:
                is_ac = (*(pos + 2))>>4 & 0x0f;
                htid = (*(pos + 2)) & 0x0f;
                if (is_ac > 1 || htid > 1) {
                    return HPM_JPEG_RET_MARKER_ERR;
                }
                if (is_ac) {
                    info->dht_ac[htid] = pos - 2;
                } else {
                    info->dht_dc[htid] = pos - 2;
                }
                pos += marker_len;
                break;
            case JPEG_MARKER_DQT:
                qtid = (*(pos + 2)) & 0x0f;
                if (qtid > 3) {
                    return HPM_JPEG_RET_MARKER_ERR;
                }
                info->dqt[qtid] = pos - 2;
                pos += marker_len;
                break;
            case JPEG_MARKER_SOF0:
                info->sof0 = pos - 2;
                pos += marker_len;
                break;
            case JPEG_MARKER_SOS:
                info->sos = pos - 2;
                pos += marker_len;
                break;
            case JPEG_MARKER_EOI:
                info->eoi = pos - 2;
                break;
            case 0xff:
                pos--;
                break;
            default:
                break;
            }
        }
    }

    pos = (uint8_t *)buf;
    pos_eof = pos + len - 2;

    while (pos < pos_eof && !info->eoi) {
        if (*pos_eof == 0xff) {
            marker = *(pos_eof + 1);
            if (marker == JPEG_MARKER_EOI) {
                info->eoi = pos_eof;
                break;
            }
        }
        pos_eof--;
    }

    if (!info->dqt[0]) {
        HPM_JPEG_ELOG("jpeg can't find DQT\n");
        return HPM_JPEG_RET_JPEG_FILE_ERROR;
    }

    if (!info->eoi) {
        HPM_JPEG_ELOG("jpeg can't find EOI\n");
        return HPM_JPEG_RET_JPEG_FILE_ERROR;
    }

    if (!info->dht_ac[0]) {
        HPM_JPEG_ELOG("jpeg can't find DHT AC\n");
        return HPM_JPEG_RET_JPEG_FILE_ERROR;
    }

    if (!info->dht_dc[0]) {
        HPM_JPEG_ELOG("jpeg can't find DHT DC\n");
        return HPM_JPEG_RET_JPEG_FILE_ERROR;
    }

    if (!info->sos) {
        HPM_JPEG_ELOG("jpeg can't find SOS\n");
        return HPM_JPEG_RET_JPEG_FILE_ERROR;
    }

    marker_len = info->sos[2]<<8 | info->sos[3];
    info->ecs = info->sos + 2 + marker_len; /* skip sos */
    info->ecs_len = info->eoi - info->ecs + 2; /* include EOI */

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_get_pixel_size(hpm_jpeg_image_format_t format)
{
    static const struct {
        int size;
        hpm_jpeg_image_format_t format;
    } tab[6] = {
        {2, HPM_JPEG_IMAGE_FORMAT_RGB565},
        {4, HPM_JPEG_IMAGE_FORMAT_ARGB8888},
        {4, HPM_JPEG_IMAGE_FORMAT_BGRA8888},
        {2, HPM_JPEG_IMAGE_FORMAT_YUYV422},
        {2, HPM_JPEG_IMAGE_FORMAT_UYVY422},
        {1, HPM_JPEG_IMAGE_FORMAT_Y8},
    };

    for (uint32_t i = 0; i < ARRAY_SIZE(tab); i++) {
        if (format == tab[i].format)
            return tab[i].size;
    }

    return 0;
}

static int hpm_jpeg_decode_image_buf_init(hpm_jpeg_decode_job_t *job)
{
    hpm_jpeg_decode_cfg_t *cfg = &job->cfg;
    hpm_jpeg_buf_t *image_buf = &job->image_buf;
    hpm_jpeg_image_t *image = &job->image;
    hpm_jpeg_sof0_info_t *sof0_info = &job->sof0_info;
    int pixel_size;
    uint32_t buf_len;
    void *alloced_buf = NULL;
    uint32_t alloced_len;
    uint32_t aligned_len;
    uint32_t aligned_width;
    uint32_t aligned_height;
    uint32_t aligned_stride;

    pixel_size = hpm_jpeg_get_pixel_size(cfg->out_format);
    if (!pixel_size) {
        HPM_JPEG_ELOG("pixel size incorrect\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    if (image_buf->alloced_len > 0) {
        hpm_jpeg_free(image_buf->alloced_buf);
        image_buf->alloced_len = 0;
        image_buf->alloced_buf = NULL;
    }

    image->format = cfg->out_format;
    image->width = sof0_info->x;
    image->height = sof0_info->y;
    aligned_width = HPM_JPEG_ROUNDUP(sof0_info->x, job->x_align);
    aligned_height = HPM_JPEG_ROUNDUP(sof0_info->y, job->y_align);
    aligned_stride = pixel_size * aligned_width;

    if (cfg->out_image_stride > 0 && cfg->out_image_stride < aligned_stride) {
        HPM_JPEG_ELOG("out_image_stride(%u) too small\n", cfg->out_image_stride);
        return HPM_JPEG_RET_PARA_ERR;
    }

    if (cfg->out_image_stride == 0)
        image->stride = pixel_size * aligned_width;
    else
        image->stride = cfg->out_image_stride;

    buf_len = image->stride * aligned_height;
    image_buf->len = buf_len;

    if (image_buf->is_direct) {
        if (image_buf->aligned_len < buf_len) {
            HPM_JPEG_ELOG("force direct buf %u bytes too small, at least %u bytes are needed\n", image_buf->aligned_len, buf_len);
            return HPM_JPEG_RET_PARA_ERR;
        }
        image->image_buf = image_buf->aligned_buf;
        return HPM_JPEG_RET_OK;
    }

    aligned_len = HPM_JPEG_ROUNDUP(buf_len, HPM_JPEG_CACHE_LINE);
    alloced_len = aligned_len + HPM_JPEG_CACHE_LINE - 1;
    alloced_buf = hpm_jpeg_malloc(alloced_len);
    if (!alloced_buf) {
        HPM_JPEG_ELOG("malloc(%u) failed\n", (uint32_t)alloced_len);
        return HPM_JPEG_RET_MALLOC_ERR;
    }

    image_buf->alloced_buf = alloced_buf;
    image_buf->alloced_len = alloced_len;
    image_buf->aligned_buf = (void *)HPM_JPEG_ROUNDUP((uint32_t)alloced_buf, HPM_JPEG_CACHE_LINE);
    image_buf->aligned_len = aligned_len;
    hpm_jpeg_cache_invalid(image_buf->aligned_buf, image_buf->aligned_len);
    image->image_buf = image_buf->aligned_buf;

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_decode_image_buf_deinit(hpm_jpeg_decode_job_t *job)
{
    hpm_jpeg_buf_t *image_buf = &job->image_buf;

    if (image_buf->alloced_len) {
        hpm_jpeg_free(image_buf->alloced_buf);
    }

    memset(image_buf, 0x00, sizeof(hpm_jpeg_buf_t));

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_encode_file_buf_init(hpm_jpeg_encode_job_t *job)
{
    hpm_jpeg_buf_t *image_buf = &job->image_buf;
    hpm_jpeg_buf_t *file_buf = &job->file_buf;
    hpm_jpeg_file_t *jpeg_file = &job->jpeg_file;
    uint32_t buf_len;
    void *alloced_buf = NULL;
    uint32_t alloced_len;
    uint32_t aligned_len;

    buf_len = 1024 + image_buf->len;

    jpeg_file->len = 0;
    if (file_buf->is_direct) {
        if (file_buf->aligned_len < buf_len) {
            HPM_JPEG_ELOG("force direct buf %u bytes too small, at least %u bytes are needed\n", file_buf->aligned_len, buf_len);
            return HPM_JPEG_RET_PARA_ERR;
        }
        jpeg_file->jpeg_buf = file_buf->aligned_buf;

        if (file_buf->alloced_len > 0) {
            hpm_jpeg_free(file_buf->alloced_buf);
            file_buf->alloced_len = 0;
            file_buf->alloced_buf = NULL;
        }

        return HPM_JPEG_RET_OK;
    }

    aligned_len = HPM_JPEG_ROUNDUP(buf_len, HPM_JPEG_CACHE_LINE);
    alloced_len = aligned_len + HPM_JPEG_CACHE_LINE - 1;

    /*
     * The original memory is used if encode is restart and memory size isn't change.
     */
    if (alloced_len != file_buf->alloced_len) {
        if (file_buf->alloced_len > 0) {
            hpm_jpeg_free(file_buf->alloced_buf);
            file_buf->alloced_len = 0;
            file_buf->alloced_buf = NULL;
        }
        alloced_buf = hpm_jpeg_malloc(alloced_len);
        if (!alloced_buf) {
            HPM_JPEG_ELOG("malloc(%u) failed\n", (uint32_t)alloced_len);
            return HPM_JPEG_RET_MALLOC_ERR;
        }
        file_buf->len = aligned_len;
        file_buf->alloced_buf = alloced_buf;
        file_buf->alloced_len = alloced_len;
        file_buf->aligned_buf = (void *)HPM_JPEG_ROUNDUP((uint32_t)alloced_buf, HPM_JPEG_CACHE_LINE);
        file_buf->aligned_len = aligned_len;
    }

    hpm_jpeg_cache_invalid(file_buf->aligned_buf, file_buf->aligned_len);

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_encode_file_buf_deinit(hpm_jpeg_encode_job_t *job)
{
    hpm_jpeg_buf_t *file_buf = &job->file_buf;

    if (file_buf->alloced_len) {
        hpm_jpeg_free(file_buf->alloced_buf);
    }

    memset(file_buf, 0x00, sizeof(hpm_jpeg_buf_t));

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_interrupt_enable(hpm_jpeg_context_t *ctx)
{
    jpeg_enable_irq(ctx->jpeg_base, JPEG_EVENT_OUT_DMA_FINISH | JPEG_EVENT_ERROR);

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_decode_config_and_start(hpm_jpeg_decode_job_t *job)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_context_t *ctx = base_job->ctx;
    hpm_jpeg_buf_t *image_buf = &job->image_buf;
    hpm_jpeg_image_t *image = &job->image;
    hpm_jpeg_jpeg_info_t *jpeg_info = &job->jpeg_info;
    hpm_jpeg_sof0_info_t *sof0_info = &job->sof0_info;
    jpeg_job_config_t config = {0};

    if (image->format == HPM_JPEG_IMAGE_FORMAT_RGB565) {
        config.out_pixel_format = jpeg_pixel_format_rgb565;
        config.out_byte_order = JPEG_BYTE_ORDER_2301;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_ARGB8888) {
        config.out_pixel_format = jpeg_pixel_format_argb8888;
        config.out_byte_order = JPEG_BYTE_ORDER_3210;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_BGRA8888) {
        config.out_pixel_format = jpeg_pixel_format_argb8888;
        config.out_byte_order = JPEG_BYTE_ORDER_0123;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_YUYV422) {
        config.out_pixel_format = jpeg_pixel_format_yuv422h1p;
        config.out_byte_order = JPEG_BYTE_ORDER_3210;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_UYVY422) {
        config.out_pixel_format = jpeg_pixel_format_yuv422h1p;
        config.out_byte_order = JPEG_BYTE_ORDER_2301;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_Y8) {
        config.out_pixel_format = jpeg_pixel_format_y8;
        config.out_byte_order = JPEG_BYTE_ORDER_3210;
    } else {
        HPM_JPEG_ELOG("Image out format [%u] can't be supported\n", (uint32_t)image->format);
        return HPM_JPEG_RET_PARA_ERR;
    }

    config.enable_ycbcr = true;

    if (job->sampling == HPM_JPEG_SAMPLING_FORMAT_420)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_420;
    else if (job->sampling == HPM_JPEG_SAMPLING_FORMAT_422H)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_422H;
    else if (job->sampling == HPM_JPEG_SAMPLING_FORMAT_422V)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_422V;
    else if (job->sampling == HPM_JPEG_SAMPLING_FORMAT_444)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_444;
    else if (job->sampling == HPM_JPEG_SAMPLING_FORMAT_400)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_400;
    else {
        HPM_JPEG_ELOG("sampling [%u] can't be supported\n", (uint32_t)job->sampling);
        return HPM_JPEG_RET_PARA_ERR;
    }

    config.in_byte_order = JPEG_BYTE_ORDER_3210;
    config.width_in_pixel = HPM_JPEG_ROUNDUP(sof0_info->x, job->x_align);
    config.height_in_pixel = HPM_JPEG_ROUNDUP(sof0_info->y, job->y_align);
    config.in_buffer = (uint32_t)jpeg_info->ecs;
    config.out_buffer = (uint32_t)image_buf->aligned_buf;
    config.stride = image->stride;

    jpeg_start_decode(ctx->jpeg_base, &config, jpeg_info->ecs_len);
    hpm_jpeg_interrupt_enable(ctx);

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_encode_config_and_start(hpm_jpeg_encode_job_t *job)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_encode_cfg_t *cfg = &job->cfg;
    hpm_jpeg_context_t *ctx = base_job->ctx;
    hpm_jpeg_buf_t *image_buf = &job->image_buf;
    hpm_jpeg_image_t *image = &job->image;
    hpm_jpeg_jpeg_info_t *jpeg_info = &job->jpeg_info;
    hpm_jpeg_sof0_info_t *sof0_info = &job->sof0_info;
    jpeg_job_config_t config = {0};

    if (image->format == HPM_JPEG_IMAGE_FORMAT_RGB565) {
        config.in_pixel_format = jpeg_pixel_format_rgb565;
        config.in_byte_order = JPEG_BYTE_ORDER_3210;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_ARGB8888) {
        config.in_pixel_format = jpeg_pixel_format_argb8888;
        config.in_byte_order = JPEG_BYTE_ORDER_3210;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_BGRA8888) {
        config.in_pixel_format = jpeg_pixel_format_argb8888;
        config.in_byte_order = JPEG_BYTE_ORDER_0123;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_YUYV422) {
        config.in_pixel_format = jpeg_pixel_format_yuv422h1p;
        config.in_byte_order = JPEG_BYTE_ORDER_3210;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_UYVY422) {
        config.in_pixel_format = jpeg_pixel_format_yuv422h1p;
        config.in_byte_order = JPEG_BYTE_ORDER_2301;
    } else if (image->format == HPM_JPEG_IMAGE_FORMAT_Y8) {
        config.in_pixel_format = jpeg_pixel_format_y8;
        config.in_byte_order = JPEG_BYTE_ORDER_3210;
    } else {
        HPM_JPEG_ELOG("Image out format [%u] can't be supported\n", (uint32_t)image->format);
        return HPM_JPEG_RET_PARA_ERR;
    }

    config.enable_ycbcr = true;

    if (cfg->jpeg_sampling == HPM_JPEG_SAMPLING_FORMAT_420)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_420;
    else if (cfg->jpeg_sampling == HPM_JPEG_SAMPLING_FORMAT_422H)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_422H;
    else if (cfg->jpeg_sampling == HPM_JPEG_SAMPLING_FORMAT_422V)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_422V;
    else if (cfg->jpeg_sampling == HPM_JPEG_SAMPLING_FORMAT_444)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_444;
    else if (cfg->jpeg_sampling == HPM_JPEG_SAMPLING_FORMAT_400)
        config.jpeg_format = JPEG_SUPPORTED_FORMAT_400;
    else {
        HPM_JPEG_ELOG("sampling [%u] can't be supported\n", (uint32_t)cfg->jpeg_sampling);
        return HPM_JPEG_RET_PARA_ERR;
    }

    config.out_byte_order = JPEG_BYTE_ORDER_3210;
    config.width_in_pixel = HPM_JPEG_ROUNDUP(sof0_info->x, job->x_align);
    config.height_in_pixel = HPM_JPEG_ROUNDUP(sof0_info->y, job->y_align);
    config.out_buffer = (uint32_t)jpeg_info->ecs;
    config.in_buffer = (uint32_t)image_buf->aligned_buf;
    config.stride = image->stride;

    jpeg_start_encode(ctx->jpeg_base, &config);
    hpm_jpeg_interrupt_enable(ctx);

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_decode_init_tabs(hpm_jpeg_decode_job_t *job)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_context_t *ctx = base_job->ctx;
    hpm_jpeg_core_htab_t *core_htab = &job->core_htab;
    hpm_jpeg_core_qtab_t *core_qtab = &job->core_qtab;

    jpeg_enable(ctx->jpeg_base);
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_huffmin, (uint8_t *)(core_htab->huffmin), ARRAY_SIZE(core_htab->huffmin));
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_huffbase, (uint8_t *)(core_htab->huffbase), ARRAY_SIZE(core_htab->huffbase));
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_huffsymb, (uint8_t *)(core_htab->huffsymb), ARRAY_SIZE(core_htab->huffsymb));
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_huffenc, (uint8_t *)(core_htab->huffenc), ARRAY_SIZE(core_htab->huffenc));
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_qmem, (uint8_t *)(core_qtab->qmem), ARRAY_SIZE(core_qtab->qmem));
    jpeg_disable(ctx->jpeg_base);

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_encode_init_tabs(hpm_jpeg_encode_job_t *job)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_context_t *ctx = base_job->ctx;
    hpm_jpeg_core_htab_t *core_htab = &job->core_htab;
    hpm_jpeg_core_qtab_t *core_qtab = &job->core_qtab;

    jpeg_enable(ctx->jpeg_base);
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_huffmin, (uint8_t *)(core_htab->huffmin), ARRAY_SIZE(core_htab->huffmin));
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_huffbase, (uint8_t *)(core_htab->huffbase), ARRAY_SIZE(core_htab->huffbase));
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_huffsymb, (uint8_t *)(core_htab->huffsymb), ARRAY_SIZE(core_htab->huffsymb));
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_huffenc, (uint8_t *)(core_htab->huffenc), ARRAY_SIZE(core_htab->huffenc));
    jpeg_fill_table(ctx->jpeg_base, jpeg_table_qmem, (uint8_t *)(core_qtab->qmem), ARRAY_SIZE(core_qtab->qmem));
    jpeg_disable(ctx->jpeg_base);

    return HPM_JPEG_RET_OK;
}

static int hpm_jpeg_start_next_job(hpm_jpeg_context_t *ctx)
{
    hpm_jpeg_base_job_t *base_job;
    hpm_jpeg_decode_job_t *djob;
    hpm_jpeg_encode_job_t *ejob;
    jpeg_list_t *next_job;
    int ret;
    uint32_t lock;

    lock = hpm_jpeg_lock();
    do {
        next_job = jpeg_list_node_get(&ctx->wait_jobs);
        djob = (hpm_jpeg_decode_job_t *)next_job;
        ejob = (hpm_jpeg_encode_job_t *)next_job;
        base_job = (hpm_jpeg_base_job_t *)next_job;
        jpeg_init(ctx->jpeg_base);
        if (next_job && base_job->codec_type == HPM_JPEG_CODEC_DECODE) {
            base_job->status = HPM_JPEG_JOB_STATUS_STARTING;
            hpm_jpeg_decode_init_tabs(djob);
            ret = hpm_jpeg_decode_config_and_start(djob);
            if (ret != HPM_JPEG_RET_OK) {
                base_job->status = HPM_JPEG_JOB_STATUS_START_FAILED;
                if (base_job->cb)
                    base_job->cb((hpm_jpeg_job_t *)base_job);
            }
            base_job->status = HPM_JPEG_JOB_STATUS_PROCESSING;
        } else if (next_job && base_job->codec_type == HPM_JPEG_CODEC_ENCODE) {
            base_job->status = HPM_JPEG_JOB_STATUS_STARTING;
            hpm_jpeg_encode_init_tabs(ejob);
            ret = hpm_jpeg_encode_config_and_start(ejob);
            if (ret != HPM_JPEG_RET_OK) {
                base_job->status = HPM_JPEG_JOB_STATUS_START_FAILED;
                if (base_job->cb)
                    base_job->cb((hpm_jpeg_job_t *)base_job);
            }
            base_job->status = HPM_JPEG_JOB_STATUS_PROCESSING;
        } else if (next_job) {
            /*
             * Unknown codec type
             */
            ret = HPM_JPEG_RET_STATUS_ERR;
            base_job->status = HPM_JPEG_JOB_STATUS_START_FAILED;
            if (base_job->cb)
                base_job->cb((hpm_jpeg_job_t *)base_job);
        }
    } while (next_job && ret != HPM_JPEG_RET_OK);

    ctx->work_job = next_job;
    hpm_jpeg_unlock(lock);

    return HPM_JPEG_RET_OK;
}

#define JPEG_INFO_DUMP() \
{\
    HPM_JPEG_DLOG("file_buf->is_direct: 0x%x\n", (uint32_t)file_buf->is_direct);\
    HPM_JPEG_DLOG("file_buf->alloced_buf: 0x%x\n", (uint32_t)file_buf->alloced_buf);\
    HPM_JPEG_DLOG("file_buf->alloced_len: %u\n", (uint32_t)file_buf->alloced_len);\
    HPM_JPEG_DLOG("file_buf->aligned_buf: 0x%x\n", (uint32_t)file_buf->aligned_buf);\
    HPM_JPEG_DLOG("file_buf->aligned_len: %u\n", (uint32_t)file_buf->aligned_len);\
\
    HPM_JPEG_DLOG("image_buf->is_direct: 0x%x\n", (uint32_t)image_buf->is_direct);\
    HPM_JPEG_DLOG("image_buf->alloced_buf: 0x%x\n", (uint32_t)image_buf->alloced_buf);\
    HPM_JPEG_DLOG("image_buf->alloced_len: %u\n", (uint32_t)image_buf->alloced_len);\
    HPM_JPEG_DLOG("image_buf->aligned_buf: 0x%x\n", (uint32_t)image_buf->aligned_buf);\
    HPM_JPEG_DLOG("image_buf->aligned_len: %u\n", (uint32_t)image_buf->aligned_len);\
\
    HPM_JPEG_DLOG("image->format: %u\n", (uint32_t)image->format);\
    HPM_JPEG_DLOG("image->height: %u\n", (uint32_t)image->height);\
    HPM_JPEG_DLOG("image->width: %u\n", (uint32_t)image->width);\
    HPM_JPEG_DLOG("image->stride: %u\n", (uint32_t)image->stride);\
    HPM_JPEG_DLOG("image->image_buf: 0x%x\n", (uint32_t)image->image_buf);\
\
    HPM_JPEG_DLOG("jpeg_info->soi: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->soi,\
                                (uint32_t)(jpeg_info->soi - jpeg_info->soi),\
                                (uint32_t)(jpeg_info->soi - jpeg_info->soi));\
    HPM_JPEG_DLOG("jpeg_info->app0: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->app0,\
                                (uint32_t)(jpeg_info->app0 - jpeg_info->soi),\
                                (uint32_t)(jpeg_info->app0 - jpeg_info->soi));\
    HPM_JPEG_DLOG("jpeg_info->dqt[0]: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->dqt[0],\
                                (uint32_t)(jpeg_info->dqt[0] ?  jpeg_info->dqt[0] - jpeg_info->soi : 0),\
                                (uint32_t)(jpeg_info->dqt[0] ?  jpeg_info->dqt[0] - jpeg_info->soi : 0));\
    HPM_JPEG_DLOG("jpeg_info->dqt[1]: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->dqt[1],\
                                (uint32_t)(jpeg_info->dqt[1] ?  jpeg_info->dqt[1] - jpeg_info->soi : 0),\
                                (uint32_t)(jpeg_info->dqt[1] ?  jpeg_info->dqt[1] - jpeg_info->soi : 0));\
    HPM_JPEG_DLOG("jpeg_info->dqt[2]: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->dqt[2],\
                                (uint32_t)(jpeg_info->dqt[2] ?  jpeg_info->dqt[2] - jpeg_info->soi : 0),\
                                (uint32_t)(jpeg_info->dqt[2] ?  jpeg_info->dqt[2] - jpeg_info->soi : 0));\
    HPM_JPEG_DLOG("jpeg_info->sof0: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->sof0,\
                                (uint32_t)(jpeg_info->sof0 - jpeg_info->soi),\
                                (uint32_t)(jpeg_info->sof0 - jpeg_info->soi));\
    HPM_JPEG_DLOG("jpeg_info->dht_dc[0]: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->dht_dc[0],\
                                (uint32_t)(jpeg_info->dht_dc[0] ? jpeg_info->dht_dc[0] - jpeg_info->soi : 0),\
                                (uint32_t)(jpeg_info->dht_dc[0] ? jpeg_info->dht_dc[0] - jpeg_info->soi : 0));\
    HPM_JPEG_DLOG("jpeg_info->dht_ac[0]: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->dht_ac[0],\
                                (uint32_t)(jpeg_info->dht_ac[0] ? jpeg_info->dht_ac[0] - jpeg_info->soi : 0),\
                                (uint32_t)(jpeg_info->dht_ac[0] ? jpeg_info->dht_ac[0] - jpeg_info->soi : 0));\
    HPM_JPEG_DLOG("jpeg_info->dht_dc[1]: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->dht_dc[1],\
                                (uint32_t)(jpeg_info->dht_dc[1] ? jpeg_info->dht_dc[1] - jpeg_info->soi : 0),\
                                (uint32_t)(jpeg_info->dht_dc[1] ? jpeg_info->dht_dc[1] - jpeg_info->soi : 0));\
    HPM_JPEG_DLOG("jpeg_info->dht_ac[1]: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->dht_ac[1],\
                                (uint32_t)(jpeg_info->dht_ac[1] ? jpeg_info->dht_ac[1] - jpeg_info->soi : 0),\
                                (uint32_t)(jpeg_info->dht_ac[1] ? jpeg_info->dht_ac[1] - jpeg_info->soi : 0));\
    HPM_JPEG_DLOG("jpeg_info->sos: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->sos,\
                                (uint32_t)(jpeg_info->sos - jpeg_info->soi),\
                                (uint32_t)(jpeg_info->sos - jpeg_info->soi));\
    HPM_JPEG_DLOG("jpeg_info->ecs: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->ecs,\
                                (uint32_t)(jpeg_info->ecs - jpeg_info->soi),\
                                (uint32_t)(jpeg_info->ecs - jpeg_info->soi));\
    HPM_JPEG_DLOG("jpeg_info->ecs_len: %u\n", (uint32_t)jpeg_info->ecs_len);\
    HPM_JPEG_DLOG("jpeg_info->eoi: 0x%x, offset: %u[0x%x]\n",\
                                (uint32_t)jpeg_info->eoi,\
                                (uint32_t)(jpeg_info->eoi - jpeg_info->soi),\
                                (uint32_t)(jpeg_info->eoi - jpeg_info->soi));\
\
    HPM_JPEG_DLOG("sof0 x: %u\n", sof0_info->x);\
    HPM_JPEG_DLOG("sof0 y: %u\n", sof0_info->y);\
    HPM_JPEG_DLOG("sof0 nf: %u\n", sof0_info->nf);\
    for (uint32_t i = 0; i < sof0_info->nf; i++) {\
        HPM_JPEG_DLOG("sof0 component[%u].c: %u\n", i, sof0_info->component[i].c);\
        HPM_JPEG_DLOG("sof0 component[%u].h: %u\n", i, sof0_info->component[i].h);\
        HPM_JPEG_DLOG("sof0 component[%u].v: %u\n", i, sof0_info->component[i].v);\
        HPM_JPEG_DLOG("sof0 component[%u].tq: %u\n", i, sof0_info->component[i].tq);\
    } \
\
    HPM_JPEG_DLOG("sos ns: %u\n", sos_info->ns);\
    for (uint32_t i = 0; i < sos_info->ns; i++) {\
        HPM_JPEG_DLOG("sos component[%u].cs: %u\n", i, sos_info->component[i].cs);\
        HPM_JPEG_DLOG("sos component[%u].ta: %u\n", i, sos_info->component[i].ta);\
        HPM_JPEG_DLOG("sos component[%u].td: %u\n", i, sos_info->component[i].td);\
    } \
}

hpm_jpeg_job_t *hpm_jpeg_decode_job_alloc(hpm_jpeg_decode_cfg_t *cfg)
{
    hpm_jpeg_base_job_t *base_job;
    hpm_jpeg_decode_job_t *job;

    base_job = hpm_jpeg_malloc(sizeof(hpm_jpeg_decode_job_t));
    if (!base_job) {
        HPM_JPEG_ELOG("malloc(%u) failed\n", (uint32_t)sizeof(hpm_jpeg_decode_job_t));
        return NULL;
    }

    memset(base_job, 0x00, sizeof(hpm_jpeg_decode_job_t));
    jpeg_list_node_init(&base_job->list);

    job = (hpm_jpeg_decode_job_t *)base_job;
    memcpy(&job->cfg, cfg, sizeof(hpm_jpeg_decode_cfg_t));

    base_job->codec_type = HPM_JPEG_CODEC_DECODE;
    base_job->ctx = &hpm_jpeg_context;

    return (hpm_jpeg_job_t *)base_job;
}

int hpm_jpeg_decode_job_fill_file(hpm_jpeg_job_t *job, const hpm_jpeg_file_t *file, int direct)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_decode_job_t *_job = (hpm_jpeg_decode_job_t *)job;
    hpm_jpeg_buf_t *file_buf = &_job->file_buf;
    void *alloced_buf = NULL;
    uint32_t alloced_len;
    uint32_t aligned_len;
    uint32_t lock;
    int ret = HPM_JPEG_RET_OK;

    if (!file || !file->jpeg_buf || !file->len) {
        HPM_JPEG_ELOG("parameter error\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    if (base_job->status == HPM_JPEG_JOB_STATUS_FILLING ||
        base_job->status == HPM_JPEG_JOB_STATUS_STARTING ||
        base_job->status == HPM_JPEG_JOB_STATUS_WAITING ||
        base_job->status == HPM_JPEG_JOB_STATUS_PROCESSING) {
        HPM_JPEG_ELOG("job is busying\n");
        return HPM_JPEG_RET_STATUS_ERR;
    }

    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_FILLING;
    hpm_jpeg_unlock(lock);

    if (file_buf->alloced_len) {
        hpm_jpeg_free(file_buf->alloced_buf);
        file_buf->alloced_len = 0;
        file_buf->alloced_buf = NULL;
    }

    if (!direct) {
        aligned_len = HPM_JPEG_ROUNDUP(file->len, HPM_JPEG_CACHE_LINE);
        alloced_len = aligned_len + HPM_JPEG_CACHE_LINE - 1;
        alloced_buf = hpm_jpeg_malloc(alloced_len);
        if (!alloced_buf) {
            HPM_JPEG_ELOG("malloc(%u) failed\n", (uint32_t)alloced_len);
            ret = HPM_JPEG_RET_MALLOC_ERR;
            goto ERROR1;
        }

        file_buf->alloced_buf = alloced_buf;
        file_buf->alloced_len = alloced_len;
        file_buf->aligned_buf = (void *)HPM_JPEG_ROUNDUP((uint32_t)alloced_buf, HPM_JPEG_CACHE_LINE);
        file_buf->aligned_len = aligned_len;

        memcpy(file_buf->aligned_buf, file->jpeg_buf, file->len);
        hpm_jpeg_cache_writeback(file_buf->aligned_buf, file_buf->aligned_len);
    } else {
        file_buf->is_direct = 1;
        file_buf->aligned_buf = file->jpeg_buf;
        file_buf->aligned_len = file->len;
    }

    file_buf->len = file->len;

    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_FILLED;
    hpm_jpeg_unlock(lock);

    return ret;

ERROR1:
    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_FILL_FAILED;
    hpm_jpeg_unlock(lock);

    return ret;
}

int hpm_jpeg_decode_job_force_direct_image_buf(hpm_jpeg_job_t *job, void *buf, uint32_t len)
{
    hpm_jpeg_decode_job_t *_job = (hpm_jpeg_decode_job_t *)job;
    hpm_jpeg_buf_t *image_buf = &_job->image_buf;
    uint32_t lock;

    if (!buf || !len) {
        HPM_JPEG_ELOG("parameter error\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    lock = hpm_jpeg_lock();
    image_buf->len = len;
    image_buf->aligned_buf = buf;
    image_buf->aligned_len = len;
    image_buf->is_direct = 1;
    hpm_jpeg_unlock(lock);

    return HPM_JPEG_RET_OK;
}

int hpm_jpeg_decode_job_start(hpm_jpeg_job_t *job, hpm_jpeg_job_state_cb_t cb)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_decode_job_t *_job = (hpm_jpeg_decode_job_t *)job;
    hpm_jpeg_context_t *ctx = base_job->ctx;
    hpm_jpeg_buf_t *file_buf = &_job->file_buf;
    hpm_jpeg_buf_t *image_buf = &_job->image_buf;
    hpm_jpeg_image_t *image = &_job->image;
    hpm_jpeg_jpeg_info_t *jpeg_info = &_job->jpeg_info;
    hpm_jpeg_sof0_info_t *sof0_info = &_job->sof0_info;
    hpm_jpeg_sos_info_t *sos_info = &_job->sos_info;
    int ret = HPM_JPEG_RET_OK;
    uint32_t lock;

    if (base_job->status == HPM_JPEG_JOB_STATUS_FILLING ||
        base_job->status == HPM_JPEG_JOB_STATUS_STARTING ||
        base_job->status == HPM_JPEG_JOB_STATUS_WAITING ||
        base_job->status == HPM_JPEG_JOB_STATUS_PROCESSING) {
        HPM_JPEG_ELOG("job is busying\n");
        return HPM_JPEG_RET_STATUS_ERR;
    }

    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_STARTING;
    hpm_jpeg_unlock(lock);

    base_job->cb = cb;

    ret = hpm_jpeg_get_jpeg_info_from_file(jpeg_info, file_buf->aligned_buf, file_buf->len);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_get_sof0_info_from_buf(jpeg_info->sof0, sof0_info);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_get_sos_info_from_buf(jpeg_info->sos, sos_info);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_get_sampling_format_from_sof0_info(sof0_info, &_job->sampling);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_get_align_pixel(_job->sampling, &_job->x_align, &_job->y_align);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_create_qtab_for_core(&_job->core_qtab, 0, jpeg_info->dqt);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_create_htab_for_core(&_job->core_htab, jpeg_info->dht_ac[0],
                                jpeg_info->dht_ac[1], jpeg_info->dht_dc[0],
                                jpeg_info->dht_dc[1]);

    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_decode_image_buf_init(_job);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR2;

    JPEG_INFO_DUMP();
    (void)image;
    (void)image_buf;

    lock = hpm_jpeg_lock();
    if (!ctx->work_job) {
        ctx->work_job = &base_job->list;
        jpeg_init(ctx->jpeg_base);
        hpm_jpeg_decode_init_tabs(job);
        ret = hpm_jpeg_decode_config_and_start(_job);
        if (ret != HPM_JPEG_RET_OK) {
            hpm_jpeg_unlock(lock);
            goto ERROR2;
        }

        base_job->status = HPM_JPEG_JOB_STATUS_PROCESSING;
    } else {
        jpeg_list_add_tail(&ctx->wait_jobs, &base_job->list);
        base_job->status = HPM_JPEG_JOB_STATUS_WAITING;
    }
    hpm_jpeg_unlock(lock);

    return ret;

ERROR2:
    hpm_jpeg_decode_image_buf_deinit(_job);
ERROR1:
    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_START_FAILED;
    hpm_jpeg_unlock(lock);

    return ret;
}

int hpm_jpeg_decode_job_stop(hpm_jpeg_job_t *job)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_context_t *ctx = base_job->ctx;
    uint32_t lock;

    lock = hpm_jpeg_lock();
    if (ctx->work_job == &base_job->list) {
        hpm_jpeg_start_next_job(ctx);
    } else {
        jpeg_list_del(&base_job->list);
    }
    base_job->status = HPM_JPEG_JOB_STATUS_STOPPED;
    hpm_jpeg_unlock(lock);

    return HPM_JPEG_RET_OK;
}

int hpm_jpeg_decode_job_get_info(hpm_jpeg_job_t *job, hpm_jpeg_decode_info_t *info)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_decode_job_t *_job = (hpm_jpeg_decode_job_t *)job;

    info->hw_status = base_job->hw_status;
    info->status = base_job->status;
    info->image = &_job->image;

    return HPM_JPEG_RET_OK;
}

int hpm_jpeg_decode_job_free(hpm_jpeg_job_t *job)
{
    hpm_jpeg_decode_job_t *_job = (hpm_jpeg_decode_job_t *)job;
    hpm_jpeg_buf_t *file_buf = &_job->file_buf;
    hpm_jpeg_buf_t *image_buf = &_job->image_buf;

    hpm_jpeg_decode_job_stop(job);

    if (image_buf->alloced_len)
        hpm_jpeg_free(image_buf->alloced_buf);

    if (file_buf->alloced_len)
        hpm_jpeg_free(file_buf->alloced_buf);

    hpm_jpeg_free(job);

    return HPM_JPEG_RET_OK;
}

hpm_jpeg_job_t *hpm_jpeg_encode_job_alloc(hpm_jpeg_encode_cfg_t *cfg)
{
    hpm_jpeg_base_job_t *base_job;
    hpm_jpeg_encode_job_t *job;

    base_job = hpm_jpeg_malloc(sizeof(hpm_jpeg_encode_job_t));
    if (!base_job) {
        HPM_JPEG_ELOG("malloc failed\n");
        return NULL;
    }

    memset(base_job, 0x00, sizeof(hpm_jpeg_encode_job_t));
    jpeg_list_node_init(&base_job->list);

    job = (hpm_jpeg_encode_job_t *)base_job;
    memcpy(&job->cfg, cfg, sizeof(hpm_jpeg_encode_cfg_t));

    base_job->codec_type = HPM_JPEG_CODEC_ENCODE;
    base_job->ctx = &hpm_jpeg_context;

    hpm_jpeg_get_align_pixel(cfg->jpeg_sampling, &job->x_align, &job->y_align);

    return (hpm_jpeg_job_t *)base_job;
}

int hpm_jpeg_encode_job_fill_image(hpm_jpeg_job_t *job, const hpm_jpeg_image_t *image, int direct)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_encode_job_t *_job = (hpm_jpeg_encode_job_t *)job;
    hpm_jpeg_buf_t *image_buf = &_job->image_buf;
    hpm_jpeg_image_t *job_image = &_job->image;
    uint32_t src_stride;
    uint32_t aligned_stride;
    uint32_t image_stride;
    uint32_t aligned_width;
    uint32_t aligned_height;
    int pixel_size;
    void *alloced_buf = NULL;
    uint32_t alloced_len;
    uint32_t aligned_len;
    uint32_t lock;
    int ret = HPM_JPEG_RET_OK;

    if (!image) {
        HPM_JPEG_ELOG("parameter error\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    if (base_job->status == HPM_JPEG_JOB_STATUS_FILLING ||
        base_job->status == HPM_JPEG_JOB_STATUS_STARTING ||
        base_job->status == HPM_JPEG_JOB_STATUS_WAITING ||
        base_job->status == HPM_JPEG_JOB_STATUS_PROCESSING) {
        HPM_JPEG_ELOG("job is busying\n");
        return HPM_JPEG_RET_STATUS_ERR;
    }

    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_FILLING;
    hpm_jpeg_unlock(lock);

    aligned_width = HPM_JPEG_ROUNDUP(image->width, _job->x_align);
    aligned_height = HPM_JPEG_ROUNDUP(image->height, _job->y_align);
    pixel_size = hpm_jpeg_get_pixel_size(image->format);
    src_stride = image->width * pixel_size;
    aligned_stride = aligned_width * pixel_size;

    if (direct && (aligned_width != image->width || aligned_height != image->height)) {
        HPM_JPEG_ELOG("Width must align to %u pixels if direct Mode is used!!!\n", _job->x_align);
        HPM_JPEG_ELOG("Height must align to %u pixels if direct Mode is used!!!\n", _job->y_align);
        ret =  HPM_JPEG_RET_PARA_ERR;
        goto ERROR1;
    }

    if (direct && image->stride > 0 && image->stride < aligned_stride) {
        HPM_JPEG_ELOG("Stride is too small, at least %u are needed\n", aligned_stride);
        ret =  HPM_JPEG_RET_PARA_ERR;
        goto ERROR1;
    }

    if (image->stride > 0 && image->stride < src_stride) {
        HPM_JPEG_ELOG("Stride is too small, at least %u are needed\n", src_stride);
        ret =  HPM_JPEG_RET_PARA_ERR;
        goto ERROR1;
    }

    src_stride = image->stride ? image->stride : src_stride;
    image_stride = image->stride ? image->stride : aligned_stride;
    job_image->format = image->format;
    job_image->height = image->height;
    job_image->width = image->width;

    if (direct) {
        image_buf->len = image_stride * image->height;
        image_buf->aligned_buf = image->image_buf;
        image_buf->aligned_len = image_buf->len;
        image_buf->is_direct = 1;

        job_image->image_buf = image_buf->aligned_buf;
        job_image->stride = image_stride;
    } else {
        aligned_len = HPM_JPEG_ROUNDUP(image_stride * aligned_height, HPM_JPEG_CACHE_LINE);
        alloced_len = aligned_len + HPM_JPEG_CACHE_LINE - 1;

        if (image_buf->alloced_len) {
            hpm_jpeg_free(image_buf->alloced_buf);
            image_buf->alloced_len = 0;
            image_buf->alloced_buf = 0;
        }

        alloced_buf = hpm_jpeg_malloc(alloced_len);
        if (!alloced_buf) {
            HPM_JPEG_ELOG("malloc failed\n");
            ret =  HPM_JPEG_RET_MALLOC_ERR;
            goto ERROR1;
        }

        image_buf->len = image_stride * image->height;
        image_buf->alloced_len = alloced_len;
        image_buf->alloced_buf = alloced_buf;
        image_buf->aligned_buf = (void *)HPM_JPEG_ROUNDUP((uint32_t)alloced_buf, HPM_JPEG_CACHE_LINE);
        image_buf->aligned_len = aligned_len;

        job_image->image_buf = image_buf->aligned_buf;
        job_image->stride = image_stride;

        uint8_t *src8 = (uint8_t *)image->image_buf;
        uint8_t *dst8 = (uint8_t *)job_image->image_buf;
        uint32_t h;
        for (h = 0; h < image->height; h++) {
            memcpy(dst8, src8, src_stride);
            src8 += src_stride;
            dst8 += job_image->stride;
        }

        /*
         * fill pixel to jpeg block align
         */
        for (; h < aligned_height; h++) {
            memset(dst8, 0x00, job_image->stride);
            dst8 += job_image->stride;
        }

        if (image_stride != job_image->stride) {
            uint32_t diff = job_image->stride - image_stride;
            dst8 = (uint8_t *)job_image->image_buf;
            dst8 += image_stride;
            for (h = 0; h < image->height; h++) {
                memset(dst8, 0x00, diff);
                dst8 += job_image->stride;
            }
        }
        hpm_jpeg_cache_writeback(image_buf->aligned_buf, image_buf->aligned_len);
    }

    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_FILLED;
    hpm_jpeg_unlock(lock);

    return ret;

ERROR1:
    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_FILL_FAILED;
    hpm_jpeg_unlock(lock);

    return ret;
}

int hpm_jpeg_encode_job_force_direct_file_buf(hpm_jpeg_job_t *job, void *file_buf, uint32_t len)
{
    hpm_jpeg_encode_job_t *_job = (hpm_jpeg_encode_job_t *)job;
    hpm_jpeg_buf_t *_file_buf = &_job->file_buf;
    uint32_t lock;

    if (!file_buf || !len) {
        HPM_JPEG_ELOG("parameter error\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    lock = hpm_jpeg_lock();
    _file_buf->is_direct = 1;
    _file_buf->len = len;
    _file_buf->aligned_buf = file_buf;
    _file_buf->aligned_len = len;
    hpm_jpeg_unlock(lock);

    return HPM_JPEG_RET_OK;
}

int hpm_jpeg_encode_job_start(hpm_jpeg_job_t *job, hpm_jpeg_job_state_cb_t cb)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_encode_job_t *_job = (hpm_jpeg_encode_job_t *)job;
    hpm_jpeg_encode_cfg_t *cfg = &_job->cfg;
    hpm_jpeg_context_t *ctx = base_job->ctx;
    hpm_jpeg_buf_t *file_buf = &_job->file_buf;
    hpm_jpeg_buf_t *image_buf = &_job->image_buf;
    hpm_jpeg_image_t *image = &_job->image;
    hpm_jpeg_std_tab_t *std_tab = &_job->std_tab;
    hpm_jpeg_jpeg_info_t *jpeg_info = &_job->jpeg_info;
    hpm_jpeg_sof0_info_t *sof0_info = &_job->sof0_info;
    hpm_jpeg_sos_info_t *sos_info = &_job->sos_info;
    int ret = HPM_JPEG_RET_OK;
    uint32_t lock;

    if (base_job->status == HPM_JPEG_JOB_STATUS_FILLING ||
        base_job->status == HPM_JPEG_JOB_STATUS_STARTING ||
        base_job->status == HPM_JPEG_JOB_STATUS_WAITING ||
        base_job->status == HPM_JPEG_JOB_STATUS_PROCESSING) {
        HPM_JPEG_ELOG("job is busying\n");
        return HPM_JPEG_RET_STATUS_ERR;
    }

    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_STARTING;
    hpm_jpeg_unlock(lock);

    base_job->cb = cb;

    ret = hpm_jpeg_init_sof0_info(sof0_info, cfg->jpeg_sampling, image->width, image->height);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_init_sos_info(sos_info, cfg->jpeg_sampling);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR1;

    ret = hpm_jpeg_encode_file_buf_init(_job);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR2;

    std_tab->app0 = std_app0;
    std_tab->app0_len = sizeof(std_app0);
    std_tab->dht_ac[0] = jpeg_std_htab_ac0;
    std_tab->dht_ac_len[0] = sizeof(jpeg_std_htab_ac0);
    std_tab->dht_ac[1] = jpeg_std_htab_ac1;
    std_tab->dht_ac_len[1] = sizeof(jpeg_std_htab_ac1);
    std_tab->dht_dc[0] = jpeg_std_htab_dc0;
    std_tab->dht_dc_len[0] = sizeof(jpeg_std_htab_dc0);
    std_tab->dht_dc[1] = jpeg_std_htab_dc1;
    std_tab->dht_dc_len[1] = sizeof(jpeg_std_htab_dc1);

    ret = hpm_jpeg_fill_jpeg_info_to_file(_job, file_buf->aligned_buf, file_buf->aligned_len);
    if (ret != HPM_JPEG_RET_OK)
        goto ERROR2;

    if (!_job->has_valid_qhtab) {
        ret = hpm_jpeg_create_qtab_for_core(&_job->core_qtab, 1, jpeg_info->dqt);
        if (ret != HPM_JPEG_RET_OK)
            goto ERROR2;

        ret = hpm_jpeg_create_htab_for_core(&_job->core_htab, jpeg_info->dht_ac[0],
                                    jpeg_info->dht_ac[1], jpeg_info->dht_dc[0],
                                    jpeg_info->dht_dc[1]);
        if (ret != HPM_JPEG_RET_OK)
            goto ERROR2;

        _job->has_valid_qhtab = true;
    }

    JPEG_INFO_DUMP();
    (void)image_buf;

    lock = hpm_jpeg_lock();
    if (!ctx->work_job) {
        ctx->work_job = &base_job->list;
        jpeg_init(ctx->jpeg_base);
        hpm_jpeg_encode_init_tabs(_job);
        ret = hpm_jpeg_encode_config_and_start(_job);
        if (ret != HPM_JPEG_RET_OK) {
            hpm_jpeg_unlock(lock);
            goto ERROR2;
        }
        base_job->status = HPM_JPEG_JOB_STATUS_PROCESSING;
    } else {
        jpeg_list_add_tail(&ctx->wait_jobs, &base_job->list);
        base_job->status = HPM_JPEG_JOB_STATUS_WAITING;
    }
    hpm_jpeg_unlock(lock);

    return ret;

ERROR2:
    hpm_jpeg_encode_file_buf_deinit(_job);
ERROR1:
    lock = hpm_jpeg_lock();
    base_job->status = HPM_JPEG_JOB_STATUS_START_FAILED;
    hpm_jpeg_unlock(lock);
    return HPM_JPEG_RET_OK;
}

int hpm_jpeg_encode_job_stop(hpm_jpeg_job_t *job)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_context_t *ctx = base_job->ctx;
    uint32_t lock;

    lock = hpm_jpeg_lock();
    if (ctx->work_job == &base_job->list) {
        hpm_jpeg_start_next_job(ctx);
    } else {
        jpeg_list_del(&base_job->list);
    }
    base_job->status = HPM_JPEG_JOB_STATUS_STOPPED;
    hpm_jpeg_unlock(lock);

    return HPM_JPEG_RET_OK;
}

int hpm_jpeg_encode_job_get_info(hpm_jpeg_job_t *job, hpm_jpeg_encode_info_t *info)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;
    hpm_jpeg_encode_job_t *_job = (hpm_jpeg_encode_job_t *)job;

    info->hw_status = base_job->hw_status;
    info->status = base_job->status;
    info->file = &_job->jpeg_file;

    return HPM_JPEG_RET_OK;
}

int hpm_jpeg_encode_job_free(hpm_jpeg_job_t *job)
{
    hpm_jpeg_encode_job_t *_job = (hpm_jpeg_encode_job_t *)job;
    hpm_jpeg_buf_t *file_buf = &_job->file_buf;
    hpm_jpeg_buf_t *image_buf = &_job->image_buf;

    hpm_jpeg_encode_job_stop(job);

    if (image_buf->alloced_len)
        hpm_jpeg_free(image_buf->alloced_buf);

    if (file_buf->alloced_len)
        hpm_jpeg_free(file_buf->alloced_buf);

    hpm_jpeg_free(job);

    return HPM_JPEG_RET_OK;
}

void hpm_jpeg_job_set_user_data(hpm_jpeg_job_t *job, void *user_data)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;

    base_job->user_data = user_data;
}

void *hpm_jpeg_job_get_user_data(hpm_jpeg_job_t *job)
{
    hpm_jpeg_base_job_t *base_job = (hpm_jpeg_base_job_t *)job;

    return base_job->user_data;
}

int hpm_jpeg_init(hpm_jpeg_cfg_t *cfg)
{
    hpm_jpeg_context_t *ctx = &hpm_jpeg_context;

    if (!cfg) {
        HPM_JPEG_ELOG("cfg is NULL!!!\n");
        return HPM_JPEG_RET_PARA_ERR;
    }

    memset(ctx, 0x00, sizeof(hpm_jpeg_context_t));
    ctx->jpeg_base = cfg->jpeg_base;
    ctx->malloc = cfg->malloc;
    ctx->free = cfg->free;
    ctx->cache_line_size = cfg->cache_line_size;
    ctx->cache_invalidate = cfg->cache_invalidate;
    ctx->cache_writeback = cfg->cache_writeback;

    jpeg_list_node_init(&ctx->wait_jobs);
    jpeg_init(ctx->jpeg_base);

    return HPM_JPEG_RET_OK;
}

int hpm_jpeg_deinit(void)
{
    hpm_jpeg_context_t *ctx = &hpm_jpeg_context;
    hpm_jpeg_buf_t *file_buf;
    hpm_jpeg_buf_t *image_buf;
    hpm_jpeg_base_job_t *base_job;
    hpm_jpeg_decode_job_t *djob;
    hpm_jpeg_encode_job_t *ejob;
    jpeg_list_t *next_job;
    uint32_t lock;

    lock = hpm_jpeg_lock();
    jpeg_reset(ctx->jpeg_base);

    do {
        if (ctx->work_job) {
            next_job = ctx->work_job;
            ctx->work_job = NULL;
        } else {
            next_job = jpeg_list_node_get(&ctx->wait_jobs);
        }

        djob = (hpm_jpeg_decode_job_t *)next_job;
        ejob = (hpm_jpeg_encode_job_t *)next_job;
        base_job = (hpm_jpeg_base_job_t *)next_job;
        file_buf = NULL;
        image_buf = NULL;

        if (next_job && base_job->codec_type == HPM_JPEG_CODEC_DECODE) {
            file_buf = &djob->file_buf;
            image_buf = &djob->image_buf;
        } else if (next_job && base_job->codec_type == HPM_JPEG_CODEC_ENCODE) {
            file_buf = &ejob->file_buf;
            image_buf = &ejob->image_buf;
        }

        if (image_buf && image_buf->alloced_len)
            hpm_jpeg_free(image_buf->alloced_buf);

        if (file_buf && file_buf->alloced_len)
            hpm_jpeg_free(file_buf->alloced_buf);

        if (next_job)
            hpm_jpeg_free(next_job);
    } while (next_job);

    memset(ctx, 0x00, sizeof(hpm_jpeg_context_t));
    hpm_jpeg_unlock(lock);

    return HPM_JPEG_RET_OK;
}

void hpm_jpeg_isr(void)
{
    hpm_jpeg_context_t *ctx = &hpm_jpeg_context;
    hpm_jpeg_encode_job_t *ejob;
    hpm_jpeg_base_job_t *base_job;
    uint32_t status;
    uint32_t lock;

    ejob = (hpm_jpeg_encode_job_t *)ctx->work_job;
    base_job = (hpm_jpeg_base_job_t *)ctx->work_job;

    status = jpeg_get_status(ctx->jpeg_base);
    jpeg_clear_status(ctx->jpeg_base, status);
    HPM_JPEG_DLOG("hw_status: 0x%08x\n", status);

    lock = hpm_jpeg_lock();
    base_job->hw_status = status;
    if (status & JPEG_STAT_OUT_DMA_TRANSFER_DONE_MASK) {
        HPM_JPEG_DLOG("status: FINISHED\n");
        base_job->status = HPM_JPEG_JOB_STATUS_FINISHED;
        if (base_job->codec_type == HPM_JPEG_CODEC_ENCODE) {
            ejob->jpeg_file.len += jpeg_get_encoded_length(ctx->jpeg_base);
            ejob->jpeg_info.eoi = ejob->jpeg_info.soi + ejob->jpeg_file.len - 2;
            HPM_JPEG_DLOG("jpeg_file.len: %u\n", ejob->jpeg_file.len);
        }
    } else {
        base_job->status = HPM_JPEG_JOB_STATUS_ERROR;
    }
    hpm_jpeg_unlock(lock);

    if (base_job->cb)
        base_job->cb((hpm_jpeg_job_t *)base_job);

    hpm_jpeg_start_next_job(ctx);
}