Audio Codec 中间件
================

概述
----
Audio Codec 中间件提供了音频解码的基础功能，目前主要支持 WAV 格式的音频文件解码。

主要功能
--------
1. 提供音频解码的基础接口
2. 支持 WAV 格式音频文件的解码
3. 提供文件操作的回调函数接口

文件结构
--------
- ``hpm_audio_codec.h``: 音频解码基础接口定义
- ``wav/``: WAV 格式解码实现
  - ``hpm_wav_codec.h``: WAV 格式解码接口定义
  - ``hpm_decoder/``: WAV 解码器实现

接口说明
--------

基础接口
~~~~~~~~
- ``hpm_audiocodec_callback``: 文件操作回调函数结构体
- ``hpm_audiocodec_search_file``: 文件搜索回调
- ``hpm_audiocodec_read_file``: 文件读取回调
- ``hpm_audiocodec_write_file``: 文件写入回调
- ``hpm_audiocodec_close_file``: 文件关闭回调

WAV 解码接口
~~~~~~~~~~~
- ``hpm_wav_decode_init``: 初始化 WAV 解码
- ``hpm_wav_decode``: WAV 文件解码

错误码
------
- ``status_audio_codec_format_err``: 格式错误
- ``status_audio_codec_non_standard``: 非标准格式
- ``status_audio_codec_none_file``: 文件不存在
- ``status_audio_codec_none_dir``: 目录不存在
- ``status_audio_codec_end``: 解码结束