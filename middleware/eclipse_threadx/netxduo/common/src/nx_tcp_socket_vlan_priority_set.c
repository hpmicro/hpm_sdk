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
/** NetX Component                                                        */
/**                                                                       */
/**   Transmission Control Protocol (TCP)                                 */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define NX_SOURCE_CODE


/* Include necessary system files.  */

#include "nx_api.h"
#include "nx_tcp.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_tcp_socket_vlan_priority_set                    PORTABLE C      */
/*                                                           6.4.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yajun Xia, Microsoft Corporation                                    */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is used for settting tcp socket vlan priority.        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    socket_ptr                            Pointer to TCP socket         */
/*    vlan_priority                         Vlan priority                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Actual completion status      */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  12-31-2023     Yajun Xia                Initial Version 6.4.0         */
/*                                                                        */
/**************************************************************************/
UINT _nx_tcp_socket_vlan_priority_set(NX_TCP_SOCKET *socket_ptr, UINT vlan_priority)
{
#ifdef NX_ENABLE_VLAN
    socket_ptr -> nx_tcp_socket_vlan_priority = (UCHAR)(vlan_priority & 0xFF);

    return(NX_SUCCESS);
#else
    NX_PARAMETER_NOT_USED(socket_ptr);
    NX_PARAMETER_NOT_USED(vlan_priority);

    return(NX_NOT_SUPPORTED);
#endif /* NX_ENABLE_VLAN */
}
