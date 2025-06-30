#include <stdio.h>
#include "board.h"
#include <hpm_lcdc_drv.h>
#include "hpm_sysctl_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_jpeg.h"
#include "ff.h"
#include "diskio.h"
#include <hpm_soc.h>
#include <file_op.h>
#if defined JPEG_USE_SDCARD
    #include "sd_fatfs.h"
#elif defined JPEG_USE_UDISK
    #include "msc_app.h"
#else
    #error "no target storage is specified, please set JPEG_USE_SDCARD or JPEG_USE_UDISK"
#endif

#define LCDC_BASE          BOARD_LCD_BASE
#define LAYER0_WIDTH       BOARD_LCD_WIDTH
#define LAYER0_HEIGHT      BOARD_LCD_HEIGHT
#define LAYER0_PIXEL_SIZE  2
#define LAYER0_PIXEL       display_pixel_format_rgb565

#define JPEG_CLOCK         clock_jpeg
#define JPEG_BASE          HPM_JPEG
#define JPEG_IRQ           IRQn_JPEG
#define JPEG_IRQ_PRIORITY  4

#define JPEG_DECODE_FILE     "/test.jpg"
#define JPEG_ENCODE_FILE0     "/test_yuv422.jpg"
#define JPEG_ENCODE_FILE1     "/test_yuv420.jpg"

typedef struct ibuf {
    uint32_t stride;
    uint32_t height;
    void *buf;
} ibuf_t;

static uint8_t __attribute__((section(".noncacheable"), aligned(64))) fb0_buffer[LAYER0_WIDTH * LAYER0_HEIGHT * LAYER0_PIXEL_SIZE];

static void image_copy(ibuf_t *dst, ibuf_t *src);
static void store_device_init(void);

static void lcdc_init_and_enable(void)
{
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer_config = {0};
    int layer_index;
    display_pixel_format_t layer_format;

    memset(fb0_buffer, 0x00, sizeof(fb0_buffer));
    lcdc_get_default_config(LCDC_BASE, &config);
    board_panel_para_to_lcdc(&config);

    config.background.u = 0x00;
    lcdc_init(LCDC_BASE, &config);

    layer_index = 0;
    layer_format = display_pixel_format_rgb565;
    lcdc_get_default_layer_config(LCDC_BASE, &layer_config, layer_format, layer_index);
    layer_config.buffer = (uint32_t)fb0_buffer;
    layer_config.position_x = 0;
    layer_config.position_y = 0;
    layer_config.width = LAYER0_WIDTH;
    layer_config.height = LAYER0_HEIGHT;
    layer_config.background.u = 0x00;

    /*
     * blender of layer0 isn't exist.
     * layer0 is dst of layer1. layer0 and layer1 are using blender of layer1.
     */
    layer_config.alphablend.mode = display_alphablend_mode_clear;
    layer_config.alphablend.src_alpha_op = display_alpha_op_invalid;
    layer_config.alphablend.src_alpha = 0x00;
    layer_config.alphablend.dst_alpha_op = display_alpha_op_invalid;
    layer_config.alphablend.dst_alpha = 0x00;
    layer_config.byteorder = display_byteorder_a3a2a1a0;
    layer_config.max_bytes = lcdc_layer_max_bytes_256;
    layer_config.max_ot = 0;
    layer_config.csc_config.enable = false;
    layer_config.csc_config.ycbcr_mode = false;
    layer_config.yuv = display_yuv_mode_422_u1y1v1y2;
    lcdc_config_layer(LCDC_BASE, layer_index, &layer_config, true);

    lcdc_turn_on_display(LCDC_BASE);
}

void image_copy(ibuf_t *dst, ibuf_t *src);

SDK_DECLARE_EXT_ISR_M(JPEG_IRQ, hpm_jpeg_isr);

static void djob0_state_cb(hpm_jpeg_job_t *job)
{
    hpm_jpeg_decode_info_t info;

    hpm_jpeg_decode_job_get_info(job, &info);
    printf("djob0 callback hw_status: 0x%x\n", info.hw_status);
}

