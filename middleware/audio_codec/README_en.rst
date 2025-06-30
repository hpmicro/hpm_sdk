Audio Codec Middleware
====================

Overview
--------
The Audio Codec middleware provides basic audio decoding functionality, currently supporting WAV format audio file decoding.

Main Features
------------
1. Provides basic audio decoding interfaces
2. Supports WAV format audio file decoding
3. Provides file operation callback function interfaces

File Structure
-------------
- ``hpm_audio_codec.h``: Basic audio decoding interface definitions
- ``wav/``: WAV format decoding implementation
  - ``hpm_wav_codec.h``: WAV format decoding interface definitions
  - ``hpm_decoder/``: WAV decoder implementation

Interface Description
-------------------

Basic Interfaces
~~~~~~~~~~~~~~~
- ``hpm_audiocodec_callback``: File operation callback function structure
- ``hpm_audiocodec_search_file``: File search callback
- ``hpm_audiocodec_read_file``: File read callback
- ``hpm_audiocodec_write_file``: File write callback
- ``hpm_audiocodec_close_file``: File close callback

WAV Decoding Interfaces
~~~~~~~~~~~~~~~~~~~~~
- ``hpm_wav_decode_init``: Initialize WAV decoding
- ``hpm_wav_decode``: WAV file decoding

Error Codes
----------
- ``status_audio_codec_format_err``: Format error
- ``status_audio_codec_non_standard``: Non-standard format
- ``status_audio_codec_none_file``: File not found
- ``status_audio_codec_none_dir``: Directory not found
- ``status_audio_codec_end``: Decoding completed