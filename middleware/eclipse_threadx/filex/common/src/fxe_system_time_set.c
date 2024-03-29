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
/** FileX Component                                                       */
/**                                                                       */
/**   System                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define FX_SOURCE_CODE


/* Include necessary system files.  */

#include "fx_api.h"
#include "fx_system.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _fxe_system_time_set                                PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the set the system time call.    */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    hour                                  Hour (0-23)                   */
/*    minute                                Minute (0-59)                 */
/*    second                                Second (0-59)                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    FX_INVALID_HOUR                       Supplied hour is invalid      */
/*    FX_INVALID_MINUTE                     Supplied minute is invalid    */
/*    FX_INVALID_SECOND                     Supplied second is invalid    */
/*    status                                Actual completion status      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _fx_system_time_set                   Actual system time set call   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*  09-30-2020     William E. Lamie         Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _fxe_system_time_set(UINT hour, UINT minute, UINT second)
{

UINT status;


    /* Check for invalid hour.  */
    if (hour > FX_MAXIMUM_HOUR)
    {
        return(FX_INVALID_HOUR);
    }

    /* Check for invalid minute.  */
    if (minute > FX_MAXIMUM_MINUTE)
    {
        return(FX_INVALID_MINUTE);
    }

    /* Check for invalid second.  */
    if (second > FX_MAXIMUM_SECOND)
    {
        return(FX_INVALID_SECOND);
    }

    /* Call the actual set system time service.  */
    status =  _fx_system_time_set(hour, minute, second);

    /* Return status.  */
    return(status);
}

