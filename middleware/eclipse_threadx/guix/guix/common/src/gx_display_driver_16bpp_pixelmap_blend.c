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
#include "gx_context.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_16bpp_pixelmap_raw_blend         PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of uncompressed      */
/*    pixlemap file                                                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    [gx_display_driver_pixel_blend]        Display driver basic pixel   */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
static VOID _gx_display_driver_16bpp_pixelmap_raw_blend(GX_DRAW_CONTEXT *context,
                                                        INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
int           xval;
int           yval;
USHORT       *get;
USHORT       *getrow;
USHORT        pixel;

GX_RECTANGLE *clip = context -> gx_draw_context_clip;
VOID          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context -> gx_draw_context_display -> gx_display_driver_pixel_blend;
    if (!blend_func)
    {
        return;
    }

    getrow = (USHORT *)(pixelmap -> gx_pixelmap_data + (INT)sizeof(USHORT) * pixelmap -> gx_pixelmap_width * (clip -> gx_rectangle_top - ypos));
    getrow += (clip -> gx_rectangle_left - xpos);
    for (yval = clip -> gx_rectangle_top; yval <= clip -> gx_rectangle_bottom; yval++)
    {
        get = getrow;
        for (xval = clip -> gx_rectangle_left; xval <= clip -> gx_rectangle_right; xval++)
        {
            pixel = *get++;
            blend_func(context, xval, yval, pixel, alpha);
        }
        getrow += pixelmap -> gx_pixelmap_width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixelmap_alpha_blend      PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of uncompressed      */
/*    pixelmap file with alpha channel.                                   */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
static VOID _gx_display_driver_565rgb_pixelmap_alpha_blend(GX_DRAW_CONTEXT *context,
                                                           INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT           skipcount;
INT           xval;
INT           yval;
USHORT       *getrow;
GX_UBYTE     *getrowalpha;
USHORT       *get;
USHORT        pixel;
GX_UBYTE     *getalpha;
INT           combined_alpha;
GX_UBYTE      internal_alpha;

GX_RECTANGLE *clip = context -> gx_draw_context_clip;
void          (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);

    blend_func = context -> gx_draw_context_display -> gx_display_driver_pixel_blend;
    if (blend_func == GX_NULL)
    {
        return;
    }


    /* calculate how many pixels to skip */
    skipcount = (pixelmap -> gx_pixelmap_width) * (clip -> gx_rectangle_top - ypos);
    skipcount += (clip -> gx_rectangle_left - xpos);
    getrow = (USHORT *)(pixelmap -> gx_pixelmap_data);
    getrow += skipcount;

    getrowalpha = (GX_UBYTE *)(pixelmap -> gx_pixelmap_aux_data);
    getrowalpha += skipcount;

    for (yval  = clip -> gx_rectangle_top; yval <= clip -> gx_rectangle_bottom; yval++)
    {
        get = getrow;
        getalpha = getrowalpha;

        for (xval = clip -> gx_rectangle_left; xval <= clip -> gx_rectangle_right; xval++)
        {
            internal_alpha = *getalpha++;
            if (internal_alpha)
            {
                combined_alpha = internal_alpha * alpha;
                combined_alpha /= 255;
                pixel = *get;
                blend_func(context, xval, yval, pixel, (GX_UBYTE)combined_alpha);
            }
            get++;
        }
        getrow += pixelmap -> gx_pixelmap_width;
        getrowalpha += pixelmap -> gx_pixelmap_width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_16bpp_pixelmap_compressed_blend                  */
/*                                                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of compressed        */
/*    pixlemap file .                                                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*     [gx_display_driver_pixel_blend]       Display driver basic pixel   */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
static VOID _gx_display_driver_16bpp_pixelmap_compressed_blend(GX_DRAW_CONTEXT *context,
                                                               INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
int              yval;
int              xval;
GX_CONST USHORT *get;
USHORT           count;
USHORT           pixel;
VOID             (*blend_func)(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
GX_RECTANGLE    *clip = context -> gx_draw_context_clip;

    blend_func = context -> gx_draw_context_display -> gx_display_driver_pixel_blend;

    if (!blend_func)
    {
        return;
    }

    get = (GX_CONST USHORT *)pixelmap -> gx_pixelmap_data;

    /* compressed with no alpha is a two-byte count and two-byte pixel value */

    /* first, skip to the starting row */
    for (yval = ypos; yval < clip -> gx_rectangle_top; yval++)
    {
        xval = 0;
        while (xval < pixelmap -> gx_pixelmap_width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                count = (USHORT)((count & 0x7fff) + 1);
                get++;      /* skip repeated pixel value */
            }
            else
            {
                count++;
                get += count;   /* skip raw pixel values */
            }
            xval += count;
        }
    }


    while (yval <= clip -> gx_rectangle_bottom)
    {
        xval = xpos;

        while (xval < xpos + pixelmap -> gx_pixelmap_width)
        {
            count = *get++;

            if (count & 0x8000)
            {
                /* repeated value */
                count = (USHORT)((count & 0x7fff) + 1);
                pixel = *get++;

                while (count--)
                {
                    if (xval >= clip -> gx_rectangle_left &&
                        xval <= clip -> gx_rectangle_right)
                    {
                        blend_func(context, xval, yval, pixel, alpha);
                    }
                    xval++;
                }
            }
            else
            {
                /* string of non-repeated values */
                count++;
                while (count--)
                {
                    if (xval >= clip -> gx_rectangle_left &&
                        xval <= clip -> gx_rectangle_right)
                    {
                        pixel = *get;
                        blend_func(context, xval, yval, pixel, alpha);
                    }
                    get++;
                    xval++;
                }
            }
        }
        yval++;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_palette_pixelmap_raw_blend  PORTABLE C    */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of raw pixlemap      */
/*    file without transparent for palette pixelmap.                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
static VOID _gx_display_driver_565rgb_palette_pixelmap_raw_blend(GX_DRAW_CONTEXT *context,
                                                                 INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT                xval;
INT                yval;
GX_UBYTE          *getrow;
GX_CONST GX_UBYTE *get;
GX_COLOR          *palette;
USHORT             pixel;
GX_UBYTE           r;
GX_UBYTE           g;
GX_UBYTE           b;

GX_RECTANGLE      *clip = context -> gx_draw_context_clip;

    getrow = (GX_UBYTE *)(pixelmap -> gx_pixelmap_data);
    getrow += pixelmap -> gx_pixelmap_width * (clip -> gx_rectangle_top - ypos);
    getrow += (clip -> gx_rectangle_left - xpos);

    palette = (GX_COLOR *)pixelmap -> gx_pixelmap_aux_data;

    for (yval = clip -> gx_rectangle_top; yval <= clip -> gx_rectangle_bottom; yval++)
    {
        get = getrow;

        for (xval = clip -> gx_rectangle_left; xval <= clip -> gx_rectangle_right; xval++)
        {
            r = (GX_UBYTE)(REDVAL_32BPP(palette[*get]) >> 3);
            g = (GX_UBYTE)(GREENVAL_32BPP(palette[*get]) >> 2);
            b = (GX_UBYTE)(BLUEVAL_32BPP(palette[*get++]) >> 3);
            pixel = (USHORT)ASSEMBLECOLOR_16BPP(r, g, b);
            _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, alpha);
        }
        getrow += pixelmap -> gx_pixelmap_width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_palette_pixelmap_transparent_blend        */
/*                                                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of raw pixlemap      */
/*    file with transparent for palette pixelmap.                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
static VOID _gx_display_driver_565rgb_palette_pixelmap_transparent_blend(GX_DRAW_CONTEXT *context,
                                                                         INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT                xval;
INT                yval;
GX_UBYTE          *getrow;
GX_CONST GX_UBYTE *get;
GX_COLOR          *palette;
USHORT             pixel;
GX_UBYTE           r;
GX_UBYTE           g;
GX_UBYTE           b;

GX_RECTANGLE      *clip = context -> gx_draw_context_clip;

    getrow = (GX_UBYTE *)(pixelmap -> gx_pixelmap_data);
    getrow += pixelmap -> gx_pixelmap_width * (clip -> gx_rectangle_top - ypos);
    getrow += (clip -> gx_rectangle_left - xpos);

    palette = (GX_COLOR *)pixelmap -> gx_pixelmap_aux_data;


    for (yval = clip -> gx_rectangle_top; yval <= clip -> gx_rectangle_bottom; yval++)
    {
        get = getrow;

        for (xval = clip -> gx_rectangle_left; xval <= clip -> gx_rectangle_right; xval++)
        {
            if ((*get) != pixelmap -> gx_pixelmap_transparent_color)
            {
                r = (GX_UBYTE)(REDVAL_32BPP(palette[*get]) >> 3);
                g = (GX_UBYTE)(GREENVAL_32BPP(palette[*get]) >> 2);
                b = (GX_UBYTE)(BLUEVAL_32BPP(palette[*get]) >> 3);
                pixel = (USHORT)ASSEMBLECOLOR_16BPP(r, g, b);
                _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, alpha);
            }
            get++;
        }
        getrow += pixelmap -> gx_pixelmap_width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_4444argb_pixelmap_raw_blend               */
/*                                                         PORTABLE C     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Internal helper function that handles blending of uncompressed      */
/*    pixlemap file with alpha channel of 4444argb format.                */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixel_blend  Display driver basic pixel   */
/*                                             blend function             */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
static VOID _gx_display_driver_565rgb_4444argb_pixelmap_raw_blend(GX_DRAW_CONTEXT *context,
                                                                  INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
INT              skipcount;
INT              xval;
INT              yval;
USHORT          *getrow;
GX_CONST USHORT *get;
UCHAR            alpha_value;
GX_UBYTE         combined_alpha;
USHORT           pixel;

GX_RECTANGLE    *clip = context -> gx_draw_context_clip;

    /* calculate how many pixels to skip */
    skipcount = (pixelmap -> gx_pixelmap_width) * (clip -> gx_rectangle_top - ypos);
    skipcount += (clip -> gx_rectangle_left - xpos);
    getrow = (USHORT *)(pixelmap -> gx_pixelmap_data);
    getrow += skipcount;

    for (yval = clip -> gx_rectangle_top; yval <= clip -> gx_rectangle_bottom; yval++)
    {
        get = getrow;

        for (xval = clip -> gx_rectangle_left; xval <= clip -> gx_rectangle_right; xval++)
        {
            /* 0x000f- -> b , 0x00f0- -> g , 0x0f00- -> r , 0xf000- -> a */
            /*4444bgra - ->  565rgb*/
            alpha_value = (UCHAR)(((*get) & 0xf000) >> 8);
            if (alpha_value)
            {
                alpha_value = alpha_value | (alpha_value >> 4);
                pixel = (USHORT)((((*get) & 0x0f00) << 4) | (((*get) & 0x00f0) << 3) | (((*get) & 0x000f) << 1));
                combined_alpha = (GX_UBYTE)(alpha * alpha_value / 255);
                _gx_display_driver_565rgb_pixel_blend(context, xval, yval, pixel, combined_alpha);
            }
            get++;
        }
        getrow += pixelmap -> gx_pixelmap_width;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixelmap_blend            PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Driver entry point for pixelmap blending function that handles      */
/*    compressed or uncompress, with or without alpha channel.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_565rgb_palette_pixelmap_transparent_blend        */
/*    _gx_display_driver_565rgb_palette_pixelmap_raw_blend                */
/*    _gx_display_driver_565rgb_4444argb_pixelmap_raw_blend               */
/*    _gx_display_driver_565rgb_pixelmap_alpha_blend                      */
/*    _gx_display_driver_16bpp_pixelmap_compressed_blend                  */
/*    _gx_display_driver_16bpp_pixelmap_raw_blend                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
VOID _gx_display_driver_565rgb_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                              INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
    if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        if (pixelmap -> gx_pixelmap_format != GX_COLOR_FORMAT_565RGB)
        {
            return;
        }
    }

    switch (pixelmap -> gx_pixelmap_format)
    {
    case GX_COLOR_FORMAT_8BIT_PALETTE:
        if (pixelmap -> gx_pixelmap_aux_data == GX_NULL)
        {
            break;
        }

        if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_TRANSPARENT)
        {
            _gx_display_driver_565rgb_palette_pixelmap_transparent_blend(context, xpos, ypos, pixelmap, alpha);
        }
        else
        {
            _gx_display_driver_565rgb_palette_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
        }
        break;

    case GX_COLOR_FORMAT_4444ARGB:
        _gx_display_driver_565rgb_4444argb_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
        break;

    case GX_COLOR_FORMAT_565RGB:
        if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
        {
            _gx_display_driver_565rgb_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);
        }
        else
        {
            if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
            {
                _gx_display_driver_16bpp_pixelmap_compressed_blend(context, xpos, ypos, pixelmap, alpha);
            }
            else
            {
                _gx_display_driver_16bpp_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
            }
        }
        break;

    default:
        return;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_1555xrgb_pixelmap_blend          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Driver entry point for pixelmap blending function that handles      */
/*    compressed or uncompress, with or without alpha channel.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_565rgb_pixelmap_alpha_blend                      */
/*    _gx_display_driver_16bpp_pixelmap_compressed_blend                  */
/*    _gx_display_driver_16bpp_pixelmap_raw_blend                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
VOID _gx_display_driver_1555xrgb_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
    switch (pixelmap -> gx_pixelmap_format)
    {
    case GX_COLOR_FORMAT_1555XRGB:
        if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
        {
            _gx_display_driver_565rgb_pixelmap_alpha_blend(context, xpos, ypos, pixelmap, alpha);
        }
        else
        {
            if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
            {
                _gx_display_driver_16bpp_pixelmap_compressed_blend(context, xpos, ypos, pixelmap, alpha);
            }
            else
            {
                _gx_display_driver_16bpp_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
            }
        }
        break;

    default:
        return;
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _gx_display_driver_4444argb_pixelmap_blend          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Kenneth Maxwell, Microsoft Corporation                              */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    Driver entry point for pixelmap blending function that handles      */
/*    compressed or uncompress, with or without alpha channel.            */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    context                               Drawing context               */
/*    xpos                                  x-coord of top-left draw point*/
/*    ypos                                  y-coord of top-left draw point*/
/*    pixelmap                              Pointer to GX_PIXELMAP struct */
/*    alpha                                 blending value 0 to 255       */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _gx_display_driver_16bpp_pixelmap_compressed_blend                  */
/*    _gx_display_driver_16bpp_pixelmap_raw_blend                         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    GUIX Internal Code                                                  */
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
VOID _gx_display_driver_4444argb_pixelmap_blend(GX_DRAW_CONTEXT *context,
                                                INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
    switch (pixelmap -> gx_pixelmap_format)
    {
    case GX_COLOR_FORMAT_4444ARGB:
        if (!(pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_ALPHA))
        {
            if (pixelmap -> gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
            {
                _gx_display_driver_16bpp_pixelmap_compressed_blend(context, xpos, ypos, pixelmap, alpha);
            }
            else
            {
                _gx_display_driver_16bpp_pixelmap_raw_blend(context, xpos, ypos, pixelmap, alpha);
            }
        }
        break;

    default:
        return;
    }
}

