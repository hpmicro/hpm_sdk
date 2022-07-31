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
#include "file_op.h"
/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
FIL s_file;

#define true	1
#define false	0
/*---------------------------------------------------------------------*
 * scan files in the specified directory of storage device
 *---------------------------------------------------------------------
 */
uint8_t file_scan(char *path, char *filetype, file_name_list_t *jpgfile)
{
    /*Number of scanned files*/
    int8_t nums = 0;
    /*Get file name length*/
    int8_t filenamelen = 0;
    DIR DirInfo;
    FILINFO fileinfo;
    FRESULT filefresult;
    int8_t nin, fin, njn;

    /*File type character length*/
    fin = strlen(filetype);
    /*Open the file under the specified path*/
    filefresult = f_opendir(&DirInfo, (const TCHAR*)path);
    if (filefresult == FR_OK) {
        printf("\nDirecotry Path: %s\r\n",path);
        while (f_readdir(&DirInfo, &fileinfo) == FR_OK) {
            /* If a file name is null, it means that the read operation is complete. */
            if (!fileinfo.fname[0]) {
                break;
            }
            if ((fileinfo.fattrib & AM_HID) == 0 && \
                (fileinfo.fattrib & AM_SYS) == 0)
            {
                if ((fileinfo.fattrib & AM_DIR) == AM_DIR) {
                    printf("Folder Name: %s\r\n", fileinfo.fname);
                } else {
                    printf("File   Name: %s\r\n", fileinfo.fname);
                    /* Get file name length. */
                    filenamelen = strlen(fileinfo.fname);
                    /* Judge the length of file name. */
                    if (filenamelen > fin) { 
                        /*Judge whether the file type is correct*/
                        for (nin = 0; nin < fin; nin++) {
                            njn = fin - nin;
                            if (fileinfo.fname[filenamelen - njn] != filetype[nin]) {
                                break;
                            }
                        }
                        /*Correct file type*/
                        if (nin == fin) {
                            if (nums < FILENAMENUM) {
                                /* Get one of the picture name*/
                                strcpy(jpgfile->filename[nums], fileinfo.fname);
                                jpgfile->filesize[nums] = fileinfo.fsize;
                                nums++;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }
    } else {
        printf("Can't open the directory: %s\n", path);
        return false;
    }
    f_closedir(&DirInfo);

    /*Get total number of picture files*/
    jpgfile->fillnum = nums;

    return true;
}

/*---------------------------------------------------------------------*
 * Get a file data from the file list
 *---------------------------------------------------------------------
 */
uint8_t file_get(int32_t filenums, file_name_list_t *jpgfiles, uint8_t *filesbuff, int32_t *jpgsize)
{
    UINT cnt = 0;
    FRESULT fresult;

    if (jpgfiles->filesize[filenums] > FILEBUFFLEN) {
        return false;
    }
    /* Open file */
    f_open(&s_file, jpgfiles->filename[filenums], FA_OPEN_ALWAYS | FA_READ);
    printf("Open file successfully, status=%d\n", fresult);
    /* read the specified file from Storage device */
    f_read(&s_file, filesbuff, FILEBUFFLEN, &cnt);
    f_close(&s_file);
    *jpgsize = cnt;
    return true;
}

/*---------------------------------------------------------------------*
 * File data store a file type
 *---------------------------------------------------------------------
 */
uint8_t file_store(uint8_t *filebuffs, int32_t *jpgsize, char *storefilename)
{
    char filename[255];
    UINT cnt = 0;

    strcpy(filename, storefilename);
    /* Open file */
    if (f_open(&s_file, filename, FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
        printf("The %s is open.\n", filename);
        /* Write the specified file via Storage device */
        f_write(&s_file, (uint8_t *)(filebuffs), *jpgsize, &cnt);
        f_close(&s_file);
        *jpgsize = 0;
    }
}