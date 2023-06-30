# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
sdk_inc(./)
sdk_inc_ifdef(CONFIG_HPM_MATH_DSP nds_dsp)
sdk_inc_ifdef(CONFIG_HPM_MATH_NN_RVP32 nds_nn)
sdk_inc_ifdef(CONFIG_HPM_MATH_NN_RVP32_ONLY_SES nds_nn)
sdk_nds_compile_options("-mext-dsp")
sdk_ld_options_ifdef(CONFIG_HPM_MATH_DSP "-ldsp")
sdk_ld_options_ifdef(CONFIG_HPM_MATH_NN_NORMAL "-lnn")
sdk_ld_options("-lm")

sdk_ses_ld_lib_ifdef(CONFIG_HPM_MATH_DSP "${CMAKE_CURRENT_SOURCE_DIR}/nds_dsp/${HPM_MATH_DSP_SES_LIB}/${HPM_MATH_DSP_SES_LIB}.a")

sdk_ses_ld_lib_ifdef(CONFIG_HPM_MATH_NN_RVP32_ONLY_SES "${CMAKE_CURRENT_SOURCE_DIR}/nds_nn/rvp32_nn/libnn_rvp32.a")
sdk_ses_ld_lib_ifdef(CONFIG_HPM_MATH_NN_RVP32 "${CMAKE_CURRENT_SOURCE_DIR}/nds_nn/rvp32_nn/libnn_rvp32.a")
sdk_ses_ld_lib_ifdef(CONFIG_HPM_MATH_NN_NORMAL "${CMAKE_CURRENT_SOURCE_DIR}/nds_nn/libnn/${HPM_MATH_NN_SES_LIB}/libnn.a")
sdk_link_libraries_ifdef(CONFIG_HPM_MATH_NN_RVP32 "${CMAKE_CURRENT_SOURCE_DIR}/nds_nn/rvp32_nn/libnn_rvp32.a")

sdk_compile_definitions_ifdef(CONFIG_HPM_MATH_DSP "-DHPM_EN_MATH_DSP_LIB=1")
if(("${CONFIG_HPM_MATH_NN_RVP32_ONLY_SES}") AND (NOT "${CONFIG_HPM_MATH_NN_RVP32_ONLY_SES}" EQUAL 0))
  sdk_compile_definitions(-DHPM_EN_MATH_NN_RVP32_LIB=1)
elseif(("${CONFIG_HPM_MATH_NN_RVP32}") AND (NOT "${CONFIG_HPM_MATH_NN_RVP32}" EQUAL 0))
  sdk_compile_definitions(-DHPM_EN_MATH_NN_RVP32_LIB=1)
elseif(("${CONFIG_HPM_MATH_NN_NORMAL}") AND (NOT "${CONFIG_HPM_MATH_NN_NORMAL}" EQUAL 0))
  sdk_compile_definitions(-DHPM_EN_MATH_NN_LIB=1)
endif()


sdk_src(sw_dsp/hpm_math_sw.c)
if((NOT "${TOOLCHAIN_VARIANT}" STREQUAL "nds-gcc") AND (NOT "${SES_TOOLCHAIN_VARIANT}" STREQUAL "Andes"))
message(FATAL_ERROR "hpm_math middleware must use nds toolchain")
endif()

# from SES 6.30, nds dsp needs to be enabled explicitly
if("${SES_TOOLCHAIN_VARIANT}" STREQUAL "Andes")
  sdk_ses_compile_options("-mext-dsp")
endif()
