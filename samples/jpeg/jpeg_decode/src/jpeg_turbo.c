/*
 * Copyright (c) 2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------
 */
#include "board.h"
#include "jpeglib.h"
#include "jpeg_turbo.h"
#include "file_op.h"
/*---------------------------------------------------------------------*
 * libjpeg-turbo  -Convert JPG data to bgr888 data
 *---------------------------------------------------------------------
 */
void jpeg_jpgmem_to_bgr888(const uint8_t *src, int32_t srcLen, uint8_t **_dst, int32_t *dstLen, int32_t *width, int32_t *height, int32_t *depth)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    uint64_t image_size;

    /* Initialize the JPEG decompression object with default error handling. */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    /* Specify data source for decompression */
    jpeg_mem_src(&cinfo, src, srcLen);
    /* Read file header, set default decompression parameters */
    jpeg_read_header(&cinfo, TRUE);

    /*Image conversion compression*/
    image_size = cinfo.image_width*cinfo.image_height*2;
    /*No compression*/
    if (image_size < RGBBUFFLEN) {
        cinfo.scale_num = 1;
        cinfo.scale_denom = 1;
    }
    /*Compression ratio: 1:2*/
    else if (image_size < (RGBBUFFLEN * 4)) {
        cinfo.scale_num = 1;
        cinfo.scale_denom = 2;
    }
    /*Compression ratio: 1:4*/
    else if (image_size < (RGBBUFFLEN * 16)) {
        cinfo.scale_num = 1;
        cinfo.scale_denom = 4;
    }
    else {
        printf("Pixels too large, Pictures can support 1024*768 at most\n");
        return;
    }

    /* Start decompressor */
    printf("start decoding\n");
    jpeg_start_decompress(&cinfo);
    (*width) = cinfo.output_width;
    (*height) = cinfo.output_height;
    (*depth) = cinfo.num_components;
    (*dstLen) = (*width) * (*height) * (*depth);
    uint8_t *dst = (uint8_t *)malloc(*dstLen);

    /* Process data */
    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = &dst[cinfo.output_scanline * cinfo.output_width * cinfo.num_components];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }
    /* Finish decompression*/
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    printf("decoding done\n");
    *_dst = dst;
}

/*---------------------------------------------------------------------*
 * Convert rgb888 data to rgb565 data
 *---------------------------------------------------------------------
 */
uint16_t jpeg_rgb888_to_rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
    /*RGB24 bit data is combined into 16 bit data*/
    uint16_t B = (blue >> 3) & 0x001F;
    uint16_t G = ((green >> 2) << 5) & 0x07E0;
    uint16_t R = ((red >> 3) << 11) & 0xF800;

    return (uint16_t)(R | G | B);
}

/*---------------------------------------------------------------------*
 * Convert bgr888 data to rgb565 data
 *---------------------------------------------------------------------
 */
void jpeg_bgr888_to_rgb565(uint8_t *bgr, int32_t *bsize, int32_t bit)
{
    int32_t fsize = (*bsize);
    int32_t fnsi = 0;

    /*Blue data bytes and red data bytes are exchanged*/
    for (int32_t i = 0; i < fsize; i += bit / 8) {
        uint8_t b = bgr[i];
        bgr[i] = bgr[i + 2];
        bgr[i + 2] = b;
    }

    /*Convert rgb888 data to rgb565 data */
    for (uint64_t iz = 0, j = 0; iz < fsize; iz += 3, j += 2) {
        uint16_t color565 = jpeg_rgb888_to_rgb565(bgr[iz], bgr[iz + 1], bgr[iz + 2]);
        memcpy(bgr + j, &color565, 2);
        fnsi += 2;
    }
    (*bsize) = fnsi;
}

/*---------------------------------------------------------------------*
 * Libjpeg conversion to convert JPG data into rgb565 data
 *---------------------------------------------------------------------
 */
void jpeg_convert_data(uint8_t *filebuffs, int32_t fileLen, int32_t *width, int32_t *height, uint8_t *rgbbuff)
{
    int32_t component;
    uint8_t *djpegbuff = NULL;
    int32_t size;

    /* jpeg  -Convert JPG data to bgr888 data */
    jpeg_jpgmem_to_bgr888(filebuffs, fileLen, &djpegbuff, &size, width, height, &component);
    /*bgr888 data conversion to rgb565*/
    jpeg_bgr888_to_rgb565(djpegbuff, &size, 24);
    /*Store data to buff*/
    memcpy(rgbbuff, &djpegbuff[0], size);
    /*Free memory*/
    free(djpegbuff);
    /*Complete JPG decoding*/
}