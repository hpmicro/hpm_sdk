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
/*    _ux_host_class_gser_reception_stop                  PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function starts a reception with the generic modem. This way   */ 
/*    allows for non blocking calls based on a packet orientated round    */ 
/*    robbin buffer. When a packet is fully or partially received, an     */
/*    application callback function is invoked and a new transfer request */
/*    is rescheduled.                                                     */
/*                                                                        */
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    gser                               Pointer to gser class            */ 
/*    gser_reception                     Pointer to reception struct      */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    Completion Status                                                   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_host_stack_endpoint_transfer_abort                              */
/*                                       Abort transfer request           */ 
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
UINT  _ux_host_class_gser_reception_stop (UX_HOST_CLASS_GSER *gser, 
                                    UX_HOST_CLASS_GSER_RECEPTION *gser_reception)
{

ULONG           interface_index;

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_CLASS_GSER_RECEPTION_STOP, gser, 0, 0, 0, UX_TRACE_HOST_CLASS_EVENTS, 0, 0)

    /* Ensure the instance is valid.  */
    if (gser -> ux_host_class_gser_state !=  UX_HOST_CLASS_INSTANCE_LIVE)
    {       

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_HOST_CLASS_INSTANCE_UNKNOWN, gser, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return(UX_HOST_CLASS_INSTANCE_UNKNOWN);
    }

    /* Check if we do have transfers for this application. If none, nothing to do. */
    if (gser_reception -> ux_host_class_gser_reception_state ==  UX_HOST_CLASS_GSER_RECEPTION_STATE_STOPPED)
        return(UX_SUCCESS);
        
    /* Get the interface index.  */
    interface_index = gser_reception -> ux_host_class_gser_reception_interface_index;

    /* We need to abort transactions on the bulk In pipe.  */
    _ux_host_stack_endpoint_transfer_abort(gser -> ux_host_class_gser_interface_array[interface_index].ux_host_class_gser_bulk_in_endpoint);

    /* Declare the reception stopped.  */
    gser_reception -> ux_host_class_gser_reception_state =  UX_HOST_CLASS_GSER_RECEPTION_STATE_STOPPED;

    /* This function never really fails.  */
    return(UX_SUCCESS);
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_gser_reception_stop                 PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in CDC ACM reception function call.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    gser                                  Pointer to CDC ACM class      */
/*    gser_reception                        Pointer to reception struct   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_gser_reception_stop    GSER reception stop           */
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
UINT  _uxe_host_class_gser_reception_stop (UX_HOST_CLASS_GSER *gser, 
                                    UX_HOST_CLASS_GSER_RECEPTION *gser_reception)
{

    /* Sanity checks.  */
    if ((gser == UX_NULL) || (gser_reception == UX_NULL))
        return(UX_INVALID_PARAMETER);

    /* Invoke CDC ACM reception stop function.  */
    return(_ux_host_class_gser_reception_stop(gser, gser_reception));
}
