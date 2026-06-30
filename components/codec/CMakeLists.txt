# Copyright (c) 2024,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(.)

string(TOUPPER ${CONFIG_CODEC_NAME} CONFIG_CODEC_NAME_UPPER)
string(TOLOWER ${CONFIG_CODEC_NAME} CONFIG_CODEC_NAME_LOWER)
if((NOT ${CONFIG_CODEC_NAME_LOWER} STREQUAL "sgtl5000") AND (NOT ${CONFIG_CODEC_NAME_LOWER} STREQUAL "wm8960") AND (NOT ${CONFIG_CODEC_NAME_LOWER} STREQUAL "wm8978") AND (NOT ${CONFIG_CODEC_NAME_LOWER} STREQUAL "es8389"))
message(FATAL_ERROR "${CONFIG_CODEC_NAME} is not supported, only \"sgtl5000\" or \"wm8960\"  or \"wm8978\" or \"es8389\" is supported")
else()
message(STATUS "Audio Codec Enabled: ${CONFIG_CODEC_NAME_UPPER}")
sdk_compile_definitions(-DCONFIG_CODEC_${CONFIG_CODEC_NAME_UPPER}=1)
add_subdirectory(${CONFIG_CODEC_NAME_LOWER})
endif()
