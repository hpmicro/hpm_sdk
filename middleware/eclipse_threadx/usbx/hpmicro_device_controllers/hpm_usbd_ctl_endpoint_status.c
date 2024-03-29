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
/*    _hpm_usbd_endpoint_status                           PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will retrieve the status of the endpoint.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    hpm_usbd                              Pointer to device controller  */
/*    endpoint_index                        Endpoint index                */
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

UINT _hpm_usbd_endpoint_status(HPM_USBD_CONTROLLER *hpm_usbd, ULONG endpoint_index)
{
    HPM_ENDPT_T *hpm_endpt;
#ifdef UX_DEVICE_BIDIRECTIONAL_ENDPOINT_SUPPORT
    ULONG ed_addr = endpoint_index; /* Passed value as endpoint address.  */
    ULONG ed_dir = ed_addr & UX_ENDPOINT_DIRECTION;
    ULONG ed_index = ed_addr & ~UX_ENDPOINT_DIRECTION;

    /* Fetch the address of the physical endpoint.  */
    hpm_endpt = ((ed_addr == 0) ? &hpm_usbd->hpm_endpt_out[0] : \
                ((ed_dir) ? &hpm_usbd->hpm_endpt_in[ed_index] : \
                &hpm_usbd->hpm_endpt_out[ed_index]));
#else

    /* Fetch the address of the physical endpoint.  */
    hpm_endpt = &hpm_usbd->hpm_endpt_out[endpoint_index];
#endif

    /* Check the endpoint status, if it is free, we have a illegal endpoint.  */
    if ((hpm_endpt->endpt_status & HPM_DCI_ED_STATUS_USED) == 0)
        return (UX_ERROR);

    /* Check if the endpoint is stalled.  */
    if ((hpm_endpt->endpt_status & HPM_DCI_ED_STATUS_STALLED) == 0)
        return (UX_FALSE);
    else
        return (UX_TRUE);
}