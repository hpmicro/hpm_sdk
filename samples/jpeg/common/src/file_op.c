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
#include "ff.h"
#include "string.h"
#include "stdio.h"
/*---------------------------------------------------------------------*
 * scan files in the specified directory of an U disk/SD card
 *---------------------------------------------------------------------
 */
char * f_scan(char *path)
{
    char *scanfilename = {""};
	DIR DirInfo;
#if FF_USE_LFN
	static FILINFO fileinfo;
    fileinfo.fsize = FF_MAX_LFN * 2 + 1;
    memcpy(fileinfo.fname, (TCHAR*)path, strlen(path));

#endif
    /*Open the file under the specified path*/
    if (f_opendir(&DirInfo,(const TCHAR*)path) == FR_OK)
    {
        printf("\nDirecotry Path: %s\r\n",path);
        while(f_readdir(&DirInfo, &fileinfo) == FR_OK)
        {
            /* If a file name is null, it means that the read operation is complete. */
            if(!fileinfo.fname[0]) {
                break;
             }
            if ((fileinfo.fattrib & AM_HID) == 0 && \
                (fileinfo.fattrib & AM_SYS) == 0)
            {
                if ((fileinfo.fattrib & AM_DIR) == AM_DIR) {
                    printf("Folder Name: %s\r\n", fileinfo.fname);
                } else {
                    printf("File   Name: %s\r\n", fileinfo.fname);
                    strcpy(scanfilename, fileinfo.fname);
                }
            }
        }
    }
    else {
        printf("Can't open the directory: %s\n", path);
    }
    f_closedir(&DirInfo);
    return scanfilename;
}
