/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "image_provider.h"
#include "model_settings.h"
#include "hpm_camera.h"
#include "board.h"
#include "hpm_pdma_drv.h"
#include "hpm_l1c_drv.h"

extern uint16_t buffer[2][800*480];
extern uint16_t dispbuffer[];
extern uint32_t tf_run_inference;
extern"C"{
void start_time(void);
}

extern uint16_t px[], py[];
extern uint8_t pnum;
uint16_t color_buf[] = {63488, 65184, 26592, 14137, 2463, 64800, 41285};
#pragma GCC push_options
#pragma GCC optimize ("O0")
void draw_square(uint16_t *buf, uint16_t xm, uint16_t ym,uint16_t length,
                int16_t pix_size, uint16_t x_max, uint16_t color)
{
    int16_t x0, y0;
    int16_t m, n;
    int16_t x_end, y_end;
    int16_t x_start, y_start;

    x0 = (int16_t)xm - (length >> 1);
    y0 = (int16_t)ym - (length >> 1);
    x_start = x0;
    y_start = y0 - pix_size + 1;
    if (y_start <= 0) {
        y_start = pix_size - 1;
    }
    if (x_start <= 0) {
        x_start = 0;
    }
    x_end = x0 + length;
    y_end = y_start + pix_size;

    for (m = x_start; m < x_end; m++) {
        for (n = y_start; n < y_end; n++) {
           buf[(n * x_max) + m]  = color;
           buf[((n + length)* x_max) + m]  = color;
        }
    }
    x_start = x0 - pix_size + 1;
    y_start = y0;
    if (x_start <= 0) {
        x_start = pix_size - 1;
    }
    if (y_start <= 0) {
        y_start = 0;
    }
    y_end = y0 + length;
    x_end = x_start + pix_size;

    for (m = x_start; m < x_end; m++) {
       for (n = y_start; n < y_end; n++) {
          buf[(n * x_max) + m]  = color;
          buf[(n * x_max) + m + length]  = color;
       }
    }
}
#pragma GCC pop_options

void face_obj_display(uint16_t *buf, int image_width, int image_height)
{
    uint16_t face_num;
    (void) image_width;
    (void) image_height;
    for (face_num = 0; face_num < pnum; face_num ++) {
        px[face_num] = 200 + 4*px[face_num];
        py[face_num] = 3*py[face_num];
        draw_square(buf, px[face_num], py[face_num], 60, 4, 800, color_buf[face_num]);
    }
}
TfLiteStatus GetImage(tflite::ErrorReporter* error_reporter, int image_width,
                      int image_height, int channels, int8_t* image_data)
{
    int x, y;
    uint32_t i = 0;
    uint32_t data_pos;
    (void)error_reporter;
    (void)channels;

    for (y = 0; y < 480; y++) {
    for (x = 0; x < 800; x++) {
        dispbuffer[i] = buffer[0][i];
        i++;
    }
    }
    i = 0;
    for( y = 0; y < image_height; y++){
        for( x = 0; x < image_width; x++){
            data_pos = ((y) * 3 + 150) * 800 + 200 + 4 * x;
            dispbuffer[(y + 480) * 800 + x + 520] = buffer[0][data_pos];
            image_data[i++] = ((((buffer[0][data_pos]&0xf800)>>11)*527+23) >> 6)-128;
            image_data[i++] = ((((buffer[0][data_pos]&0x7e0)>>5)*259+23) >>6)-128;
            image_data[i++] = (((buffer[0][data_pos]&0x1f)*527+23) >>6)-128;
        }
    }
    face_obj_display(&dispbuffer[150*800], 800, 480);

  return kTfLiteOk;
}