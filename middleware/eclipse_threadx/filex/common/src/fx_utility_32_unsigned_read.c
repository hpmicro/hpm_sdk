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
/**   Utility                                                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define FX_SOURCE_CODE


/* Include necessary system files.  */

#include "fx_api.h"
#include "fx_system.h"
#include "fx_utility.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _fx_utility_32_unsigned_read                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function reads (with endian awareness) a 32-bit unsigned data  */
/*    from the specified source and returns the value to the caller.      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    source_ptr                            Source memory pointer         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    ULONG value                                                         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    FileX System Functions                                              */
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
ULONG  _fx_utility_32_unsigned_read(UCHAR *source_ptr)
{

ULONG value;

    /* Pickup the UINT from the destination with endian-awareness.  */
    value =  ((((ULONG) *(source_ptr+3)) & 0xFF) << 24) |
             ((((ULONG) *(source_ptr+2)) & 0xFF) << 16) |
             ((((ULONG) *(source_ptr+1)) & 0xFF) << 8)  |
              (((ULONG) *(source_ptr)) & 0xFF);

    /* Return value to caller.  */
    return(value);
}

