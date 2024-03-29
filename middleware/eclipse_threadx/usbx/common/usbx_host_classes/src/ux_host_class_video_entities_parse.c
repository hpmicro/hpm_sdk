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
/**   Video Class                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_video.h"
#include "ux_host_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_video_entities_parse                 PORTABLE C      */
/*                                                           6.1.12       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function parsees all interface descriptors and their video     */
/*    class specific descriptors.                                         */
/*                                                                        */
/*    The function scans the device configuration descriptor. Once the    */
/*    interface descriptors of the video instance are found, each video   */
/*    class descriptors belong to the interface are passed to parse       */
/*    function for caller one by one to parse. The interface descriptor   */
/*    is also passed to provide more information. Another pointer to      */
/*    arguments is also available as parse function parameter for caller  */
/*    to pass necessary caller specific data to process in parse function.*/
/*                                                                        */
/*    The descriptor parsing can be terminated by returning non-zero      */
/*    code in parse function.                                             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    video                                 Pointer to video instance     */
/*    parse_function                        Parse function for each       */
/*                                          video class descriptor        */
/*    arg                                   Parse function argument       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Video Class                                                         */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     Chaoqiong Xiao           Initial Version 6.1           */
/*  08-02-2021     Wen Wang                 Modified comment(s),          */
/*                                            fixed spelling error,       */
/*                                            resulting in version 6.1.8  */
/*  07-29-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            fixed parameter/variable    */
/*                                            names conflict C++ keyword, */
/*                                            resulting in version 6.1.12 */
/*                                                                        */
/**************************************************************************/
UINT _ux_host_class_video_entities_parse(UX_HOST_CLASS_VIDEO *video,
        UINT(*parse_function)(VOID  *arg,
                              UCHAR *packed_interface_descriptor,
                              UCHAR *packed_entity_descriptor),
        VOID* arg)
{

UCHAR                                           *descriptor;
UCHAR                                           *interface_descriptor;
UX_INTERFACE                                    *interface_ptr;
ULONG                                           total_descriptor_length;
ULONG                                           descriptor_length;
ULONG                                           descriptor_type;
UINT                                            status;


    /* Device state check.  */
    if (video -> ux_host_class_video_device -> ux_device_state != UX_DEVICE_CONFIGURED)
        return(UX_HOST_CLASS_VIDEO_DEVICE_NOT_READY);

    /* Get the descriptor to the selected format.  */
    descriptor =  video -> ux_host_class_video_configuration_descriptor;
    total_descriptor_length =  video -> ux_host_class_video_configuration_descriptor_length;

    /* Haven't found interface yet.  */
    interface_descriptor = UX_NULL;

    /* Scan the descriptor for the Video Streaming interface.  */
    while (total_descriptor_length)
    {

        /* Gather the length, type and subtype of the descriptor.  */
        descriptor_length =   *descriptor;
        descriptor_type =     *(descriptor + 1);

        /* Make sure this descriptor has at least the minimum length.  */
        if (descriptor_length < 3)
            return(UX_DESCRIPTOR_CORRUPTED);

        /* Process relative to descriptor type.  */
        switch (descriptor_type)
        {

            case UX_INTERFACE_DESCRIPTOR_ITEM:

                /* Haven't found it.  */
                interface_descriptor = UX_NULL;

                /* Ensure we have the correct interface for Video Control/Streaming.  */
                if (descriptor[5] == UX_HOST_CLASS_VIDEO_CLASS)
                {

                    /* VideoControl Interface.  */
                    if (descriptor[6] == UX_HOST_CLASS_VIDEO_SUBCLASS_CONTROL)

                        /* Mark we have found it.  */
                        interface_descriptor = descriptor;
                    else
                    {

                        /* VideoStreaming interface, currently only one supported.  */
                        interface_ptr = video -> ux_host_class_video_streaming_interface;
                        if (descriptor[2] == interface_ptr -> ux_interface_descriptor.bInterfaceNumber)

                            /* Mark we have found it.  */
                            interface_descriptor = descriptor;
                    }
                }
                break;

            case UX_HOST_CLASS_VIDEO_CS_INTERFACE:

                /* Have we found the video interface yet?  */
                if (interface_descriptor != UX_NULL)
                {

                    /* Yes, parse the entity descriptor.  */
                    status = parse_function(arg, interface_descriptor, descriptor);

                    /* Terminate the parsing if status is not 0.  */
                    if (status)
                        return(UX_SUCCESS);
                }
                break;
        }

        /* Verify if the descriptor is still valid.  */
        if (descriptor_length > total_descriptor_length)
            return(UX_DESCRIPTOR_CORRUPTED);

        /* Jump to the next descriptor if we have not reached the end.  */
        descriptor +=  descriptor_length;

        /* And adjust the length left to parse in the descriptor.  */
        total_descriptor_length -=  descriptor_length;
    }

    /* We get here when all descriptors scanned.  */
    return(UX_SUCCESS);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_video_entities_parse                PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yajun Xia, Microsoft Corporation                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in video entities parse function call.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    video                                 Pointer to video instance     */
/*    parse_function                        Parse function for each       */
/*                                          video class descriptor        */
/*    arg                                   Parse function argument       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_video_entities_parse   Video entities parse          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Video Class                                                         */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2023        Yajun xia             Initial Version 6.3.0         */
/*                                                                        */
/**************************************************************************/
UINT _uxe_host_class_video_entities_parse(UX_HOST_CLASS_VIDEO *video,
        UINT(*parse_function)(VOID  *arg,
                              UCHAR *packed_interface_descriptor,
                              UCHAR *packed_entity_descriptor),
        VOID* arg)
{

    /* Sanity checks.  */
    if (video == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Call the actual video entities parse function.  */
    return(_ux_host_class_video_entities_parse(video, parse_function, arg));
}