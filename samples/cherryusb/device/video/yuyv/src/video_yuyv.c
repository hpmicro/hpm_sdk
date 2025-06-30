/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbd_core.h"
#include "usbd_video.h"
#include "board.h"
#include "hpm_camera.h"
#include "hpm_cam_drv.h"
#include "camera.h"

#define _CONCAT3(x, y, z) x##y##z
#define CONCAT3(x, y, z)  _CONCAT3(x, y, z)

#if (CAMERA_MAX_IMAGE_OUTPUT_WIDTH > 640)
#define IMAGE_WIDTH 640
#else
#define IMAGE_WIDTH CAMERA_MAX_IMAGE_OUTPUT_WIDTH
#endif

#if (CAMERA_MAX_IMAGE_OUTPUT_HEIGHT > 480)
#define IMAGE_HEIGHT 480
#else
#define IMAGE_HEIGHT CAMERA_MAX_IMAGE_OUTPUT_HEIGHT
#endif

#define PIXEL_FORMAT     display_pixel_format_yuv422
#define CAMERA_INTERFACE camera_interface_dvp

#define COLOR_SIZE 16
typedef CONCAT3(uint, COLOR_SIZE, _t) color_t;

#define VIDEO_IN_EP  0x81
#define VIDEO_INT_EP 0x83

#define CAM_FPS        (30)
#define INTERVAL       (10000000 / CAM_FPS)
#define MIN_BIT_RATE   (IMAGE_WIDTH * IMAGE_HEIGHT * COLOR_SIZE * CAM_FPS)
#define MAX_BIT_RATE   (IMAGE_WIDTH * IMAGE_HEIGHT * COLOR_SIZE * CAM_FPS)
#define MAX_FRAME_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT * 2)

#define MAX_PAYLOAD_SIZE  (((MAX_FRAME_SIZE * CAM_FPS) / 8000) + 12) /* for high speed with two transcations every one micro frame, (MAX_FRAME_SIZE * CAM_FPS) / 8000 + sizeof(struct video_payload_header) */
#if (MAX_PAYLOAD_SIZE > 1024)
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 2)) | (0x01 << 11))
#elif (MAX_PAYLOAD_SIZE > 2048)
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 3)) | (0x02 << 11))
#else
#define VIDEO_PACKET_SIZE (MAX_PAYLOAD_SIZE)
#endif

#define VS_HEADER_SIZ \
    (unsigned int)(VIDEO_SIZEOF_VS_INPUT_HEADER_DESC(1, 1) + VIDEO_SIZEOF_VS_FORMAT_UNCOMPRESSED_DESC + VIDEO_SIZEOF_VS_FRAME_UNCOMPRESSED_DESC(1))

#define USB_VIDEO_DESC_SIZ (unsigned long)(9 + VIDEO_VC_NOEP_DESCRIPTOR_LEN + 9 + VS_HEADER_SIZ + 6 + 9 + 7)

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01),
};

static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    VIDEO_VC_NOEP_DESCRIPTOR_INIT(0x00, VIDEO_INT_EP, 0x0110, VIDEO_VC_TERMINAL_LEN, 48000000, 0x02),
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x00),
    VIDEO_VS_INPUT_HEADER_DESCRIPTOR_INIT(0x01, VS_HEADER_SIZ, VIDEO_IN_EP, 0x00),
    VIDEO_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR_INIT(0x01, 0x01, VIDEO_GUID_YUY2),
    VIDEO_VS_FRAME_UNCOMPRESSED_DESCRIPTOR_INIT(0x01, IMAGE_WIDTH, IMAGE_HEIGHT, MIN_BIT_RATE, MAX_BIT_RATE, MAX_FRAME_SIZE, DBVAL(INTERVAL), 0x01,
                                                DBVAL(INTERVAL)),
    VIDEO_VS_COLOR_MATCHING_DESCRIPTOR_INIT(),
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x01, 0x01),
    /* 1.2.2.2 Standard VideoStream Isochronous Video Data Endpoint Descriptor */
    USB_ENDPOINT_DESCRIPTOR_INIT(VIDEO_IN_EP, 0x05, VIDEO_PACKET_SIZE, 0x01),
};

static const uint8_t device_quality_descriptor[] = {
    USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, 0x00),
};

static const char * const string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "HPMicro",                    /* Manufacturer */
    "HPMicro UVC YUYV",           /* Product */
    "2025052705",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return config_descriptor_hs;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return device_quality_descriptor;
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return NULL;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor video_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static volatile bool tx_flag;
static volatile bool iso_ilde;

ATTR_PLACE_AT_FAST_RAM USB_MEM_ALIGNX uint8_t packet_buffer[MAX_PAYLOAD_SIZE];
ATTR_PLACE_AT_NONCACHEABLE color_t cam_buffer[2][IMAGE_WIDTH * IMAGE_HEIGHT];
static camera_config_t camera_config;

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    (void)busid;
    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

