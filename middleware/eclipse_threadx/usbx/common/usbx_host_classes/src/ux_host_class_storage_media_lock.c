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
/** USBX Component                                                        */
/**                                                                       */
/**   Storage Class                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_storage.h"
#include "ux_host_stack.h"


#if defined(UX_HOST_CLASS_STORAGE_NO_FILEX)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_storage_media_lock                   PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function lock storage and select storage media for read/write. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    storage_media                         Pointer to storage media to   */
/*                                          select                        */
/*    wait                                  Wait option                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Storage Class                                                       */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Chaoqiong Xiao           Initial Version 6.1           */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added standalone support,   */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
UINT    _ux_host_class_storage_media_lock(UX_HOST_CLASS_STORAGE_MEDIA *storage_media, ULONG wait)
{
#if !defined(UX_HOST_CLASS_STORAGE_NO_FILEX)
    UX_PARAMETER_NOT_USED(storage_media);
    UX_PARAMETER_NOT_USED(wait);
    return(UX_FUNCTION_NOT_SUPPORTED);
#else

UX_HOST_CLASS_STORAGE           *storage;
UINT                            status;


    /* Get storage instance.  */
    storage = storage_media -> ux_host_class_storage_media_storage;
    if (storage == UX_NULL)
        return(UX_ERROR);

    /* Protect thread reentry to this instance.  */
    status = _ux_host_class_storage_lock(storage, wait);
    if (status != UX_SUCCESS)
        return(status);

    /* Select the media if success.  */
    storage -> ux_host_class_storage_lun = storage_media -> ux_host_class_storage_media_lun;
    storage -> ux_host_class_storage_sector_size = storage_media -> ux_host_class_storage_media_sector_size;
    storage -> ux_host_class_storage_last_sector_number = storage_media -> ux_host_class_storage_media_number_sectors - 1;

    /* Return success.  */
    return(UX_SUCCESS);
#endif
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_storage_media_lock                  PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in storage media lock function call.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    storage_media                         Pointer to storage media to   */
/*                                          select                        */
/*    wait                                  Wait option                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_storage_media_lock     Lock and select storage media */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2023     Chaoqiong Xiao           Initial Version 6.3.0         */
/*                                                                        */
/**************************************************************************/
UINT    _uxe_host_class_storage_media_lock(UX_HOST_CLASS_STORAGE_MEDIA *storage_media, ULONG wait)
{

    /* Sanity check.  */
    if (storage_media == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Invoke storage media lock function.  */
    return(_ux_host_class_storage_media_lock(storage_media, wait));
}
#endif
