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
#include "hpm_usbx_port.h"
#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"
#include "ux_utility.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_callback                                  PORTABLE C      */
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
/*    handle                                Pointer to device handle      */
/*    event                                 Event for callback            */
/*    param                                 Parameter for callback        */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_device_stack_control_request_process                            */
/*                                          Process control request       */
/*    USB_DeviceGetStatus                   Get status                    */
/*    _hpm_usbd_initialize_complete         Complete initialization       */
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
UINT _hpm_usbd_callback(void *handle, uint32_t event, void *param)
{
    usb_device_handle_t *phandle = (usb_device_handle_t *)handle;
    (void)param;
    if ((uint32_t)USBD_EVENT_BUS_RESET == event) {

        /* If the device is attached or configured, we need to disconnect it.  */
        if (_ux_system_slave->ux_system_slave_device.ux_slave_device_state != UX_DEVICE_RESET) {

            /* Disconnect the device.  */
            _ux_device_stack_disconnect();
        }

        uint8_t device_speed = usb_device_get_port_speed(phandle);

        /* Set USB Current Speed */
        if (device_speed == 0x02) {

            /* We are connected at high speed.  */
            _ux_system_slave->ux_system_slave_speed = UX_HIGH_SPEED_DEVICE;
        } else {

            /* We are connected at full speed.  */
            _ux_system_slave->ux_system_slave_speed = UX_FULL_SPEED_DEVICE;
        }

        /* Complete the device initialization.  */
        _hpm_usbd_initialize_complete();

        /* Mark the device as attached now.  */
        _ux_system_slave->ux_system_slave_device.ux_slave_device_state = UX_DEVICE_ATTACHED;
    }
    if ((uint32_t)USBD_EVENT_DETACH == event) {

        /* Disconnect the device.  */
        _ux_device_stack_disconnect();
    }
    return UX_SUCCESS;
}