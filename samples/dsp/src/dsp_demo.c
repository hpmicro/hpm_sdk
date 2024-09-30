/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hpm_math.h"
#include "hpm_common.h"
#include "board.h"

#define TRANSFORM_SAMPLES_LOG 5

#ifdef __zcc__
#define NRSD_FFT    (0.0002f)
#define NRSD_FFT_INV (0.005f)
#else
#define NRSD_FFT    (0.00007f)
#define NRSD_FFT_INV (0.0003f)
#endif

static const unsigned char golden_cifft_rd2_f32[] = {
    0xa0, 0xc2, 0x6e, 0xb2, 0xa0, 0xc2, 0x6e, 0xb2,
    0xbd, 0xc5, 0x47, 0x3e, 0xc4, 0xc5, 0x47, 0x3e,
    0x13, 0xef, 0xc3, 0x3e, 0x14, 0xef, 0xc3, 0x3e,
    0xda, 0x39, 0x0e, 0x3f, 0xd9, 0x39, 0x0e, 0x3f,
    0xf2, 0x04, 0x35, 0x3f, 0xf2, 0x04, 0x35, 0x3f,
    0x30, 0xdb, 0x54, 0x3f, 0x30, 0xdb, 0x54, 0x3f,
    0x5e, 0x83, 0x6c, 0x3f, 0x5e, 0x83, 0x6c, 0x3f,
    0xbd, 0x14, 0x7b, 0x3f, 0xbd, 0x14, 0x7b, 0x3f,
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f,
    0xbe, 0x14, 0x7b, 0x3f, 0xbc, 0x14, 0x7b, 0x3f,
    0x5c, 0x83, 0x6c, 0x3f, 0x5c, 0x83, 0x6c, 0x3f,
    0x30, 0xdb, 0x54, 0x3f, 0x30, 0xdb, 0x54, 0x3f,
    0xf3, 0x04, 0x35, 0x3f, 0xf3, 0x04, 0x35, 0x3f,
    0xd9, 0x39, 0x0e, 0x3f, 0xd9, 0x39, 0x0e, 0x3f,
    0x10, 0xef, 0xc3, 0x3e, 0x10, 0xef, 0xc3, 0x3e,
    0xbe, 0xc5, 0x47, 0x3e, 0xc0, 0xc5, 0x47, 0x3e,
    0x54, 0xd8, 0x9d, 0xb3, 0x54, 0xd8, 0x9d, 0xb3,
    0xc7, 0xc5, 0x47, 0xbe, 0xd0, 0xc5, 0x47, 0xbe,
    0x13, 0xef, 0xc3, 0xbe, 0x14, 0xef, 0xc3, 0xbe,
    0xda, 0x39, 0x0e, 0xbf, 0xd9, 0x39, 0x0e, 0xbf,
    0xf4, 0x04, 0x35, 0xbf, 0xf4, 0x04, 0x35, 0xbf,
    0x34, 0xdb, 0x54, 0xbf, 0x34, 0xdb, 0x54, 0xbf,
    0x60, 0x83, 0x6c, 0xbf, 0x60, 0x83, 0x6c, 0xbf,
    0xbb, 0x14, 0x7b, 0xbf, 0xbd, 0x14, 0x7b, 0xbf,
    0x00, 0x00, 0x80, 0xbf, 0x00, 0x00, 0x80, 0xbf,
    0xbe, 0x14, 0x7b, 0xbf, 0xbc, 0x14, 0x7b, 0xbf,
    0x5c, 0x83, 0x6c, 0xbf, 0x5c, 0x83, 0x6c, 0xbf,
    0x2e, 0xdb, 0x54, 0xbf, 0x30, 0xdb, 0x54, 0xbf,
    0xef, 0x04, 0x35, 0xbf, 0xef, 0x04, 0x35, 0xbf,
    0xdb, 0x39, 0x0e, 0xbf, 0xdb, 0x39, 0x0e, 0xbf,
    0x14, 0xef, 0xc3, 0xbe, 0x14, 0xef, 0xc3, 0xbe,
    0xba, 0xc5, 0x47, 0xbe, 0xbc, 0xc5, 0x47, 0xbe};

