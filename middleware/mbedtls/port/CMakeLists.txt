# Copyright (c) 2023-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if (CONFIG_MBEDTLS_USE_HPM_SDP)
    if (HPMSOC_HAS_HPMSDK_SDP)
        message(STATUS "Using HPM SDP for mbedtls")
        add_subdirectory(sdp)
        sdk_compile_definitions(-DCONFIG_MBEDTLS_USE_HPM_SDP=1)
    else()
        message(FATAL_ERROR "CONFIG_MBEDTLS_USE_HPM_SDP is enabled, but there is no SDP on this MCU ! Please set CONFIG_MBEDTLS_USE_HPM_SDP to 0 !")
    endif()
endif()


