/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   HID Remote Control Class                                            */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    ux_host_class_hid_remote_control.h                  PORTABLE C      */ 
/*                                                           6.1.11       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This file contains all the header and extern functions used by the  */
/*    USBX HID remote control class.                                      */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  08-02-2021     Wen Wang                 Modified comment(s),          */
/*                                            added extern "C" keyword    */
/*                                            for compatibility with C++, */
/*                                            resulting in version 6.1.8  */
/*  04-25-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            fixed clients management,   */
/*                                            resulting in version 6.1.11 */
/*                                                                        */
/**************************************************************************/

#ifndef UX_HOST_CLASS_HID_REMOTE_CONTROL_H
#define UX_HOST_CLASS_HID_REMOTE_CONTROL_H

/* Determine if a C++ compiler is being used.  If so, ensure that standard 
   C is used to process the API information.  */ 

#ifdef   __cplusplus 

/* Yes, C++ compiler is present.  Use standard C.  */ 
extern   "C" { 

#endif  


/* Internal option: enable the basic USBX error checking. This define is typically used
   while debugging application.  */
#if defined(UX_ENABLE_ERROR_CHECKING) && !defined(UX_DEVICE_CLASS_HID_REMOTE_CONTROL_ENABLE_ERROR_CHECKING)
#define UX_DEVICE_CLASS_HID_REMOTE_CONTROL_ENABLE_ERROR_CHECKING
#endif


/* Define HID Remote Control Class constants.  */

#define UX_HOST_CLASS_HID_REMOTE_CONTROL_BUFFER_LENGTH          128
#define UX_HOST_CLASS_HID_REMOTE_CONTROL_USAGE_ARRAY_LENGTH     64

/* Each item in usage array takes 4 bytes. Check memory bytes calculation overflow here.  */
#if UX_OVERFLOW_CHECK_MULC_ULONG(UX_HOST_CLASS_HID_REMOTE_CONTROL_USAGE_ARRAY_LENGTH, 4)
#error UX_HOST_CLASS_HID_REMOTE_CONTROL_USAGE_ARRAY_LENGTH is too large for memory allocation, please check
#endif

/* Define HID Remote Control Class structure.  */

typedef struct UX_HOST_CLASS_HID_REMOTE_CONTROL_STRUCT
{

    ULONG           ux_host_class_hid_remote_control_state;    
    UX_HOST_CLASS_HID   *ux_host_class_hid_remote_control_hid;
    ULONG           *ux_host_class_hid_remote_control_usage_array;
    ULONG           *ux_host_class_hid_remote_control_usage_array_head;
    ULONG           *ux_host_class_hid_remote_control_usage_array_tail;
} UX_HOST_CLASS_HID_REMOTE_CONTROL;

typedef struct UX_HOST_CLASS_HID_CLIENT_REMOTE_CONTROL_STRUCT
{
    UX_HOST_CLASS_HID_REMOTE_CONTROL
                    ux_host_class_hid_client_remote_control_remote_control;
    UX_HOST_CLASS_HID_CLIENT
                    ux_host_class_hid_client_remote_control_client;
} UX_HOST_CLASS_HID_CLIENT_REMOTE_CONTROL;

/* Define HID Remote Control Class function prototypes.  */

VOID    _ux_host_class_hid_remote_control_callback(UX_HOST_CLASS_HID_REPORT_CALLBACK *callback);
UINT    _ux_host_class_hid_remote_control_activate(UX_HOST_CLASS_HID_CLIENT_COMMAND *command);
UINT    _ux_host_class_hid_remote_control_deactivate(UX_HOST_CLASS_HID_CLIENT_COMMAND *command);
UINT    _ux_host_class_hid_remote_control_entry(UX_HOST_CLASS_HID_CLIENT_COMMAND *command);
UINT    _ux_host_class_hid_remote_control_usage_get(UX_HOST_CLASS_HID_REMOTE_CONTROL *remote_control_instance, ULONG *usage, ULONG *value);

UINT    _uxe_host_class_hid_remote_control_usage_get(UX_HOST_CLASS_HID_REMOTE_CONTROL *remote_control_instance, ULONG *usage, ULONG *value);


/* Define HID Keyboard Class API prototypes.  */

#define ux_host_class_hid_remote_control_entry                   _ux_host_class_hid_remote_control_entry

#if defined(UX_DEVICE_CLASS_HID_REMOTE_CONTROL_ENABLE_ERROR_CHECKING)

#define ux_host_class_hid_remote_control_usage_get               _uxe_host_class_hid_remote_control_usage_get

#else

#define ux_host_class_hid_remote_control_usage_get               _ux_host_class_hid_remote_control_usage_get

#endif

/* Determine if a C++ compiler is being used.  If so, complete the standard 
   C conditional started above.  */   
#ifdef __cplusplus
} 
#endif 

#endif

