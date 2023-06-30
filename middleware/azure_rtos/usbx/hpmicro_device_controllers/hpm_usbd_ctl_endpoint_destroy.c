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
/*    _hpm_usbd_endpoint_destroy                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will destroy a physical endpoint.                     */
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
/*    USB_DeviceDeinitEndpoint              Deactivate endpoint           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    HPM USB Controller Driver                                           */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  04-13-2023        Zihan Xu                 Initial Version 6.1        */
/*                                                                        */
/**************************************************************************/
UINT _hpm_usbd_endpoint_destroy(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_ENDPOINT *endpoint)
{

    /* Deactivate the endpoint.  */
    usb_device_edpt_close(hpm_usbd->handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);

    /* This function never fails.  */
    return UX_SUCCESS;
}