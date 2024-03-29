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
/*    _ux_host_class_audio_interrupt_start                PORTABLE C      */
/*                                                           6.1.12       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function starts audio control (AC) interface interrupt         */
/*    message polling.                                                    */
/*                                                                        */
/*    Note it should be called after audio control instance is activated  */
/*    and should be called only once before the instance is deactivated.  */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    audio                                 Pointer to audio control (AC) */
/*                                          instance                      */
/*    callback_function                     Callback function invoked on  */
/*                                          interrupt message reception   */
/*    arg                                   Callback argument passed to   */
/*                                          callback function             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_stack_class_instance_verify  Verify instance is valid      */
/*    _ux_host_stack_transfer_request       Process transfer request      */
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
UINT    _ux_host_class_audio_interrupt_start(UX_HOST_CLASS_AUDIO_AC *audio,
                        VOID(*callback_function)(UX_HOST_CLASS_AUDIO_AC *audio,
                                                 UCHAR *message, ULONG length,
                                                 VOID *arg),
                        VOID *arg)
{
#if !defined(UX_HOST_CLASS_AUDIO_INTERRUPT_SUPPORT)
    UX_PARAMETER_NOT_USED(audio);
    UX_PARAMETER_NOT_USED(callback_function);
    UX_PARAMETER_NOT_USED(arg);
    return(UX_FUNCTION_NOT_SUPPORTED);
#else
UX_ENDPOINT     *endpoint;
UX_TRANSFER     *transfer;
UINT            status;


    /* Ensure the instance is valid.  */
    if (_ux_host_stack_class_instance_verify(_ux_system_host_class_audio_name, (VOID *) audio) != UX_SUCCESS)
    {

        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_HOST_CLASS_INSTANCE_UNKNOWN);

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_HOST_CLASS_INSTANCE_UNKNOWN, audio, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return(UX_HOST_CLASS_INSTANCE_UNKNOWN);
    }

    /* Ensure endpoint is valid.  */
    endpoint = audio -> ux_host_class_audio_interrupt_endpoint;
    if (endpoint == UX_NULL)
        return(UX_FUNCTION_NOT_SUPPORTED);
    
    /* Check if interrupt started.  */
    if (audio -> ux_host_class_audio_interrupt_started)
        return(UX_ALREADY_ACTIVATED);

    /* Set application callback and its argument.  */
    audio -> ux_host_class_audio_interrupt_callback_arg = arg;
    audio -> ux_host_class_audio_interrupt_callback = callback_function;

    /* Get the transfer request.  */
    transfer = &endpoint -> ux_endpoint_transfer_request;

    /* Save class instance to issue this transfer.  */
    transfer -> ux_transfer_request_class_instance = (VOID *)audio;

    /* Save transfer complete callback.  */
    transfer -> ux_transfer_request_completion_function = _ux_host_class_audio_interrupt_notification;

    /* The transfer on the interrupt endpoint can be started.  */
    status =  _ux_host_stack_transfer_request(transfer);
    if (status == UX_SUCCESS)
        audio -> ux_host_class_audio_interrupt_started = UX_TRUE;

    /* Return completion status.  */
    return(status);
#endif
}


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_audio_interrupt_start               PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in audio interrupt start function call. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    audio                                 Pointer to audio control (AC) */
/*                                          instance                      */
/*    callback_function                     Callback function invoked on  */
/*                                          interrupt message reception   */
/*    arg                                   Callback argument passed to   */
/*                                          callback function             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Status                                                              */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _uxe_host_class_audio_interrupt_start Start audio interrupt polling */
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
UINT    _uxe_host_class_audio_interrupt_start(UX_HOST_CLASS_AUDIO_AC *audio,
                        VOID(*callback_function)(UX_HOST_CLASS_AUDIO_AC *audio,
                                                 UCHAR *message, ULONG length,
                                                 VOID *arg),
                        VOID *arg)
{

    /* Sanity checks.  */
    if (audio == UX_NULL)
        return(UX_INVALID_PARAMETER);

    /* Invoke interrupt start function.  */
    return(_ux_host_class_audio_interrupt_start(audio, callback_function, arg));
}
