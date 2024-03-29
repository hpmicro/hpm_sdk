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
#include "usb_util.h"
#include "usb_def.h"
#include "hpm_usbx_port.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_ctl_control_callback                      PORTABLE C      */
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
/*    _ux_device_stack_control_request_process                            */
/*                                          Process control request       */
/*    hpm_usbd_send                         Transmit data                 */
/*    hpm_usbd_recv                         Receive data                  */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    HPM USB Driver                                                      */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  04-13-2023         Zihan Xu             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT _hpm_usbd_ctl_control_callback(usb_message_t *message, uint8_t ep_addr)
{
    /* USBX core classes */
    UX_SLAVE_DCD *ux_dcd = &_ux_system_slave->ux_system_slave_dcd;

    UX_SLAVE_TRANSFER *ux_tx_req; /* transfer request abstract */

    UX_SLAVE_ENDPOINT *ux_endpt;

    /* Hpmicro defined classes */
    HPM_USBD_CONTROLLER *hpm_usbd = (HPM_USBD_CONTROLLER *)ux_dcd->ux_slave_dcd_controller_hardware;

    /* ux_endpt 0 only use endpt_in at software's sight, it is not a hardware's abstract */
    HPM_ENDPT_T *hpm_endpt = &hpm_usbd->hpm_endpt_in[0];

    ux_tx_req = &hpm_endpt->ux_endpt->ux_slave_endpoint_transfer_request;

    /* -----------DEAL--WITH--SETUP--PACKET--------------- */
    if (message->is_setup_packet) {
        _ux_utility_memory_copy(ux_tx_req->ux_slave_transfer_request_setup, (void *)(message->buffer), UX_SETUP_SIZE);

        /* Clear the length of the data received.  */
        ux_tx_req->ux_slave_transfer_request_actual_length = 0;

        /* Mark the phase as SETUP. */
        ux_tx_req->ux_slave_transfer_request_type = UX_TRANSFER_PHASE_SETUP;

        /* Mark the transfer as successful.  */
        ux_tx_req->ux_slave_transfer_request_completion_code = UX_SUCCESS;

        /* Set the status of the ux_endpt to not stalled.  */
        hpm_endpt->endpt_status &= ~HPM_DCI_ED_STATUS_STALLED;

        /* Check if the transaction is IN.  */
        if (*ux_tx_req->ux_slave_transfer_request_setup & UX_REQUEST_IN) {

            /* Call the Control Transfer dispatcher.  */
            _ux_device_stack_control_request_process(ux_tx_req);

        } else {

            /* We are in a OUT transaction. Check if there is a data payload. If so, wait for the payload
               to be delivered.  */
            if (*(ux_tx_req->ux_slave_transfer_request_setup + 6) == 0 && *(ux_tx_req->ux_slave_transfer_request_setup + 7) == 0) {

                /* Call the Control Transfer dispatcher.  */
                if (_ux_device_stack_control_request_process(ux_tx_req) == UX_SUCCESS) {

                    hpm_usbd_send(hpm_usbd->handle, USB_CONTROL_IN_EP0, UX_NULL, 0U);
                }
            } else {
                /* Get the pointer to the logical ux_endpt from the transfer request.  */
                ux_endpt = ux_tx_req->ux_slave_transfer_request_endpoint;

                /* Get the length we expect from the SETUP packet.  */
                ux_tx_req->ux_slave_transfer_request_requested_length = _ux_utility_short_get(ux_tx_req->ux_slave_transfer_request_setup + 6);

                /* Check if we have enough space for the request.  */
                if (ux_tx_req->ux_slave_transfer_request_requested_length > UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH) {

                    /* No space available, stall the ux_endpt.  */
                    _hpm_usbd_endpoint_stall(hpm_usbd, ux_endpt);

                    /* We are done.  */
                    return UX_SUCCESS;
                } else {

                    /* Reset what we have received so far.  */
                    ux_tx_req->ux_slave_transfer_request_actual_length = 0;

                    /* And reprogram the current buffer address to the beginning of the buffer.  */
                    ux_tx_req->ux_slave_transfer_request_current_data_pointer = ux_tx_req->ux_slave_transfer_request_data_pointer;

                    /* Receive data.  */
                    hpm_usbd_recv(hpm_usbd->handle, ux_endpt->ux_slave_endpoint_descriptor.bEndpointAddress, ux_tx_req->ux_slave_transfer_request_current_data_pointer, ux_tx_req->ux_slave_transfer_request_requested_length);

                }
            }
        }
        /* We are done with SETUP packet */
        return UX_SUCCESS; 
    }

    /* Get the pointer to the logical ux_endpt from the transfer request.  */
    ux_endpt = ux_tx_req->ux_slave_transfer_request_endpoint;

    /* usbd_event_ep0_out_complete_handler */
    if ((ep_addr & USB_EP_DIR_MASK) == USB_EP_DIR_OUT) { /* deal with EP0OUT */
        if (message->length == 0) { /* EP0 recv out status */
            return UX_SUCCESS;
        } else { /* deal with EP0OUT (bytes transfered > 0) */

            /* Update the length of the data received.  */
            ux_tx_req->ux_slave_transfer_request_actual_length += message->length;

            USB_LOG_DBG("EP0 OUT recved %d bytes, expected %d bytes\n", ux_tx_req->ux_slave_transfer_request_actual_length, ux_tx_req->ux_slave_transfer_request_requested_length);
            
            /* Can we accept this much?  */
            if (ux_tx_req->ux_slave_transfer_request_actual_length <= ux_tx_req->ux_slave_transfer_request_requested_length) {

                /* Are we done with this transfer ? */
                if ((ux_tx_req->ux_slave_transfer_request_actual_length == ux_tx_req->ux_slave_transfer_request_requested_length)) {

                    /* Set the completion code to no error.  */
                    ux_tx_req->ux_slave_transfer_request_completion_code = UX_SUCCESS;

                    /* We are using a Control ux_endpt on a OUT transaction and there was a payload.  */
                    if (_ux_device_stack_control_request_process(ux_tx_req) == UX_SUCCESS) {

                        /*Send status to host*/
                        hpm_usbd_send(hpm_usbd->handle, USB_CONTROL_IN_EP0, UX_NULL, 0U);
                    }
                } else {

                    /* Rearm the OUT control ux_endpt for one packet. */
                    ux_tx_req->ux_slave_transfer_request_current_data_pointer += ux_endpt->ux_slave_endpoint_descriptor.wMaxPacketSize;
                    hpm_usbd_recv(hpm_usbd->handle, USB_CONTROL_OUT_EP0, ux_tx_req->ux_slave_transfer_request_current_data_pointer, ux_tx_req->ux_slave_transfer_request_requested_length - ux_tx_req->ux_slave_transfer_request_actual_length);
                }
            } else {

                /*  We have an overflow situation. Set the completion code to overflow.  */
                ux_tx_req->ux_slave_transfer_request_completion_code = UX_TRANSFER_BUFFER_OVERFLOW;

                /* If trace is enabled, insert this event into the trace buffer.  */
                UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_TRANSFER_BUFFER_OVERFLOW, ux_tx_req, 0, 0, UX_TRACE_ERRORS, 0, 0)

                /* We are using a Control ux_endpt, if there is a callback, invoke it. We are still under ISR.  */
                if (ux_tx_req->ux_slave_transfer_request_completion_function)
                    ux_tx_req->ux_slave_transfer_request_completion_function(ux_tx_req);
            }
        }
        return UX_SUCCESS;
    }

    if ((ep_addr & USB_EP_DIR_MASK) == USB_EP_DIR_IN) { /* deal with IN transfer completed ISR */
        /* Are we done with this transfer ? */
        ux_tx_req->ux_slave_transfer_request_in_transfer_length -= message->length;

        if (ux_tx_req->ux_slave_transfer_request_in_transfer_length > 0) {
            /* Adjust the data pointer.  */
            ux_tx_req->ux_slave_transfer_request_current_data_pointer += message->length;

            USB_LOG_DBG("transfer remain %d bytes\n", ux_tx_req->ux_slave_transfer_request_in_transfer_length);
            /* Transmit data.  */
            hpm_usbd_send(hpm_usbd->handle, USB_CONTROL_IN_EP0, ux_tx_req->ux_slave_transfer_request_current_data_pointer, ux_tx_req->ux_slave_transfer_request_in_transfer_length);
        } else if (ux_tx_req->ux_slave_transfer_request_in_transfer_length == 0) {
            /* There is no data to send but we may need to send a Zero Length Packet.  */
            if (ux_tx_req->ux_slave_transfer_request_force_zlp == UX_TRUE) {

                /* Arm a ZLP packet on IN.  */
                hpm_usbd_send(hpm_usbd->handle, USB_CONTROL_IN_EP0, UX_NULL, 0);

                /* Reset the ZLP condition.  */
                ux_tx_req->ux_slave_transfer_request_force_zlp = UX_FALSE;

            } else {
                /* Satisfying three conditions will jump here.
                 * 1. send status completely
                 * 2. send zlp completely
                 * 3. send last data completely.
                 */
                ULONG request_length = _ux_utility_short_get(ux_tx_req->ux_slave_transfer_request_setup + UX_SETUP_LENGTH);
                /* Set the completion code to no error.  */
                ux_tx_req->ux_slave_transfer_request_completion_code = UX_SUCCESS;

                /* The transfer is completed.  */
                ux_tx_req->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;

                /* We are using a Control ux_endpt, if there is a callback, invoke it. We are still under ISR.  */
                if (ux_tx_req->ux_slave_transfer_request_completion_function)
                    ux_tx_req->ux_slave_transfer_request_completion_function(ux_tx_req);

                if (*ux_tx_req->ux_slave_transfer_request_setup & UX_REQUEST_IN) {
                    if (request_length) {
                        hpm_usbd_recv(hpm_usbd->handle, USB_CONTROL_OUT_EP0, UX_NULL, 0);
                    }
                }
            }
        } else {
            USB_LOG_ERR("ERROR, EP0 Sended more data than expected\n");
            while (1)
                ;
        }
    }
    return UX_SUCCESS;
}