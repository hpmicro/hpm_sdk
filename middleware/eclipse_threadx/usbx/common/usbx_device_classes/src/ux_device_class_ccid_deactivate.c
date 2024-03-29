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
/**   Device CCID Class                                                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_ccid.h"
#include "ux_device_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_device_class_ccid_deactivate                    PORTABLE C      */
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deactivate an instance of the ccid class.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    command                               Pointer to a class command    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_device_stack_transfer_all_request_abort                         */
/*                                          Abort all transfers           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    CCID Class                                                          */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  04-25-2022     Chaoqiong Xiao           Initial Version 6.1.11        */
/*                                                                        */
/**************************************************************************/
UINT  _ux_device_class_ccid_deactivate(UX_SLAVE_CLASS_COMMAND *command)
{

UX_DEVICE_CLASS_CCID        *ccid;
UX_SLAVE_CLASS              *ccid_class;
UX_SLAVE_ENDPOINT           *endpoint;

    /* Get the class container.  */
    ccid_class =  command -> ux_slave_class_command_class_ptr;

    /* Get the class instance in the container.  */
    ccid = (UX_DEVICE_CLASS_CCID *) ccid_class -> ux_slave_class_instance;

    /* Terminate the transactions pending on the endpoints.  */
    endpoint = ccid -> ux_device_class_ccid_endpoint_out;
    _ux_device_stack_transfer_all_request_abort(endpoint, UX_TRANSFER_BUS_RESET);

    endpoint = ccid -> ux_device_class_ccid_endpoint_in;
    _ux_device_stack_transfer_all_request_abort(endpoint, UX_TRANSFER_BUS_RESET);

    endpoint = ccid -> ux_device_class_ccid_endpoint_notify;
    if (endpoint)
        _ux_device_stack_transfer_all_request_abort(endpoint, UX_TRANSFER_BUS_RESET);

    /* If there is a deactivate function call it.  */
    if (ccid -> ux_device_class_ccid_parameter.ux_device_class_ccid_instance_deactivate != UX_NULL)
    {

        /* Invoke the application.  */
        ccid -> ux_device_class_ccid_parameter.ux_device_class_ccid_instance_deactivate(ccid);
    }

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_CLASS_CCID_DEACTIVATE, ccid, 0, 0, 0, UX_TRACE_DEVICE_CLASS_EVENTS, 0, 0)

    /* If trace is enabled, register this object.  */
    UX_TRACE_OBJECT_UNREGISTER(ccid);

    /* Return completion status.  */
    return(UX_SUCCESS);
}
