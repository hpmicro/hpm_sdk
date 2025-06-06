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
#include "gx_utility.h"



/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_32bpp_rotated_canvas_copy        PORTABLE C      */
/*                                                           6.1.4        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Generic rotated 32bpp canvas copy function.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*   canvas                                 The canvas to copy from       */
/*   composite                              The canvas to copy to         */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_utility_rectangle_shift           Move the rectangle            */
/*    _gx_utility_rectangle_overlap_detect  Detect two rectangles being   */
/*                                            overlap to each other       */
/*    memcpy                                Move canvas data              */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  02-02-2021     Kenneth Maxwell          Initial Version 6.1.4         */
/*                                                                        */
/**************************************************************************/
VOID _gx_display_driver_32bpp_rotated_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite)
{
GX_RECTANGLE dirty;
GX_RECTANGLE overlap;
ULONG       *read;
ULONG       *write;
INT          width;
INT          row;

    dirty.gx_rectangle_left = dirty.gx_rectangle_top = 0;
    dirty.gx_rectangle_right = (GX_VALUE)(canvas -> gx_canvas_x_resolution - 1);
    dirty.gx_rectangle_bottom = (GX_VALUE)(canvas -> gx_canvas_y_resolution - 1);

    _gx_utility_rectangle_shift(&dirty, canvas -> gx_canvas_display_offset_x, canvas -> gx_canvas_display_offset_y);

    if (_gx_utility_rectangle_overlap_detect(&dirty, &composite -> gx_canvas_dirty_area, &overlap))
    {
        width = overlap.gx_rectangle_bottom - overlap.gx_rectangle_top + 1;
        read = (ULONG *)canvas -> gx_canvas_memory;
        write = (ULONG *)composite -> gx_canvas_memory;

        if (canvas -> gx_canvas_display -> gx_display_rotation_angle == GX_SCREEN_ROTATION_CW)
        {
            /* Index into starting row.  */
            read += (dirty.gx_rectangle_right - overlap.gx_rectangle_right) * canvas -> gx_canvas_y_resolution;

            /* Index into pixel.  */
            read += overlap.gx_rectangle_top - dirty.gx_rectangle_top;

            /* Calculate the write pointer.  */
            write += (composite -> gx_canvas_x_resolution - overlap.gx_rectangle_right - 1) * composite -> gx_canvas_y_resolution;
            write += overlap.gx_rectangle_top;
        }
        else
        {
            /* Index into starting row.  */
            read += (overlap.gx_rectangle_left - dirty.gx_rectangle_left) * canvas -> gx_canvas_y_resolution;

            /* Index into pixel.  */
            read += dirty.gx_rectangle_bottom - overlap.gx_rectangle_bottom;

            /* Calculate the write pointer.  */
            write += overlap.gx_rectangle_left * composite -> gx_canvas_y_resolution;
            write += (composite -> gx_canvas_y_resolution - overlap.gx_rectangle_bottom - 1);
        }

        for (row = overlap.gx_rectangle_left; row <= overlap.gx_rectangle_right; row++)
        {
            memcpy(write, read, (size_t)width * 4); /* Use case of memcpy is verified. */

            write += composite -> gx_canvas_y_resolution;
            read += canvas -> gx_canvas_y_resolution;
        }
    }
}