static const unsigned char golden_cfft_rd2_f32[] = {
    0x2a, 0xec, 0xee, 0xb4, 0x2a, 0xec, 0xee, 0xb4,
    0x00, 0x00, 0x80, 0x41, 0xfe, 0xff, 0x7f, 0xc1,
    0xbe, 0xe1, 0x67, 0xb5, 0x50, 0xb9, 0x81, 0x34,
    0x00, 0x00, 0x00, 0xb5, 0x00, 0x00, 0x00, 0xb4,
    0x9e, 0xe6, 0x86, 0x34, 0x62, 0x19, 0xd9, 0xb4,
    0x00, 0x00, 0x00, 0x34, 0xf3, 0x04, 0xb5, 0x34,
    0x2a, 0x66, 0x25, 0x34, 0xdc, 0x86, 0x5d, 0xb4,
    0x00, 0x00, 0x40, 0x35, 0x00, 0x00, 0x00, 0x35,
    0x2a, 0xec, 0x9e, 0xb4, 0xeb, 0x89, 0x70, 0x35,
    0x00, 0x00, 0x80, 0xb4, 0x00, 0x00, 0x00, 0x00,
    0x6d, 0x92, 0xd4, 0xb4, 0xd2, 0xb5, 0xbc, 0x34,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xb5,
    0x79, 0x5f, 0x52, 0xb5, 0xe5, 0x7d, 0x09, 0xb4,
    0xe6, 0x09, 0x6a, 0x34, 0x00, 0x00, 0x00, 0xb5,
    0x7d, 0xb4, 0xcc, 0x33, 0x48, 0x5d, 0xa2, 0xb3,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xac, 0x27, 0x62, 0x34, 0xac, 0x27, 0x62, 0x34,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x44, 0x5d, 0xa2, 0xb3, 0x7d, 0xb4, 0xcc, 0x33,
    0x00, 0x00, 0x40, 0xb5, 0x00, 0x00, 0x20, 0x35,
    0xe5, 0x7d, 0x09, 0xb4, 0x79, 0x5f, 0x52, 0xb5,
    0x00, 0x00, 0x00, 0x34, 0xf3, 0x04, 0xb5, 0xb4,
    0xd3, 0xb5, 0xbc, 0x34, 0x6c, 0x92, 0xd4, 0xb4,
    0x00, 0x00, 0x40, 0xb5, 0x00, 0x00, 0x00, 0xb5,
    0xeb, 0x89, 0x70, 0x35, 0x2a, 0xec, 0x9e, 0xb4,
    0x00, 0x00, 0x80, 0x34, 0x00, 0x00, 0x00, 0x00,
    0xda, 0x86, 0x5d, 0xb4, 0x2b, 0x66, 0x25, 0x34,
    0x00, 0x00, 0x80, 0x34, 0x00, 0x00, 0xc0, 0x34,
    0x62, 0x19, 0xd9, 0xb4, 0x9e, 0xe6, 0x86, 0x34,
    0xf3, 0x04, 0xf5, 0xb4, 0x00, 0x00, 0x00, 0xb5,
    0x50, 0xb9, 0x81, 0x34, 0xbe, 0xe1, 0x67, 0xb5,
    0xfe, 0xff, 0x7f, 0xc1, 0x00, 0x00, 0x80, 0x41};

//Default: LOG is disable.
//If you want to enable LOG, please use next line.
//#define PRINT_DETAIL
#ifdef PRINT_DETAIL
#define LOG printf
#else
#define LOG(...)
#endif

#ifdef TRANSFORM_SAMPLES_LOG
#define TRANSFORM_SAMPLES (1 << TRANSFORM_SAMPLES_LOG)
#endif

// floating-point to Q(destination type)
static inline q15_t convert_float_to_q15(float x)
{
    q31_t q31;
    q15_t q15;
    x *= 32768.0f;
    x += (x < 0.0f ? -0.5f : 0.5f);
    q31 = (q31_t)x;
    if (q31 >= 32768)
    {
        q15 = INT16_MAX;
    }
    else if (q31 <= -32768)
    {
        q15 = INT16_MIN;
    }
    else
    {
        q15 = (q15_t)q31;
    }
    return q15;
}

static inline void dump_q15(const char *hdr, q15_t *buf, uint32_t size)
{
    unsigned int i;
    (void)hdr;

    LOG("%s:\n", hdr);
    for (i = 0; i < size; i++, buf++)
    {
        LOG("%4d: 0x%04x [%2.8f]\n", i, (*buf) & 0xffff, (*buf) / 32768.0f);
    }
    LOG("------------------\n");
}

#define ERROR_EQ_ZERO (-1.0f)

