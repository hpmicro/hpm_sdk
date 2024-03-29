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

/*
    VS_FRAME_UNCOMPRESSED  : wWidth @ 5, wHeight @ 7, dwDefaultFrameInterval @ 21, bFrameIntervalType @ 25, intervals @ 26
    VS_FRAME_MJPEG         : wWidth @ 5, wHeight @ 7, dwDefaultFrameInterval @ 21, bFrameIntervalType @ 25, intervals @ 26
    VS_FRAME_FRAME_BASED   : wWidth @ 5, wHeight @ 7, dwDefaultFrameInterval @ 17, bFrameIntervalType @ 21, intervals @ 26
    VS_FRAME_H264          : wWidth @ 4, wHeight @ 6, dwDefaultFrameInterval @ 39, bNumFrameIntervals @ 43, intervals @ 44
    VS_FRAME_VP8           : wWidth @ 4, wHeight @ 6, dwDefaultFrameInterval @ 26, bNumFrameIntervals @ 30, intervals @ 31
*/
static const UCHAR _ux_host_class_video_frame_descriptor_offsets[][5] =
{
    { 5, 7, 21, 25, 26 },
    { 5, 7, 17, 21, 26 },
    { 4, 6, 39, 43, 44 },
    { 4, 6, 26, 30, 31 },
};
#define _OFFSETS_UNCOMPRESSED_MJPEG             0
#define _OFFSETS_FRAME_BASED                    1
#define _OFFSETS_H264                           2
#define _OFFSETS_VP8                            3


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_video_frame_data_get                 PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function finds the frame data within the input terminal.       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    video                                 Pointer to video class        */
/*    frame_data                            Frame request structure       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_system_error_handler              Log system error              */
/*    _ux_utility_descriptor_parse          Parse descriptor              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Video Class                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  10-31-2023     Chaoqiong Xiao           Modified comment(s),          */
/*                                            improved extracted data,    */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_video_frame_data_get(UX_HOST_CLASS_VIDEO *video, UX_HOST_CLASS_VIDEO_PARAMETER_FRAME_DATA *frame_parameter)
{

UCHAR                                           *descriptor;
ULONG                                           total_descriptor_length;
ULONG                                           descriptor_length;
ULONG                                           descriptor_type;
ULONG                                           descriptor_subtype;
UCHAR                                           *field_offsets;


    /* Get the descriptor to the selected format.  */
    descriptor =  video -> ux_host_class_video_current_format_address;
    total_descriptor_length =  video -> ux_host_class_video_length_formats;

    /* Descriptors are arranged in order. First FORMAT then FRAME.  */
    while (total_descriptor_length)
    {

        /* Gather the length, type and subtype of the descriptor.  */
        descriptor_length =   *descriptor;
        descriptor_type =     *(descriptor + 1);
        descriptor_subtype =  *(descriptor + 2);

        /* Make sure this descriptor has at least the minimum length.  */
        if (descriptor_length < 3)
        {

            /* Error trap. */
            _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_DESCRIPTOR_CORRUPTED);

            /* If trace is enabled, insert this event into the trace buffer.  */
            UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_DESCRIPTOR_CORRUPTED, descriptor, 0, 0, UX_TRACE_ERRORS, 0, 0)

            return(UX_DESCRIPTOR_CORRUPTED);
        }

        /* Must be CS_INTERFACE.  */
        if (descriptor_type == UX_HOST_CLASS_VIDEO_CS_INTERFACE)
        {

            /* Process relative to descriptor type.  */
            switch (descriptor_subtype)
            {

            case UX_HOST_CLASS_VIDEO_VS_FRAME_UNCOMPRESSED  : /* wWidth @ 5, wHeight @ 7, dwDefaultFrameInterval @ 21, bFrameIntervalType @ 25, intervals @ 26  */
            case UX_HOST_CLASS_VIDEO_VS_FRAME_MJPEG         : /* wWidth @ 5, wHeight @ 7, dwDefaultFrameInterval @ 21, bFrameIntervalType @ 25, intervals @ 26  */
            case UX_HOST_CLASS_VIDEO_VS_FRAME_FRAME_BASED   : /* wWidth @ 5, wHeight @ 7, dwDefaultFrameInterval @ 17, bFrameIntervalType @ 21, intervals @ 26  */
            case UX_HOST_CLASS_VIDEO_VS_FRAME_H264          : /* wWidth @ 4, wHeight @ 6, dwDefaultFrameInterval @ 39, bNumFrameIntervals @ 43, intervals @ 44  */
            case UX_HOST_CLASS_VIDEO_VS_FRAME_VP8           : /* wWidth @ 4, wHeight @ 6, dwDefaultFrameInterval @ 26, bNumFrameIntervals @ 30, intervals @ 31  */

                /* We found a Frame descriptor.  Is it the right one ? */
                if (frame_parameter -> ux_host_class_video_parameter_frame_requested == *(descriptor + 3))
                {

                    /* Get key offsets.  */
                    if (descriptor_subtype == UX_HOST_CLASS_VIDEO_VS_FRAME_FRAME_BASED)
                        field_offsets = (UCHAR*)_ux_host_class_video_frame_descriptor_offsets[_OFFSETS_FRAME_BASED];
                    else if (descriptor_subtype == UX_HOST_CLASS_VIDEO_VS_FRAME_H264)
                        field_offsets = (UCHAR*)_ux_host_class_video_frame_descriptor_offsets[_OFFSETS_H264];
                    else if (descriptor_subtype == UX_HOST_CLASS_VIDEO_VS_FRAME_VP8)
                        field_offsets = (UCHAR*)_ux_host_class_video_frame_descriptor_offsets[_OFFSETS_VP8];
                    else
                        field_offsets = (UCHAR*)_ux_host_class_video_frame_descriptor_offsets[_OFFSETS_UNCOMPRESSED_MJPEG];

                    /* Save the frame subtype.  */
                    frame_parameter -> ux_host_class_video_parameter_frame_subtype =  descriptor_type;

                    /* Save useful frame parameters. */
                    frame_parameter -> ux_host_class_video_parameter_frame_width = _ux_utility_short_get(descriptor + field_offsets[0]);
                    frame_parameter -> ux_host_class_video_parameter_frame_height = _ux_utility_short_get(descriptor + field_offsets[1]);
                    frame_parameter -> ux_host_class_video_parameter_default_frame_interval = _ux_utility_long_get(descriptor + field_offsets[2]);
                    frame_parameter -> ux_host_class_video_parameter_frame_interval_type = *(descriptor + field_offsets[3]);
                    frame_parameter -> ux_host_class_video_parameter_frame_intervals = (descriptor + field_offsets[4]);

                    video -> ux_host_class_video_current_frame_address = descriptor;
                    video -> ux_host_class_video_current_frame_interval = frame_parameter -> ux_host_class_video_parameter_default_frame_interval;

                    /* We are done here. */
                    return(UX_SUCCESS);
                }
                break;
            }
        }

        /* Verify if the descriptor is still valid.  */
        if (descriptor_length > total_descriptor_length)
        {

            /* Error trap. */
            _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_DESCRIPTOR_CORRUPTED);

            /* If trace is enabled, insert this event into the trace buffer.  */
            UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_DESCRIPTOR_CORRUPTED, descriptor, 0, 0, UX_TRACE_ERRORS, 0, 0)

            return(UX_DESCRIPTOR_CORRUPTED);
        }

        /* Jump to the next descriptor if we have not reached the end.  */
        descriptor +=  descriptor_length;

        /* And adjust the length left to parse in the descriptor.  */
        total_descriptor_length -=  descriptor_length;
    }

    /* Error trap. */
    _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_HOST_CLASS_VIDEO_WRONG_TYPE);

    /* We get here when either the report descriptor has a problem or we could
       not find the right video device.  */
    return(UX_HOST_CLASS_VIDEO_WRONG_TYPE);
}
