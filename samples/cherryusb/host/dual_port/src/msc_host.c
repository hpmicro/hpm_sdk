/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "usbh_core.h"
#include "usbh_msc.h"
#include "ff.h"
#include "hpm_fatfs_usb.h"

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_write_buffer[25 * 100];

USB_NOCACHE_RAM_SECTION FATFS fs;
USB_NOCACHE_RAM_SECTION FIL fnew;
UINT fnum;
FRESULT res_sd;
static volatile bool mounted_flag;

int usb_msc_fatfs_write_read_test(void)
{
    const char *tmp_data = "cherryusb fatfs demo...\r\n";

    /*USB_LOG_RAW("data len:%d\r\n", strlen(tmp_data));*/
    for (uint32_t i = 0; i < 100; i++) {
        memcpy(&read_write_buffer[i * 25], tmp_data, strlen(tmp_data));
    }

    USB_LOG_RAW("test fatfs write\r\n");
    res_sd = f_open(&fnew, "cherryusb_msc_test.txt", FA_OPEN_ALWAYS | FA_WRITE);
    if (res_sd == FR_OK) {
        res_sd = f_write(&fnew, read_write_buffer, sizeof(read_write_buffer), &fnum);
        if (res_sd == FR_OK) {
            USB_LOG_RAW("write success, write len：%d\n", fnum);
        } else {
            USB_LOG_RAW("write fail\r\n");
            goto unmount;
        }
        f_close(&fnew);
    } else {
        USB_LOG_RAW("open fail\r\n");
        goto unmount;
    }
    USB_LOG_RAW("test fatfs read\r\n");

    res_sd = f_open(&fnew, "cherryusb_msc_test.txt", FA_OPEN_EXISTING | FA_READ);
    if (res_sd == FR_OK) {
        res_sd = f_read(&fnew, read_write_buffer, sizeof(read_write_buffer), &fnum);
        if (res_sd == FR_OK) {
            USB_LOG_RAW("read success, read len：%d\n", fnum);
        } else {
            USB_LOG_RAW("read fail\r\n");
            goto unmount;
        }
        f_close(&fnew);
    } else {
        USB_LOG_RAW("open fail\r\n");
        goto unmount;
    }
    return 0;

unmount:
    f_unmount("0:");
    return -1;
}

void usb_msc_fatfs_scan_file_test(char *path)
{
    DIR DirInfo;
#if FF_USE_LFN
    static FILINFO fileinfo;
    fileinfo.fsize = FF_MAX_LFN * 2 + 1;
    memcpy(fileinfo.fname, (TCHAR *)path, strlen(path));
#endif
    if (f_opendir(&DirInfo, (const TCHAR *)path) == FR_OK) {
        printf("\nDirecotry Path: %s\r\n", path);

        while (f_readdir(&DirInfo, &fileinfo) == FR_OK) {
            /* If a file name is null, it means that the read operation is complete. */
            if (!fileinfo.fname[0]) {
                break;
            }

            if ((fileinfo.fattrib & AM_HID) == 0 && (fileinfo.fattrib & AM_SYS) == 0) {
                if ((fileinfo.fattrib & AM_DIR) == AM_DIR) {
                    printf("Folder Name: %s\r\n", fileinfo.fname);
                } else {
                    printf("File   Name: %s\r\n", fileinfo.fname);
                }
            }
        }
    } else {
        printf("Can't open the directory: %s\n", path);
    }

    f_closedir(&DirInfo);
}

void usbh_msc_run(struct usbh_msc *msc_class)
{
    if (msc_class != NULL) {
        usb_disk_set_active_msc_class((void *)msc_class);
        mounted_flag = true;
    }
}

void usbh_msc_stop(struct usbh_msc *msc_class)
{
    (void)msc_class;
    mounted_flag = false;
}

static void msc_test(void)
{
    res_sd = f_mount(&fs, "0:", 1);
    if (res_sd != FR_OK) {
        USB_LOG_RAW("FATFS cherryusb mount fail,res:%d\r\n", res_sd);
    } else {
        USB_LOG_RAW("FATFS cherryusb mount succeeded!\r\n");
        if (usb_msc_fatfs_write_read_test() == 0) {
            usb_msc_fatfs_scan_file_test("/");
            f_unmount("0:");
            mounted_flag = false;
        }
    }
}

static void usbh_class_test_thread(void *argument)
{
    (void)argument;
    while (1) {
        usb_osal_msleep(1000);
        if (mounted_flag) {
            usb_osal_msleep(1000);
            msc_test();
        }
    }
}

void usbh_msc_class_test(void)
{
    printf("cherryusb msc host test\r\n");
    usb_osal_thread_create("usbh_test", 4096, CONFIG_USBHOST_PSC_PRIO + 1, usbh_class_test_thread, NULL);
}
