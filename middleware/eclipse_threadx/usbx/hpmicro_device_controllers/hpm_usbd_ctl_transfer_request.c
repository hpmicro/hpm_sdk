/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** USBX Component                                                        */
/**                                                                       */
/**   HPM USB Controller Driver                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary system files.  */

#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"
#include "ux_utility.h"
#include "hpm_usbx_port.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_transfer_request                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will initiate a transfer to a specific endpoint.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    hpm_usbd                              Pointer to device controller  */
/*    transfer_request                      Pointer to transfer request   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    hpm_usbd_send                         Transmit data                 */
/*    hpm_usbd_recv                         Receive data                  */
/*    _ux_utility_semaphore_get             Get semaphore                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    HPM USB Controller Driver                                           */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  04-13-2023         Zihan Xu             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT _hpm_usbd_transfer_request(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_TRANSFER *transfer_request)
{
    UX_SLAVE_ENDPOINT *endpoint;
    UINT status;
    HPM_ENDPT_T *hpm_endpt;

    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint = transfer_request->ux_slave_transfer_request_endpoint;

    /* Get the slave endpoint.  */
    hpm_endpt = (HPM_ENDPT_T *)endpoint->ux_slave_endpoint_ed;
    /* Check for transfer direction.  Is this a IN endpoint ? */
    if (transfer_request->ux_slave_transfer_request_phase == UX_TRANSFER_PHASE_DATA_OUT) {
        /* Transmit data.  */
        hpm_usbd_send(hpm_usbd->handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, transfer_request->ux_slave_transfer_request_data_pointer, transfer_request->ux_slave_transfer_request_requested_length);

        /* We have a request for a OUT or IN transaction from the host.
           If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if ((endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & 0x0F) != 0) {

            /* Set the ED to TRANSFER status.  */
            hpm_endpt->endpt_status |= HPM_DCI_ED_STATUS_TRANSFER;

            /* We should wait for the semaphore to wake us up.  */
            status = _ux_utility_semaphore_get(&transfer_request->ux_slave_transfer_request_semaphore, UX_WAIT_FOREVER);

            /* Check the completion code. */
            if (status != UX_SUCCESS) {
                return (status);
            }

            transfer_request -> ux_slave_transfer_request_actual_length = transfer_request->ux_slave_transfer_request_requested_length;

            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request->ux_slave_transfer_request_completion_code != UX_SUCCESS)
                return (transfer_request->ux_slave_transfer_request_completion_code);
        }
    } else {

        /* We have a request for a SETUP or OUT Endpoint.  */
        /* Receive data.  */
        hpm_usbd_recv(hpm_usbd->handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, transfer_request->ux_slave_transfer_request_data_pointer, transfer_request->ux_slave_transfer_request_requested_length);

        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if ((endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & (UINT)~UX_ENDPOINT_DIRECTION) != 0) {

            /* We should wait for the semaphore to wake us up.  */
            status = _ux_utility_semaphore_get(&transfer_request->ux_slave_transfer_request_semaphore, UX_WAIT_FOREVER);

            /* Check the completion code. */
            if (status != UX_SUCCESS)
                return (status);

            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request->ux_slave_transfer_request_completion_code != UX_SUCCESS)
                return (transfer_request->ux_slave_transfer_request_completion_code);

            /* Return to caller with success.  */
            return (UX_SUCCESS);
        }
    }
    /* Return to caller with success.  */
    return (UX_SUCCESS);
}