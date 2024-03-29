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
#define UX_DCD_HPM_DCI_SOURCE_CODE

/* Include necessary system files.  */

#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_uninitialize                              PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function uninitializes the HPM USB USB device controller       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    controller_id                         Controller ID                 */
/*    handle_ptr                            Pointer to driver handle      */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    USB_DeviceStop                        Disable device                */
/*    USB_DeviceDeinit                      Uninitialize device           */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    USBX Device Stack                                                   */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  xx-xx-xxxx     Zihan Xu                 Initial Version 6.1           */
/*                                                                        */
/**************************************************************************/
UINT _hpm_usbd_uninitialize(ULONG controller_id, usb_device_handle_t **handle_ptr)
{
    UX_SLAVE_DCD *dcd;
    HPM_USBD_CONTROLLER *hpm_usbd;

    UX_PARAMETER_NOT_USED(controller_id);

    /* Get the pointer to the DCD.  */
    dcd = &_ux_system_slave->ux_system_slave_dcd;

    /* Set the state of the controller to HALTED now.  */
    dcd->ux_slave_dcd_status = UX_DCD_STATUS_HALTED;

    /* Get controller driver.  */
    hpm_usbd = (HPM_USBD_CONTROLLER *)dcd->ux_slave_dcd_controller_hardware;

    /* Check parameter.  */
    if (hpm_usbd->handle == *handle_ptr) {

        /* Stop the device.  */
        usb_hcd_stop(hpm_usbd->handle->regs);

        /* Uninitialize the device.  */
        usb_device_deinit(hpm_usbd->handle);

        _ux_utility_memory_free(hpm_usbd);
        dcd->ux_slave_dcd_controller_hardware = UX_NULL;
        return (status_success);
    }

    /* Parameter not correct.  */
    return (status_fail);
}