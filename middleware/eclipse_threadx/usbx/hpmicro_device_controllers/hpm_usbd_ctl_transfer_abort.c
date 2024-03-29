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

#define UX_SOURCE_CODE

/* Include necessary system files.  */

#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"
#include "ux_utility.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_transfer_abort                            PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will terminate a transfer.                            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    hpm_controller                        Pointer to device controller  */
/*    transfer_request                      Pointer to transfer request   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
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
UINT _hpm_usbd_transfer_abort(HPM_USBD_CONTROLLER *hpm_controller, UX_SLAVE_TRANSFER *transfer_request)
{

    HPM_ENDPT_T *hpm_endpt;
    UX_SLAVE_ENDPOINT *endpoint;

    UX_PARAMETER_NOT_USED(hpm_controller);

    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint = transfer_request->ux_slave_transfer_request_endpoint;

    /* Keep the physical endpoint address in the endpoint container.  */
    hpm_endpt = (HPM_ENDPT_T *)endpoint->ux_slave_endpoint_ed;

    /* Turn off the transfer bit.  */
    hpm_endpt->endpt_status &= ~(ULONG)(HPM_DCI_ED_STATUS_TRANSFER | HPM_DCI_ED_STATUS_DONE);

    /* This function never fails.  */
    return (UX_SUCCESS);
}