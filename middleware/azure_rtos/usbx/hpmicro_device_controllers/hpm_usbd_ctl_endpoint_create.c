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
#include <stdint.h>
#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_endpoint_create                           PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will create a physical endpoint.                      */
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
/*    USB_DeviceInitEndpoint                Init endpoint                 */
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
UINT _hpm_usbd_endpoint_create(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_ENDPOINT *endpoint)
{
    ULONG endpoint_index;
    usb_endpoint_config_t ep_cfg;

    HPM_ENDPT_T *hpm_endpt;

    /* The endpoint index in the array of the HPM USB must match the endpoint number.  */
    endpoint_index = endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & ~UX_ENDPOINT_DIRECTION;

    /* Fetch the address of the physical endpoint.  */
    hpm_endpt = ((endpoint->ux_slave_endpoint_descriptor.bEndpointAddress == 0) ? \
                &hpm_usbd->hpm_endpt_out[0] : \
                    ((endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) ? \
                    &hpm_usbd->hpm_endpt_in[endpoint_index] : \
                    &hpm_usbd->hpm_endpt_out[endpoint_index]));

    /* Check the index range and endpoint status, if it is free, reserve it. If not reject this endpoint.  */
    if ((endpoint_index < USB_EP_NUM) && ((hpm_endpt->endpt_status & HPM_DCI_ED_STATUS_USED) == 0)) {

        /* We can use this endpoint.  */
        hpm_endpt->endpt_status |= HPM_DCI_ED_STATUS_USED;

        /* Keep the physical endpoint address in the endpoint container.  */
        endpoint->ux_slave_endpoint_ed = (VOID *)hpm_endpt;

        /* Save the endpoint pointer.  */
        hpm_endpt->ux_endpt = endpoint;

        /* And its index.  */
        hpm_endpt->endpt_index = endpoint_index;

        /* Check if it is non-control endpoint.  */
        if (endpoint_index != 0) {
            ep_cfg.max_packet_size = endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
            ep_cfg.xfer = endpoint->ux_slave_endpoint_descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE;
            ep_cfg.ep_addr = endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
            usb_device_edpt_open(hpm_usbd->handle, &ep_cfg);
        }

        /* Return successful completion.  */
        return UX_SUCCESS;
    }

    /* Return an error.  */
    return UX_INVALID_PARAMETER;
}