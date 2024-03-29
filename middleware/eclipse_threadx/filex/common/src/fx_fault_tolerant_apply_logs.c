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
/**   Fault Tolerant                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define FX_SOURCE_CODE

#include "fx_api.h"
#include "fx_utility.h"
#include "fx_fault_tolerant.h"


#ifdef FX_ENABLE_FAULT_TOLERANT
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _fx_fault_tolerant_apply_logs                       PORTABLE C      */
/*                                                           6.2.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function applies changes to the file system.  The changes are  */
/*    already recorded in the fault tolerant log file.  Therefore this    */
/*    function reads the content of the log entries and apply these       */
/*    changes to the file system.                                         */
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
/*    _fx_utility_logical_sector_write      Write a logical sector        */
/*    _fx_utility_logical_sector_read       Read a logical sector         */
/*    _fx_utility_16_unsigned_read          Read a USHORT from memory     */
/*    _fx_utility_32_unsigned_read          Read a ULONG from memory      */
/*    _fx_utility_64_unsigned_read          Read a ULONG64 from memory    */
/*    _fx_utility_FAT_entry_write           Write a FAT entry             */
/*    _fx_fault_tolerant_cleanup_FAT_chain  Cleanup FAT chain             */
/*    memcpy                                Memory Copy                   */
/*    _fx_utility_FAT_flush                 Flush written FAT entries     */
/*    _fx_utility_logical_sector_flush      Flush written logical sectors */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _fx_fault_tolerant_enable                                           */
/*    _fx_fault_tolerant_transaction_end                                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     William E. Lamie         Modified comment(s), verified */
/*                                            memcpy usage,               */
/*                                            resulting in version 6.1    */
/*  10-31-2022     Tiejun Zhou              Modified comment(s), fixed    */
/*                                            overflow in log size check, */
/*                                            resulting in version 6.2.0  */
/*                                                                        */
/**************************************************************************/
UINT _fx_fault_tolerant_apply_logs(FX_MEDIA *media_ptr)
{
UINT                           status;
ULONG64                        log_sector;
ULONG                          remaining_logs;
ULONG                          copy_size;
ULONG                          copy_offset;
UCHAR                         *current_ptr;
ULONG                          size;
USHORT                         log_type;
ULONG                          log_len;
FX_FAULT_TOLERANT_LOG_HEADER  *log_header;
FX_FAULT_TOLERANT_FAT_CHAIN   *FAT_chain;
FX_FAULT_TOLERANT_LOG_CONTENT *log_content;
FX_FAULT_TOLERANT_FAT_LOG     *fat_log;
FX_FAULT_TOLERANT_DIR_LOG     *dir_log;

    /* Set log header, FAT chain and log content pointer. */
    log_header = (FX_FAULT_TOLERANT_LOG_HEADER *)media_ptr -> fx_media_fault_tolerant_memory_buffer;
    FAT_chain = (FX_FAULT_TOLERANT_FAT_CHAIN *)(media_ptr -> fx_media_fault_tolerant_memory_buffer + FX_FAULT_TOLERANT_FAT_CHAIN_OFFSET);
    log_content = (FX_FAULT_TOLERANT_LOG_CONTENT *)(media_ptr -> fx_media_fault_tolerant_memory_buffer +
                                                    FX_FAULT_TOLERANT_LOG_CONTENT_OFFSET);

    /* Find the size of the log file. */
    size = _fx_utility_16_unsigned_read((UCHAR *)&log_header -> fx_fault_tolerant_log_header_total_size);

    /* Extended port-specific processing macro, which is by default defined to white space.  */
    FX_FAULT_TOLERANT_APPLY_LOGS_EXTENSION

    size -= FX_FAULT_TOLERANT_LOG_CONTENT_HEADER_SIZE;

    /* Find the number of log entries. */
    remaining_logs = _fx_utility_16_unsigned_read((UCHAR *)&log_content -> fx_fault_tolerant_log_content_count);

    /* Get start address of logs. */
    current_ptr = (UCHAR *)log_content + FX_FAULT_TOLERANT_LOG_CONTENT_HEADER_SIZE;

    /* Loop through all the logs to apply the changes. */
    while (remaining_logs)
    {

        /* Obtain log type of this entry. */
        log_type = (USHORT)_fx_utility_16_unsigned_read(current_ptr);

        /* Read log length.  */
        log_len = _fx_utility_16_unsigned_read(current_ptr + 2);


        /* Validate log entry size. */
        if (log_len > size)
        {

            /* Something wrong with log file. */
            return(FX_FILE_CORRUPT);
        }

        /* Reduce the total log file size. */
        size -= log_len;

        /* Process log entry by type. */
        switch (log_type)
        {
        case FX_FAULT_TOLERANT_FAT_LOG_TYPE:

            /* This is a FAT log. */
            fat_log = (FX_FAULT_TOLERANT_FAT_LOG *)current_ptr;

            /* Write FAT entry. */
            status = _fx_utility_FAT_entry_write(media_ptr,
                                                 _fx_utility_32_unsigned_read((UCHAR *)&fat_log -> fx_fault_tolerant_FAT_log_cluster),
                                                 _fx_utility_32_unsigned_read((UCHAR *)&fat_log -> fx_fault_tolerant_FAT_log_value));

            if (status != FX_SUCCESS)
            {

                /* Return the error status.  */
                return(status);
            }
            break;

        case FX_FAULT_TOLERANT_DIR_LOG_TYPE:

            /* This is a DIR log. */
            dir_log = (FX_FAULT_TOLERANT_DIR_LOG *)current_ptr;

            log_sector = _fx_utility_64_unsigned_read((UCHAR *)&dir_log -> fx_fault_tolerant_dir_log_sector);

            /* Get the destination sector. */
            status =  _fx_utility_logical_sector_read(media_ptr,
                                                      log_sector,
                                                      media_ptr -> fx_media_memory_buffer,
                                                      ((ULONG) 1), FX_DATA_SECTOR);

            if (status != FX_SUCCESS)
            {

                /* Return the error status.  */
                return(status);
            }

            /* Set copy information. */
            copy_offset = _fx_utility_32_unsigned_read((UCHAR *)&dir_log -> fx_fault_tolerant_dir_log_offset);


            copy_size = log_len - FX_FAULT_TOLERANT_DIR_LOG_ENTRY_SIZE;

            if (((ULONG64)copy_offset + (ULONG64)copy_size) > (ULONG64)(media_ptr -> fx_media_memory_size))
            {
                return(FX_FILE_CORRUPT);
            }

            /* Copy data into destination sector. */
            memcpy(media_ptr -> fx_media_memory_buffer + copy_offset, /* Use case of memcpy is verified. */
                   current_ptr + FX_FAULT_TOLERANT_DIR_LOG_ENTRY_SIZE, copy_size);


            /* Write back the current logical sector.  */
            status = _fx_utility_logical_sector_write(media_ptr,
                                                      log_sector,
                                                      media_ptr -> fx_media_memory_buffer,
                                                      ((ULONG) 1), FX_DIRECTORY_SECTOR);

            if (status != FX_SUCCESS)
            {

                /* Return the error status.  */
                return(status);
            }
            break;

        default:

            /* Wrong type.  */
            return(FX_SECTOR_INVALID);
        }

        /* Decrement the remaining log counter.  */
        remaining_logs--;

        /* Move to start position of next log entry. */
        current_ptr += log_len;
    }

    /* Check whether or not to process FAT chain. */
    if (FAT_chain -> fx_fault_tolerant_FAT_chain_flag & FX_FAULT_TOLERANT_FLAG_FAT_CHAIN_VALID)
    {

        /* Free old link of FAT. */
        status = _fx_fault_tolerant_cleanup_FAT_chain(media_ptr, FX_FAULT_TOLERANT_FAT_CHAIN_CLEANUP);
        if (status != FX_SUCCESS)
        {

            /* Return the error status.  */
            return(status);
        }
    }

    /* Flush the internal logical sector cache.  */
    status =  _fx_utility_logical_sector_flush(media_ptr, 1, (ULONG64) media_ptr -> fx_media_total_sectors, FX_FALSE);

    /* Check for a bad status.  */
    if (status != FX_SUCCESS)
    {

        /* Return the bad status.  */
        return(status);
    }

    /* Flush FAT table. */
#ifdef FX_FAULT_TOLERANT

    /* Ensure the new FAT chain is properly written to the media.  */

    /* Flush the cached individual FAT entries */
    _fx_utility_FAT_flush(media_ptr);
#endif

    /* Return success. */
    return(FX_SUCCESS);
}
#endif /* FX_ENABLE_FAULT_TOLERANT */

