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
/**   HPM_DCI Controller Driver                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE
#define UX_DCD_HPM_DCI_SOURCE_CODE

/* Include necessary system files.  */

#include "hpm_common.h"
#include <stdint.h>
#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "usb_util.h"
#include "usb_def.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_initialize_complete                       PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function completes the initialization of the HPM_DCI USB       */
/*    device controller.                                                  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    (ux_slave_dcd_function)               Process the DCD function      */
/*    usb_device_edpt_open                  Init endpoint                 */
/*    _ux_utility_descriptor_parse          Parse descriptor              */
/*    _ux_utility_memory_allocate           Allocate memory               */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    HPM_DCI Controller Driver                                           */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  04-13-2023        Zihan Xu                 Initial Version 6.1        */
/*                                                                        */
/**************************************************************************/
UINT _hpm_usbd_initialize_complete(void)
{
    hpm_stat_t stat = status_success;

    UX_SLAVE_DCD *dcd = &_ux_system_slave->ux_system_slave_dcd;
    UX_SLAVE_DEVICE *device = &_ux_system_slave->ux_system_slave_device;
    UX_SLAVE_TRANSFER *ux_tx_req;

    HPM_USBD_CONTROLLER *hpm_usbd = (HPM_USBD_CONTROLLER *)dcd->ux_slave_dcd_controller_hardware;
    UCHAR *device_framework;
    usb_endpoint_config_t ep0_cfg;

    /* Are we in DFU mode ? If so, check if we are in a Reset mode.  */
    if (_ux_system_slave->ux_system_slave_device_dfu_state_machine == UX_SYSTEM_DFU_STATE_APP_DETACH) {

        /* The device is now in DFU reset mode. Switch to the DFU device framework.  */
        USB_LOG_INFO("device_framework is DFU\n");
        _ux_system_slave->ux_system_slave_device_framework = _ux_system_slave->ux_system_slave_dfu_framework;
        _ux_system_slave->ux_system_slave_device_framework_length = _ux_system_slave->ux_system_slave_dfu_framework_length;

    } else {

        /* Set State to App Idle. */
        _ux_system_slave->ux_system_slave_device_dfu_state_machine = UX_SYSTEM_DFU_STATE_APP_IDLE;

        /* Check the speed and set the correct descriptor.  */
        if (_ux_system_slave->ux_system_slave_speed == UX_FULL_SPEED_DEVICE) {

            /* The device is operating at full speed.  */
            _ux_system_slave->ux_system_slave_device_framework = _ux_system_slave->ux_system_slave_device_framework_full_speed;
            _ux_system_slave->ux_system_slave_device_framework_length = _ux_system_slave->ux_system_slave_device_framework_length_full_speed;
        } else {
            /* The device is operating at high speed.  */
            _ux_system_slave->ux_system_slave_device_framework = _ux_system_slave->ux_system_slave_device_framework_high_speed;
            _ux_system_slave->ux_system_slave_device_framework_length = _ux_system_slave->ux_system_slave_device_framework_length_high_speed;
        }
    }

    /* Get the device framework pointer.  */
    device_framework = _ux_system_slave->ux_system_slave_device_framework;

    /* And create the decompressed device descriptor structure.  */
    _ux_utility_descriptor_parse(device_framework, _ux_system_device_descriptor_structure, UX_DEVICE_DESCRIPTOR_ENTRIES, (UCHAR *)&device->ux_slave_device_descriptor);

    /* Now we create a transfer request to accept the first SETUP packet
       and get the ball running. First get the address of the endpoint
       transfer request container.  */
    ux_tx_req = &device->ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;

    /* Set the timeout to be for Control Endpoint.  */
    ux_tx_req->ux_slave_transfer_request_timeout = UX_MS_TO_TICK(UX_CONTROL_TRANSFER_TIMEOUT);

    /* Adjust the current data pointer as well.  */
    ux_tx_req->ux_slave_transfer_request_current_data_pointer = ux_tx_req->ux_slave_transfer_request_data_pointer;

    /* Update the transfer request endpoint pointer with the default endpoint.  */
    ux_tx_req->ux_slave_transfer_request_endpoint = &device->ux_slave_device_control_endpoint;

    ux_tx_req->ux_slave_transfer_request_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress = 0x80;
    /* The control endpoint max packet size needs to be filled manually in its descriptor.  */
    ux_tx_req->ux_slave_transfer_request_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize = device->ux_slave_device_descriptor.bMaxPacketSize0;

    /* On the control endpoint, always expect the maximum.  */
    ux_tx_req->ux_slave_transfer_request_requested_length = device->ux_slave_device_descriptor.bMaxPacketSize0;
    ux_tx_req->ux_slave_transfer_request_transfer_length = device->ux_slave_device_descriptor.bMaxPacketSize0;

    /* Attach the control endpoint to the transfer request.  */
    ux_tx_req->ux_slave_transfer_request_endpoint = &device->ux_slave_device_control_endpoint;

    /* Create the default control endpoint attached to the device.
       Once this endpoint is enabled, the host can then send a setup packet
       The device controller will receive it and will call the setup function
       module.  */
    dcd->ux_slave_dcd_function(dcd, UX_DCD_CREATE_ENDPOINT, (VOID *)&device->ux_slave_device_control_endpoint);

    ep0_cfg.max_packet_size = USB_CTRL_EP_MPS;
    ep0_cfg.xfer = USB_ENDPOINT_TYPE_CONTROL;
    ep0_cfg.ep_addr = USB_CONTROL_IN_EP0;
    usb_device_edpt_open(hpm_usbd->handle, &ep0_cfg);

    ep0_cfg.ep_addr = USB_CONTROL_OUT_EP0;
    usb_device_edpt_open(hpm_usbd->handle, &ep0_cfg);

    /* Ensure the control endpoint is properly reset.  */
    device->ux_slave_device_control_endpoint.ux_slave_endpoint_state = UX_ENDPOINT_RESET;

    /* Mark the phase as SETUP.  */
    ux_tx_req->ux_slave_transfer_request_type = UX_TRANSFER_PHASE_SETUP;

    /* Mark this transfer request as pending.  */
    ux_tx_req->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_PENDING;

    /* Ask for 8 bytes of the SETUP packet.  */
    ux_tx_req->ux_slave_transfer_request_requested_length = UX_SETUP_SIZE;
    ux_tx_req->ux_slave_transfer_request_in_transfer_length = UX_SETUP_SIZE;

    /* Reset the number of bytes sent/received.  */
    ux_tx_req->ux_slave_transfer_request_actual_length = 0;

    /* Check the status change callback.  */
    if (_ux_system_slave->ux_system_slave_change_function != UX_NULL) {

        /* Inform the application if a callback function was programmed.  */
        _ux_system_slave->ux_system_slave_change_function(UX_DEVICE_ATTACHED);
    }

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_DEVICE_STACK_CONNECT, 0, 0, 0, 0, UX_TRACE_DEVICE_STACK_EVENTS, 0, 0)

    /* If trace is enabled, register this object.  */
    UX_TRACE_OBJECT_REGISTER(UX_TRACE_DEVICE_OBJECT_TYPE_DEVICE, device, 0, 0, 0)

    /* We are now ready for the USB device to accept the first packet when connected.  */

    return stat;
}