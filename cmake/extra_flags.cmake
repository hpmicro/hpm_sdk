# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

separate_arguments(EXTRA_C_FLAGS_LIST UNIX_COMMAND ${EXTRA_C_FLAGS})
separate_arguments(EXTRA_LD_FLAGS_LIST UNIX_COMMAND ${EXTRA_LD_FLAGS})
separate_arguments(EXTRA_LD_SYMBOLS_LIST UNIX_COMMAND ${EXTRA_LD_SYMBOLS})
separate_arguments(EXTRA_AS_FLAGS_LIST UNIX_COMMAND ${EXTRA_AS_FLAGS})

if(EXTRA_C_FLAGS_LIST)
    foreach(flag ${EXTRA_C_FLAGS_LIST})
        sdk_compile_options(${flag})
    endforeach()
endif()

if(EXTRA_AS_FLAGS_LIST)
    foreach(flag ${EXTRA_AS_FLAGS_LIST})
        sdk_compile_options(${flag})
    endforeach()
endif()

if(EXTRA_LD_FLAGS_LIST)
    foreach(flag ${EXTRA_LD_FLAGS_LIST})
        sdk_link_libraries(${EXTRA_LD_FLAGS})
    endforeach()
endif()

if(EXTRA_LD_SYMBOLS_LIST)
    foreach(flag ${EXTRA_LD_SYMBOLS_LIST})
        sdk_linker_global_symbols(${EXTRA_LD_SYMBOLS})
    endforeach()
endif()
