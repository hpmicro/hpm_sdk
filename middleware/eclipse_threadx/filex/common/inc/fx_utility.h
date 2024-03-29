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


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    fx_utility.h                                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the FileX Utility component constants, data       */
/*    definitions, and external references.  It is assumed that fx_api.h  */
/*    (and fx_port.h) have already been included.                         */
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

#ifndef FX_UTILITY_H
#define FX_UTILITY_H


/* Define the internal Utility component function prototypes.  */

UINT    _fx_utility_16_unsigned_read(UCHAR *source_ptr);
VOID    _fx_utility_16_unsigned_write(UCHAR *dest_ptr, UINT value);
ULONG   _fx_utility_32_unsigned_read(UCHAR *source_ptr);
VOID    _fx_utility_32_unsigned_write(UCHAR *dest_ptr, ULONG value);
ULONG64 _fx_utility_64_unsigned_read(UCHAR *source_ptr);
VOID    _fx_utility_64_unsigned_write(UCHAR *dest_ptr, ULONG64 value);
VOID    _fx_utility_memory_copy(UCHAR *source_ptr, UCHAR *dest_ptr, ULONG size);
VOID    _fx_utility_memory_set(UCHAR *dest_ptr, UCHAR value, ULONG size);
FX_CACHED_SECTOR
       *_fx_utility_logical_sector_cache_entry_read(FX_MEDIA *media_ptr, ULONG64 logical_sector, FX_CACHED_SECTOR **previous_cache_entry);
UINT    _fx_utility_logical_sector_read(FX_MEDIA *media_ptr, ULONG64 logical_sector,
                                        VOID *buffer_ptr, ULONG sectors, UCHAR sector_type);
UINT    _fx_utility_logical_sector_write(FX_MEDIA *media_ptr, ULONG64 logical_sector,
                                         VOID *buffer_ptr, ULONG sectors, UCHAR sector_type);
UINT    _fx_utility_logical_sector_flush(FX_MEDIA *media_ptr, ULONG64 starting_sector, ULONG64 sectors, UINT invalidate);
UINT    _fx_utility_FAT_entry_read(FX_MEDIA *media_ptr, ULONG cluster, ULONG *entry_ptr);
UINT    _fx_utility_FAT_entry_write(FX_MEDIA *media_ptr, ULONG cluster, ULONG next_cluster);
UINT    _fx_utility_FAT_flush(FX_MEDIA *media_ptr);
UINT    _fx_utility_FAT_map_flush(FX_MEDIA *media_ptr);
ULONG   _fx_utility_FAT_sector_get(FX_MEDIA *media_ptr, ULONG cluster);
UINT    _fx_utility_string_length_get(CHAR *string, UINT max_length);




#endif

