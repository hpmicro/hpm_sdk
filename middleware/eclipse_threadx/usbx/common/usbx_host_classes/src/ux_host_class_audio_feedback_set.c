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
/**   Audio Class                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_audio.h"
#include "ux_host_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_audio_feedback_set                   PORTABLE C      */
/*                                                           6.1.12       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function copies packed feedback data from given buffer.        */
/*    Note high speed data size is 4 and full speed data size is 3 bytes. */
/*                                                                        */
/*    Note if feedback is supported this function can work even there is  */
/*    no interface alternate setting selected. This way the default       */
/*    feedback data can be set before streaming starts.                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    audio                                 Pointer to audio class        */
/*    feedback                              Pointer to packed feedback    */
/*                                          data to copy                  */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  07-29-2022     Chaoqiong Xiao           Initial Version 6.1.12        */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_audio_feedback_set(UX_HOST_CLASS_AUDIO *audio, UCHAR *feedback)
{
#if !defined(UX_HOST_CLASS_AUDIO_FEEDBACK_SUPPORT)
    UX_PARAMETER_NOT_USED(audio);
    UX_PARAMETER_NOT_USED(feedback);
    return(UX_FUNCTION_NOT_SUPPORTED);
#else
UX_ENDPOINT         *endpoint;
UX_TRANSFER         *transfer;

    /* Ensure the instance is valid.  */
    if (_ux_host_stack_class_instance_verify(_ux_system_host_class_audio_name, (VOID *) audio) != UX_SUCCESS)
    {

        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_HOST_CLASS_INSTANCE_UNKNOWN);

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_HOST_CLASS_INSTANCE_UNKNOWN, audio, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return(UX_HOST_CLASS_INSTANCE_UNKNOWN);
    }

    audio -> ux_host_class_audio_feedback_buffer[0] = feedback[0];
    audio -> ux_host_class_audio_feedback_buffer[1] = feedback[1];
    audio -> ux_host_class_audio_feedback_buffer[2] = feedback[2];
    if (_ux_host_class_audio_speed_get(audio) == UX_HIGH_SPEED_DEVICE)
        audio -> ux_host_class_audio_feedback_buffer[3] = feedback[3];

    /* Sync with transfer buffer.  */
    endpoint = audio -> ux_host_class_audio_feedback_endpoint;
    if (endpoint)
    {
        transfer = &endpoint -> ux_endpoint_transfer_request;
        _ux_utility_memory_copy(transfer -> ux_transfer_request_data_pointer,
                                audio -> ux_host_class_audio_feedback_buffer,
                                sizeof(audio -> ux_host_class_audio_feedback_buffer)); /* Use case of memcpy is verified. */
    }

    return(UX_SUCCESS);
#endif
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_audio_feedback_set                  PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in audio feedback set function call.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    audio                                 Pointer to audio class        */
/*    feedback                              Pointer to buffer to fill     */
/*                                          packed feedback data          */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _uxe_host_class_audio_feedback_set    Set audio feedback            */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2023     Chaoqiong Xiao           Initial Version 6.3.0         */
/*                                                                        */
/**************************************************************************/
UINT  _uxe_host_class_audio_feedback_set(UX_HOST_CLASS_AUDIO *audio, UCHAR *feedback)
{

    /* Sanity checks.  */
    if ((audio == UX_NULL) || (feedback == UX_NULL))
        return(UX_INVALID_PARAMETER);

    /* Invoke audio feedback set function.  */
    return(_ux_host_class_audio_feedback_set(audio, feedback));
}
