# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)
sdk_inc(log)
sdk_inc(proto)
sdk_inc(transport)
sdk_inc(protobuf-c)
sdk_inc(rpc)
sdk_inc(utils)
sdk_src_glob(proto/*.c)
sdk_src_glob(protobuf-c/protobuf-c/*.c)
