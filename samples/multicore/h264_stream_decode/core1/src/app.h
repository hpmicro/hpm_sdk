#ifndef _APP_H_
#define _APP_H_

typedef struct _app_config {
    uint8_t* h264_data;
    int32_t h264_data_len;
    int32_t flag_data_output;
    int32_t flag_decode;
    int32_t output_addr;
    int got_picture;
} app_config_t;

extern volatile app_config_t app_config;
#endif