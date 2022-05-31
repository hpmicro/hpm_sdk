# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED ENV{HPM_SDK_BASE})
    message(FATAL_ERROR "HPM_SDK_BASE is not set yet") 
endif()

set(HPM_SDK_BASE $ENV{HPM_SDK_BASE})

include(${HPM_SDK_BASE}/cmake/application.cmake)
