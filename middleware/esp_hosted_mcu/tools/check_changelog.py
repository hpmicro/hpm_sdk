#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# check that the CHANGELOG.md file contains the changelog for the version
# in idf_component.yml
# exit with 0 if ok
# exit with 1 if fail

# to be run whenever idf_component.yml is updated

import re
import sys

# paths to files to check
yml_file = "idf_component.yml"
changelog_file = "CHANGELOG.md"

def get_idf_yml_version_as_string() -> str:
	# read the yml file
	file_info = open(yml_file, "r")
	info = file_info.read()
	file_info.close()

	# extract the version info
	ver = re.search("^version: \"([0-9.]+)\"", info)
	# print("yml:", ver.group(1))
	return ver.group(1)

def changelog_has_version(ver_string: str) -> int:
	# iterate over the changelog file
	matching_line = "## " + ver_string
	with open(changelog_file, "r") as changelog:
		for line in changelog:
			if re.match(matching_line, line):
				return 0
	return 1

def check() -> int:
	yml_string = get_idf_yml_version_as_string()
	result = changelog_has_version(yml_string)
	if result:
		print(f"Changelog for version {yml_string} not found in {changelog_file}")
		return 1
	return 0

if __name__ == '__main__':
	sys.exit(check())
