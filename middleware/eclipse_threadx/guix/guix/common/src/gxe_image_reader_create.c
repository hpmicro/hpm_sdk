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
/** GUIX Component                                                        */
/**                                                                       */
/**   Image Reader Management (Image Reader)                              */
/**                                                                       */
/**************************************************************************/

#define GX_SOURCE_CODE


/* Include necessary system files.  */

#include "gx_api.h"
#include "gx_image_reader.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gxe_image_reader_create                            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in creating an image reader         */
/*    instance.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    image_reader              Image reader control block.               */
/*    read_data                 pointer to raw input data.                */
/*    read_data_size            size of raw input data.                   */
/*    color_format              the requested output color format         */
/*    mode                      compress dither and alpha modes flags     */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                    Completion status                         */
/*                                                                        */
/*  CALLS                                                                 */
/*   _gx_image_reader_create    Actual image reader create call.          */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Kenneth Maxwell          Initial Version 6.0           */
/*  09-30-2020     Kenneth Maxwell          Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
UINT  _gxe_image_reader_create(GX_IMAGE_READER *image_reader,
                               GX_CONST GX_UBYTE *read_data,
                               INT read_data_size,
                               GX_UBYTE color_format,
                               GX_UBYTE mode)
{
UINT status;

    /* Check for invalid input pointers.  */
    if ((image_reader == GX_NULL) || (read_data == GX_NULL))
    {
        return(GX_PTR_ERROR);
    }

    if (read_data_size <= 20)
    {
        return(GX_INVALID_VALUE);
    }

    /* Call the actual image reader info set function.  */
    status = _gx_image_reader_create(image_reader, read_data, read_data_size, color_format, mode);

    /* Return completion status.  */
    return status;
}
#endif

