# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_TRACE_RECORDER_STREAM_MODE STREQUAL "JLink RTT")
    add_subdirectory(Jlink_RTT)
endif()
if(CONFIG_TRACE_RECORDER_STREAM_MODE STREQUAL "RingBuffer")
    add_subdirectory(RingBuffer)
endif()

