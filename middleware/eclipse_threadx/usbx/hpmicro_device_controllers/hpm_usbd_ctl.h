/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_USBD_CTL_INTERFACE_H_
#define _HPM_USBD_CTL_INTERFACE_H_

#include "hpm_common.h"
#include "hpm_usb_device.h"

#define UX_DCD_HPM_DCI_SLAVE_CONTROLLER 0x80

/*! @brief uninitialized value */
#define USB_UNINITIALIZED_VAL_32 (0xFFFFFFFFU)

/* DEBUG level */
#define USB_DBG_ERROR 0
#define USB_DBG_WARNING 1
#define USB_DBG_INFO 2
#define USB_DBG_LOG 3

/* Define USB HPM USB physical endpoint status definition.  */

#define HPM_DCI_ED_STATUS_UNUSED 0
#define HPM_DCI_ED_STATUS_USED 1
#define HPM_DCI_ED_STATUS_TRANSFER 2
#define HPM_DCI_ED_STATUS_STALLED 4
#define HPM_DCI_ED_STATUS_DONE 8u

#define usb_dbg_log_line(lvl, fmt, ...)                                                                                                                                                                                                                                                                                                                                \
    do {                                                                                                                                                                                                                                                                                                                                                               \
        printf("%c:", lvl);                                                                                                                                                                                                                                                                                                                                            \
        printf(fmt, ##__VA_ARGS__);                                                                                                                                                                                                                                                                                                                                    \
    } while (0)

#ifndef CONFIG_USB_DBG_LEVEL
#define CONFIG_USB_DBG_LEVEL USB_DBG_ERROR
#endif

#if (CONFIG_USB_DBG_LEVEL >= USB_DBG_LOG)
#define USB_LOG_DBG(fmt, ...) usb_dbg_log_line('D', fmt, ##__VA_ARGS__)
#else
#define USB_LOG_DBG(...)
#endif

#if (CONFIG_USB_DBG_LEVEL >= USB_DBG_INFO)
#define USB_LOG_INFO(fmt, ...) usb_dbg_log_line('I', fmt, ##__VA_ARGS__)
#else
#define USB_LOG_INFO(...)
#endif

#if (CONFIG_USB_DBG_LEVEL >= USB_DBG_WARNING)
#define USB_LOG_WRN(fmt, ...) usb_dbg_log_line('W', fmt, ##__VA_ARGS__)
#else
#define USB_LOG_WRN(...)
#endif

#if (CONFIG_USB_DBG_LEVEL >= USB_DBG_ERROR)
#define USB_LOG_ERR(fmt, ...) usb_dbg_log_line('E', fmt, ##__VA_ARGS__)
#else
#define USB_LOG_ERR(...)
#endif

#define USB_EP_NUM 16

typedef struct HPM_DCI_ED_STRUCT {
    UCHAR endpt_status;
    UCHAR endpt_index;
    struct UX_SLAVE_ENDPOINT_STRUCT *ux_endpt;
} HPM_ENDPT_T;

/*! @brief Endpoint callback message structure */

/* Define USB HPM_DCI DCD structure definition.  */

typedef struct UX_DCD_HPM_DCI_STRUCT {
    usb_device_handle_t *handle;
    HPM_ENDPT_T hpm_endpt_out[USB_EP_NUM];
    HPM_ENDPT_T hpm_endpt_in[USB_EP_NUM];
} HPM_USBD_CONTROLLER;

/*! @brief Endpoint callback message structure */
typedef struct _usb_device_endpoint_callback_message_struct
{
    uint8_t *buffer; /*!< Transferred buffer */
    uint32_t length; /*!< Transferred data length */
    uint8_t is_setup_packet; /*!< Is in a setup phase */
} usb_message_t;

/* Index to bit position in register */
static inline uint8_t ep_idx2bit(uint8_t ep_idx) {
    return ep_idx / 2 + ((ep_idx % 2) ? 16 : 0);
}

/*!
 * @brief _hpm_usbd_callback: call back function when usb status is changed.
 */
UINT _hpm_usbd_callback(void *handle, uint32_t event, void *param);

/*!
 * @brief _hpm_usbd_ctl_control_callback: deal with EP0's interrupt, include SETUP interrupt.
 */
UINT _hpm_usbd_ctl_control_callback(usb_message_t *message, uint8_t endpoint);

/*!
 * @brief _hpm_usbd_transfer_complete_callback: deal with other endpoints's transfer complete interrupt.
 */
UINT _hpm_usbd_transfer_complete_callback(usb_message_t *message, uint8_t ep);

/*!
 * @brief _hpm_usbd_endpoint_xxx: deal with endpoint operate functions.
 */
UINT _hpm_usbd_endpoint_create(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_ENDPOINT *endpoint);
UINT _hpm_usbd_endpoint_destroy(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_ENDPOINT *endpoint);
UINT _hpm_usbd_endpoint_reset(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_ENDPOINT *endpoint);
UINT _hpm_usbd_endpoint_stall(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_ENDPOINT *endpoint);
UINT _hpm_usbd_endpoint_status(HPM_USBD_CONTROLLER *hpm_usbd, ULONG endpoint_index);

/*!
 * @brief _hpm_usbd_transfer_xxx: deal with transfer operate functions.
 *        This function is very important. 
 */
UINT _hpm_usbd_transfer_request(HPM_USBD_CONTROLLER *hpm_usbd, UX_SLAVE_TRANSFER *transfer_request);
UINT _hpm_usbd_transfer_abort(HPM_USBD_CONTROLLER *hpm_controller, UX_SLAVE_TRANSFER *transfer_request);

UINT _hpm_usbd_frame_number_get(HPM_USBD_CONTROLLER *hpm_usbd, ULONG *frame_number);
UINT _hpm_usbd_state_change(HPM_USBD_CONTROLLER *hpm_controller, ULONG state);

/*!
 * @brief _hpm_usbd_initialize: called when usbx initializing.
 */
UINT _hpm_usbd_initialize(ULONG controller_id, usb_device_handle_t **handle);

/*!
 * @brief _hpm_usbd_initialize_complete: called when usb RESET interrupt is triggered.
 */
UINT _hpm_usbd_initialize_complete(void);
UINT _hpm_usbd_uninitialize(ULONG controller_id, usb_device_handle_t **handle_ptr);

/*!
 * @brief _hpm_usbd_function: called by usbx stack to indicate controller what to do.
 */
UINT _hpm_usbd_function(UX_SLAVE_DCD *dcd, UINT function, VOID *parameter);

#endif