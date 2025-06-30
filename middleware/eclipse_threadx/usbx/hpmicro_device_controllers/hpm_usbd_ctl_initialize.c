/*
 * Copyright (c) 2023-2025 HPMicro
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

#include "assert.h"
#include "board.h"
#include "hpm_usb_device.h"
#include "ux_api.h"
#include "hpm_usbd_ctl.h"
#include "ux_device_stack.h"
static ATTR_PLACE_AT_NONCACHEABLE_BSS_WITH_ALIGNMENT(USB_SOC_DCD_DATA_RAM_ADDRESS_ALIGNMENT)
    uint8_t _dcd_data[USB_SOC_MAX_COUNT][HPM_ALIGN_UP(sizeof(dcd_data_t), USB_SOC_DCD_DATA_RAM_ADDRESS_ALIGNMENT)];
static ATTR_PLACE_AT_NONCACHEABLE_BSS usb_device_handle_t usb_device_handle[USB_SOC_MAX_COUNT];
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _hpm_usbd_initialize                                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Zihan Xu, Hpmicro                                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the HPM USB USB device controller.        */
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
/*    USB_DeviceInit                        Initialize USB driver         */
/*    USB_DeviceRun                         Enable device                 */
/*    _ux_utility_memory_allocate           Allocate memory               */
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
UINT _hpm_usbd_initialize(ULONG controller_id, usb_device_handle_t **handle)
{
    UX_SLAVE_DCD *dcd;
    HPM_USBD_CONTROLLER *hpm_usbd;
    HPM_ENDPT_T *ed;
    (void)controller_id;
    /* Get the pointer to the DCD.  */
    dcd = &_ux_system_slave->ux_system_slave_dcd;

    /* The controller initialized here is of HPM_DCI type.  */
    dcd->ux_slave_dcd_controller_type = UX_DCD_HPM_DCI_SLAVE_CONTROLLER;

    /* Allocate memory for this HPM_DCI DCD instance.  */
    hpm_usbd = _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, sizeof(HPM_USBD_CONTROLLER));
    assert(hpm_usbd != NULL);
    for (int i = 0; i < 8; i++) {
        ed = &hpm_usbd->hpm_endpt_out[i];
        ed->endpt_index = i;
        ed->ux_endpt = _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, sizeof(struct UX_SLAVE_ENDPOINT_STRUCT));
        assert(ed->ux_endpt != NULL);
        struct UX_SLAVE_ENDPOINT_STRUCT *ux_ed = ed->ux_endpt;
        ux_ed->ux_slave_endpoint_descriptor.bEndpointAddress = i;
    }
    for (int i = 0; i < 8; i++) {
        ed = &hpm_usbd->hpm_endpt_in[i];
        ed->endpt_index = i;
        ed->ux_endpt = _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, sizeof(struct UX_SLAVE_ENDPOINT_STRUCT));
        assert(ed->ux_endpt != NULL);
        struct UX_SLAVE_ENDPOINT_STRUCT *ux_ed = ed->ux_endpt;
        ux_ed->ux_slave_endpoint_descriptor.bEndpointAddress = i | 0x80;
    }

    /* Set the pointer to the HPM USB DCD.  */
    dcd->ux_slave_dcd_controller_hardware = (VOID *)hpm_usbd;

    /* Initialize the function collector for this DCD.  */
    dcd->ux_slave_dcd_function = _hpm_usbd_function;

    uint32_t int_mask;
    int_mask = (USB_USBINTR_UE_MASK | USB_USBINTR_UEE_MASK | USB_USBINTR_PCE_MASK | USB_USBINTR_URE_MASK);

    usb_device_handle[0].regs = (USB_Type *)HPM_USB0_BASE;
    usb_device_handle[0].dcd_data = (dcd_data_t *)&_dcd_data[0][0];

    if (!usb_device_init(&usb_device_handle[0], int_mask)) {
        assert(0);
    }
    hpm_usbd->handle = &usb_device_handle[0];

    *handle = hpm_usbd->handle;

    /* Set the state of the controller to OPERATIONAL now.  */
    dcd->ux_slave_dcd_status = UX_DCD_STATUS_OPERATIONAL;
    /* Return completion status.  */
    return (UX_SUCCESS);
}