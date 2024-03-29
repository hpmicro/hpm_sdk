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
/**   System                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_system.h"
#include "ux_host_stack.h"
#include "ux_device_stack.h"


#if defined(UX_STANDALONE) || defined(UX_HOST_STANDALONE) || defined(UX_DEVICE_STANDALONE) || defined(UX_OTG_STANDALONE)
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_system_tasks_run                                PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function runs USB system tasks, including possible tasks for   */
/*    host, device and OTG.                                               */
/*                                                                        */
/*    It's for standalone mode.                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _ux_device_stack_tasks_run            Run device tasks              */
/*    _ux_host_stack_tasks_run              Run host tasks                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  01-31-2022     Chaoqiong Xiao           Initial Version 6.1.10        */
/*                                                                        */
/**************************************************************************/
UINT _ux_system_tasks_run(VOID)
{
#if defined(UX_DEVICE_STANDALONE) && !defined(UX_HOST_SIDE_ONLY)
    _ux_device_stack_tasks_run();
#endif
#if defined(UX_HOST_STANDALONE) && !defined(UX_DEVICE_SIDE_ONLY)
    _ux_host_stack_tasks_run();
#endif
#if defined(UX_OTG_STANDALONE) && defined(UX_OTG_SUPPORT)
    _ux_otg_tasks_run();
#endif

   /* Return code not used now.  */
   return(0);
}
#endif
