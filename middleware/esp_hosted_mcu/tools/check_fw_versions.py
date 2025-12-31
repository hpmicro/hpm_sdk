#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# check the host and co-processor version numbers aginst the value in
# idf_component.yml
# exit with 0 if ok
# exit with 1 if fail

import argparse
from datetime import date
import os
import re
import sys

# paths to files to check
yml_file = "idf_component.yml"
coprocessor_version_file = "slave/main/esp_hosted_coprocessor_fw_ver.h"
host_version_file = "host/esp_hosted_host_fw_ver.h"

# year starts from 2025
start_year = 2025

def get_idf_yml_version() -> (int, int, int):
	# read the yml file
	file_info = open(yml_file, "r")
	info = file_info.read()
	file_info.close()

	# extract the version info
	ver = re.search("^version: \"([0-9]+).([0-9]+).([0-9]+)\"", info)
	#print("yml:", ver.group(1), ver.group(2), ver.group(3))

	# return version info as a tuple (major, minor, patch)
	return (ver.group(1), ver.group(2), ver.group(3))

def get_coprocessor_version() -> (int, int, int):
	# read the coprocessor file
	try:
		file_info = open(coprocessor_version_file, "r")
	except:
		print("Coprocessor file open error: default to 0.0.0")
		return (0, 0, 0)
	info = file_info.read()
	file_info.close()

	# extract the version info
	major_re = re.search("_VERSION_MAJOR_1 ([0-9]+)", info)
	minor_re = re.search("_VERSION_MINOR_1 ([0-9]+)", info)
	patch_re = re.search("_VERSION_PATCH_1 ([0-9]+)", info)

	if ((not major_re) or (not minor_re) or (not patch_re)):
		print("No coprocessor version info found: default to 0.0.0")
		return (0, 0, 0)

	#print("coprocessor:", major_re.group(1), minor_re.group(1), patch_re.group(1))

	return (major_re.group(1), minor_re.group(1), patch_re.group(1))

def get_host_version() -> (int, int, int):
	# read the host file
	try:
		file_info = open(host_version_file, "r")
	except:
		print("Host file open error: default to 0.0.0")
		return (0, 0, 0)
	info = file_info.read()
	file_info.close()

	# extract the version info
	major_re = re.search("_VERSION_MAJOR_1 ([0-9]+)", info)
	minor_re = re.search("_VERSION_MINOR_1 ([0-9]+)", info)
	patch_re = re.search("_VERSION_PATCH_1 ([0-9]+)", info)

	if ((not major_re) or (not minor_re) or (not patch_re)):
		print("No host version info found: default to 0.0.0")
		return (0, 0, 0)

	#print("host:", major_re.group(1), minor_re.group(1), patch_re.group(1))

	return (major_re.group(1), minor_re.group(1), patch_re.group(1))

# write common header using the provided file handle
def write_common_header(file_info):
	year = date.today().year

	file_info.write("/*\n")
	if (year == start_year):
		file_info.write(f" * SPDX-FileCopyrightText: {year} Espressif Systems (Shanghai) CO LTD\n")
	else:
		file_info.write(f" * SPDX-FileCopyrightText: {start_year}-{year} Espressif Systems (Shanghai) CO LTD\n")
	file_info.write(" *\n")
	file_info.write(" * SPDX-License-Identifier: Apache-2.0\n")
	file_info.write(" */\n")

def set_coprocessor_version(version: tuple) -> int:
	# write the coprocessor file
	file_info = open(coprocessor_version_file, "w")

	write_common_header(file_info)

	file_info.write("#ifndef __ESP_HOSTED_COPROCESSOR_FW_VER_H__\n")
	file_info.write("#define __ESP_HOSTED_COPROCESSOR_FW_VER_H__\n")
	ver = version[0]
	file_info.write(f"#define PROJECT_VERSION_MAJOR_1 {ver}\n")
	ver = version[1]
	file_info.write(f"#define PROJECT_VERSION_MINOR_1 {ver}\n")
	ver = version[2]
	file_info.write(f"#define PROJECT_VERSION_PATCH_1 {ver}\n")
	file_info.write("#endif\n")
	file_info.close()
	return 0

def set_host_version(version: tuple) -> int:
	# write the host file
	file_info = open(host_version_file, "w")

	write_common_header(file_info)

	file_info.write("#ifndef __ESP_HOSTED_HOST_FW_VERSION_H__\n");
	file_info.write("#define __ESP_HOSTED_HOST_FW_VERSION_H__\n");
	ver = version[0]
	file_info.write(f"#define ESP_HOSTED_VERSION_MAJOR_1 {ver}\n")
	ver = version[1]
	file_info.write(f"#define ESP_HOSTED_VERSION_MINOR_1 {ver}\n")
	ver = version[2]
	file_info.write(f"#define ESP_HOSTED_VERSION_PATCH_1 {ver}\n")
	file_info.write("#endif\n")
	file_info.close()
	return 0

def check_slave_version(yml_version: tuple) -> int:
	version = get_coprocessor_version()
	if (version == yml_version):
		return 0
	else:
		return 1

def check_host_version(yml_version: tuple) -> int:
	version = get_host_version()
	if (version == yml_version):
		return 0
	else:
		return 1

def check(args) -> int:
	ret = 0
	yml_version = get_idf_yml_version()
	update = args.update
	force = args.force

	if (check_slave_version(yml_version)):
		print(f"Co-processor version check failed. Info in {coprocessor_version_file} different from {yml_file}.")
		if (update or force):
			print(f"{coprocessor_version_file} updated with correct version info.")
			set_coprocessor_version(yml_version)
		ret = 1
	elif force:
		print(f"Force updating version info in {coprocessor_version_file}.")
		set_coprocessor_version(yml_version)

	if (check_host_version(yml_version)):
		print(f"Host version check failed. Info in {host_version_file} different from {yml_file}.")
		if (update or force):
			set_host_version(yml_version)
			print(f"{host_version_file} updated with correct version info.")
		ret = 1
	elif force:
		print(f"Force updating version info in {host_version_file}.")
		set_host_version(yml_version)

	return ret

if __name__ == '__main__':
	parser = argparse.ArgumentParser(description = "Check, update version headers against component version")
	parser.add_argument('-u', '--update', action = 'store_true',
		help = "Updates version headers if different from component version")
	parser.add_argument('-f', '--force', action = 'store_true',
		help = "Force update version headers")

	args = parser.parse_args()

	sys.exit(check(args))
