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
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_storage_lock                         PORTABLE C      */
/*                                                           6.2.1        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function locks storage for further operations.                 */
/*                                                                        */
/*    It's valid only in standalone mode.                                 */
/*    It's non-blocking if wait is zero, otherwise it blocks.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    storage_media                         Pointer to storage to operate */
/*    wait                                  Wait option                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*    UX_TIMEOUT                            Time out                      */
/*    UX_REENTRY                            Reentry in storage task       */
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
/*  01-31-2022     Chaoqiong Xiao           Initial Version 6.1.10        */
/*  03-08-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            checked device state,       */
/*                                            resulting in version 6.2.1  */
/*                                                                        */
/**************************************************************************/
UINT    _ux_host_class_storage_lock(UX_HOST_CLASS_STORAGE *storage, ULONG wait)
{
UX_INTERRUPT_SAVE_AREA
ULONG           t0, t1;
UX_DEVICE       *device;

    /* Get device.  */
    device = storage -> ux_host_class_storage_device;

    t0 = _ux_utility_time_get();
    while(1)
    {
        UX_DISABLE

        /* Check instance lock.  */
        if (storage -> ux_host_class_storage_flags & UX_HOST_CLASS_STORAGE_FLAG_LOCK)
        {

            /* Check once, just return busy.  */
            if (wait == 0)
            {
                UX_RESTORE
                return(UX_TIMEOUT);
            }

            /* Check timeout.  */
            if (wait != UX_WAIT_FOREVER)
            {
                t1 = _ux_utility_time_get();
                t1 = _ux_utility_time_elapsed(t0, t1);
                if (t1 >= wait)
                {
                    UX_RESTORE
                    return(UX_TIMEOUT);
                }
            }
        }
        else
        {
            /* It's free, time to lock it.  */
            break;
        }

        /* Check task reentry.  */
        if (storage -> ux_host_class_storage_flags & UX_HOST_CLASS_STORAGE_FLAG_PROTECT)
        {
            UX_RESTORE

            /* Error trap. */
            _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_REENTRY);

            /* If trace is enabled, insert this event into the trace buffer.  */
            UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_REENTRY, storage, 0, 0, UX_TRACE_ERRORS, 0, 0)

            return(UX_REENTRY);
        }
        UX_RESTORE

        /* Run stack tasks.  */
        _ux_system_host_tasks_run();

        /* Check if device is still available.  */
        if (device -> ux_device_state != UX_DEVICE_CONFIGURED)
        {

            /* Instance should have been destroyed, just return.  */
            return(UX_STATE_EXIT);
        }
    }

    /* Lock storage.  */
    storage -> ux_host_class_storage_flags |= UX_HOST_CLASS_STORAGE_FLAG_LOCK;

    /* Stop main state machine.  */
    storage -> ux_host_class_storage_state_state = UX_STATE_IDLE;

    /* Reset operation state machine.  */
    storage -> ux_host_class_storage_op_state = UX_STATE_RESET;

    UX_RESTORE
    return(UX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_storage_lock                         PORTABLE C     */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in storage lock function call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    storage                               Pointer to storage to operate */
/*    wait                                  Wait option                   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_storage_lock           Lock storage                  */
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
UINT    _uxe_host_class_storage_lock(UX_HOST_CLASS_STORAGE *storage, ULONG wait)
{

    /* Sanity check.  */
    if (storage == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Invoke storage lock function.  */
    return(_ux_host_class_storage_lock(storage, wait));
}
#endif
