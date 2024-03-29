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
/**   Generic Serial Host module class                                    */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_gser.h"
#include "ux_host_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_gser_command                         PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will send a command to the ACM device. The command    */
/*    can be one of the following :                                       */
/*    SET_CONTROL                                                         */
/*    SET_LINE                                                            */
/*    SEND_BREAK                                                          */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    acm                                   Pointer to acm class          */
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
UINT  _ux_host_class_gser_command(UX_HOST_CLASS_GSER *gser, ULONG interface_index, ULONG command,
                                    ULONG value, UCHAR *data_buffer, ULONG data_length)
{

UX_ENDPOINT     *control_endpoint;
UX_TRANSFER     *transfer_request;
UINT            status;
ULONG           request_direction;


    /* We need to get the default control endpoint transfer request pointer.  */
    control_endpoint =  &gser -> ux_host_class_gser_device -> ux_device_control_endpoint;
    transfer_request =  &control_endpoint -> ux_endpoint_transfer_request;

    /* Check the direction of the command.  */
    switch (command)
    {

        case UX_HOST_CLASS_GSER_REQ_SEND_ENCAPSULATED_COMMAND        :
        case UX_HOST_CLASS_GSER_REQ_SET_COMM_FEATURE                 :
        case UX_HOST_CLASS_GSER_REQ_CLEAR_COMM_FEATURE               :
        case UX_HOST_CLASS_GSER_REQ_SET_AUX_LINE_STATE               :
        case UX_HOST_CLASS_GSER_REQ_SET_HOOK_STATE                   :
        case UX_HOST_CLASS_GSER_REQ_PULSE_SETUP                      :
        case UX_HOST_CLASS_GSER_REQ_SEND_PULSE                       :
        case UX_HOST_CLASS_GSER_REQ_SET_PUSLE_TIME                   :
        case UX_HOST_CLASS_GSER_REQ_RING_AUX_JACK                    :
        case UX_HOST_CLASS_GSER_REQ_SET_LINE_CODING                  :
        case UX_HOST_CLASS_GSER_REQ_SET_LINE_STATE                   :
        case UX_HOST_CLASS_GSER_REQ_SEND_BREAK                       :
        case UX_HOST_CLASS_GSER_REQ_SET_RINGER_PARMS                 :
        case UX_HOST_CLASS_GSER_REQ_SET_OPERATION_PARMS              :
        case UX_HOST_CLASS_GSER_REQ_SET_LINE_PARMS                   :

            /* Direction is out */
            request_direction = UX_REQUEST_OUT;
            break;


        case UX_HOST_CLASS_GSER_REQ_GET_ENCAPSULATED_COMMAND         :
        case UX_HOST_CLASS_GSER_REQ_GET_COMM_FEATURE                 :
        case UX_HOST_CLASS_GSER_REQ_GET_LINE_CODING                  :
        case UX_HOST_CLASS_GSER_REQ_GET_RINGER_PARMS                 :
        case UX_HOST_CLASS_GSER_REQ_GET_OPERATION_PARMS              :
        case UX_HOST_CLASS_GSER_REQ_GET_LINE_PARMS                   :

            /* Direction is in */
            request_direction = UX_REQUEST_IN;
            break;


        default :

            return(UX_ERROR);

    }

    /* Protect the control endpoint semaphore here.  It will be unprotected in the
       transfer request function.  */
    status =  _ux_host_semaphore_get(&gser -> ux_host_class_gser_device -> ux_device_protection_semaphore, UX_WAIT_FOREVER);

    /* Check for status.  */
    if (status != UX_SUCCESS)

        /* Something went wrong. */
        return(status);

    /* Create a transfer_request for the request.  */
    transfer_request -> ux_transfer_request_data_pointer     =  data_buffer;
    transfer_request -> ux_transfer_request_requested_length =  data_length;
    transfer_request -> ux_transfer_request_function         =  command;
    transfer_request -> ux_transfer_request_type             =  request_direction | UX_REQUEST_TYPE_CLASS | UX_REQUEST_TARGET_INTERFACE;
    transfer_request -> ux_transfer_request_value            =  value;
    transfer_request -> ux_transfer_request_index            =  gser -> ux_host_class_gser_interface_array[interface_index].ux_host_class_gser_interface->ux_interface_descriptor.bInterfaceNumber;

    /* Send request to HCD layer.  */
    status =  _ux_host_stack_transfer_request(transfer_request);

    /* Return completion status.  */
    return(status);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_gser_command                        PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in GSER command function call.          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gser                                  Pointer to GSER class         */
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
/*    _ux_host_class_gser_command           Send GSER request             */
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
UINT  _uxe_host_class_gser_command(UX_HOST_CLASS_GSER *gser, ULONG interface_index, ULONG command,
                                    ULONG value, UCHAR *data_buffer, ULONG data_length)
{

    /* Sanity check.  */
    if (gser == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Invoke GSER command function.  */
    return(_ux_host_class_gser_command(gser, interface_index,
                                    command, value, data_buffer, data_length));
}
