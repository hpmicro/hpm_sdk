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
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Device CDC Class                                                    */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_pima.h"
#include "ux_device_stack.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_device_class_pima_activate                      PORTABLE C      */ 
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function activates the USB Pima device.                        */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    command                              Pointer to pima command        */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_device_thread_resume              Resume thread                 */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    USBX Source Code                                                    */ 
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
/*                                            added variables initialize, */
/*                                            resulting in version 6.1.10 */
/*  04-25-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            fixed standalone compile,   */
/*                                            resulting in version 6.1.11 */
/*  07-29-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            fixed parameter/variable    */
/*                                            names conflict C++ keyword, */
/*                                            resulting in version 6.1.12 */
/*  10-31-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            supported optional INT EP,  */
/*                                            added a new mode to manage  */
/*                                            endpoint buffer in classes, */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
UINT  _ux_device_class_pima_activate(UX_SLAVE_CLASS_COMMAND *command)
{
                                          
UX_SLAVE_INTERFACE                      *interface_ptr;
UX_SLAVE_CLASS                          *class_ptr;
UX_SLAVE_CLASS_PIMA                     *pima;
UX_SLAVE_ENDPOINT                       *endpoint_in;
UX_SLAVE_ENDPOINT                       *endpoint_out;
UX_SLAVE_ENDPOINT                       *endpoint_interrupt;


    /* Get the class container.  */
    class_ptr =  command -> ux_slave_class_command_class_ptr;

    /* Store the class instance in the container.  */
    pima = (UX_SLAVE_CLASS_PIMA *) class_ptr -> ux_slave_class_instance;

    /* Get the interface that owns this instance.  */
    interface_ptr =  (UX_SLAVE_INTERFACE  *) command -> ux_slave_class_command_interface;
    
    /* Store the class instance into the interface.  */
    interface_ptr -> ux_slave_interface_class_instance =  (VOID *)pima;
         
    /* Now the opposite, store the interface in the class instance.  */
    pima -> ux_slave_class_pima_interface =  interface_ptr;
    
    /* Locate the endpoints.  */
    endpoint_in =  interface_ptr -> ux_slave_interface_first_endpoint;
    
    /* Check the endpoint direction, if IN we have the correct endpoint.  */
    if ((endpoint_in -> ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) == UX_ENDPOINT_OUT)
    {

        /* Wrong direction, we found the OUT endpoint first.  */
        endpoint_out =  endpoint_in;
            
        /* So the next endpoint has to be the IN endpoint.  */
        endpoint_in =  endpoint_out -> ux_slave_endpoint_next_endpoint;
        
        /* And the endpoint after that interrupt.  */
        endpoint_interrupt =  endpoint_in -> ux_slave_endpoint_next_endpoint;
        
    }
    else
    {

        /* We found the endpoint IN first, so next endpoint is OUT.  */
        endpoint_out =  endpoint_in -> ux_slave_endpoint_next_endpoint;

        /* And the endpoint after that interrupt.  */
        endpoint_interrupt =  endpoint_out -> ux_slave_endpoint_next_endpoint;
    }

    /* Save the endpoints in the pima instance.  */
    pima -> ux_device_class_pima_bulk_in_endpoint           = endpoint_in;
    pima -> ux_device_class_pima_bulk_out_endpoint          = endpoint_out;
    pima -> ux_device_class_pima_interrupt_endpoint         = endpoint_interrupt;

#if UX_DEVICE_ENDPOINT_BUFFER_OWNER == 1
    endpoint_in -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer =
                                    UX_DEVICE_CLASS_PIMA_BULKIN_BUFFER(pima);
    endpoint_out -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer =
                                    UX_DEVICE_CLASS_PIMA_BULKOUT_BUFFER(pima);
    if (endpoint_interrupt)
        endpoint_interrupt -> ux_slave_endpoint_transfer_request.
                                    ux_slave_transfer_request_data_pointer =
                                    UX_DEVICE_CLASS_PIMA_INTERRUPTIN_BUFFER(pima);
#endif

    /* Initialize status code.  */
    pima -> ux_device_class_pima_state = UX_DEVICE_CLASS_PIMA_PHASE_IDLE;
    pima -> ux_device_class_pima_session_id = 0;
    pima -> ux_device_class_pima_device_status = UX_DEVICE_CLASS_PIMA_RC_OK;

    /* Resume thread.  */
    _ux_device_thread_resume(&class_ptr -> ux_slave_class_thread); 
    
    /* If there is a activate function call it.  */
    if (pima -> ux_device_class_pima_instance_activate != UX_NULL)
    {        
        /* Invoke the application.  */
        pima -> ux_device_class_pima_instance_activate(pima);
    }
    
    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_CLASS_PIMA_ACTIVATE, pima, 0, 0, 0, UX_TRACE_DEVICE_CLASS_EVENTS, 0, 0)

    /* If trace is enabled, register this object.  */
    UX_TRACE_OBJECT_REGISTER(UX_TRACE_DEVICE_OBJECT_TYPE_INTERFACE, pima, 0, 0, 0)

    /* Return completion status.  */
    return(UX_SUCCESS);
}