static inline float32_t dump_rmsd2(float32_t *goldensig, float32_t *testsig, uint32_t size, float32_t result[])
{
    uint32_t i;
    double Sumsquareerr = 0.0f;
    float32_t Sumerr = 0.0f;
    float32_t RMSD;
    float32_t MAE;
    float32_t NRMSD;
    float32_t MAXDIFF = 0.0f;
    float32_t diff;
    float32_t SNR;
    double squaregloden = 0.0f;
    float32_t maxsig = 0.0f, minsig = 0.0f;
    int temp;
    void *temp_ptr;

    for (i = 0; i < size; i++)
    {
        float32_t testin = *testsig++, golden = *goldensig++;

        temp_ptr = (void *)(&testin);
        temp = *(int *)(temp_ptr);

        if (temp == 0x7FC00000)
        {
            return (-1.0f);
        }
        maxsig = (golden > maxsig) ? golden : maxsig;
        minsig = (golden < minsig) ? golden : minsig;
        diff = fabsf(testin - golden);
        Sumerr += diff;

        MAXDIFF = (diff > MAXDIFF) ? diff : MAXDIFF;
        Sumsquareerr += diff * diff;
        squaregloden += golden * golden;
    }

    temp_ptr = (void *)(&Sumsquareerr);
    temp = *(int *)(temp_ptr);

    if (temp == 0x7FC00000)
    {
        return (-1.0f);
    }

    MAE = Sumerr / size;
    RMSD = sqrtf(Sumsquareerr / size);
    NRMSD = RMSD / (maxsig - minsig);
    if (Sumsquareerr == 0.0f)
    {
        SNR = ERROR_EQ_ZERO;
    }
    else
    {
        SNR = 10.0 * log10f(squaregloden / Sumsquareerr);
    }

    if (result != NULL)
    {
        result[0] = MAE;
        result[1] = RMSD;
        result[2] = MAXDIFF;
        result[3] = SNR;
    }
    else
    {
        printf("--------------------\n");
        printf("MAE is %5.8f, RMSD is %5.8f, NRMSD is %5.8f, MAXDIFF is %5.8f, SNR is %5.8f\n", MAE, RMSD, NRMSD, MAXDIFF, SNR);
    }

    return NRMSD;
}

#define dump_rmsd(go, te, s) dump_rmsd2(go, te, s, NULL);

// dump transform function diff with golden results.
// type: 1 for dct, cfft_rd2, cfft_rd4;  0 for dct4, rfft.
static inline float32_t dump2_goldendiff_q15(const char *hdr, float32_t *buf, q15_t *buf2, uint32_t size, uint8_t type)
{
    unsigned int i;
    float32_t maxdiff = 0.0;
    q15_t maxdiff_q15 = 0;
    float32_t testoutf32[size];
    float32_t golden_f32[size];

    LOG("----- output [scaleingout] | -- [golden] - | ---- abs diff ----\n");
    for (i = 0; i < size; i++)
    {
        golden_f32[i] = *buf++;
        q15_t testout = *buf2++;
        if (type == 0)
        {
            testoutf32[i] = testout / 32768.0 * (4 * size);
        }
        else
        {
            testoutf32[i] = testout / 32768.0 * (2 * size);
        }
        float32_t diff = golden_f32[i] - testoutf32[i];
        diff = fabsf(diff);

        LOG("%4d: 0x%04x [%11.8f] | [%11.8f] | 0x%04x [%2.8f]\n", i, testout & 0xffff, testoutf32[i], golden_f32[i], (q15_t)(diff * 32768.0) & 0xffff, diff);
        maxdiff = diff > maxdiff ? diff : maxdiff;
        maxdiff_q15 = maxdiff * 32768.0;
    }
    printf("%s, maxdiff= 0x%04x [%2.8f]\n", hdr, maxdiff_q15, maxdiff);
    float32_t NRMSD;
    NRMSD = dump_rmsd(golden_f32, testoutf32, size);

    return NRMSD;
}

static inline float32_t dump2_inv_goldendiff_q15(const char *hdr, float32_t *buf, q15_t *buf2, uint32_t size)
{
    unsigned int i;
    float32_t maxdiff = 0.0;
    q15_t maxdiff_q15 = 0;
    float32_t testoutf32[size];
    float32_t golden_f32[size];

    LOG("----- output [scaleingout] | -- [golden] - | ---- abs diff ----\n");
    for (i = 0; i < size; i++)
    {
        golden_f32[i] = *buf++;
        q15_t testout = *buf2++;
        testoutf32[i] = testout / 32768.0 * 2;
        float32_t diff = golden_f32[i] - testoutf32[i];
        diff = fabsf(diff);

        LOG("%4d: 0x%04x [%11.8f] | [%11.8f] | 0x%04x [%2.8f]\n", i, testout & 0xffff, testoutf32[i], golden_f32[i], (q15_t)(diff * 32768.0) & 0xffff, diff);
        maxdiff = diff > maxdiff ? diff : maxdiff;
        maxdiff_q15 = maxdiff * 32768.0;
    }
    printf("%s, maxdiff= 0x%04x [%2.8f]\n", hdr, maxdiff_q15, maxdiff);

    float32_t NRMSD;
    NRMSD = dump_rmsd(golden_f32, testoutf32, size);

    return NRMSD;
}

