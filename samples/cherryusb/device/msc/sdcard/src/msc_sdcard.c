/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbh_core.h"
#include "usbd_core.h"
#include "usbd_msc.h"
#include "board.h"
#include "hpm_sdmmc_sd.h"

#define MSC_IN_EP  0x81
#define MSC_OUT_EP 0x02

#define USB_CONFIG_SIZE (9 + MSC_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define MSC_MAX_MPS 512
#else
#define MSC_MAX_MPS 64
#endif

const uint8_t msc_device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, MSC_MAX_MPS, 0x02),
    /*
     * string0 descriptor
     */
    USB_LANGID_INIT(USBD_LANGID_STRING),
    /*
     * string1 descriptor
     */
    0x18,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'H', 0x00,                  /* wcChar0 */
    'P', 0x00,                  /* wcChar1 */
    'M', 0x00,                  /* wcChar2 */
    'i', 0x00,                  /* wcChar3 */
    'c', 0x00,                  /* wcChar4 */
    'r', 0x00,                  /* wcChar5 */
    'o', 0x00,                  /* wcChar6 */
    ' ', 0x00,                  /* wcChar7 */
    'U', 0x00,                  /* wcChar8 */
    'S', 0x00,                  /* wcChar9 */
    'B', 0x00,                  /* wcChar10 */
    /*
     * string2 descriptor
     */
    0x2a,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'H', 0x00,                  /* wcChar0 */
    'P', 0x00,                  /* wcChar1 */
    'M', 0x00,                  /* wcChar2 */
    'i', 0x00,                  /* wcChar3 */
    'c', 0x00,                  /* wcChar4 */
    'r', 0x00,                  /* wcChar5 */
    'o', 0x00,                  /* wcChar6 */
    ' ', 0x00,                  /* wcChar7 */
    'U', 0x00,                  /* wcChar8 */
    'S', 0x00,                  /* wcChar9 */
    'B', 0x00,                  /* wcChar10 */
    ' ', 0x00,                  /* wcChar11 */
    'M', 0x00,                  /* wcChar12 */
    'S', 0x00,                  /* wcChar13 */
    'C', 0x00,                  /* wcChar14 */
    ' ', 0x00,                  /* wcChar15 */
    'D', 0x00,                  /* wcChar16 */
    'E', 0x00,                  /* wcChar17 */
    'M', 0x00,                  /* wcChar18 */
    'O', 0x00,                  /* wcChar19 */
    /*
     * string3 descriptor
     */
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '3', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '1', 0x00,                  /* wcChar5 */
    '2', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '2', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    /*
     * device qualifier descriptor
     */
    0x0a, USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00,
#endif
    0x00
};

static struct usbd_interface intf0;
ATTR_PLACE_AT_NONCACHEABLE_BSS sdmmc_host_t g_sdmmc_host;
static sd_card_t s_sd = {.host = &g_sdmmc_host};

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

void usbd_msc_get_cap(uint8_t busid, uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
    (void)busid;
    (void)lun;

    if (s_sd.host->card_inserted) {
        *block_num = s_sd.block_count;
        *block_size = s_sd.block_size;
    } else {
        *block_num = 0;
        *block_size = 0;
    }
    printf("blocknum:%d, block_size:%d\r\n", *block_num, *block_size);
}

int usbd_msc_sector_read(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{
    (void)busid;
    (void)lun;

    if (s_sd.host->card_inserted) {
        uint32_t sys_buf_addr = core_local_mem_to_sys_address(0, (uint32_t)buffer);
        hpm_stat_t status = sd_read_blocks(&s_sd, (uint8_t *) sys_buf_addr, sector, length / s_sd.block_size);
        if (status != status_success) {
            printf("SD Read failed at sector %lu\n", sector);
        }
        return status;
    } else {
        return -1;
    }
}

int usbd_msc_sector_write(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{
    (void)busid;
    (void)lun;

    if (s_sd.host->card_inserted) {
        uint32_t sys_buf_addr = core_local_mem_to_sys_address(0, (uint32_t)buffer);
        hpm_stat_t status;
        status =  sd_write_blocks(&s_sd, (uint8_t *) sys_buf_addr, sector, length / s_sd.block_size);
        return status;
    } else {
        return -1;
    }
}

void msc_device_init(uint8_t busid, uint32_t reg_base)
{
    hpm_stat_t status;

    do {
        status = board_init_sd_host_params(&g_sdmmc_host, BOARD_APP_SDCARD_SDXC_BASE);
        if (status != status_success) {
            break;
        }
        status = sd_init(&s_sd);
        if (status != status_success) {
            printf("Please insert the SD card to SD slot...\n");
            board_delay_ms(1000);
        }
    } while (status != status_success);

    usbd_desc_register(busid, msc_device_descriptor);
    usbd_add_interface(busid, usbd_msc_init_intf(busid, &intf0, MSC_OUT_EP, MSC_IN_EP));

    usbd_initialize(busid, reg_base, usbd_event_handler);
}
