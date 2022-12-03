# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause


## some files not used
if(DEFINED CONFIG_FFMPEG_INCLUDE_UNUSED)

    sdk_src(dump.c)
    sdk_src(os_support.c)

endif()
sdk_src(avio.c)
sdk_src(aviobuf.c)
sdk_src(cutils.c)
sdk_src(file.c)
sdk_src(format.c)
sdk_src(id3v1.c)
sdk_src(id3v2.c)
sdk_src(isom.c)
sdk_src(metadata.c)
sdk_src(mov.c)
sdk_src(mov_chan.c)
sdk_src(mux.c)
sdk_src(options.c)
sdk_src(qtpalette.c)
sdk_src(replaygain.c)
sdk_src(riff.c)
sdk_src(riffdec.c)
sdk_src(sdp.c)
sdk_src(url.c)
sdk_src(utilsformat.c)
