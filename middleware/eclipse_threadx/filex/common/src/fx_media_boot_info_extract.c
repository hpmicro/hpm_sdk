/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** FileX Component                                                       */
/**                                                                       */
/**   Media                                                               */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define FX_SOURCE_CODE


/* Include necessary system files.  */

#include "fx_api.h"
#include "fx_system.h"
#include "fx_media.h"
#include "fx_utility.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _fx_media_boot_info_extract                         PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function extracts and validates the information from the boot  */
/*    record found in the memory buffer.  If the boot record is invalid,  */
/*    an FX_MEDIA_INVALID status is returned to the caller.               */
/*                                                                        */
/*    The FAT boot sector (512 bytes) that is operated on by this         */
/*    function must look like the following:                              */
/*                                                                        */
/*          Byte Offset         Meaning             Size                  */
/*                                                                        */
/*            0x000         Jump Instructions        3                    */
/*            0x003         OEM Name                 8                    */
/*            0x00B        *Bytes per Sector         2                    */
/*            0x00D        *Sectors per Cluster      1                    */
/*            0x00E        *Reserved Sectors         2                    */
/*            0x010        *Number of FATs           1                    */
/*            0x011        *Max Root Dir Entries     2                    */
/*            0x013        *Number of Sectors        2                    */
/*            0x015         Media Type               1                    */
/*            0x016        *Sectors per FAT          2                    */
/*            0x018        *Sectors per Track        2                    */
/*            0x01A        *Number of Heads          2                    */
/*            0x01C        *Hidden Sectors           4                    */
/*            0x020        *Huge Sectors             4                    */
/*            0x024         Drive Number             1                    */
/*            0x025         Reserved                 1                    */
/*            0x026         Boot Signature           1                    */
/*            0x027         Volume ID                4                    */
/*            0x02B         Volume Label             11                   */
/*            0x036         File System Type         8                    */
/*             ...              ...                 ...                   */
/*            0x1FE       **Signature (0x55aa)       2                    */
/*                                                                        */
/*            * Denotes which elements of the boot record                 */
/*              FileX uses.                                               */
/*                                                                        */
/*            **Denotes the element is checked by the I/O                 */
/*              driver.  This eliminates the need for a minimum           */
/*              512-byte buffer for FileX.                                */
/*                                                                        */
/*  Note: All values above are in little endian format, i.e. the LSB is   */
/*        in the lowest address.                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    media_ptr                             Media control block pointer   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    return status                                                       */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _fx_utility_16_unsigned_read          Read a UINT from buffer       */
/*    _fx_utility_32_unsigned_read          Read a ULONG from buffer      */
/*    _fx_utility_64_unsigned_read          Read a ULONG64 from memory    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _fx_media_open                        Media open function           */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  01-31-2022     Bhupendra Naphade        Modified comment(s), added    */
/*                                            check for bimap cache size, */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
UINT  _fx_media_boot_info_extract(FX_MEDIA *media_ptr)
{

UCHAR *boot_sector;


    /* Move the buffer pointer into a local copy.  */
    boot_sector =  media_ptr -> fx_media_driver_buffer;

    /* Extract the number of bytes per sector.  */
    media_ptr -> fx_media_bytes_per_sector =    _fx_utility_16_unsigned_read(&boot_sector[FX_BYTES_SECTOR]);
    if (media_ptr -> fx_media_bytes_per_sector == 0)
        return(FX_MEDIA_INVALID);


    /* FAT12/16/32 volume.  */
    /* Extract the number of sectors per track.  */
    media_ptr -> fx_media_sectors_per_track =   _fx_utility_16_unsigned_read(&boot_sector[FX_SECTORS_PER_TRK]);

    /* Extract the number of heads.  */
    media_ptr -> fx_media_heads =               _fx_utility_16_unsigned_read(&boot_sector[FX_HEADS]);

    /* Extract the total number of sectors.  */
    media_ptr -> fx_media_total_sectors =       _fx_utility_16_unsigned_read(&boot_sector[FX_SECTORS]);
    if (media_ptr -> fx_media_total_sectors == 0)
    {
        media_ptr -> fx_media_total_sectors = _fx_utility_32_unsigned_read(&boot_sector[FX_HUGE_SECTORS]);
    }

    if (media_ptr -> fx_media_total_sectors == 0)
    {
        return(FX_MEDIA_INVALID);
    }

    /* Extract the number of reserved sectors before the first FAT.  */
    media_ptr -> fx_media_reserved_sectors =    _fx_utility_16_unsigned_read(&boot_sector[FX_RESERVED_SECTORS]);
    if (media_ptr -> fx_media_reserved_sectors == 0)
    {
        return(FX_MEDIA_INVALID);
    }

    /* Extract the number of sectors per cluster.  */
    media_ptr -> fx_media_sectors_per_cluster = ((UINT)boot_sector[FX_SECTORS_CLUSTER] & 0xFF);

    /* There should always be at least one reserved sector, representing the boot record itself.  */
    if (media_ptr -> fx_media_sectors_per_cluster == 0)
    {
        return(FX_MEDIA_INVALID);
    }

    /* Extract the number of sectors per FAT.  */
    media_ptr -> fx_media_sectors_per_FAT =     _fx_utility_16_unsigned_read(&boot_sector[FX_SECTORS_PER_FAT]);
    if (media_ptr -> fx_media_sectors_per_FAT == 0)
    {
        media_ptr -> fx_media_sectors_per_FAT = _fx_utility_32_unsigned_read(&boot_sector[FX_SECTORS_PER_FAT_32]);
    }

    if (media_ptr -> fx_media_sectors_per_FAT == 0)
    {
        return(FX_MEDIA_INVALID);
    }

    /* Extract the number of FATs.  */
    media_ptr -> fx_media_number_of_FATs =      ((UINT)boot_sector[FX_NUMBER_OF_FATS] & 0xFF);
    if (media_ptr -> fx_media_number_of_FATs == 0)
    {
        return(FX_BOOT_ERROR);
    }

    /* Extract the number of hidden sectors.  */
#ifdef FX_DRIVER_USE_64BIT_LBA
    media_ptr -> fx_media_hidden_sectors =      _fx_utility_64_unsigned_read(&boot_sector[FX_HIDDEN_SECTORS]);
#else
    media_ptr -> fx_media_hidden_sectors =      _fx_utility_32_unsigned_read(&boot_sector[FX_HIDDEN_SECTORS]);
#endif
    /* Extract the number of root directory entries.  */
    media_ptr -> fx_media_root_directory_entries =  _fx_utility_16_unsigned_read(&boot_sector[FX_ROOT_DIR_ENTRIES]);

    /* Extract root directory starting cluster (32 bit only) and compute start sector */
    media_ptr -> fx_media_root_cluster_32 = _fx_utility_32_unsigned_read(&boot_sector[FX_ROOT_CLUSTER_32]);


    /* Return a successful status.  */
    return(FX_SUCCESS);
}

