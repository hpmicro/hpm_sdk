/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * Includes
 *
 */
#include "board.h"
#include "tusb.h"
#include "diskio.h"
#include "ff.h"
#include "file_op.h"
#include "msc_app.h"
/*
 * Define variables
 *
 */
#if CFG_TUH_MSC
static ATTR_PLACE_AT_NONCACHEABLE_BSS scsi_inquiry_resp_t inquiry_resp;
static ATTR_PLACE_AT_NONCACHEABLE_BSS FATFS udisk[CFG_TUH_DEVICE_MAX];
static bool disk_mounted;
/*
 * Tinyusb FatFs module reads image file data
 *
 */
bool file_system_mount(uint8_t dev_addr)
{
    char logic_drv_num[10] = {0};
    uint8_t phy_disk = dev_addr - 1;

    /* file system */
    disk_initialize(phy_disk);

    if (!disk_is_ready(phy_disk)) {
        return false;
    }

    sprintf(logic_drv_num, "%d:", phy_disk);
    if (f_mount(&udisk[phy_disk], logic_drv_num, 0) != FR_OK) {
        printf("FatFs mount failed!\n");
        disk_mounted = false;
    } else {
        printf("FatFs mount succeeded!\n");

        /* change to newly mounted drive */
        f_chdrive(logic_drv_num);
        /* set root directory as current directory */
        f_chdir("/");
        disk_mounted = true;
    }

    return disk_mounted;
}

/*
 * inquiry_complete_cb
 *
 */
bool inquiry_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const *cb_data)
{
    uint32_t block_count;
    uint32_t block_size;

    if (cb_data->csw->status != 0) {
        printf("Inquiry failed\r\n");
        return false;
    }

    /* Print out Vendor ID, Product ID and Rev */
    printf("%.8s %.16s rev %.4s\r\n", inquiry_resp.vendor_id, inquiry_resp.product_id, inquiry_resp.product_rev);

    /* Get capacity of device */
    block_count = tuh_msc_get_block_count(dev_addr, cb_data->cbw->lun);
    block_size = tuh_msc_get_block_size(dev_addr, cb_data->cbw->lun);

    printf("Disk Size: %lu MB\r\n", block_count / ((1024 * 1024) / block_size));
    printf("Block Count = %lu, Block Size: %lu\r\n", block_count, block_size);

    return file_system_mount(dev_addr);
}

/*
 * tuh_msc_mount_cb
 *
 */
void tuh_msc_mount_cb(uint8_t dev_addr)
{
    uint8_t const lun = 0;

    printf("\nA MassStorage device is mounted.\r\n");
    tuh_msc_inquiry(dev_addr, lun, &inquiry_resp, inquiry_complete_cb, (uintptr_t)NULL);
}

void tuh_msc_unmount_cb(uint8_t dev_addr)
{
    uint8_t phy_disk = dev_addr - 1;
    char logic_drv_num[10] = {0};

    printf("A MassStorage device is unmounted.\r\n");
    sprintf(logic_drv_num, "%d", phy_disk);
    if (FR_OK == f_unmount(logic_drv_num)) {
        printf("FatFs unmount succeeded!\n");
        disk_mounted = false;
        disk_deinitialize(phy_disk);
    }
}
#endif /* End of CFG_TUH_MSC */

void init_disk(void)
{
    /* tinyusb Host initialization */
    if (BOARD_TUH_RHPORT == 0) {
        board_init_usb(HPM_USB0);
#ifdef HPM_USB1
    } else if (BOARD_TUH_RHPORT == 1) {
        board_init_usb(HPM_USB1);
#endif
    } else {
        printf("Don't support HPM_USB%d!\n", BOARD_TUH_RHPORT);
        while (1) {
            ;
        }
    }

    tusb_init();
}

/*
 * Tinyusb task, reading external file data
 *
 */
bool check_disk(void)
{
    if (!disk_mounted) {
        for (uint32_t i = 0; i < 50000; i++) {
            tuh_task();
        }
    }
    return disk_mounted;
}
