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
#include "fx_directory.h"
#include "fx_fault_tolerant.h"


#ifdef FX_ENABLE_FAULT_TOLERANT
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _fx_fault_tolerant_create_log_file                  PORTABLE C      */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function finds an available cluster used by log file. Then it  */
/*    sets the cluster number to boot sector.                             */
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
/*    _fx_utility_FAT_entry_read            Read a FAT entry              */
/*    _fx_utility_FAT_entry_write           Write a FAT entry             */
/*    _fx_utility_FAT_flush                 Flush written FAT entries     */
/*    _fx_utility_32_unsigned_write         Write a ULONG from memory     */
/*    _fx_utility_logical_sector_read       Read a logical sector         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _fx_fault_tolerant_enable                                           */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  01-31-2022     Bhupendra Naphade        Modified comment(s), replaced */
/*                                            sector size constant,       */
/*                                            resulting in version 6.1.10 */
/*  04-25-2022     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 6.1.11 */
/*                                                                        */
/**************************************************************************/
UINT _fx_fault_tolerant_create_log_file(FX_MEDIA *media_ptr)
{
ULONG clusters;
ULONG FAT_index;
ULONG FAT_value = 0;
UINT  found;
UINT  status;
UINT  i;

    /* Yes. Create a log file. */
    /* First find a free cluster. */
    if (media_ptr -> fx_media_available_clusters < media_ptr -> fx_media_fault_tolerant_clusters)
    {

        /* Out of disk space.  */
        return(FX_NO_MORE_SPACE);
    }

    /* Now we need to find the consecutive clusters.  */
    clusters = media_ptr -> fx_media_fault_tolerant_clusters;
    FAT_index =  FX_FAT_ENTRY_START;
    found =  FX_FALSE;
    while (FAT_index <= (media_ptr -> fx_media_total_clusters - clusters + FX_FAT_ENTRY_START))
    {

        /* Determine if enough consecutive FAT entries are available.  */
        i =  0;


        do
        {

            /* Read a FAT entry.  */
            status =  _fx_utility_FAT_entry_read(media_ptr, (FAT_index + i), &FAT_value);

            /* Check for a successful status.  */
            if (status != FX_SUCCESS)
            {

                /* Return the error status.  */
                return(status);
            }

            /* Determine if the entry is free.  */
            if (FAT_value != FX_FREE_CLUSTER)
            {
                break;
            }

            /* Otherwise, increment the consecutive FAT indices.  */
            i++;
        } while (i < clusters);

        /* Determine if we found enough FAT entries.  */
        if (i >= clusters)
        {

            /* Yes, we have found enough FAT entries - set the found
               flag and get out of this loop.  */
            found =  FX_TRUE;
            break;
        }
        else
        {
            /* Position to the next possibly free FAT entry.  */
            FAT_index =  FAT_index + i + 1;
        }
    }

    /* Determine if we found enough consecutive clusters to satisfy the
       request.  */
    if (!found)
    {

        /* Not enough contiguous space on the media.  Return error status.  */
        return(FX_NO_MORE_SPACE);
    }

    /* Update the link pointers in the new clusters.  */
    for (i = 0; i < (clusters - 1); i++)
    {

        /* Update the cluster links.  Since the allocation is
           sequential, we just have to link each FAT entry to the
           next one.  */
        status =  _fx_utility_FAT_entry_write(media_ptr, FAT_index + i, FAT_index + i + 1);

        /* Check for a bad status.  */
        if (status != FX_SUCCESS)
        {

            /* Return the error status.  */
            return(status);
        }

    }

    /* Now place an EOF in the last cluster entry.  */
    status =  _fx_utility_FAT_entry_write(media_ptr, FAT_index + clusters - 1, media_ptr -> fx_media_fat_last);

    /* Check for a bad status.  */
    if (status != FX_SUCCESS)
    {

        /* Return the error status.  */
        return(status);
    }


#ifdef FX_FAULT_TOLERANT

    /* Flush the cached individual FAT entries */
    _fx_utility_FAT_flush(media_ptr);

#endif /* FX_FAULT_TOLERANT */

    /* Write start cluster for the file tolerant log file into the boot sector. */
    _fx_utility_32_unsigned_write(media_ptr -> fx_media_fault_tolerant_memory_buffer + FX_FAULT_TOLERANT_BOOT_INDEX, FAT_index);

    /* Write the boot sector.  */
    media_ptr -> fx_media_driver_request =       FX_DRIVER_BOOT_WRITE;
    media_ptr -> fx_media_driver_status =        FX_IO_ERROR;
    media_ptr -> fx_media_driver_buffer =        media_ptr -> fx_media_fault_tolerant_memory_buffer;
    media_ptr -> fx_media_driver_sectors =       1;
    media_ptr -> fx_media_driver_sector_type =   FX_BOOT_SECTOR;

    /* If trace is enabled, insert this event into the trace buffer.  */
    FX_TRACE_IN_LINE_INSERT(FX_TRACE_INTERNAL_IO_DRIVER_BOOT_WRITE, media_ptr, media_ptr -> fx_media_fault_tolerant_memory_buffer, 0, 0, FX_TRACE_INTERNAL_EVENTS, 0, 0)

    /* Invoke the driver to read the boot sector.  */
    (media_ptr -> fx_media_driver_entry) (media_ptr);

    /* Determine if the boot sector was read correctly. */
    if (media_ptr -> fx_media_driver_status != FX_SUCCESS)
    {

        /* Return the boot sector error status.  */
        return(FX_BOOT_ERROR);
    }


    /* Set the start cluster. */
    media_ptr -> fx_media_fault_tolerant_start_cluster = FAT_index;

    /* Decrease available cluster. */
    media_ptr -> fx_media_available_clusters =
        media_ptr -> fx_media_available_clusters - clusters;

    /* Return success.  */
    return(FX_SUCCESS);
}
#endif /* FX_ENABLE_FAULT_TOLERANT */

