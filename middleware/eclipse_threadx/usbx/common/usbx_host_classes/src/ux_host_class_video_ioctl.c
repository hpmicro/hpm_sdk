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
/**   VIDEO Class                                                         */
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
/*    _ux_host_class_video_ioctl                          PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the ioctl entry point for the application to       */
/*    configure the video class based device.                             */
/*                                                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    video                                 Pointer to video class        */
/*    ioctl_function                        Ioctl function                */
/*    parameter                             Pointer to structure          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_video_format_data_get  Get video format data.        */
/*    _ux_host_class_video_frame_data_get   Get video frame data          */
/*    _ux_host_class_video_frame_interval_get                             */
/*                                          Get video frame internal data.*/
/*    _ux_host_class_video_channel_start    Start the video.              */
/*    _ux_host_class_video_stop             Stop the video.               */
/*    _ux_host_stack_endpoint_transfer_abort                              */
/*                                          Abort the transfer            */
/*    _ux_system_error_handler              Log system error              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  07-29-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            reset indexes of requests   */
/*                                            ring when it is aborted,    */
/*                                            resulting in version 6.1.12 */
/*  10-31-2023     Yajun xia                Modified comment(s),          */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_video_ioctl(UX_HOST_CLASS_VIDEO *video, ULONG ioctl_function,
                                 VOID *parameter)
{

UINT                                            status;
UX_HOST_CLASS_VIDEO_PARAMETER_INPUT_TERMINAL    *input_terminal;
UX_HOST_CLASS_VIDEO_PARAMETER_NUMBER_FORMATS    *number_formats;
UX_HOST_CLASS_VIDEO_PARAMETER_FORMAT_DATA       *format_parameter;
UX_HOST_CLASS_VIDEO_PARAMETER_FRAME_DATA        *frame_parameter;
UX_HOST_CLASS_VIDEO_PARAMETER_CHANNEL           *channel_parameter;
UX_HOST_CLASS_VIDEO_PARAMETER_FRAME_INTERVAL    *interval_parameter;

    /* Ensure the instance is valid.  */
    if ((video -> ux_host_class_video_state !=  UX_HOST_CLASS_INSTANCE_LIVE) &&
        (video -> ux_host_class_video_state !=  UX_HOST_CLASS_INSTANCE_MOUNTING))
    {

        /* If trace is enabled, insert this event into the trace buffer.  */
        //UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_HOST_CLASS_INSTANCE_UNKNOWN, video, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return(UX_HOST_CLASS_INSTANCE_UNKNOWN);
    }

    /* The command request will tell us what we need to do here.  */
    switch (ioctl_function)
    {

    case UX_HOST_CLASS_VIDEO_IOCTL_GET_INPUT_TERMINAL:

        /* Set status to error by default.  */
        status = UX_ERROR;

        /* Is the video terminal defined ? */
        if (video -> ux_host_class_video_terminal_id != 0)
        {

            /* Cast answer.  */
            input_terminal = (UX_HOST_CLASS_VIDEO_PARAMETER_INPUT_TERMINAL *) parameter;

            /* Return input terminal id.  */
            input_terminal -> ux_host_class_video_parameter_input_terminal_id = video -> ux_host_class_video_terminal_id;

            /* Return input terminal type.  */
            input_terminal -> ux_host_class_video_parameter_input_terminal_type = video -> ux_host_class_video_terminal_type;

            /* Status ok.  */
            status = UX_SUCCESS;
        }

        break;

    case UX_HOST_CLASS_VIDEO_IOCTL_GET_FORMAT_NUMBER:

        /* Cast answer.  */
        number_formats = (UX_HOST_CLASS_VIDEO_PARAMETER_NUMBER_FORMATS *) parameter;

        /* Save the number of formats.  */
        number_formats -> ux_host_class_video_parameter_number_formats = video -> ux_host_class_video_number_formats;

        /* Status ok.  */
        status = UX_SUCCESS;

        break;

    case UX_HOST_CLASS_VIDEO_IOCTL_GET_FORMAT_DATA:

        /* Cast answer.  */
        format_parameter = (UX_HOST_CLASS_VIDEO_PARAMETER_FORMAT_DATA *) parameter;

        /* Get the format data for the format index requested.  */
        status = _ux_host_class_video_format_data_get(video, format_parameter);
        break;

    case UX_HOST_CLASS_VIDEO_IOCTL_GET_FRAME_DATA:

        /* Cast answer.  */
        frame_parameter = (UX_HOST_CLASS_VIDEO_PARAMETER_FRAME_DATA *) parameter;

        /* Get the frame data for the frame index requested.  */
        status = _ux_host_class_video_frame_data_get(video, frame_parameter);
        break;

    case UX_HOST_CLASS_VIDEO_IOCTL_GET_FRAME_INTERVAL:

        /* Cast answer.  */
        interval_parameter = (UX_HOST_CLASS_VIDEO_PARAMETER_FRAME_INTERVAL *) parameter;

        /* Get the frame intervals for the frame index requested.  */
        status = _ux_host_class_video_frame_interval_get(video, interval_parameter);
        break;

    case UX_HOST_CLASS_VIDEO_IOCTL_CHANNEL_START:

        /* Cast answer.  */
        channel_parameter = (UX_HOST_CLASS_VIDEO_PARAMETER_CHANNEL *) parameter;

        /* Start the channel for reading video input.  */
        status = _ux_host_class_video_channel_start(video, channel_parameter);
        break;

    case UX_HOST_CLASS_VIDEO_IOCTL_CHANNEL_STOP:

        /* Stop the channel.  */
        status = _ux_host_class_video_stop(video);
        break;


    case UX_HOST_CLASS_VIDEO_IOCTL_ABORT_IN_PIPE :

        /* If trace is enabled, insert this event into the trace buffer.  */
        //UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_CLASS_VIDEO_IOCTL_ABORT_IN_PIPE, video, video -> ux_host_class_video_isochronous_endpoint, 0, 0, UX_TRACE_HOST_CLASS_EVENTS, 0, 0)

        /* We need to abort transactions on the bulk In pipe.  */
        _ux_host_stack_endpoint_transfer_abort(video -> ux_host_class_video_isochronous_endpoint);

        /* All linked requests are aborted, reset indexes.  */
        video -> ux_host_class_video_transfer_request_start_index = 0;
        video -> ux_host_class_video_transfer_request_end_index = 0;

        /* Status is successful.  */
        status = UX_SUCCESS;
        break;

    default:

        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_FUNCTION_NOT_SUPPORTED);

        /* If trace is enabled, insert this event into the trace buffer.  */
        //UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_FUNCTION_NOT_SUPPORTED, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)

        /* Function not supported. Return an error.  */
        status =  UX_FUNCTION_NOT_SUPPORTED;
    }

    /* Return status to caller.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_video_ioctl                         PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yajun Xia, Microsoft Corporation                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in video ioctl function call.           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    video                                 Pointer to video class        */
/*    ioctl_function                        Ioctl function                */
/*    parameter                             Pointer to structure          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_video_ioctl            Video ioctl                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2023        Yajun xia             Initial Version 6.3.0         */
/*                                                                        */
/**************************************************************************/
UINT  _uxe_host_class_video_ioctl(UX_HOST_CLASS_VIDEO *video, ULONG ioctl_function,
                                 VOID *parameter)
{

    /* Sanity checks.  */
    if (video == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Call the actual video ioctl function.  */
    return(_ux_host_class_video_ioctl(video, ioctl_function, parameter));
}