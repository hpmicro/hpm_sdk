# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
add_subdirectory_ifdef(CONFIG_AUDIO_DECODER_WAV wav)
add_subdirectory_ifdef(CONFIG_AUDIO_DECODER_FLAC flac)
sdk_inc(./)