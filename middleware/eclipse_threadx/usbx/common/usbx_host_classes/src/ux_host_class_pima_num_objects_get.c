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
/**   PIMA Class                                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_host_class_pima.h"
#include "ux_host_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_host_class_pima_num_objects_get                 PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function returns the number of objects for a specific storage. */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pima                                       Pointer to pima class    */
/*    pima_session                               Pointer to pima session  */
/*    storage_id                                 The storage ID           */
/*    object_format_code                         The object format code   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  _ux_host_class_pima_command                 Pima command function     */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    USB application                                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  10-31-2023     Yajun xia                Modified comment(s),          */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
UINT  _ux_host_class_pima_num_objects_get(UX_HOST_CLASS_PIMA *pima,
                                        UX_HOST_CLASS_PIMA_SESSION *pima_session,
                                        ULONG storage_id,
                                        ULONG object_format_code)
{

UX_HOST_CLASS_PIMA_COMMAND             command;
UINT                                status;

    /* If trace is enabled, insert this event into the trace buffer.  */
    UX_TRACE_IN_LINE_INSERT(UX_TRACE_HOST_CLASS_PIMA_NUM_OBJECTS_GET, pima, 0, 0, 0, UX_TRACE_HOST_CLASS_EVENTS, 0, 0)

    /* Check if this session is valid or not.  */
    if (pima_session -> ux_host_class_pima_session_magic != UX_HOST_CLASS_PIMA_MAGIC_NUMBER)
        return (UX_HOST_CLASS_PIMA_RC_SESSION_NOT_OPEN);

    /* Check if this session is opened or not.  */
    if (pima_session -> ux_host_class_pima_session_state != UX_HOST_CLASS_PIMA_SESSION_STATE_OPENED)
        return (UX_HOST_CLASS_PIMA_RC_SESSION_NOT_OPEN);

    /* Issue command to get the storage IDs.  2 parameter.  */
    command.ux_host_class_pima_command_nb_parameters =  2;

    /* Parameter 1 is the Storage ID.  */
    command.ux_host_class_pima_command_parameter_1 =  storage_id;

    /* Parameter 2 is the Object Format Code.  */
    command.ux_host_class_pima_command_parameter_2 =  object_format_code;

    /* Other parameters unused.  */
    command.ux_host_class_pima_command_parameter_3 =  0;
    command.ux_host_class_pima_command_parameter_4 =  0;
    command.ux_host_class_pima_command_parameter_5 =  0;

    /* Then set the command to GET_NUM_OBJETCS.  */
    command.ux_host_class_pima_command_operation_code =  UX_HOST_CLASS_PIMA_OC_GET_NUM_OBJECTS;

    /* Issue the command.  */
    status = _ux_host_class_pima_command(pima, &command, 0 , UX_NULL, 0, 0);

    /* Check the result. If the result is OK, the parameter1 has the number of objects. */
    if (status == UX_SUCCESS)

        /* Store the number of objects into the current pima session.  */
        pima_session -> ux_host_class_pima_session_nb_objects =  command.ux_host_class_pima_command_parameter_1;

    /* Return completion status.  */
    return(status);
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _uxe_host_class_pima_num_objects_get                PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yajun Xia, Microsoft Corporation                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks errors in pima object number get function      */
/*    call.                                                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    pima                                       Pointer to pima class    */
/*    pima_session                               Pointer to pima session  */
/*    storage_id                                 The storage ID           */
/*    object_format_code                         The object format code   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_host_class_pima_num_objects_get   Get pima object number        */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    USB application                                                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  10-31-2023        Yajun xia             Initial Version 6.3.0         */
/*                                                                        */
/**************************************************************************/
UINT  _uxe_host_class_pima_num_objects_get(UX_HOST_CLASS_PIMA *pima,
                                        UX_HOST_CLASS_PIMA_SESSION *pima_session,
                                        ULONG storage_id,
                                        ULONG object_format_code)
{

    /* Sanity checks.  */
    if ((pima == UX_NULL) || (pima_session == UX_NULL))
        return(UX_INVALID_PARAMETER);

    /* Call the actual pima num objects get function.  */
    return(_ux_host_class_pima_num_objects_get(pima, pima_session, storage_id, object_format_code));
}