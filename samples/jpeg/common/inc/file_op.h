/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef FILE_OP_H
#define FILE_OP_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*JPG data buff length*/
#define FILEBUFFLEN 27000

/*rgb565 data buff length*/
#define RGBBUFFLEN 200000

#define FILENAMENUM 10
#define FILENAMELENGTH 255

typedef struct file_name_list {
    char filename[FILENAMENUM][FILENAMELENGTH];
    uint64_t filesize[FILENAMENUM];
    uint8_t fillnum;
} file_name_list_t;

uint8_t file_scan(char *path, char *filetype, file_name_list_t *jpgfile);
uint8_t file_get(int32_t filenums, file_name_list_t *jpgfiles, uint8_t *filesbuff, int32_t *jpgsize);
uint8_t file_store(uint8_t *filebuffs, int32_t *jpgsize, char *storefilename);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif  /* FILE_OP_H */