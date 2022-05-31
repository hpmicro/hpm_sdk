/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------
 */
#include "board.h"
#include "tusb.h"
#include "diskio.h"
#include "ff.h"
#include "file_op.h"
#include "msc_app.h"
/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
/*JPG data buff size*/
extern int32_t jpg_size;
/*JPG file data buff*/
extern uint8_t filebuff[];
/*USB execution result status*/
static uint8_t usbtask = false;

#if CFG_TUH_MSC
static scsi_inquiry_resp_t inquiry_resp;
FATFS fatfs[CFG_TUSB_HOST_DEVICE_MAX];
FIL file;
/*---------------------------------------------------------------------*
 * Tinyusb FatFs module reads image file data
 *---------------------------------------------------------------------
 */
bool file_system_mount(uint8_t dev_addr)
{
    char *filename;
    char logic_drv_num[10] = {0};
    uint8_t phy_disk = dev_addr - 1;
    UINT cnt = 0;

    /* file system */
    disk_initialize(phy_disk);

    if (disk_is_ready(phy_disk)){
        sprintf(logic_drv_num, "%d", phy_disk);
        if (f_mount(&fatfs[phy_disk], _T(logic_drv_num), 0) != FR_OK){
            printf("FatFs mount failed!\n");
            return false;
        }
        else{
            printf("FatFs mount succeeded!\n");
        }

        /* change to newly mounted drive */
        f_chdrive(logic_drv_num);
        /* set root directory as current directory */
        f_chdir("/");
#if defined DECOMPRESS_MODE        
        /* scan files in the specified directory of an U disk */
        filename = f_scan("/");
        if (f_open(&file, filename, FA_OPEN_ALWAYS | FA_READ) == FR_OK){
            printf("The %s is open.\n", filename);
            /* read the specified file from U disk */
            f_read(&file, filebuff, FILEBUFFLEN, &cnt);
            f_close(&file);
            jpg_size= cnt;
            usbtask = true;
        }
#elif defined COMPRESS_MODE
        f_scan("/");
        filename = "Camera.jpg";
        if (f_open(&file, filename, FA_OPEN_ALWAYS | FA_WRITE) == FR_OK){
            printf("The %s is open.\n", filename);
            /* Write the specified file via U disk */
            for (int32_t fl = 0; fl < jpg_size; fl++){
                f_write(&file, (uint8_t *)(filebuff + fl), 1, &cnt);
            }
            /* scan files in the specified directory of an U disk */
            f_scan("/");
            f_close(&file);
            jpg_size = 0;
            usbtask = true;
        }
#endif
        else{
            printf("Can't Open the %s file!\n", filename);
        }
    }
    return true;
}

/*---------------------------------------------------------------------*
 * inquiry_complete_cb
 *---------------------------------------------------------------------
 */
bool inquiry_complete_cb(uint8_t dev_addr, msc_cbw_t const *cbw, msc_csw_t const *csw)
{
    uint32_t block_count;
    uint32_t block_size;

    if (csw->status != 0){
        printf("Inquiry failed\r\n");
        return false;
    }

    /* Print out Vendor ID, Product ID and Rev */
    printf("%.8s %.16s rev %.4s\r\n", inquiry_resp.vendor_id, inquiry_resp.product_id, inquiry_resp.product_rev);

    /* Get capacity of device */
    block_count = tuh_msc_get_block_count(dev_addr, cbw->lun);
    block_size = tuh_msc_get_block_size(dev_addr, cbw->lun);

    printf("Disk Size: %lu MB\r\n", block_count / ((1024 * 1024) / block_size));
    printf("Block Count = %lu, Block Size: %lu\r\n", block_count, block_size);

    file_system_mount(dev_addr);

    return true;
}

/*---------------------------------------------------------------------*
 * tuh_msc_mount_cb
 *---------------------------------------------------------------------
 */
void tuh_msc_mount_cb(uint8_t dev_addr)
{
    uint8_t const lun = 0;

    printf("\nA MassStorage device is mounted.\r\n");
    tuh_msc_inquiry(dev_addr, lun, &inquiry_resp, inquiry_complete_cb);
}

void tuh_msc_unmount_cb(uint8_t dev_addr)
{
    uint8_t phy_disk = dev_addr - 1;
    char logic_drv_num[10] = {0};

    printf("A MassStorage device is unmounted.\r\n");
    sprintf(logic_drv_num, "%d", phy_disk);
    if (FR_OK == f_unmount(logic_drv_num)){
        printf("FatFs unmount succeeded!\n");
        disk_deinitialize(phy_disk);
    }
}
#endif /* End of CFG_TUH_MSC */

/*---------------------------------------------------------------------*
 * Tinyusb task, reading external file data
 *---------------------------------------------------------------------
 */
void tinyusb_task(void)
{
    /* tinyusb Device initialization */
    board_init_usb_pins();
    tusb_init();

    do{
        /* tinyusb host task */
        tuh_task();
    }while (!usbtask);
}