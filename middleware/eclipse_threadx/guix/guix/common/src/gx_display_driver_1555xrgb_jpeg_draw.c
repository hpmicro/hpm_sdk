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
/**   Display Management (Display)                                        */
/**                                                                       */
/**************************************************************************/

#define GX_SOURCE_CODE


/* Include necessary system files.  */

#include "gx_api.h"
#include "gx_display.h"
#include "gx_image_reader.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_dislay_driver_565rgb_jpeg_draw                  PORTABLE C      */
/*                                                           6.3.0        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    565rgb screen driver JPEG image draw routine.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_image_reader_jpeg_mcu_decode                                    */
/*    [_gx_display_driver_1555xrgb_mcu_draw]                              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Kenneth Maxwell          Initial Version 6.0           */
/*  09-30-2020     Kenneth Maxwell          Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*  10-31-2023     Ting Zhu                 Modified comment(s),          */
/*                                            removed a parameter from    */
/*                                            jpeg mcu decode function,   */
/*                                            resulting in version 6.3.0  */
/*                                                                        */
/**************************************************************************/
#if defined(GX_SOFTWARE_DECODER_SUPPORT)
VOID _gx_display_driver_1555xrgb_jpeg_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{

    _gx_image_reader_jpeg_mcu_decode(pixelmap -> gx_pixelmap_data,
                                     pixelmap -> gx_pixelmap_data_size,
                                     context, xpos, ypos);
}
#endif

