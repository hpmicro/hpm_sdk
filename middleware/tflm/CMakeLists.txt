# Copyright (c) 2022-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

set_target_properties(${HPM_SDK_LIB} PROPERTIES CXX_STANDARD 17)
sdk_compile_definitions(TF_LITE_STATIC_MEMORY)
sdk_compile_definitions(TF_LITE_DISABLE_X86_NEON)
if(NOT CMAKE_CXX_FLAGS MATCHES "-fno-rtti")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics" CACHE STRING "" FORCE)
endif()

sdk_inc(tf_core)
sdk_inc(third_party/flatbuffers/include)
sdk_inc(third_party/gemmlowp)
sdk_inc(third_party/ruy)
sdk_inc(third_party/kissfft)
add_subdirectory(tf_core/tensorflow/lite)
add_subdirectory(third_party/kissfft)