void usbd_video_open(uint8_t busid, uint8_t intf)
{
    (void)busid;
    (void)intf;

    USB_LOG_RAW("OPEN\r\n");
    tx_flag = 1;
    iso_ilde = true;
}

void usbd_video_close(uint8_t busid, uint8_t intf)
{
    (void)busid;
    (void)intf;

    USB_LOG_RAW("CLOSE\r\n");
    tx_flag = 0;
    iso_ilde = true;
}

void usbd_video_iso_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)nbytes;

    if (usbd_video_stream_split_transfer(busid, ep)) {
        iso_ilde = true;
    }
}

static struct usbd_endpoint video_in_ep = { .ep_cb = usbd_video_iso_callback, .ep_addr = VIDEO_IN_EP };

void video_init(uint8_t busid, uintptr_t reg_base)
{
    usbd_desc_register(busid, &video_descriptor);
    usbd_add_interface(busid, usbd_video_init_intf(busid, &intf0, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_interface(busid, usbd_video_init_intf(busid, &intf1, INTERVAL, MAX_FRAME_SIZE, MAX_PAYLOAD_SIZE));
    usbd_add_endpoint(busid, &video_in_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

void init_camera_device(void)
{
    camera_context_t camera_context = { 0 };

    camera_context.i2c_device_addr = CAMERA_DEVICE_ADDR;
    camera_context.ptr = CAM_I2C;
    camera_context.delay_ms = board_delay_ms;
#ifdef BOARD_SUPPORT_CAM_RESET
    camera_context.write_rst = board_write_cam_rst;
#endif
#ifdef BOARD_SUPPORT_CAM_PWDN
    camera_context.write_pwdn = board_write_cam_pwdn;
#endif

    camera_config.width = IMAGE_WIDTH;
    camera_config.height = IMAGE_HEIGHT;
    camera_config.pixel_format = PIXEL_FORMAT;
    camera_config.interface = CAMERA_INTERFACE;

    /* get dvp interface parameters */
    if (CAMERA_INTERFACE == camera_interface_dvp) {
        camera_device_get_dvp_param(&camera_context, &camera_config);
    }

    if (status_success != camera_device_init(&camera_context, &camera_config)) {
        printf("failed to init camera device\n");
        while (1) {
            ;
        }
    }
}

void init_cam(void)
{
    cam_config_t cam_config;
    camera_param_dvp_t *dvp;
    assert((camera_config.interface == camera_interface_dvp) && (camera_config.interface_param != NULL));
    dvp = (camera_param_dvp_t *)camera_config.interface_param;

    cam_get_default_config(TEST_CAM, &cam_config, PIXEL_FORMAT);

    cam_config.width = IMAGE_WIDTH;
    cam_config.height = IMAGE_HEIGHT;
    cam_config.hsync_active_low = dvp->hsync_active_low;
    cam_config.vsync_active_low = dvp->vsync_active_low;
    cam_config.buffer1 = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&cam_buffer[0][0]);
    cam_config.buffer2 = core_local_mem_to_sys_address(HPM_CORE0, (uint32_t)&cam_buffer[1][0]);
    cam_config.color_format = cam_get_pixel_format(PIXEL_FORMAT);
    cam_config.csc_config.enable = false;
    if (CAM_COLOR_FORMAT_UNSUPPORTED == cam_config.color_format) {
        printf("cam does not support this pixel format\n");
        return;
    }

    cam_init(TEST_CAM, &cam_config);

    cam_enable_irq(TEST_CAM, cam_irq_fb1_dma_transfer_done);
    cam_enable_irq(TEST_CAM, cam_irq_fb2_dma_transfer_done);

    cam_start(TEST_CAM);
}

SDK_DECLARE_EXT_ISR_M(TEST_CAM_IRQ, cam_isr)
void cam_isr(void)
{
    if (cam_check_status(TEST_CAM, cam_status_fb1_dma_transfer_done)) {
        cam_clear_status(TEST_CAM, cam_status_fb1_dma_transfer_done);
        if (tx_flag) {
            if (iso_ilde) {
                iso_ilde = false;
                usbd_video_stream_start_write(0, VIDEO_IN_EP, &packet_buffer[0], (uint8_t *)&cam_buffer[0][0], MAX_FRAME_SIZE, false);
            }
        }
    } else if (cam_check_status(TEST_CAM, cam_status_fb2_dma_transfer_done)) {
        cam_clear_status(TEST_CAM, cam_status_fb2_dma_transfer_done);
        if (tx_flag) {
            if (iso_ilde) {
                iso_ilde = false;
                usbd_video_stream_start_write(0, VIDEO_IN_EP, &packet_buffer[0], (uint8_t *)&cam_buffer[1][0], MAX_FRAME_SIZE, false);
            }
        }
    } else {
        ;   /* Do Nothing */
    }
}