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
/**   Prolific Class                                                      */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_prolific.h"
#include "ux_host_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_prolific_command                     PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will send a command to the Prolific device.           */
/*    The command can be one of the following :                           */
/*    SET_CONTROL                                                         */
/*    SET_LINE                                                            */
/*    SEND_BREAK                                                          */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prolific                              Pointer to prolific class     */
/*    command                               command value                 */
/*    value                                 value to be sent in the       */
/*                                          command request               */
/*    data_buffer                           buffer to be sent             */
/*    data_length                           length of the buffer to send  */
/*                                                                        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_stack_transfer_request       Process transfer request      */
/*    _ux_host_semaphore_get                Get semaphore                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            refined macros names,       */
/*                                            resulting in version 6.1.10 */
/*  10-31-2023     Yajun xia                Modified comment(s),          */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_prolific_command(UX_HOST_CLASS_PROLIFIC *prolific, ULONG command,
                                    ULONG value, UCHAR *data_buffer, ULONG data_length)
{

UX_ENDPOINT     *control_endpoint;
UX_TRANSFER     *transfer_request;
UINT            status;


    /* We need to get the default control endpoint transfer request pointer.  */
    control_endpoint =  &prolific -> ux_host_class_prolific_device -> ux_device_control_endpoint;
    transfer_request =  &control_endpoint -> ux_endpoint_transfer_request;

    /* Protect the control endpoint semaphore here.  It will be unprotected in the
       transfer request function.  */
    status =  _ux_host_semaphore_get(&prolific -> ux_host_class_prolific_device -> ux_device_protection_semaphore, UX_WAIT_FOREVER);

    /* Check for status.  */
    if (status != UX_SUCCESS)

        /* Something went wrong. */
        return(status);

    /* Create a transfer_request for the request.  */
    transfer_request -> ux_transfer_request_data_pointer     =  data_buffer;
    transfer_request -> ux_transfer_request_requested_length =  data_length;
    transfer_request -> ux_transfer_request_function         =  command;
    transfer_request -> ux_transfer_request_type             =  UX_REQUEST_OUT | UX_REQUEST_TYPE_CLASS | UX_REQUEST_TARGET_INTERFACE;
    transfer_request -> ux_transfer_request_value            =  value;
    transfer_request -> ux_transfer_request_index            =  prolific -> ux_host_class_prolific_interface -> ux_interface_descriptor.bInterfaceNumber;

    /* Send request to HCD layer.  */
    status =  _ux_host_stack_transfer_request(transfer_request);

    /* Return completion status.  */
    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_prolific_command                     PORTABLE C     */
/*                                                            6.3.0       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in PROLIFIC command function call.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prolific                              Pointer to prolific class     */
/*    command                               command value                 */
/*    value                                 value to be sent in the       */
/*                                          command request               */
/*    data_buffer                           buffer to be sent             */
/*    data_length                           length of the buffer to send  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_prolific_command        PROLIFIC command             */
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
UINT  _uxe_host_class_prolific_command(UX_HOST_CLASS_PROLIFIC *prolific, ULONG command,
                                    ULONG value, UCHAR *data_buffer, ULONG data_length)
{

    /* Sanity check.  */
    if (prolific == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Invoke PROLIFIC command function.  */
    return(_ux_host_class_prolific_command(prolific, command, value, data_buffer, data_length));
}
