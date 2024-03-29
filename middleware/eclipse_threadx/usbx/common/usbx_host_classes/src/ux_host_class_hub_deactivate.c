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
/**   HUB Class                                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_hub.h"
#include "ux_host_stack.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_host_class_hub_deactivate                       PORTABLE C      */ 
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function is called when this instance of the HUB has been      */
/*    removed from the bus either directly or indirectly. The interrupt   */ 
/*    pipe will be destroyed and the instance removed.                    */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    command                               Pointer to class command      */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_host_stack_class_instance_destroy Destroy class instance        */ 
/*    _ux_host_stack_device_remove          Remove device                 */ 
/*    _ux_host_stack_endpoint_transfer_abort                              */
/*                                          Abort transfer                */ 
/*    _ux_utility_memory_free               Release memory block          */ 
/*    _ux_host_semaphore_get                Get semaphore                 */ 
/*    _ux_host_semaphore_put                Release semaphore             */ 
/*    _ux_utility_thread_schedule_other     Schedule other threads        */
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    HUB Class                                                           */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            optimized based on compile  */
/*                                            definitions,                */
/*                                            resulting in version 6.1    */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            refined macros names,       */
/*                                            resulting in version 6.1.10 */
/*  07-29-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added standalone support,   */
/*                                            resulting in version 6.1.12 */
/*  10-31-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            improved deactivate flow,   */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_hub_deactivate(UX_HOST_CLASS_COMMAND *command)
{

UX_HOST_CLASS_HUB       *hub;
UX_HCD                  *hcd;
UX_TRANSFER             *transfer_request = UX_NULL;
UINT                    port_index;


    /* Get the instance to the class.  */
    hub =  (UX_HOST_CLASS_HUB *) command -> ux_host_class_command_instance;

    /* Get the HCD used by this instance.  */
    hcd = UX_DEVICE_HCD_GET(hub -> ux_host_class_hub_device);

    /* The HUB is being shut down.  */
    hub -> ux_host_class_hub_state =  UX_HOST_CLASS_INSTANCE_SHUTDOWN;
    
    /* We need to abort transactions on the interrupt pipe.  */
#if defined(UX_HOST_STANDALONE)
    if (hub -> ux_host_class_hub_interrupt_endpoint)
#endif
    {
        _ux_host_stack_endpoint_transfer_abort(hub -> ux_host_class_hub_interrupt_endpoint);

        /* If the Hub class instance has a interrupt pipe with a data payload associated with it
           it must be freed.  First get the transfer request. */
        transfer_request =  &hub -> ux_host_class_hub_interrupt_endpoint -> ux_endpoint_transfer_request;
    }

    /* Each device which is downstream on the HUB ports must be removed.  */
    for (port_index = 1; port_index <= hub -> ux_host_class_hub_descriptor.bNbPorts; port_index++)
    {

        /* Is there a device on this port?  */
        if (hub -> ux_host_class_hub_port_state & (1UL << port_index))
        {

            /* The stack will remove the device and its resources.  */
            _ux_host_stack_device_remove(hcd, hub -> ux_host_class_hub_device, port_index);
        }
    }


    /* The enumeration thread needs to sleep a while to allow the application or the class that may be using
       endpoints to exit properly.  */
    _ux_host_thread_schedule_other(UX_THREAD_PRIORITY_ENUM); 

    /* Then de allocate the memory.  */
#if defined(UX_HOST_STANDALONE)
    if (transfer_request)
#endif
    _ux_utility_memory_free(transfer_request -> ux_transfer_request_data_pointer);

#if defined(UX_HOST_STANDALONE)
    if (hub -> ux_host_class_hub_allocated)
    {
        _ux_utility_memory_free(hub -> ux_host_class_hub_allocated);
    }
#endif

    /* Destroy the instance.  */
    _ux_host_stack_class_instance_destroy(hub -> ux_host_class_hub_class, (VOID *) hub);

    /* Before we free the device resources, we need to inform the application
        that the device is removed.  */
    if (_ux_system_host -> ux_system_host_change_function != UX_NULL)
    {
        
        /* Inform the application the device is removed.  */
        _ux_system_host -> ux_system_host_change_function(UX_DEVICE_REMOVAL, hub -> ux_host_class_hub_class, (VOID *) hub);
    }

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_CLASS_HUB_DEACTIVATE, hub, 0, 0, 0, UX_TRACE_HOST_CLASS_EVENTS, 0, 0)

    /* If trace is enabled, register this object.  */
    UX_TRACE_OBJECT_UNREGISTER(hub);

#if defined(UX_HOST_STANDALONE)

    /* Unlink from device class instance.  */        
    hub -> ux_host_class_hub_device -> ux_device_class_instance =  (VOID *) hub;
#endif

    /* Free the memory block used by the class.  */
    _ux_utility_memory_free(hub);

    /* Return successful completion.  */
    return(UX_SUCCESS);         
}
