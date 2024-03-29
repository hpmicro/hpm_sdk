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

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_endpoint_reset                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will reset a physical endpoint.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    hpm_usbd                              Pointer to device controller  */
/*    endpoint                              Pointer to endpoint container */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    USB_DeviceUnstallEndpoint             Clear STALL                   */
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
UINT _hpm_usbd_endpoint_reset(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_ENDPOINT *endpoint)
{
    HPM_ENDPT_T *hpm_endpt;

    UX_PARAMETER_NOT_USED(hpm_usbd);

    /* Get the physical endpoint address in the endpoint container.  */
    hpm_endpt = (HPM_ENDPT_T *)endpoint->ux_slave_endpoint_ed;

    /* Clear pending transfer and stall status.  */
    hpm_endpt->endpt_status &= ~(ULONG)(HPM_DCI_ED_STATUS_STALLED);

    /* Deactivate the endpoint.  */
    usb_device_edpt_clear_stall(hpm_usbd->handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);
    
    /* This function never fails. */
    return (UX_SUCCESS);
}