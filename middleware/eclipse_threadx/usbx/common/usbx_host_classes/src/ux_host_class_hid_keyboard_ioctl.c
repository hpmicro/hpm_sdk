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
/**   HID Keyboard Client                                                 */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hid_keyboard.h"
#include "ux_host_stack.h"

extern UX_HOST_CLASS_HID_KEYBOARD_LAYOUT ux_host_class_hid_keyboard_layout;

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_hid_keyboard_ioctl                   PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the ioctl entry point for the application to       */
/*    configure the HID keyboard device.                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    keyboard_instance                    Pointer to hid keyboard        */
/*    ioctl_function                       ioctl function                 */
/*    parameter                            pointer to parameter/structure */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    HID Keyboard Client                                                 */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT _ux_host_class_hid_keyboard_ioctl(UX_HOST_CLASS_HID_KEYBOARD *keyboard_instance,
                                        ULONG ioctl_function, VOID *parameter)
{

UINT status = UX_SUCCESS;


    switch(ioctl_function)
    {

    case UX_HID_KEYBOARD_IOCTL_SET_LAYOUT:

        /* Change the keyboard layout setting.  */
        keyboard_instance -> ux_host_class_hid_keyboard_layout = (parameter == UX_NULL) ?
                                                                    &ux_host_class_hid_keyboard_layout :
                                                                    (UX_HOST_CLASS_HID_KEYBOARD_LAYOUT *) parameter;

        break;

    case UX_HID_KEYBOARD_IOCTL_DISABLE_KEYS_DECODE:

        /* Disable the keys decode setting.  */
        keyboard_instance -> ux_host_class_hid_keyboard_keys_decode_disable = UX_TRUE;

        break;

    case UX_HID_KEYBOARD_IOCTL_ENABLE_KEYS_DECODE:

        /* Enable the keys decode setting.  */
        keyboard_instance -> ux_host_class_hid_keyboard_keys_decode_disable = UX_FALSE;

        break;

    default:

        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_FUNCTION_NOT_SUPPORTED);

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_FUNCTION_NOT_SUPPORTED, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)

        /* Function not supported. Return an error.  */
        status =  UX_FUNCTION_NOT_SUPPORTED;

    }

    /* Return status to caller.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_hid_keyboard_ioctl                  PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in HID keyboard ioctl function call.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    keyboard_instance                    Pointer to hid keyboard        */
/*    ioctl_function                       IOCTL function                 */
/*    parameter                            Pointer to parameter/structure */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_hid_keyboard_ioctl    IOCTL function                 */
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
UINT _uxe_host_class_hid_keyboard_ioctl(UX_HOST_CLASS_HID_KEYBOARD *keyboard_instance,
                                        ULONG ioctl_function, VOID *parameter)
{

    /* Sanity checks.  */
    if (keyboard_instance == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Invoke report ID get function.  */
    return(_ux_host_class_hid_keyboard_ioctl(keyboard_instance, ioctl_function, parameter));
}