static void ejob0_state_cb(hpm_jpeg_job_t *job)
{
    hpm_jpeg_encode_info_t info;

    hpm_jpeg_encode_job_get_info(job, &info);
    printf("ejob0 callback hw_status: 0x%x\n", info.hw_status);
}

static void ejob1_state_cb(hpm_jpeg_job_t *job)
{
    hpm_jpeg_encode_info_t info;

    hpm_jpeg_encode_job_get_info(job, &info);

    printf("ejob1 callback hw_status: 0x%x\n", info.hw_status);
}

int main(void)
{
    hpm_jpeg_cfg_t hpm_jpeg_cfg;
    hpm_jpeg_decode_cfg_t dcfg;
    hpm_jpeg_encode_cfg_t ecfg;
    hpm_jpeg_job_t *ejob0;
    hpm_jpeg_job_t *ejob1;
    hpm_jpeg_job_t *djob0;
    hpm_jpeg_encode_info_t ejob_info;
    hpm_jpeg_decode_info_t djob_info;
    hpm_jpeg_file_t hpm_jpeg_file;
    static FIL fp;
    FRESULT fret;
    UINT len;
    void *buf;
    int ret;

    board_init();
    board_init_lcd();
    lcdc_init_and_enable();
    store_device_init();

    clock_add_to_group(JPEG_CLOCK, 0);
    intc_m_enable_irq_with_priority(JPEG_IRQ, JPEG_IRQ_PRIORITY);

    memset(&hpm_jpeg_cfg, 0x00, sizeof(hpm_jpeg_cfg));
    hpm_jpeg_cfg.jpeg_base = (void *)JPEG_BASE;
    hpm_jpeg_init(&hpm_jpeg_cfg);

    dcfg.out_format = HPM_JPEG_IMAGE_FORMAT_RGB565;
    dcfg.out_image_stride = 0;
    djob0 = hpm_jpeg_decode_job_alloc(&dcfg);
    if (!djob0) {
        printf("djob0 alloc failed\n");
        goto ERROR;
    }

    ecfg.jpeg_sampling = HPM_JPEG_SAMPLING_FORMAT_422V;
    ecfg.jpeg_quality = 90;
    ejob0 = hpm_jpeg_encode_job_alloc(&ecfg);
    if (!ejob0) {
        printf("ejob0 alloc failed\n");
        goto ERROR;
    }

    ecfg.jpeg_sampling = HPM_JPEG_SAMPLING_FORMAT_420;
    ecfg.jpeg_quality = 40;
    ejob1 = hpm_jpeg_encode_job_alloc(&ecfg);
    if (!ejob1) {
        printf("ejob1 alloc failed\n");
        goto ERROR;
    }

    fret = f_open(&fp, JPEG_DECODE_FILE, FA_READ);
    if (fret != FR_OK) {
        printf("file open failed(%d)!!!\n", fret);
        goto ERROR;
    }

    len = file_get_size(JPEG_DECODE_FILE);
    if (len == 0) {
        printf("file get size failed!!!\n");
        goto ERROR;
    }

    buf = malloc(len);
    if (!buf) {
        printf(JPEG_DECODE_FILE " buf alloc(%u) failed\n", len);
        goto ERROR;
    }

    ret = file_restore(JPEG_DECODE_FILE, buf, len);
    if (ret) {
        hpm_jpeg_file.jpeg_buf = buf;
        hpm_jpeg_file.len = len;
        ret = hpm_jpeg_decode_job_fill_file(djob0, &hpm_jpeg_file, 0);
        if (ret) {
            printf(JPEG_DECODE_FILE " fill failed\n");
            goto ERROR;
        }
        free(buf);
    } else {
        printf(JPEG_DECODE_FILE " file read failed!!!\n");
        goto ERROR;
    }

    ret = hpm_jpeg_decode_job_start(djob0, djob0_state_cb);
    if (ret) {
        printf(JPEG_DECODE_FILE " decode start failed!!!\n");
        goto ERROR;
    }

    while (1) {
        hpm_jpeg_decode_job_get_info(djob0, &djob_info);
        if (djob_info.status == HPM_JPEG_JOB_STATUS_FINISHED) {
            break;
        } else if (djob_info.status == HPM_JPEG_JOB_STATUS_ERROR) {
            printf(JPEG_DECODE_FILE " decode failed!!!\n");
            goto ERROR;
        }
    }

    printf("decode: width: %u\n", djob_info.image->width);
    printf("decode: height: %u\n", djob_info.image->height);
    printf("decode: stride: %u\n", djob_info.image->stride);

    ibuf_t idst = {LAYER0_WIDTH * LAYER0_PIXEL_SIZE, LAYER0_HEIGHT, fb0_buffer};
    ibuf_t isrc = {djob_info.image->stride, djob_info.image->height, djob_info.image->image_buf};
    image_copy(&idst, &isrc);

    hpm_jpeg_encode_job_fill_image(ejob0, djob_info.image, 0);
    hpm_jpeg_encode_job_fill_image(ejob1, djob_info.image, 0);

    hpm_jpeg_decode_job_free(djob0);

    ret = hpm_jpeg_encode_job_start(ejob0, ejob0_state_cb);
    if (ret) {
        printf(JPEG_ENCODE_FILE0 " start encode failed\n");
        goto ERROR;
    }

    ret = hpm_jpeg_encode_job_start(ejob1, ejob1_state_cb);
    if (ret) {
        printf(JPEG_ENCODE_FILE1 " start encode failed\n");
        goto ERROR;
    }

    while (ejob0 || ejob1) {
        if (ejob0) {
            hpm_jpeg_encode_job_get_info(ejob0, &ejob_info);
            if (ejob_info.status == HPM_JPEG_JOB_STATUS_FINISHED) {
                ret = file_store(JPEG_ENCODE_FILE0, ejob_info.file->jpeg_buf, ejob_info.file->len);
                if (ret) {
                    printf("save %u bytes to %s finished\n", ejob_info.file->len, JPEG_ENCODE_FILE0);
                } else {
                    printf("save %u bytes to %s failed\n", ejob_info.file->len, JPEG_ENCODE_FILE0);
                    goto ERROR;
                }
                hpm_jpeg_encode_job_free(ejob0);
                ejob0 = NULL;
            }
        }

        if (ejob1) {
            hpm_jpeg_encode_job_get_info(ejob1, &ejob_info);
            if (ejob_info.status == HPM_JPEG_JOB_STATUS_FINISHED) {
                ret = file_store(JPEG_ENCODE_FILE1, ejob_info.file->jpeg_buf, ejob_info.file->len);
                if (ret) {
                    printf("save %u bytes to %s finished\n", ejob_info.file->len, JPEG_ENCODE_FILE1);
                } else {
                    printf("save %u bytes to %s failed\n", ejob_info.file->len, JPEG_ENCODE_FILE1);
                    goto ERROR;
                }
                hpm_jpeg_encode_job_free(ejob1);
                ejob1 = NULL;
            }
        }
    }

    printf("hpm_jpeg sample end!\n");

ERROR:
    while (1) {
    }
    return 0;
}

static void store_device_init(void)
{
#if defined JPEG_USE_SDCARD
    /*Read picture data by SD card*/
    while (!check_disk()) {
    }
    printf("Read picture data by SD card\n");
#elif defined JPEG_USE_UDISK
    /*Reading picture data in tinyusb mode*/
    printf("Read picture data by usb-mode\n");
    init_disk();
    while (!check_disk()) {
    }
#endif
}

static void image_copy(ibuf_t *dst, ibuf_t *src)
{
    int height = src->height < dst->height ? src->height : dst->height;
    int stride = src->stride < dst->stride ? src->stride : dst->stride;
    uint8_t *src8 = (uint8_t *)src->buf;
    uint8_t *dst8 = (uint8_t *)dst->buf;

    for (int i = 0; i < height; i++) {
        memcpy(dst8, src8, stride);
        src8 += src->stride;
        dst8 += dst->stride;
    }
}