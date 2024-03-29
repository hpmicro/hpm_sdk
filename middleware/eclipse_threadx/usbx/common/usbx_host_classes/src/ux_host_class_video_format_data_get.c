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
/*    _ux_host_class_video_format_data_get                PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function finds the format data within the input terminal.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    video                                 Pointer to video class        */
/*    format_parameter                      Format Parameter structure    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_system_error_handler              Log system error              */
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
UINT  _ux_host_class_video_format_data_get(UX_HOST_CLASS_VIDEO *video, UX_HOST_CLASS_VIDEO_PARAMETER_FORMAT_DATA *format_parameter)
{

UCHAR                                           *descriptor;
ULONG                                           total_descriptor_length;
ULONG                                           descriptor_length;
ULONG                                           descriptor_type;
ULONG                                           descriptor_subtype;
UCHAR                                           *guid;


    /* Get the descriptor to the selected format.  */
    descriptor =  video -> ux_host_class_video_format_address;
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
            guid = UX_NULL;
            switch (descriptor_subtype)
            {

                case UX_HOST_CLASS_VIDEO_VS_FORMAT_UNCOMPRESSED   : /* guidFormat @ 5  */
                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_FRAME_BASED    : /* guidFormat @ 5  */
                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_STREAM_BASED   : /* guidFormat @ 4, no frame  */
                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_MPEG2TS        : /* guidStrideFormat @ 7, no frame  */
                    if (descriptor_subtype == UX_HOST_CLASS_VIDEO_VS_FORMAT_STREAM_BASED)
                        guid = (descriptor + 4);
                    else if (descriptor_subtype == UX_HOST_CLASS_VIDEO_VS_FORMAT_MPEG2TS)
                        guid = (descriptor + 7);
                    else
                        guid = (descriptor + 5);

                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_MJPEG          : /* no GUID  */
                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_H264           : /* no GUID  */
                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_H264_SIMULCAST :
                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_VP8            : /* no GUID  */
                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_VP8_SIMULCAST  :
                /* Fall through.  */
                case UX_HOST_CLASS_VIDEO_VS_FORMAT_DV             : /* no GUID, no frame  */

                    /* We found a Format descriptor.  Is it the right one ? */
                    if (format_parameter -> ux_host_class_video_parameter_format_requested == *(descriptor + 3))
                    {

                        /* Save the subtype for this format.  */
                        format_parameter -> ux_host_class_video_parameter_format_subtype = descriptor_subtype;

                        /* Save the number of frame descriptors associated with this format.  */
                        if (descriptor_subtype != UX_HOST_CLASS_VIDEO_VS_FORMAT_STREAM_BASED &&
                            descriptor_subtype != UX_HOST_CLASS_VIDEO_VS_FORMAT_DV &&
                            descriptor_subtype != UX_HOST_CLASS_VIDEO_VS_FORMAT_MPEG2TS)
                            format_parameter -> ux_host_class_video_parameter_number_frame_descriptors = *(descriptor + 4);
                        else
                            format_parameter -> ux_host_class_video_parameter_number_frame_descriptors = 0;

                        /* Save the GUID of frame descriptor.  */
                        format_parameter -> ux_host_class_video_parameter_format_guid = guid;

                        /* Save the address of this format.  */
                        video -> ux_host_class_video_current_format_address = descriptor;

                        /* And the current format index.  */
                        video -> ux_host_class_video_current_format = format_parameter -> ux_host_class_video_parameter_format_requested;

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
