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

#include "hpm_common.h"
#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"
#include "ux_utility.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_transfer_complete_callback                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function handles callback from the USB driver.                 */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    message                               Message for callback          */
/*    ep_addr                               Endpoint address for callback */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_utility_semaphore_put             Put semaphore                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    HPM USB Driver                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  04-13-2023        Zihan Xu                 Initial Version 6.1        */
/*                                                                        */
/**************************************************************************/
UINT _hpm_usbd_transfer_complete_callback(usb_message_t *message, uint8_t ep_addr)
{
    HPM_USBD_CONTROLLER *hpm_usbd = _ux_system_slave->ux_system_slave_dcd.ux_slave_dcd_controller_hardware;
    UX_SLAVE_TRANSFER *ux_tx_req;
    UX_SLAVE_ENDPOINT *endpoint;

    endpoint = (ep_addr & UX_ENDPOINT_DIRECTION) ? \
               hpm_usbd->hpm_endpt_in[ep_addr & 0x7f].ux_endpt : \
               hpm_usbd->hpm_endpt_out[ep_addr & 0x7f].ux_endpt;

    /* Get the pointer to the transfer request.  */
    ux_tx_req = &(endpoint->ux_slave_endpoint_transfer_request);

    if (message->length != USB_UNINITIALIZED_VAL_32) {

        if ((ep_addr & UX_ENDPOINT_DIRECTION) == 0) {
            /* Update the length of the data sent in previous transaction.  */
            ux_tx_req->ux_slave_transfer_request_actual_length = message->length;
        }

        /* Set the completion code to no error.  */
        ux_tx_req->ux_slave_transfer_request_completion_code = UX_SUCCESS;

        /* The transfer is completed.  */
        ux_tx_req->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;

        /* Non control endpoint operation, use semaphore.  */
        _ux_utility_semaphore_put(&ux_tx_req->ux_slave_transfer_request_semaphore);
    }

    return UX_SUCCESS;
}