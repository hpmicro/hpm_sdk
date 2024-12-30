# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause


sdk_inc(include)
sdk_src(src/rv_backtrace.c)

if(RV_BACKTRACE_USE_FP)
    sdk_src(src/rv_backtrace_fno.c)
    sdk_compile_options("-fno-omit-frame-pointer")
if(SES_COMPILER_VARIANT STREQUAL "SEGGER")
    sdk_ses_compile_options("-mframe-pointer=all")
endif()
else()
    sdk_src(src/rv_backtrace_fomit.c)
endif()