#ifdef TRANSFORM_SAMPLES_LOG

#if TRANSFORM_SAMPLES == 32
const float32_t test_nds_Input_32[32] =
    {
        0.0000000000,
        0.1950903237,
        0.3826834559,
        0.5555702448,
        0.7071068287,
        0.8314695954,
        0.9238795638,
        0.9807853103,
        1.0000000000,
        0.9807852507,
        0.9238795042,
        0.8314695358,
        0.7071067691,
        0.5555701852,
        0.3826832771,
        0.1950903088,
        -0.0000000874,
        -0.1950904876,
        -0.3826834559,
        -0.5555703044,
        -0.7071068883,
        -0.8314697742,
        -0.9238796830,
        -0.9807852507,
        -1.0000000000,
        -0.9807852507,
        -0.9238794446,
        -0.8314694762,
        -0.7071065307,
        -0.5555703044,
        -0.3826834261,
        -0.1950902343,
};
#define test_nds_Input test_nds_Input_32

#endif

#endif

int main(void)
{

    board_init();
    q15_t buf[2 * TRANSFORM_SAMPLES] __attribute__((aligned(4)));
    q15_t buf2[2 * TRANSFORM_SAMPLES];
    int i;
    float32_t NRMSD;

    float32_t *golden_buf = (float32_t *)golden_cfft_rd2_f32;
    float32_t *golden_ibuf = (float32_t *)golden_cifft_rd2_f32;

    printf("\nAndes V5 demo-dsp program\n");

    for (i = 0; i < TRANSFORM_SAMPLES; i++)
    {
        buf[2 * i] = buf2[2 * i] = (convert_float_to_q15(test_nds_Input[i])) >> 1;
        buf[2 * i + 1] = buf2[2 * i + 1] = (convert_float_to_q15(test_nds_Input[i])) >> 1;
    }
    dump_q15("randomly generated input array", buf, 2 * TRANSFORM_SAMPLES);

    hpm_dsp_cfft_rd2_q15(buf, TRANSFORM_SAMPLES_LOG);
#ifdef __zcc__
    for (i = 0; i < TRANSFORM_SAMPLES * 2; i++) {
        buf[i] = buf[i] >> 1;
    }
#endif
    dump_q15("after cFFT", buf, 2 * TRANSFORM_SAMPLES);

    /* check dct with golden */
    printf("--------------------\n");
    NRMSD = dump2_goldendiff_q15("after CFFT_RD2", golden_buf, buf, 2 * TRANSFORM_SAMPLES, 1);
    printf("CFFT_RD2 out scaleup by %d\n", (2 << TRANSFORM_SAMPLES_LOG));
    LOG("--------------\n");

    if (NRMSD > NRSD_FFT)
    {
        printf("NRMSD = %2.8f > %f, CFFT_RD2 FAIL\n", NRMSD, NRSD_FFT);
        return -1;
    }

    /* CIFFT function */
    for (i = 0; i < 2 * TRANSFORM_SAMPLES; i++)
    {
        buf[i] = (convert_float_to_q15(golden_buf[i] / (4 << TRANSFORM_SAMPLES_LOG)));
#ifdef __zcc__
        buf[i] = buf[i] << 1;
#endif
    }
    dump_q15("randomly generated input array", buf, TRANSFORM_SAMPLES);

    hpm_dsp_cifft_rd2_q15(buf, TRANSFORM_SAMPLES_LOG);
    dump_q15("after CIFFT", buf, 2 * TRANSFORM_SAMPLES);

    /* check idct with golden */
    NRMSD = dump2_inv_goldendiff_q15("after CIFFT_RD2", golden_ibuf, buf, 2 * TRANSFORM_SAMPLES);
    printf("CIFFT_RD2 out scaleup by %d\n", 2);
    LOG("--------------\n");
    if (TRANSFORM_SAMPLES_LOG <= 10)
    {
        if (NRMSD < NRSD_FFT_INV)
        {
            printf("CIFFT_RD2 PASS\n");
        }
        else
        {
            printf("NRMSD = %2.8f > %f, CIFFT_RD2 FAIL\n", NRMSD, NRSD_FFT_INV);
            return -1;
        }
    }
    else
    {
        printf("CIFFT_RD2 PASS\n");
    }
}
