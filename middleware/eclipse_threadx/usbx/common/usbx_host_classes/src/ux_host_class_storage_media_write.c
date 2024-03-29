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


#if defined(UX_HOST_STANDALONE)
VOID _ux_host_class_storage_write_initialize(UX_HOST_CLASS_STORAGE *storage,
                ULONG sector_start, ULONG sector_count);

VOID
#else
static inline VOID
#endif
_ux_host_class_storage_write_initialize(UX_HOST_CLASS_STORAGE *storage,
                ULONG sector_start, ULONG sector_count)
{
UCHAR       *cbw;
UCHAR       *cbw_cb;
ULONG       command_length;

    /* Use a pointer for the cbw, easier to manipulate.  */
    cbw = (UCHAR *)storage -> ux_host_class_storage_cbw;
    cbw_cb = cbw + UX_HOST_CLASS_STORAGE_CBW_CB;

    /* Get the Write Command Length.  */
#ifdef UX_HOST_CLASS_STORAGE_INCLUDE_LEGACY_PROTOCOL_SUPPORT
    if (storage -> ux_host_class_storage_interface -> ux_interface_descriptor.bInterfaceSubClass ==
        UX_HOST_CLASS_STORAGE_SUBCLASS_UFI)
        command_length =  UX_HOST_CLASS_STORAGE_WRITE_COMMAND_LENGTH_UFI;
    else
        command_length =  UX_HOST_CLASS_STORAGE_WRITE_COMMAND_LENGTH_SBC;
#else
    command_length =  UX_HOST_CLASS_STORAGE_WRITE_COMMAND_LENGTH_SBC;
#endif

    /* Initialize the CBW for this command.  */
    _ux_host_class_storage_cbw_initialize(storage, UX_HOST_CLASS_STORAGE_DATA_OUT,
                                    sector_count * storage -> ux_host_class_storage_sector_size,
                                    command_length);

    /* Prepare the MEDIA WRITE command block.  */
    *(cbw_cb + UX_HOST_CLASS_STORAGE_WRITE_OPERATION) =  UX_HOST_CLASS_STORAGE_SCSI_WRITE16;

    /* Store the sector start (LBA field).  */
    _ux_utility_long_put_big_endian(cbw_cb + UX_HOST_CLASS_STORAGE_WRITE_LBA, sector_start);

    /* Store the number of sectors to write.  */
    _ux_utility_short_put_big_endian(cbw_cb + UX_HOST_CLASS_STORAGE_WRITE_TRANSFER_LENGTH, (USHORT) sector_count);
}


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_host_class_storage_media_write                  PORTABLE C      */ 
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function will write one or more logical sector to the media.   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    storage                               Pointer to storage class      */ 
/*    sector_start                          Starting sector               */ 
/*    sector_count                          Number of sectors to write    */ 
/*    data_pointer                          Pointer to data to write      */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_host_class_storage_cbw_initialize Initialize the CBW            */ 
/*    _ux_host_class_storage_transport      Send command                  */ 
/*    _ux_utility_long_put_big_endian       Put 32-bit word               */ 
/*    _ux_utility_short_put_big_endian      Put 16-bit word               */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Storage Class                                                       */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added standalone support,   */
/*                                            resulting in version 6.1.10 */
/*  03-08-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            checked device removal,     */
/*                                            resulting in version 6.2.1  */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_storage_media_write(UX_HOST_CLASS_STORAGE *storage, ULONG sector_start,
                                        ULONG sector_count, UCHAR *data_pointer)
{
#if defined(UX_HOST_STANDALONE)
UINT            status;
    do {
        status = _ux_host_class_storage_read_write_run(storage, UX_FALSE,
                                    sector_start, sector_count, data_pointer);
    } while(status == UX_STATE_WAIT);
    if (status < UX_STATE_IDLE)
        return(UX_HOST_CLASS_INSTANCE_UNKNOWN);
    return(storage -> ux_host_class_storage_status);
#else
UINT            status;
UINT            media_retry;

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_CLASS_STORAGE_MEDIA_WRITE, storage, sector_start, sector_count, data_pointer, UX_TRACE_HOST_CLASS_EVENTS, 0, 0)

    /* Initialize CBW.  */
    _ux_host_class_storage_write_initialize(storage, sector_start, sector_count);

    /* Reset the retry count.  */
    media_retry =  UX_HOST_CLASS_STORAGE_REQUEST_SENSE_RETRY;

    /* We may need several attempts.  */
    while (media_retry-- != 0)
    {

        /* Send the command to transport layer.  */
        status =  _ux_host_class_storage_transport(storage, data_pointer);
        if (status != UX_SUCCESS)
            return(status);

        /* Check the sense code */
        if (storage -> ux_host_class_storage_sense_code == UX_SUCCESS)
            return(UX_SUCCESS);
    }

    /* Return sense error.  */
    return(UX_HOST_CLASS_STORAGE_SENSE_ERROR);                                            
#endif
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_storage_media_write                  PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in storage media write function call.   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    storage                               Pointer to storage class      */
/*    sector_start                          Starting sector               */
/*    sector_count                          Number of sectors to write    */
/*    data_pointer                          Pointer to data to write      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_storage_media_write     write storage media          */
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
UINT  _uxe_host_class_storage_media_write(UX_HOST_CLASS_STORAGE *storage, ULONG sector_start,
                                    ULONG sector_count, UCHAR *data_pointer)
{

    /* Sanity check.  */
    if (storage == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Invoke storage media write function.  */
    return(_ux_host_class_storage_media_write(storage, sector_start, sector_count, data_pointer));
}
