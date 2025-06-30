/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_common.h"

#define UPALIGN4(n) (((n) + 3) & ~3)

void convert_rgb565_to_rgb888(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst)
{
    uint32_t srclinesize = UPALIGN4(width * 2);
    uint32_t dstlinesize = UPALIGN4(width * 3);

    uint8_t *psrcline;
    const uint16_t *psrcdot;
    uint8_t *pdstline;
    uint8_t *pdstdot;

    psrcline = psrc;
    pdstline = pdst;
    for (uint32_t i = 0; i < height; i++) {
        psrcdot = (const uint16_t *)psrcline;
        pdstdot = pdstline;
        for (uint32_t j = 0; j < width; j++) {
            *(pdstdot) = (uint8_t)(((*psrcdot & 0x1F) >> 0) << 3);        /* Blue */
            *(pdstdot + 1) = (uint8_t)(((*psrcdot & 0x7E0) >> 5) << 2);   /* Green */
            *(pdstdot + 2) = (uint8_t)(((*psrcdot & 0xF800) >> 11) << 3); /* Red */
            psrcdot++;
            pdstdot += 3;
        }
        psrcline += srclinesize;
        pdstline += dstlinesize;
    }
}

void convert_rgb565_to_bgr888(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst)
{
    uint32_t srclinesize = UPALIGN4(width * 2);
    uint32_t dstlinesize = UPALIGN4(width * 3);

    uint8_t *psrcline;
    const uint16_t *psrcdot;
    uint8_t *pdstline;
    uint8_t *pdstdot;

    psrcline = psrc;
    pdstline = pdst;
    for (uint32_t i = 0; i < height; i++) {
        psrcdot = (const uint16_t *)psrcline;
        pdstdot = pdstline;
        for (uint32_t j = 0; j < width; j++) {
            *(pdstdot) = (uint8_t)(((*psrcdot & 0xF800) >> 11) << 3);   /* Red */
            *(pdstdot + 1) = (uint8_t)(((*psrcdot & 0x7E0) >> 5) << 2); /* Green */
            *(pdstdot + 2) = (uint8_t)(((*psrcdot & 0x1F) >> 0) << 3);  /* Blue */
            psrcdot++;
            pdstdot += 3;
        }
        psrcline += srclinesize;
        pdstline += dstlinesize;
    }
}

void convert_rgb888_to_rgb565(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst)
{
    uint32_t srclinesize = UPALIGN4(width * 3);
    uint32_t dstlinesize = UPALIGN4(width * 2);

    uint8_t *psrcline;
    uint8_t *psrcdot;
    uint8_t *pdstline;
    uint16_t *pdstdot;

    psrcline = psrc;
    pdstline = pdst;
    for (uint32_t i = 0; i < height; i++) {
        psrcdot = psrcline;
        pdstdot = (uint16_t *)pdstline;
        for (uint32_t j = 0; j < width; j++) {
            *(pdstdot) = ((uint16_t)(*(psrcdot) & 0xF8) >> 3)                /* Blue */
                         | (((uint16_t)(*(psrcdot + 1) & 0xFC) >> 2) << 5)   /* Green */
                         | (((uint16_t)(*(psrcdot + 2) & 0xF8) >> 3) << 11); /* Red */
            psrcdot += 3;
            pdstdot++;
        }
        psrcline += srclinesize;
        pdstline += dstlinesize;
    }
}

void convert_bgr888_to_rgb565(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst)
{
    uint32_t srclinesize = UPALIGN4(width * 3);
    uint32_t dstlinesize = UPALIGN4(width * 2);

    uint8_t *psrcline;
    uint8_t *psrcdot;
    uint8_t *pdstline;
    uint16_t *pdstdot;

    psrcline = psrc;
    pdstline = pdst;
    for (uint32_t i = 0; i < height; i++) {
        psrcdot = psrcline;
        pdstdot = (uint16_t *)pdstline;
        for (uint32_t j = 0; j < width; j++) {
            *(pdstdot) = (((uint16_t)(*(psrcdot) & 0xF8) >> 3) << 11)      /* Red */
                         | (((uint16_t)(*(psrcdot + 1) & 0xFC) >> 2) << 5) /* Green */
                         | (((uint16_t)(*(psrcdot + 2) & 0xF8) >> 3));     /* Blue */
            psrcdot += 3;
            pdstdot++;
        }
        psrcline += srclinesize;
        pdstline += dstlinesize;
    }
}

void convert_abgr8888_to_argb8888(uint8_t *psrc, uint32_t width, uint32_t height, uint8_t *pdst)
{
    for (uint32_t i = 0; i < (width * height); i++) {
        pdst[4 * i] = psrc[4 * i + 2];
        pdst[4 * i + 1] = psrc[4 * i + 1];
        pdst[4 * i + 2] = psrc[4 * i];
        pdst[4 * i + 3] = psrc[4 * i + 3];
    }
}
