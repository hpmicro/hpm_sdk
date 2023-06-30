/*
 * Copyright 2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UX_DEVICE_DESCRIPTOR_H_
#define _UX_DEVICE_DESCRIPTOR_H_

/*!
 * @brief USB device get the high speed device descriptor function.
 *
 * This function gets the device descriptor of the USB high speed device.
 *
 * @return A USB high speed device descriptor.
 */
unsigned char *ux_get_hs_framework(void);

/*!
 * @brief USB device get the length of high speed device descriptor function.
 *
 * This function gets the length of high speed device descriptor.
 *
 * @return A USB high speed device descriptor length.
 */
unsigned long ux_get_hs_framework_length(void);

/*!
 * @brief USB device get full speed device descriptor function.
 *
 * This function gets the device descriptor of the USB full speed device.
 *
 * @return A USB full speed device descriptor.
 */
unsigned char *ux_get_fs_framework(void);

/*!
 * @brief USB device get the length of full speed device descriptor function.
 *
 * This function gets the length of full speed device descriptor.
 *
 * @return A USB full speed device descriptor length.
 */
unsigned long ux_get_fs_framework_length(void);

/*!
 * @brief USB device get device string descriptor function.
 *
 * This function gets the device string descriptor.
 *
 * @return A USB device string descriptor.
 */
unsigned char *ux_get_string_framework(void);

/*!
 * @brief USB device get the length of device string descriptor function.
 *
 * This function gets the length of device string descriptor.
 *
 * @return A USB device length of string descriptor.
 */
unsigned long ux_get_string_framework_length(void);

/*!
 * @brief USB device get device language descriptor function.
 *
 * This function gets the device language descriptor.
 *
 * @return A USB device language descriptor.
 */
unsigned char *ux_get_language_framework(void);

/*!
 * @brief USB device get the length of device language descriptor function.
 *
 * This function gets the length of device language descriptor.
 *
 * @return A USB device length of language descriptor.
 */
unsigned long ux_get_language_framework_length(void);

#endif /* _UX_DEVICE_DESCRIPTOR_H_ */