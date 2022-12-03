/*
 * buffered file I/O
 * Copyright (c) 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/avstring.h"
#include "libavutil/internal.h"
#include "libavutil/opt.h"
#include "avformat.h"
#if HAVE_DIRENT_H
//#include <dirent.h>
#endif
//#include <fcntl.h>
#if HAVE_IO_H
#include <io.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
//#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "os_support.h"
#include "url.h"
#include "libavutil/log.h"
#include "ff.h"
#include "diskio.h"
//#include "fsl_sd_disk.h"
/* Some systems may not have S_ISFIFO */
#ifndef S_ISFIFO
#  ifdef S_IFIFO
#    define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#  else
#    define S_ISFIFO(m) 0
#  endif
#endif

/* Not available in POSIX.1-1996 */
#ifndef S_ISLNK
#  ifdef S_IFLNK
#    define S_ISLNK(m) (((m) & S_IFLNK) == S_IFLNK)
#  else
#    define S_ISLNK(m) 0
#  endif
#endif

/* Not available in POSIX.1-1996 */
#ifndef S_ISSOCK
#  ifdef S_IFSOCK
#    define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#  else
#    define S_ISSOCK(m) 0
#  endif
#endif
#define ENOSYS 38
/* standard file protocol */
static FIL file;
typedef struct FileContext {
    const AVClass *class;
    int trunc;
    int blocksize;
} FileContext;

//int fd;
//FILE *fd;

static const AVClass file_class = {
    .class_name = "file",
    .item_name  = av_default_item_name,
    //.option     = file_options,
    .version    = LIBAVUTIL_VERSION_INT,
};

UINT bytes_read;
static int file_read(URLContext *h, unsigned char *buf, int size)
{
    int ret;
  //ret = read(fd, buf, 32768);
    //ret=fread (buf, 1, 32768, fd);
    f_read(&file, buf, size, &bytes_read);
    ret = (int) bytes_read;
    printf("read size:%d\n",ret);
   // return (ret == -1) ? AVERROR(errno) : ret;
    return (ret == -1) ? AVERROR(errno) : ret;
}

static int file_write(URLContext *h, const unsigned char *buf, int size)
{
    int ret;
   printf("write\n");
}

static int file_get_handle(URLContext *h)
{
    printf("handle\n");
    return 0;
}

static int file_check(URLContext *h, int mask)
{
    int ret = 0;
    printf("check\n");
    return ret;
}

static int file_delete(URLContext *h)
{
    printf("delete\n");
    return AVERROR(ENOSYS);
}

static int file_move(URLContext *h_src, URLContext *h_dst)
{
    printf("move\n");
    return AVERROR(ENOSYS);
}

#if CONFIG_FILE_PROTOCOL

static int file_open(URLContext *h, const char *filename, int flags)
{
    int access;
 // fd = open(filename, access);
//fd= fopen(filename, "rb");
//   //rintf("open:%d\n",*fd);
//	if (!fd){
//		fprintf(stderr, "Could not open '%s'\n", filename);
//		//it(1);
//	}
      FRESULT c = f_open(&file, filename, FA_READ | FA_OPEN_EXISTING);
	if (c != FR_OK)
        {
		printf("Could not open '%s'\n", filename);
		exit(1);
	}
    return 0;
}

/* XXX: use llseek */
static int64_t file_seek(URLContext *h, int64_t pos, int whence)
{
    int64_t ret;
    int kk, error;
    int length;
    printf("pos:%d\n",pos);
    //printf("SEEK_CUR:%d\n",SEEK_CUR);
    f_lseek(&file, 0);
    if(whence==SEEK_END){
        printf("SEEK_END\n");
        error=f_lseek(&file, pos + f_size(&file));
        if (error == FR_OK) {
            kk = pos + f_size(&file);
        } else {
            kk = -1;
        }
    }else if(whence==65536)
    {
        printf("AVSEEK_SIZE\n");
        kk = -1;
    }else
    {
        printf("whence:%d\n",whence);
        length = (int)whence;
        error = f_lseek(&file, pos+length);
        kk=pos+length;
    }
    ret=kk;
    return ret ;
}

static int file_close(URLContext *h)
{
     printf("close\n");
    return f_close(&file);
}

static int file_open_dir(URLContext *h)
{
  printf("open_dir\n");  
  return AVERROR(ENOSYS);

}

static int file_read_dir(URLContext *h, AVIODirEntry **next)
{
     printf("read_dir\n");  
    return AVERROR(ENOSYS);
}

static int file_close_dir(URLContext *h)
{
    printf("open_dir\n");  
    return AVERROR(ENOSYS);
}

URLProtocol ff_file_protocol = {
    .name                = "file",
    .url_open            = file_open,
    .url_read            = file_read,
    .url_write           = file_write,
    .url_seek            = file_seek,
    .url_close           = file_close,
    .url_get_file_handle = file_get_handle,
    .url_check           = file_check,
    .url_delete          = file_delete,
    .url_move            = file_move,
    .priv_data_size      = sizeof(FileContext),
    .priv_data_class     = &file_class,
    .url_open_dir        = file_open_dir,
    .url_read_dir        = file_read_dir,
    .url_close_dir       = file_close_dir,
    .default_whitelist   = "file,crypto"
};

#endif /* CONFIG_FILE_PROTOCOL */


