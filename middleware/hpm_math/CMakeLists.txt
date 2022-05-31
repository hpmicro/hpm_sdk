# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause
sdk_inc(./)
sdk_inc(nds_dsp)
sdk_nds_compile_options("-mext-dsp")
sdk_ld_options("-ldsp -lm")
sdk_ses_ld_lib("${CMAKE_CURRENT_SOURCE_DIR}/nds_dsp/${HPM_MATH_SES_LIB}/${HPM_MATH_SES_LIB}.a")
sdk_src(sw_dsp/hpm_math_sw.c)
if((NOT "${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc") AND (NOT "${SES_TOOLCHAIN_VARIANT}" STREQUAL "Andes"))
message(FATAL_ERROR "hpm_math middleware must use nds toolchain")
endif()

# from SES 6.30, nds dsp needs to be enabled explicitly
if("${SES_TOOLCHAIN_VARIANT}" STREQUAL "Andes")
  sdk_ses_compile_options("-mext-dsp")
endif()
