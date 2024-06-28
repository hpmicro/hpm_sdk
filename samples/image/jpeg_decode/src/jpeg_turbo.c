/*
 * Copyright (c) 2022 HPMicro
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

void jpeg_convert_to_rgb565(uint8_t *in, uint32_t len, uint16_t *out)
{
    for (uint32_t i = 0, j = 0; i < len; i += 3, j++) {
        out[j] = (((uint16_t)(in[i + 2] >> 3) << 11) & 0xF800) /* R */
              | (((uint16_t)(in[i + 1] >> 2) << 5) & 0x07E0)  /* G */
              | (uint16_t)((in[i] >> 3) & 0x1F); /* B */
    }
}

/*---------------------------------------------------------------------*
 * Libjpeg conversion to convert JPG data into rgb565 data
 *---------------------------------------------------------------------
 */
bool jpeg_sw_decode(uint8_t *file_buf, uint32_t file_len, uint32_t *width, uint32_t *height, uint8_t *components, uint16_t *out_buf)
{
    uint8_t *tmp_buf;
    uint32_t size;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* Initialize the JPEG decompression object with default error handling. */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    /* Specify data source for decompression */
    jpeg_mem_src(&cinfo, file_buf, file_len);
    /* Read file header, set default decompression parameters */
    jpeg_read_header(&cinfo, true);

    /* Scale the image by the fraction scale_num/scale_denom.  Default is
     * 1/1, or no scaling.Currently, the only supported scaling ratios
     * are M/8 with all M from 1 to 16, or any reduced fraction thereof (such
     * as 1/2, 3/4, etc.)
     */
    if ((cinfo.image_width <= BOARD_LCD_WIDTH) && (cinfo.image_height <= BOARD_LCD_HEIGHT)) {
        cinfo.scale_num = 1;
        cinfo.scale_denom = 1;
    } else {
        cinfo.scale_num = 1;
        cinfo.scale_denom = 2;
    }

    jpeg_start_decompress(&cinfo);
    (*width) = cinfo.output_width;
    (*height) = cinfo.output_height;
    (*components) = cinfo.num_components;
    size = cinfo.output_width * cinfo.output_height * cinfo.num_components;
    if (cinfo.num_components == 1) {
        tmp_buf = (uint8_t *)out_buf;
    } else {
        tmp_buf = (uint8_t *)malloc(size);
        if (!tmp_buf) {
            printf("out of memory\n");
            jpeg_abort_decompress(&cinfo);
            jpeg_destroy_decompress(&cinfo);
            return false;
        }
    }
    /* Process data */
    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = &tmp_buf[cinfo.output_scanline * cinfo.output_width * cinfo.num_components];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }
    /* Finish decompression*/
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    if (*components == 3) {
        jpeg_convert_to_rgb565(tmp_buf, size, out_buf);
        free(tmp_buf);
    }
    return true;
}
