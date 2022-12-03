
#include "h264_stream.h"
#include "avcodec.h"
#include "mp4.h"
#include "hpm_l1c_drv.h"
#include "app.h"
#define H264_STREAM_DEBUG     1

extern AVCodec ff_h264_decoder;
extern AVInputFormat ff_mov_demuxer;
extern URLProtocol ff_file_protocol;
extern AVCodecParser ff_h264_parser;

struct AVCodecContext *g_avcodec_ctx = NULL;
struct AVCodec *g_pavcodec;
struct AVPacket g_avpacket;
struct AVFrame *g_pavframe = NULL;

AVCodecParserContext *g_pavcodec_parse_context = NULL;

extern int erpc_loop(void);
unsigned int f_cnt = 0;

int h264_decode_init(void)
{
    av_log_set_level(AV_LOG_WARNING);
    h264_decode_release();
    avcodec_register(&ff_h264_decoder);

    g_pavcodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!g_pavcodec) {
        printf("Can not find codec:%d\n", AV_CODEC_ID_H264);
        return -2;
    }
    if (!g_avcodec_ctx) {
        g_avcodec_ctx = avcodec_alloc_context3(g_pavcodec);
        if (!g_avcodec_ctx) {
            printf("Failed to alloc codec context.");
            h264_decode_release();
            return -3;
        }
    }
    int tmp = avcodec_open2(g_avcodec_ctx, g_pavcodec, NULL);
    if (tmp < 0) {
        printf("Failed to open h264 decoder %d\n", tmp);
        h264_decode_release();
        return -4;
    }

    if (!g_pavframe) {
        av_init_packet(&g_avpacket);
        g_pavframe = av_frame_alloc();
        if (!g_pavframe) {
            perror("av_frame_alloc\n");
            return -5;
        }
    }

    av_register_codec_parser(&ff_h264_parser);
    g_pavcodec_parse_context = av_parser_init(AV_CODEC_ID_H264);
    if (!g_pavcodec_parse_context) {
        printf("error while av_parser_init(AV_CODEC_ID_H264)\n");
        while (1)
            ;
    }


    return 0;
}

int h264_decode_release(void)
{
    if (g_avcodec_ctx != NULL) {
        avcodec_close(g_avcodec_ctx);
        avcodec_free_context(&g_avcodec_ctx);
        g_avcodec_ctx = NULL;
    }

    if (g_pavframe != NULL) {
        av_packet_unref(&g_avpacket);
        av_free(g_pavframe);
        g_pavframe = NULL;
    }


    av_packet_unref(&g_avpacket);

    return 0;
}

/*!
 * @brief decode function loop
 */
void core1_decode_loop(void)
{
    int ret = 0;
    int first_time = 1;
    unsigned char *cur_ptr;
    app_config.got_picture = 0;
    for (int i = 0; i < 10; i++)
        printf("\n");

    while (1) {
        printf("wait for data\n");

        /* wait core0 transfer data to core1 */
        while (app_config.flag_decode == 0) {
            erpc_loop();
        }
        app_config.flag_decode = 0;
        printf("get %d bytes\n", app_config.h264_data_len);
        cur_ptr = app_config.h264_data;


        /* parse and decode stream data */
        while (app_config.h264_data_len > 0) {
            int len = av_parser_parse2(
                g_pavcodec_parse_context, g_avcodec_ctx,
                &g_avpacket.data, &g_avpacket.size,
                cur_ptr, app_config.h264_data_len,
                AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

            cur_ptr += len;
            app_config.h264_data_len -= len;

            if (g_avpacket.size == 0)
                continue;

            ret = avcodec_decode_video2(g_avcodec_ctx, g_pavframe, &app_config.got_picture, &g_avpacket);
            if (ret < 0) {
                printf("Decode Error.\n");
                return;
            }
            if (app_config.got_picture) {
                if (first_time) {
                    printf("\nCodec Full Name:%s\n", g_avcodec_ctx->codec->long_name);
                    printf("width:%d\nheight:%d\n\n", g_avcodec_ctx->width, g_avcodec_ctx->height);
                    first_time = 0;
                }
                f_cnt += app_config.got_picture;
                printf("-- %d\n", f_cnt);
                app_config.output_addr = (int32_t)g_pavframe->data;


                /*
                * write decoded data cache back to sdram
                */
                {
                    int* cur_buffer = (int *)core_local_mem_to_sys_address(HPM_CORE1, (uint32_t) g_pavframe->data[0]);
                    if (l1c_dc_is_enabled()) {
                        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)cur_buffer);
                        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)cur_buffer + g_pavframe->width*g_pavframe->height);
                        uint32_t aligned_size = aligned_end - aligned_start;
                        disable_global_irq(CSR_MSTATUS_MIE_MASK);
                        l1c_dc_writeback(aligned_start, aligned_size);
                        enable_global_irq(CSR_MSTATUS_MIE_MASK);
                    }
                    cur_buffer = (int *)core_local_mem_to_sys_address(HPM_CORE1, (uint32_t) g_pavframe->data[1]);
                    if (l1c_dc_is_enabled()) {
                        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)cur_buffer);
                        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)cur_buffer + g_pavframe->width*g_pavframe->height / 2);
                        uint32_t aligned_size = aligned_end - aligned_start;
                        disable_global_irq(CSR_MSTATUS_MIE_MASK);
                        l1c_dc_writeback(aligned_start, aligned_size);
                        enable_global_irq(CSR_MSTATUS_MIE_MASK);
                    }
                    cur_buffer = (int *)core_local_mem_to_sys_address(HPM_CORE1, (uint32_t) g_pavframe->data[2]);
                    if (l1c_dc_is_enabled()) {
                        uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN((uint32_t)cur_buffer);
                        uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP((uint32_t)cur_buffer + g_pavframe->width*g_pavframe->height / 2);
                        uint32_t aligned_size = aligned_end - aligned_start;
                        disable_global_irq(CSR_MSTATUS_MIE_MASK);
                        l1c_dc_writeback(aligned_start, aligned_size);
                        enable_global_irq(CSR_MSTATUS_MIE_MASK);
                    }
                }

                /* wait for core0 get decoded data out, then core1 can decode next frame */
                while (app_config.flag_data_output == 0) {
                    erpc_loop();
                }
                app_config.flag_data_output = 0;
            }
        } // end of while (cur_size > 0), decode done.
    }
}