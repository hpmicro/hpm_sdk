#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# check that RPC calls in protobuf and documentation files match
# exit with 0 if ok
# exit with 1 if fail

import os
import re
import sys

# method:
# 1. get the set of RPC calls from both the protobuf and documentation files
# 2. compare the sets
# 3. if there are differences, print the differences

protobuf_file = "common/proto/esp_hosted_rpc.proto"
doc_file = "docs/implemented_rpcs.md"

def get_set_from_document() -> set:
	set_rpcs = set()

	# read the protobuf file
	try:
		file_info = open(doc_file, "r")
	except:
		print("Document file open error")
		return set_of_rpcs

	# from this pattern in the doc_file:
    # |   1 |    257 | GetMacAddress | 0.0.6            |
	# it will extract GetMacAddress
	# this pattern works for the list of RCP Requests and Events
	pattern = re.compile("\|\s*\d+\s*\|\s*\d+\s*\|\s*(\S+)")

	for line in file_info:
		# get the list of rpc calls
		rpc_call = pattern.search(line)
		if rpc_call:
			# print(rpc_call.group(1))
			set_rpcs.add(rpc_call.group(1))

	file_info.close()

	# print(set_rpcs)

	return set_rpcs

def get_set_from_protobuf() -> set:
	set_rpcs = set()

	# read the protobuf file
	try:
		file_info = open(protobuf_file, "r")
	except:
		print("Protobuf file open error")
		return set_of_rpcs

	# extract requests
    # from this pattern in the protobuf_file
	# 		Rpc_Req_GetMacAddress               req_get_mac_address               = 257;
    # it will extract GetMacAddress
	pattern_req = re.compile("\s+(Rpc_Req_)(\S+)\s+req_")

	# extract events
    # from this pattern in the protobuf_file
    #		Rpc_Event_ESPInit                   event_esp_init                     = 769;
    # it will extract ESPInit
	pattern_event = re.compile("\s+(Rpc_Event_)(\S+)\s+event_")

	for line in file_info:
		# get the list of rpc calls
		rpc_call = pattern_req.search(line)
		if rpc_call:
			set_rpcs.add(rpc_call.group(2))
		rpc_call = pattern_event.search(line)
		if rpc_call:
			set_rpcs.add(rpc_call.group(2))

	file_info.close()

	# print(set_rpcs)

	return set_rpcs

def check() -> int:
	ret = 1

	# get the set of RPC calls in the protobuf file
	protobuf_set = get_set_from_protobuf()

	# get the set of RPC calls in the documentation file
	document_set = get_set_from_document()

	# compare the two

	# if not equal
	if (protobuf_set == document_set):
		ret = 0
	else:
		# get the differences
		only_in_docs = document_set.difference(protobuf_set)
		only_in_protobuf = protobuf_set.difference(document_set)

		if only_in_docs:
			print("Error: RPCs only found in document:", only_in_docs)
		if only_in_protobuf:
			print("Error: RPCs only found in protobuf:", only_in_protobuf)

	return ret

if __name__ == '__main__':
    sys.exit(check())
