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
/**   PIMA Class                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_pima.h"
#include "ux_host_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_pima_object_close                    PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function closes an object,                                     */
/*  INPUT                                                                 */
/*                                                                        */
/*    pima                                       Pointer to pima class    */
/*    pima_session                               Pointer to pima session  */
/*    object_handle                              The object handle        */
/*    object                                     Pointer to object info   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_stack_transfer_request            Transfer request         */
/*    _ux_host_stack_transfer_request_abort      Abort transfer           */
/*    _ux_host_stack_endpoint_reset              Reset endpoint           */
/*    _ux_host_semaphore_get                     Get semaphore            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    USB application                                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            prefixed UX to MS_TO_TICK,  */
/*                                            resulting in version 6.1    */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            refined macros names,       */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_pima_object_close(UX_HOST_CLASS_PIMA *pima,
                                        UX_HOST_CLASS_PIMA_SESSION *pima_session,
                                        ULONG object_handle, UX_HOST_CLASS_PIMA_OBJECT *object)
{

UX_TRANSFER                         *transfer_request;
UCHAR                                 *ptp_payload;
ULONG                                requested_length;
UINT                                status;

    UX_PARAMETER_NOT_USED(object_handle);

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_CLASS_PIMA_OBJECT_CLOSE, pima, object, 0, 0, UX_TRACE_HOST_CLASS_EVENTS, 0, 0)

    /* Check if this session is valid or not.  */
    if (pima_session -> ux_host_class_pima_session_magic != UX_HOST_CLASS_PIMA_MAGIC_NUMBER)
        return (UX_HOST_CLASS_PIMA_RC_SESSION_NOT_OPEN);

    /* Check if this session is opened or not.  */
    if (pima_session -> ux_host_class_pima_session_state != UX_HOST_CLASS_PIMA_SESSION_STATE_OPENED)
        return (UX_HOST_CLASS_PIMA_RC_SESSION_NOT_OPEN);

    /* Check if the object is already closed.  */
    if (object -> ux_host_class_pima_object_state != UX_HOST_CLASS_PIMA_OBJECT_STATE_OPENED)
        return (UX_HOST_CLASS_PIMA_RC_OBJECT_ALREADY_CLOSED );

    /* Close the object.  */
    object -> ux_host_class_pima_object_state = UX_HOST_CLASS_PIMA_OBJECT_STATE_CLOSED;

    /* If the transfer was not ended prematurely, we need to do the status phase.  */
    if ( object -> ux_host_class_pima_object_transfer_status == UX_HOST_CLASS_PIMA_OBJECT_TRANSFER_STATUS_COMPLETED)
    {

        /* The transfer for this transaction is now inactive.  */
        object -> ux_host_class_pima_object_transfer_status = UX_HOST_CLASS_PIMA_OBJECT_TRANSFER_STATUS_INACTIVE;

        /* Check if we had a ZLP condition during the data phase.  */
        if (pima -> ux_host_class_pima_zlp_flag != UX_HOST_CLASS_PIMA_ZLP_NONE)
        {

            /* We had a ZLP, so we now expect a zero length packet prior to the status phase.
               We need to determine the direction.  */
            if (pima -> ux_host_class_pima_zlp_flag == UX_HOST_CLASS_PIMA_ZLP_IN)

                /* We use the Bulk In pipe for receiving the zlp.  */
                transfer_request =  &pima -> ux_host_class_pima_bulk_in_endpoint -> ux_endpoint_transfer_request;

            else

                /* We use the Bulk Out pipe for sending the zlp.  */
                transfer_request =  &pima -> ux_host_class_pima_bulk_in_endpoint -> ux_endpoint_transfer_request;

            /* Initialize the payload to zero length. */
            transfer_request -> ux_transfer_request_data_pointer =  UX_NULL;
            transfer_request -> ux_transfer_request_requested_length =  0;

            /* Reset the ZLP now.  */
            pima -> ux_host_class_pima_zlp_flag = UX_HOST_CLASS_PIMA_ZLP_NONE;

            /* Send request to HCD layer.  */
            status =  _ux_host_stack_transfer_request(transfer_request);

            /* If the transfer is successful, we need to wait for the transfer request to be completed.  */
            if (status == UX_SUCCESS)
            {

                /* Wait for the completion of the transfer request.  */
                status =  _ux_host_semaphore_get(&transfer_request -> ux_transfer_request_semaphore, UX_MS_TO_TICK(UX_HOST_CLASS_PIMA_CLASS_TRANSFER_TIMEOUT));

                /* If the semaphore did not succeed we probably have a time out.  */
                if (status != UX_SUCCESS)
                {

                    /* All transfers pending need to abort. There may have been a partial transfer.  */
                    _ux_host_stack_transfer_request_abort(transfer_request);

                    /* The endpoint was halted by a transfer error and needs to be reset.  */
                    _ux_host_stack_endpoint_reset(pima -> ux_host_class_pima_bulk_in_endpoint);

                    /* The endpoint was halted by a transfer error  and needs to be reset.  */
                    _ux_host_stack_endpoint_reset(pima -> ux_host_class_pima_bulk_out_endpoint);

                    /* There was an error, return to the caller.  */
                    return(status);
                }
            }
        }

        /* We use the Bulk In pipe for receiving the response payload.  */
        transfer_request =  &pima -> ux_host_class_pima_bulk_in_endpoint -> ux_endpoint_transfer_request;

        /* Get the pointer to the ptp payload.  */
        ptp_payload =  pima -> ux_host_class_pima_container ;

        /* Calculate the requested length for this payload.  */
        requested_length =  UX_HOST_CLASS_PIMA_RESPONSE_HEADER_SIZE;

        /* Initialize the transfer_request.  */
        transfer_request -> ux_transfer_request_data_pointer =  ptp_payload;
        transfer_request -> ux_transfer_request_requested_length =  requested_length;

        /* Send request to HCD layer.  */
        status =  _ux_host_stack_transfer_request(transfer_request);

        /* If the transfer is successful, we need to wait for the transfer request to be completed.  */
        if (status == UX_SUCCESS)
        {

            /* Wait for the completion of the transfer request.  */
            status =  _ux_host_semaphore_get(&transfer_request -> ux_transfer_request_semaphore, UX_MS_TO_TICK(UX_HOST_CLASS_PIMA_CLASS_TRANSFER_TIMEOUT));

            /* If the semaphore did not succeed we probably have a time out.  */
            if (status != UX_SUCCESS)
            {

                /* All transfers pending need to abort. There may have been a partial transfer.  */
                _ux_host_stack_transfer_request_abort(transfer_request);

                /* The endpoint was halted by a transfer error and needs to be reset.  */
                _ux_host_stack_endpoint_reset(pima -> ux_host_class_pima_bulk_in_endpoint);

                /* The endpoint was halted by a transfer error  and needs to be reset.  */
                _ux_host_stack_endpoint_reset(pima -> ux_host_class_pima_bulk_out_endpoint);

                /* There was an error, return to the caller.  */
                return(status);
            }
        }
        else
        {

            /* There was a non transfer error, no partial transfer to be checked */
            return(status);
        }

    }

    /* The transfer for this transaction is now inactive.  */
    object -> ux_host_class_pima_object_transfer_status = UX_HOST_CLASS_PIMA_OBJECT_TRANSFER_STATUS_INACTIVE;

    /* Return completion status.  */
    return(UX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_pima_object_close                   PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yajun Xia, Microsoft Corporation                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in pima object close function call.     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pima                                       Pointer to pima class    */
/*    pima_session                               Pointer to pima session  */
/*    object_handle                              The object handle        */
/*    object                                     Pointer to object info   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_pima_object_close      Close pima object             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    USB application                                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2023        Yajun xia             Initial Version 6.3.0         */
/*                                                                        */
/**************************************************************************/
UINT  _uxe_host_class_pima_object_close(UX_HOST_CLASS_PIMA *pima,
                                        UX_HOST_CLASS_PIMA_SESSION *pima_session,
                                        ULONG object_handle, UX_HOST_CLASS_PIMA_OBJECT *object)
{

    /* Sanity checks.  */
    if ((pima == UX_NULL) || (pima_session == UX_NULL) || (object == UX_NULL))
        return(UX_INVALID_PARAMETER);

    /* Call the actual object close function.  */
    return(_ux_host_class_pima_object_close(pima, pima_session, object_handle, object));
}