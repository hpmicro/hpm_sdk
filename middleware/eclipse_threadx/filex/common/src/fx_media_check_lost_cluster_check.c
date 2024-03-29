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
#include "fx_media.h"
#include "fx_utility.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _fx_media_check_lost_cluster_check                  PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function examines all clusters to see if there are any unused  */
/*    clusters that are also unavailable. If specified, this routine will */
/*    also mark the cluster as available.                                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    media_ptr                             Pointer to a previously       */
/*                                            opened media                */
/*    logical_fat                           Pointer to the logical FAT    */
/*                                            bit map                     */
/*    total_clusters                        Total number of clusters      */
/*    error_correction_option               Option for correcting lost    */
/*                                            cluster errors              */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    error                                 Error code                    */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _fx_utility_FAT_entry_read            Read a FAT entry              */
/*    _fx_utility_FAT_entry_write           Write a FAT entry             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _fx_check_media                       Check media function          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
ULONG  _fx_media_check_lost_cluster_check(FX_MEDIA *media_ptr, UCHAR *logical_fat, ULONG total_clusters, ULONG error_correction_option)
{

ULONG cluster, next_cluster = 0;
ULONG fat_last;
ULONG error;
UINT  status;


    /* Calculate the FAT reserved and last sector values.  */
    if (media_ptr -> fx_media_32_bit_FAT)
    {
        fat_last =      FX_LAST_CLUSTER_1_32;
    }
    else
    {
        fat_last =      FX_LAST_CLUSTER_1;
    }

    /* Initialize the error.  */
    error =  0;

    /* Loop through all the clusters to see if any clusters NOT in the logical sector FAT have
       a non zero value.  */
    for (cluster = FX_FAT_ENTRY_START; cluster < total_clusters; cluster++)
    {

        /* Determine if this cluster is in the logical FAT.  */
        if (logical_fat[cluster >> 3] & (1 << (cluster & 7)))
        {

            /* Yes, the cluster is in use by a file or sub-directory.  Just continue the loop.  */
            continue;
        }

        /* Otherwise, the cluster is not in use.  */

        /* Read the contents of what should be a free cluster.  */
        status = _fx_utility_FAT_entry_read(media_ptr, cluster, &next_cluster);

        /* Check for a good status.  */
        if (status)
        {

            /* Set the error code.  */
            error = error | FX_IO_ERROR;

            /* Return error code.  */
            return(error);
        }

        /* Determine if the contents of the cluster is valid.  */
        if (((next_cluster > (ULONG)FX_FREE_CLUSTER) && (next_cluster < media_ptr -> fx_media_fat_reserved)) ||
            (next_cluster >= fat_last))
        {

            /* Lost cluster is present.  */

            /* Set the error code status.  */
            error =  FX_LOST_CLUSTER_ERROR;

            /* Determine if the lost cluster should be recovered.  */
            if (error_correction_option & FX_LOST_CLUSTER_ERROR)
            {

                /* Make the cluster available again.  */
                status =  _fx_utility_FAT_entry_write(media_ptr, cluster, FX_FREE_CLUSTER);

                /* Check for a good status.  */
                if (status)
                {

                    /* Increment the available clusters.  */
                    media_ptr -> fx_media_available_clusters++;

                    /* Set the error code.  */
                    error =  error | FX_IO_ERROR;

                    /* Return error code.  */
                    return(error);
                }
            }
        }
    }

    /* Return error code.  */
    return(error);
}

