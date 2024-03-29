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
/**   Directory                                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    fx_directory.h                                      PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file defines the FileX Directory component constants, data     */
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

#ifndef FX_DIRECTORY_H
#define FX_DIRECTORY_H


/* Define the external Directory component function prototypes.  */

UINT _fx_directory_attributes_read(FX_MEDIA *media_ptr, CHAR *directory_name, UINT *attributes_ptr);
UINT _fx_directory_attributes_set(FX_MEDIA *media_ptr, CHAR *directory_name, UINT attributes);
UINT _fx_directory_create(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fx_directory_default_get(FX_MEDIA *media_ptr, CHAR **return_path_name);
UINT _fx_directory_default_get_copy(FX_MEDIA *media_ptr, CHAR *return_path_name_buffer, UINT return_path_name_buffer_size);
UINT _fx_directory_default_set(FX_MEDIA *media_ptr, CHAR *new_path_name);
UINT _fx_directory_delete(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fx_directory_entry_read_FAT(FX_MEDIA *media_ptr, FX_DIR_ENTRY *source_dir, ULONG *entry_ptr,
                                  FX_DIR_ENTRY *destination_ptr);
UINT _fx_directory_first_entry_find(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fx_directory_first_full_entry_find(FX_MEDIA *media_ptr, CHAR *directory_name, UINT *attributes,
                                         ULONG *size, UINT *year, UINT *month, UINT *day, UINT *hour, UINT *minute, UINT *second);
UINT _fx_directory_information_get(FX_MEDIA *media_ptr, CHAR *directory_name, UINT *attributes, ULONG *size,
                                   UINT *year, UINT *month, UINT *day, UINT *hour, UINT *minute, UINT *second);
UINT _fx_directory_local_path_clear(FX_MEDIA *media_ptr);
UINT _fx_directory_local_path_get(FX_MEDIA *media_ptr, CHAR **return_path_name);
UINT _fx_directory_local_path_get_copy(FX_MEDIA *media_ptr, CHAR *return_path_name_buffer, UINT return_path_name_buffer_size);
UINT _fx_directory_local_path_restore(FX_MEDIA *media_ptr, FX_LOCAL_PATH *local_path_ptr);
UINT _fx_directory_local_path_set(FX_MEDIA *media_ptr, FX_LOCAL_PATH *local_path_ptr, CHAR *new_path_name);
UINT _fx_directory_long_name_get(FX_MEDIA *media_ptr, CHAR *short_file_name, CHAR *long_file_name);
UINT _fx_directory_long_name_get_extended(FX_MEDIA *media_ptr, CHAR *short_file_name, CHAR *long_file_name, UINT long_file_name_buffer_length);
UINT _fx_directory_name_test(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fx_directory_next_entry_find(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fx_directory_next_full_entry_find(FX_MEDIA *media_ptr, CHAR *directory_name, UINT *attributes,
                                        ULONG *size, UINT *year, UINT *month, UINT *day, UINT *hour, UINT *minute, UINT *second);
UINT _fx_directory_rename(FX_MEDIA *media_ptr, CHAR *old_directory_name, CHAR *new_directory_name);
UINT _fx_directory_short_name_get(FX_MEDIA *media_ptr, CHAR *long_file_name, CHAR *short_file_name);
UINT _fx_directory_short_name_get_extended(FX_MEDIA *media_ptr, CHAR *long_file_name, CHAR *short_file_name, UINT short_file_name_length);

UINT _fxe_directory_attributes_read(FX_MEDIA *media_ptr, CHAR *directory_name, UINT *attributes_ptr);
UINT _fxe_directory_attributes_set(FX_MEDIA *media_ptr, CHAR *directory_name, UINT attributes);
UINT _fxe_directory_create(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fxe_directory_default_get(FX_MEDIA *media_ptr, CHAR **return_path_name);
UINT _fxe_directory_default_get_copy(FX_MEDIA *media_ptr, CHAR *return_path_name_buffer, UINT return_path_name_buffer_size);
UINT _fxe_directory_default_set(FX_MEDIA *media_ptr, CHAR *new_path_name);
UINT _fxe_directory_delete(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fxe_directory_first_entry_find(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fxe_directory_first_full_entry_find(FX_MEDIA *media_ptr, CHAR *directory_name, UINT *attributes,
                                          ULONG *size, UINT *year, UINT *month, UINT *day, UINT *hour, UINT *minute, UINT *second);
UINT _fxe_directory_information_get(FX_MEDIA *media_ptr, CHAR *directory_name, UINT *attributes, ULONG *size,
                                    UINT *year, UINT *month, UINT *day, UINT *hour, UINT *minute, UINT *second);
UINT _fxe_directory_local_path_clear(FX_MEDIA *media_ptr);
UINT _fxe_directory_local_path_get(FX_MEDIA *media_ptr, CHAR **return_path_name);
UINT _fxe_directory_local_path_get_copy(FX_MEDIA *media_ptr, CHAR *return_path_name_buffer, UINT return_path_name_buffer_size);
UINT _fxe_directory_local_path_restore(FX_MEDIA *media_ptr, FX_LOCAL_PATH *local_path_ptr);
UINT _fxe_directory_local_path_set(FX_MEDIA *media_ptr, FX_LOCAL_PATH *local_path_ptr, CHAR *new_path_name, UINT local_path_control_block_size);
UINT _fxe_directory_long_name_get(FX_MEDIA *media_ptr, CHAR *short_file_name, CHAR *long_file_name);
UINT _fxe_directory_long_name_get_extended(FX_MEDIA *media_ptr, CHAR *short_file_name, CHAR *long_file_name, UINT long_file_name_buffer_length);
UINT _fxe_directory_name_test(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fxe_directory_next_entry_find(FX_MEDIA *media_ptr, CHAR *directory_name);
UINT _fxe_directory_next_full_entry_find(FX_MEDIA *media_ptr, CHAR *directory_name, UINT *attributes,
                                         ULONG *size, UINT *year, UINT *month, UINT *day, UINT *hour, UINT *minute, UINT *second);
UINT _fxe_directory_rename(FX_MEDIA *media_ptr, CHAR *old_directory_name, CHAR *new_directory_name);
UINT _fxe_directory_short_name_get(FX_MEDIA *media_ptr, CHAR *long_file_name, CHAR *short_file_name);
UINT _fxe_directory_short_name_get_extended(FX_MEDIA *media_ptr, CHAR *long_file_name, CHAR *short_file_name, UINT short_file_name_length);


/* Define the internal Directory component function prototypes.  */

UINT  _fx_directory_entry_read(FX_MEDIA *media_ptr, FX_DIR_ENTRY *source_dir, ULONG *entry, FX_DIR_ENTRY *destination_ptr);
UINT  _fx_directory_entry_write(FX_MEDIA *media_ptr, FX_DIR_ENTRY *entry_ptr);
UINT  _fx_directory_free_search(FX_MEDIA *media_ptr, FX_DIR_ENTRY *directory_ptr, FX_DIR_ENTRY *entry_ptr);
CHAR *_fx_directory_name_extract(CHAR *source_ptr, CHAR *dest_ptr);
UINT  _fx_directory_search(FX_MEDIA *media_ptr, CHAR *name_ptr, FX_DIR_ENTRY *entry_ptr, FX_DIR_ENTRY *last_dir_ptr, CHAR **last_name_ptr);

#endif

