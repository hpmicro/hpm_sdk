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
#include "hpm_usb_drv.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_function                                  PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function dispatches the DCD function internally to the HPM USB */
/*    controller.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd                                   Pointer to device controller  */
/*    function                              Function requested            */
/*    parameter                             Pointer to function parameters*/
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _hpm_usbd_endpoint_create       Create endpoint                     */
/*    _hpm_usbd_endpoint_destroy      Destroy endpoint                    */
/*    _hpm_usbd_endpoint_reset        Reset endpoint                      */
/*    _hpm_usbd_endpoint_stall        Stall endpoint                      */
/*    _hpm_usbd_endpoint_status       Get endpoint status                 */
/*    _hpm_usbd_frame_number_get      Get frame number                    */
/*    _hpm_usbd_transfer_request      Request data transfer               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    USBX Device Stack                                                   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  04-13-2023         Zihan Xu             Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT _hpm_usbd_function(UX_SLAVE_DCD *dcd, UINT function, VOID *parameter)
{

    UINT status;
    HPM_USBD_CONTROLLER *hpm_usbd;
    uint32_t address;

    /* Check the status of the controller.  */
    if (dcd->ux_slave_dcd_status == UX_UNUSED) {

        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_DCD, UX_CONTROLLER_UNKNOWN);

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_CONTROLLER_UNKNOWN, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return (UX_CONTROLLER_UNKNOWN);
    }

    /* Get the pointer to the HPM_DCI DCD.  */
    hpm_usbd = (HPM_USBD_CONTROLLER *)dcd->ux_slave_dcd_controller_hardware;

    /* Look at the function and route it.  */
    switch (function) {

    case UX_DCD_GET_FRAME_NUMBER:
        status = _hpm_usbd_frame_number_get(hpm_usbd, (ULONG *)parameter);
        break;
#if defined(UX_DEVICE_STANDALONE)
    case UX_DCD_TRANSFER_RUN:

        status =  _ux_dcd_sim_slave_transfer_run(dcd_sim_slave, (UX_SLAVE_TRANSFER *) parameter);
        break;
#else
    case UX_DCD_TRANSFER_REQUEST:
        status = _hpm_usbd_transfer_request(hpm_usbd, (UX_SLAVE_TRANSFER *)parameter);
        break;
#endif
    case UX_DCD_TRANSFER_ABORT:
        status =  _hpm_usbd_transfer_abort(hpm_usbd, (UX_SLAVE_TRANSFER *) parameter);
        break;
    case UX_DCD_CREATE_ENDPOINT:
        status = _hpm_usbd_endpoint_create(hpm_usbd, parameter);
        break;

    case UX_DCD_DESTROY_ENDPOINT:
        status = _hpm_usbd_endpoint_destroy(hpm_usbd, parameter);
        break;

    case UX_DCD_RESET_ENDPOINT:
        status = _hpm_usbd_endpoint_reset(hpm_usbd, parameter);
        break;

    case UX_DCD_STALL_ENDPOINT:
        status = _hpm_usbd_endpoint_stall(hpm_usbd, parameter);
        break;

    case UX_DCD_SET_DEVICE_ADDRESS:
        address = (uint32_t)parameter;
        usb_dcd_set_address(hpm_usbd->handle->regs, address);
        status = UX_SUCCESS;
        break;

    case UX_DCD_CHANGE_STATE:
        status =  _hpm_usbd_state_change(hpm_usbd, (ULONG) (ALIGN_TYPE)  parameter);
        break;

    case UX_DCD_ENDPOINT_STATUS:
        status = _hpm_usbd_endpoint_status(hpm_usbd, (ULONG)parameter);
        break;

    default:
        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_DCD, UX_FUNCTION_NOT_SUPPORTED);

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_FUNCTION_NOT_SUPPORTED, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)

        status = UX_FUNCTION_NOT_SUPPORTED;
        break;
    }

    /* Return completion status.  */
    return (status);
}