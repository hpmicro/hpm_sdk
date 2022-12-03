
#include "h264_stream.h"
#include "avcodec.h"
#include "mp4.h"
#include "uart.h"
#include "uart_demo_conf.h"
#define DISPLAY

#define DECODE_BUFFER_SIZE (128 * 1024)
#define FIRST_PACKET_MIN_SIZE (64 * 1024)
#define PACKET_DEAL_MIN_SIZE (1 * 1024)

/* var in ffmpeg lib */
extern AVCodec ff_h264_decoder;
extern AVInputFormat ff_mov_demuxer;
extern URLProtocol ff_file_protocol;
extern AVCodecParser ff_h264_parser;

struct AVCodecContext *g_avcodec_ctx = NULL;
struct AVCodec *g_pavcodec;
struct AVPacket g_avpacket;
struct AVFrame *g_pavframe = NULL;
struct AVFrame *g_pframe_yuv = NULL;
AVCodecParserContext *g_pavcodec_parse_context = NULL;

unsigned char h264_buf[DECODE_BUFFER_SIZE];
extern uint16_t framebuffer[2][BOARD_LCD_WIDTH * BOARD_LCD_HEIGHT];

int h264_decoder_init(void)
{
    av_log_set_level(AV_LOG_INFO);
    h264_decoder_release();
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
            h264_decoder_release();
            return -3;
        }
    }
    int tmp = avcodec_open2(g_avcodec_ctx, g_pavcodec, NULL);
    if (tmp < 0) {
        printf("Failed to open h264 decoder %d\n", tmp);
        h264_decoder_release();
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
    if (!g_pframe_yuv) {
        g_pframe_yuv = av_frame_alloc();
        if (!g_pframe_yuv) {
            perror("av_frame_alloc\n");
            return -6;
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

int h264_decoder_release(void)
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

    if (g_pframe_yuv) {
        av_frame_unref(g_pframe_yuv);
        av_free(g_pframe_yuv);
        g_pframe_yuv = NULL;
    }

    av_packet_unref(&g_avpacket);

    return 0;
}

void display(void)
{
    yuv420p_to_yuyv((unsigned char **)g_pavframe->data, (unsigned char *)&framebuffer[1], g_pavframe->width, g_pavframe->height);
    uint32_t status;
    pdma_scale(BOARD_PDMA_BASE,
        (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&framebuffer[0]),
        800,
        (uint32_t)core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&framebuffer[1]),
        g_pavframe->width,
        0, 0, g_pavframe->width, g_pavframe->height,
        800, 480,
        0xFF,
        display_pixel_format_ycbcr422,
        false, &status);
}

size_t decode_buffer_avilable_size;
size_t decode_buffer_read_size = 0;

/* Wait stream data from uart, until it's size is >= len */
void wait_data_until_size(size_t len)
{
    while (decode_buffer_read_size < len) {
        decode_buffer_read_size += get_uart_data(h264_buf + decode_buffer_read_size, &decode_buffer_avilable_size);
    }
}

void stream_decode(void)
{
    int ret = 0;
    int got_picture = 0;
    int first_time = 1;
    unsigned char *cur_ptr;
    size_t cur_size;
    decode_buffer_avilable_size = DECODE_BUFFER_SIZE;
    decode_buffer_read_size = 0;

    for (int i = 0; i < 10; i++)
        printf("\n");

    while (1) {
        wait_data_until_size(PACKET_DEAL_MIN_SIZE);

        cur_size = decode_buffer_read_size;
        cur_ptr = h264_buf;
        while (cur_size > 0) {
            int len = av_parser_parse2(
                g_pavcodec_parse_context, g_avcodec_ctx,
                &g_avpacket.data, &g_avpacket.size,
                cur_ptr, cur_size,
                AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

            cur_ptr += len;
            cur_size -= len;

            if (g_avpacket.size == 0)
                continue;

            ret = avcodec_decode_video2(g_avcodec_ctx, g_pavframe, &got_picture, &g_avpacket);
            if (ret < 0) {
                printf("Decode Error.\n");
                return;
            }
            if (got_picture) {
                if (first_time) {
                    printf("\nCodec Full Name:%s\n", g_avcodec_ctx->codec->long_name);
                    printf("width:%d\nheight:%d\n\n", g_avcodec_ctx->width, g_avcodec_ctx->height);
                    first_time = 0;
                }
                display();
            }
        } // end of while (cur_size > 0), decode done.

        // Reset decode buffer avilable size and read size to get a new packet.
        decode_buffer_read_size = 0;
        decode_buffer_avilable_size = DECODE_BUFFER_SIZE;
    }
}