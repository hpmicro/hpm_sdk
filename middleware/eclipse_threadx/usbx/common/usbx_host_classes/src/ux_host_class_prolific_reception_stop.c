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
/*    _ux_host_class_prolific_reception_stop              PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function starts a reception with a prolific device. This       */ 
/*    mechanism allows for non blocking calls based on a packet           */ 
/*    orientated round robbin buffer. When a packet is fully or partially */
/*    received, an application callback function is invoked and a new     */
/*    transfer request is rescheduled.                                    */
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    prolific                               Pointer to prolific class    */ 
/*    prolific_reception                     Pointer to reception struct  */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_host_stack_endpoint_transfer_abort                              */ 
/*                                          Abort transfer                */
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
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_prolific_reception_stop (UX_HOST_CLASS_PROLIFIC *prolific, 
                                    UX_HOST_CLASS_PROLIFIC_RECEPTION *prolific_reception)
{

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_CLASS_PROLIFIC_RECEPTION_STOP, prolific, 0, 0, 0, UX_TRACE_HOST_CLASS_EVENTS, 0, 0)

    /* Ensure the instance is valid.  */
    if (prolific -> ux_host_class_prolific_state !=  UX_HOST_CLASS_INSTANCE_LIVE)
    {        

        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_HOST_CLASS_INSTANCE_UNKNOWN);

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_HOST_CLASS_INSTANCE_UNKNOWN, prolific, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return(UX_HOST_CLASS_INSTANCE_UNKNOWN);
    }

    /* Check if we do have transfers for this application. If none, nothing to do. */
    if (prolific_reception -> ux_host_class_prolific_reception_state ==  UX_HOST_CLASS_PROLIFIC_RECEPTION_STATE_STOPPED)
        return(UX_SUCCESS);
        
    /* We need to abort transactions on the bulk In pipe.  */
    _ux_host_stack_endpoint_transfer_abort(prolific -> ux_host_class_prolific_bulk_in_endpoint);

    /* Declare the reception stopped.  */
    prolific_reception -> ux_host_class_prolific_reception_state =  UX_HOST_CLASS_PROLIFIC_RECEPTION_STATE_STOPPED;

    /* This function never really fails.  */
    return(UX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_prolific_reception_stop            PORTABLE C       */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in PROLIFIC reception function call.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    prolific                              Pointer to PROLIFIC class     */
/*    prolific_reception                    Pointer to reception struct   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_prolific_reception_stop                              */
/*                                          PROLIFIC reception stop       */
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
UINT  _uxe_host_class_prolific_reception_stop (UX_HOST_CLASS_PROLIFIC *prolific, 
                                    UX_HOST_CLASS_PROLIFIC_RECEPTION *prolific_reception)
{

    /* Sanity checks.  */
    if ((prolific == UX_NULL) || (prolific_reception == UX_NULL))
        return(UX_INVALID_PARAMETER);

    /* Invoke PROLIFIC reception stop function.  */
    return(_ux_host_class_prolific_reception_stop(prolific, prolific_reception));
}
